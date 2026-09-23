#include "memory/MemorySystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <algorithm>

namespace nlm {

struct MemorySystem::Impl {
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    Brain* brain;
    
    // Configuration
    size_t workingMemoryCapacity;
    size_t maxEpisodes;
    float consolidationThreshold;
    size_t replayInterval;
    size_t stepsSinceLastConsolidation;
    size_t stepsSinceLastReplay;
    
    Impl()
        : workingMemory(nullptr)
        , episodicMemory(nullptr)
        , associativeMemory(nullptr)
        , brain(nullptr)
        , workingMemoryCapacity(100)
        , maxEpisodes(1000)
        , consolidationThreshold(0.3f)
        , replayInterval(100)
        , stepsSinceLastConsolidation(0)
        , stepsSinceLastReplay(0) {}
};

MemorySystem::MemorySystem() : pImpl(std::make_unique<Impl>()) {}
MemorySystem::~MemorySystem() = default;

MemorySystem::MemorySystem(MemorySystem&& other) noexcept = default;
MemorySystem& MemorySystem::operator=(MemorySystem&& other) noexcept = default;

void MemorySystem::initialize(Brain* b) {
    brain = b;
    
    // Create memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize with brain reference
    workingMemory->initialize(b);
    episodicMemory->initialize(b);
    associativeMemory->initialize(b);
    
    // Apply configuration
    setWorkingMemoryCapacity(pImpl->workingMemoryCapacity);
    setMaxEpisodes(pImpl->maxEpisodes);
    setConsolidationThreshold(pImpl->consolidationThreshold);
    setReplayInterval(pImpl->replayInterval);
}

NeuralWorkingMemory* MemorySystem::getWorkingMemory() {
    return workingMemory.get();
}

const NeuralWorkingMemory* MemorySystem::getWorkingMemory() const {
    return workingMemory.get();
}

NeuralEpisodicMemory* MemorySystem::getEpisodicMemory() {
    return episodicMemory.get();
}

const NeuralEpisodicMemory* MemorySystem::getEpisodicMemory() const {
    return episodicMemory.get();
}

NeuralAssociativeMemory* MemorySystem::getAssociativeMemory() {
    return associativeMemory.get();
}

const NeuralAssociativeMemory* MemorySystem::getAssociativeMemory() const {
    return associativeMemory.get();
}

void MemorySystem::updateMemory(float timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void MemorySystem::storeSensoryExperience(const class SensoryInput& input, const class Action& action, float reward) {
    // Store the sensory experience in associative memory
    if (brain && associativeMemory) {
        // Convert input to neural pattern
        std::vector<NeuronId> sensoryNeurons;
        for (size_t i = 0; i < input.getData().size(); ++i) {
            sensoryNeurons.push_back(static_cast<NeuronId>(i));
        }
        
        // Store in associative memory with action and reward
        associativeMemory->storePattern(sensoryNeurons, action, reward);
    }
}

void MemorySystem::updateEpisodicMemory(SimulationStep currentStep, float reward, const class Brain& brain) {
    if (episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = reward;
        
        // Store active neurons from brain
        for (const auto& region : brain.getRegions()) {
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
        
        episodicMemory->storeEpisode(episode);
    }
}

void MemorySystem::consolidateMemories(float consolidationThreshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(consolidationThreshold);
    }
    pImpl->stepsSinceLastConsolidation = 0;
}

void MemorySystem::replayMemories(size_t episodeCount) {
    if (episodicMemory) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(episodeCount);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
    pImpl->stepsSinceLastReplay = 0;
}

void MemorySystem::clearAllMemories() {
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
}

size_t MemorySystem::getWorkingMemoryTraceCount() const {
    return workingMemory ? workingMemory->getActiveTraces() : 0;
}

size_t MemorySystem::getEpisodicMemoryEpisodeCount() const {
    return episodicMemory ? episodicMemory->getEpisodeCount() : 0;
}

float MemorySystem::getMemoryUtilization() const {
    size_t totalCapacity = pImpl->workingMemoryCapacity + pImpl->maxEpisodes;
    size_t currentUsage = getWorkingMemoryTraceCount() + getEpisodicMemoryEpisodeCount();
    return totalCapacity > 0 ? static_cast<float>(currentUsage) / totalCapacity : 0.0f;
}

void MemorySystem::setWorkingMemoryCapacity(size_t capacity) {
    pImpl->workingMemoryCapacity = capacity;
    if (workingMemory) {
        workingMemory->setCapacity(capacity);
    }
}

void MemorySystem::setMaxEpisodes(size_t maxEpisodes) {
    pImpl->maxEpisodes = maxEpisodes;
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

void MemorySystem::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = threshold;
}

void MemorySystem::setReplayInterval(size_t interval) {
    pImpl->replayInterval = interval;
}

} // namespace nlm