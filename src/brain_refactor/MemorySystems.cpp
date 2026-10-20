#include "MemorySystems.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct MemorySystems::Impl {
    Brain* brain;
    
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Memory coordination
    std::vector<EpisodicMemoryItem> episodesToReplay;
    size_t consolidationCount;
    
    // Configuration
    size_t workingMemoryCapacity;
    float workingMemoryDecayRate;
    size_t episodicMemoryMaxEpisodes;
    size_t replayInterval;
    size_t consolidationInterval;
    
    bool initialized;
};

MemorySystems::MemorySystems()
    : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
    pImpl->consolidationCount = 0;
    pImpl->workingMemoryCapacity = 100;
    pImpl->workingMemoryDecayRate = 0.01f;
    pImpl->episodicMemoryMaxEpisodes = 1000;
    pImpl->replayInterval = 100;
    pImpl->consolidationInterval = 1000;
}

MemorySystems::~MemorySystems() = default;

bool MemorySystems::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Create memory system components
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(brain);
        pImpl->workingMemory->setCapacity(pImpl->workingMemoryCapacity);
        pImpl->workingMemory->setDecayRate(pImpl->workingMemoryDecayRate);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(brain);
        pImpl->episodicMemory->setMaxEpisodes(pImpl->episodicMemoryMaxEpisodes);
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(brain);
    }
    
    pImpl->initialized = true;
    
    return true;
}

void MemorySystems::update(float dt) {
    if (!pImpl->initialized || !pImpl->brain) {
        return;
    }
    
    // Update working memory (maintenance and competition)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(dt);
    }
    
    // Update episodic memory (automatic storage during rest)
    static SimulationStep lastEpisodeStep = 0;
    SimulationStep currentStep = pImpl->brain->getCurrentStep();
    
    if (currentStep - lastEpisodeStep >= pImpl->replayInterval) {
        lastEpisodeStep = currentStep;
        
        // Create and store a new episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        
        // Capture current brain state as an episode
        // Store active neurons and their activations
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& population : region->getPopulations()) {
                for (auto* neuron : population->getNeurons()) {
                    const auto& state = neuron->getState();
                    if (neuron->isFiring() || 
                        std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store episode in episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->storeEpisode(episode);
        }
        
        // Also store in working memory
        if (pImpl->workingMemory) {
            for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
                pImpl->workingMemory->storeToNeuron(
                    episode.activeNeurons[i], 
                    episode.neuronActivations[i]);
            }
        }
    }
    
    // Update associative memory
    if (pImpl->associativeMemory) {
        // In a real implementation, this would learn associations
        // from current neural activity patterns
    }
    
    // Periodically replay important memories
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        pImpl->episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : pImpl->episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // Periodically consolidate memories
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
        pImpl->consolidationCount++;
    }
    
    // Synchronize memory systems
    synchronizeMemorySystems();
}

void MemorySystems::reset() {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

void MemorySystems::logStatus() const {
    NLM_LOG_INFO("=== MemorySystems Status ===");
    NLM_LOG_INFO("Initialized: " + std::string(pImpl->initialized ? "Yes" : "No"));
    
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    if (pImpl->associativeMemory) {
        NLM_LOG_INFO("Associative memory patterns: " + std::to_string(pImpl->associativeMemory->getPatternCount()));
    }
}

NeuralWorkingMemory* MemorySystems::getWorkingMemory() const {
    return pImpl->workingMemory.get();
}

void MemorySystems::storeToWorkingMemory(NeuronId neuron, float activation) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuron, activation);
    }
}

std::vector<float> MemorySystems::retrieveFromWorkingMemory() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->retrieve();
    }
    return std::vector<float>();
}

bool MemorySystems::containsInWorkingMemory(NeuronId neuron) const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->contains(neuron);
    }
    return false;
}

NeuralEpisodicMemory* MemorySystems::getEpisodicMemory() const {
    return pImpl->episodicMemory.get();
}

void MemorySystems::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

void MemorySystems::replayEpisodes(size_t count) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->replayEpisodes(count);
    }
}

float MemorySystems::getEpisodicMemoryConsolidationStrength() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getConsolidationStrength();
    }
    return 0.0f;
}

NeuralAssociativeMemory* MemorySystems::getAssociativeMemory() const {
    return pImpl->associativeMemory.get();
}

void MemorySystems::storeAssociation(const std::vector<float>& pattern1, 
                                    const std::vector<float>& pattern2, 
                                    float strength) {
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->storeAssociation(pattern1, pattern2, strength);
    }
}

std::vector<float> MemorySystems::retrieveAssociation(const std::vector<float>& pattern) const {
    if (pImpl->associativeMemory) {
        return pImpl->associativeMemory->retrieve(pattern);
    }
    return std::vector<float>();
}

void MemorySystems::synchronizeMemorySystems() {
    // Coordinate between memory systems
    // Example: Update working memory based on episodic memory
    if (pImpl->workingMemory && pImpl->episodicMemory) {
        // Rehearse important memories
        size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
        if (episodeCount > 0) {
            // Get the most recent episode
            const EpisodicMemoryItem* latestEpisode = pImpl->episodicMemory->getLatestEpisode();
            if (latestEpisode) {
                // Rehearse in working memory
                for (size_t i = 0; i < latestEpisode->activeNeurons.size(); ++i) {
                    pImpl->workingMemory->storeToNeuron(
                        latestEpisode->activeNeurons[i], 
                        latestEpisode->neuronActivations[i]);
                }
            }
        }
    }
}

void MemorySystems::updateReplaySchedule() {
    // Update when memories should be replayed
    // This could be influenced by novelty detection, reward prediction errors, etc.
}

void MemorySystems::applyConsolidation() {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void MemorySystems::setWorkingMemoryCapacity(size_t capacity) {
    pImpl->workingMemoryCapacity = capacity;
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(capacity);
    }
}

void MemorySystems::setWorkingMemoryDecayRate(float rate) {
    pImpl->workingMemoryDecayRate = rate;
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setDecayRate(rate);
    }
}

void MemorySystems::setEpisodicMemoryMaxEpisodes(size_t maxEpisodes) {
    pImpl->episodicMemoryMaxEpisodes = maxEpisodes;
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

void MemorySystems::setReplayInterval(size_t interval) {
    pImpl->replayInterval = interval;
}

void MemorySystems::setConsolidationInterval(size_t interval) {
    pImpl->consolidationInterval = interval;
}

size_t MemorySystems::getWorkingMemoryCapacity() const {
    return pImpl->workingMemoryCapacity;
}

size_t MemorySystems::getWorkingMemoryActiveTraces() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->getActiveTraces();
    }
    return 0;
}

size_t MemorySystems::getEpisodicMemoryEpisodeCount() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

size_t MemorySystems::getEpisodicMemoryReplayCount() const {
    return pImpl->episodesToReplay.size();
}

size_t MemorySystems::getConsolidationCount() const {
    return pImpl->consolidationCount;
}

void MemorySystems::clearAllMemory() {
    reset();
}

} // namespace nlm
