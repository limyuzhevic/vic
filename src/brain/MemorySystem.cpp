#include "MemorySystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include <algorithm>

namespace nlm {

struct MemorySystem::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Memory management
    size_t replayInterval = 100;      // Replay every 100 steps
    size_t consolidationInterval = 1000;  // Consolidate every 1000 steps
    SimulationStep lastReplayStep = 0;
    SimulationStep lastConsolidationStep = 0;
    
    // Current episode buffer (for replay)
    std::vector<const EpisodicMemoryItem*> episodesForReplay;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        episodesForReplay.reserve(10);
    }
};

MemorySystem::MemorySystem(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

MemorySystem::~MemorySystem() = default;

MemorySystem::MemorySystem(MemorySystem&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

MemorySystem& MemorySystem::operator=(MemorySystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool MemorySystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("MemorySystem: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize memory components
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Configure memory capacities from config
    if (auto workingMemoryCap = config_->getOr<size_t>("working_memory_capacity", 100)) {
        pImpl->workingMemory->setCapacity(*workingMemoryCap);
    }
    if (auto episodicMemoryCap = config_->getOr<size_t>("episodic_memory_capacity", 1000)) {
        pImpl->episodicMemory->setMaxEpisodes(*episodicMemoryCap);
    }
    if (auto replayInterval = config_->getOr<size_t>("replay_interval", 100)) {
        pImpl->replayInterval = *replayInterval;
    }
    if (auto consolidationInterval = config_->getOr<size_t>("consolidation_interval", 1000)) {
        pImpl->consolidationInterval = *consolidationInterval;
    }
    
    // Initialize memory components
    pImpl->workingMemory->initialize(brain);
    pImpl->episodicMemory->initialize(brain);
    pImpl->associativeMemory->initialize(brain);
    
    NLM_LOG_INFO("MemorySystem initialized successfully");
    return true;
}

void MemorySystem::update(TimestepDuration dt) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(dt);
    }
    
    // Update episodic memory (store periodic episodes)
    static SimulationStep episodeCounter = 0;
    episodeCounter++;
    if (episodeCounter % 10 == 0 && pImpl->episodicMemory) {
        // Store current brain state as episode
        EpisodicMemoryItem episode;
        episode.timestamp = episodeCounter;
        episode.reward = 0.0f;  // Would come from neuromodulation
        
        // Store active neurons
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) / 20.0f);
                    }
                }
            }
        }
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
    
    // Schedule replay
    pImpl->lastReplayStep = episodeCounter;
    
    // Schedule consolidation
    pImpl->lastConsolidationStep = episodeCounter;
}

void MemorySystem::storeMemory(const std::vector<float>& pattern, float strength) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->store(pattern, strength);
    }
}

std::vector<float> MemorySystem::retrieveMemory() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->retrieve();
    }
    return std::vector<float>();
}

void MemorySystem::clearMemory() {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

void MemorySystem::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

const std::vector<const EpisodicMemoryItem*>& MemorySystem::getEpisodesForReplay(size_t count) const {
    pImpl->episodesForReplay.clear();
    
    if (pImpl->episodicMemory) {
        pImpl->episodesForReplay = pImpl->episodicMemory->getEpisodesForReplay(count);
    }
    
    return pImpl->episodesForReplay;
}

void MemorySystem::replayEpisode(const EpisodicMemoryItem* episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void MemorySystem::consolidate(float threshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(threshold);
    }
}

void MemorySystem::scheduleReplay(SimulationStep step) {
    pImpl->lastReplayStep = step;
}

bool MemorySystem::shouldReplay(SimulationStep step) const {
    return (step - pImpl->lastReplayStep) >= pImpl->replayInterval;
}

size_t MemorySystem::getActiveMemoryTraces() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->getActiveTraces();
    }
    return 0;
}

size_t MemorySystem::getEpisodeCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

float MemorySystem::getMemoryActivity() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->getMemoryActivity();
    }
    return 0.0f;
}

void MemorySystem::setWorkingMemoryCapacity(size_t capacity) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(capacity);
    }
}

void MemorySystem::setEpisodicMemoryCapacity(size_t maxEpisodes) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

void MemorySystem::reset() {
    NLM_LOG_INFO("MemorySystem reset");
    clearMemory();
    pImpl->episodesForReplay.clear();
}

} // namespace nlm