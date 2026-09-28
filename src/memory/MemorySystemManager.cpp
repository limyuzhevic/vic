#include "MemorySystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>

namespace nlm {

struct MemorySystemManager::Impl {
    Brain* brain = nullptr;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Configuration
    size_t workingMemoryCapacity = 0;
    size_t maxEpisodes = 0;
    size_t maxAssociations = 0;
    float memoryDecayRate = 0.01f;
    float consolidationFrequency = 1000.0f;
    
    // State
    size_t totalStoredNeurons = 0;
    size_t totalReplayEvents = 0;
    double lastConsolidationStep = 0.0;
};

MemorySystemManager::MemorySystemManager() : pImpl(std::make_unique<Impl>()) {}

MemorySystemManager::~MemorySystemManager() = default;

void MemorySystemManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize with brain reference if needed
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(pImpl->brain);
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(pImpl->brain);
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(pImpl->brain);
    }
    
    NLM_LOG_INFO("MemorySystemManager initialized");
}

void MemorySystemManager::update(float timestep) {
    if (!pImpl->workingMemory && !pImpl->episodicMemory && !pImpl->associativeMemory) {
        return;
    }
    
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
    
    // Decay memory traces based on time
    if (pImpl->memoryDecayRate > 0.0f) {
        float decayFactor = std::exp(-pImpl->memoryDecayRate * timestep);
        // Apply decay to working memory traces (simplified implementation)
        pImpl->workingMemory->decay(pImpl->brain, decayFactor);
    }
    
    NLM_LOG_DEBUG("MemorySystemManager updated");
}

void MemorySystemManager::configure(size_t workingMemoryCapacity, size_t maxEpisodes,
                                    size_t maxAssociations) {
    pImpl->workingMemoryCapacity = workingMemoryCapacity;
    pImpl->maxEpisodes = maxEpisodes;
    pImpl->maxAssociations = maxAssociations;
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(workingMemoryCapacity);
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->setMaxAssociations(maxAssociations);
    }
    
    NLM_LOG_INFO("MemorySystemManager configured");
}

void MemorySystemManager::consolidate(float importanceThreshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(importanceThreshold);
    }
    lastConsolidationStep = pImpl->brain->getCurrentStep();
    NLM_LOG_INFO("Memory consolidation completed");
}

void MemorySystemManager::processReplay(uint32_t episodeCount) {
    if (pImpl->episodicMemory) {
        auto episodes = pImpl->episodicMemory->getEpisodesForReplay(episodeCount);
        for (const auto* episode : episodes) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
        pImpl->totalReplayEvents += episodes.size();
    }
    NLM_LOG_INFO("Memory replay processed");
}

void MemorySystemManager::reset() {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    
    pImpl->totalStoredNeurons = 0;
    pImpl->totalReplayEvents = 0;
    
    NLM_LOG_INFO("MemorySystemManager reset");
}

void MemorySystemManager::setEfficiencyParameters(float memoryDecayRate, 
                                                   float consolidationFrequency) {
    pImpl->memoryDecayRate = memoryDecayRate;
    pImpl->consolidationFrequency = consolidationFrequency;
    
    // Trigger consolidation if frequency is met
    if (pImpl->consolidationFrequency > 0.0f) {
        double currentStep = pImpl->brain->getCurrentStep();
        if (currentStep - lastConsolidationStep >= pImpl->consolidationFrequency) {
            consolidate(0.5f);
        }
    }
}

void MemorySystemManager::processSensoryInput(const class SensoryInput& input) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeInput(input);
    }
    // Also update associative memories with sensory patterns
    if (pImpl->associativeMemory) {
        auto& data = input.getData();
        std::vector<std::pair<NeuronId, float>> patterns;
        for (size_t i = 0; i < std::min(data.size(), size_t(100)); ++i) {
            patterns.emplace_back(static_cast<NeuronId>(i), static_cast<float>(data[i]));
        }
        pImpl->associativeMemory->updateAssociations(patterns);
    }
}

void MemorySystemManager::recordCurrentActivity(Timestamp timestamp, float reward) {
    if (pImpl->episodicMemory && pImpl->brain) {
        // Collect current neural activity
        NeuralEpisodicMemory::EpisodicMemoryItem episode;
        episode.timestamp = timestamp;
        episode.reward = reward;
        
        // Store active neurons from all regions
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        pImpl->episodicMemory->storeEpisode(episode);
        pImpl->totalStoredNeurons += episode.activeNeurons.size();
    }
}

void MemorySystemManager::updateAssociations(const std::vector<std::pair<NeuronId, float>>& patterns) {
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->updateAssociations(patterns);
    }
}

size_t MemorySystemManager::getActiveMemoryTraces() const {
    return pImpl->workingMemory ? pImpl->workingMemory->getActiveTraces() : 0;
}

size_t MemorySystemManager::getEpisodeCount() const {
    return pImpl->episodicMemory ? pImpl->episodicMemory->getEpisodeCount() : 0;
}

size_t MemorySystemManager::getActiveAssociations() const {
    return pImpl->associativeMemory ? pImpl->associativeMemory->getActiveAssociations() : 0;
}

} // namespace nlm
