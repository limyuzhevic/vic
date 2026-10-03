// MemorySystem.cpp - Implementation of the memory management system

#include "MemorySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

MemorySystem::MemorySystem()
    : lastConsolidationTime_(0.0)
    , lastReplayTime_(0.0)
    , workingMemoryCapacity_(0)
    , maxEpisodicMemorySize_(0)
    , consolidationThreshold_(0.3f)
    , replayFrequency_(0.1f) {
}

void MemorySystem::initialize(BrainImpl* brain) {
    brain_ = brain;
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize MemorySystem with null brain");
        return;
    }
    
    // Initialize memory subsystems
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize memory subsystems with brain reference
    workingMemory_->initialize(brain_);
    episodicMemory_->initialize(brain_);
    associativeMemory_->initialize(brain_);
    
    NLM_LOG_INFO("Memory system initialized successfully");
}

void MemorySystem::update(double timestep) {
    if (!brain_) return;
    
    maintainWorkingMemory();
    updateEpisodicMemory(timestep);
    compressMemoryIfNeeded();
}

void MemorySystem::storeToNeuron(NeuronId neuronId, float activation) {
    if (workingMemory_) {
        workingMemory_->storeToNeuron(neuronId, activation);
    }
    
    // Track active neurons for memory consolidation
    activeNeurons_.push_back(neuronId);
    activations_.push_back(activation);
    
    // Limit tracking size
    if (activeNeurons_.size() > 1000) {
        activeNeurons_.erase(activeNeurons_.begin());
        activations_.erase(activations_.begin());
    }
}

void MemorySystem::consolidate(double currentTime) {
    if (!episodicMemory_ || currentTime - lastConsolidationTime_ < 10.0) {
        return; // Skip consolidation if too recent
    }
    
    lastConsolidationTime_ = currentTime;
    episodicMemory_->consolidate(consolidationThreshold_);
    
    NLM_LOG_INFO("Memory consolidation completed, threshold: " + 
                std::to_string(consolidationThreshold_));
}

void MemorySystem::replayMemories(double currentTime, size_t episodeCount) {
    if (!episodicMemory_ || currentTime - lastReplayTime_ < 100.0) {
        return; // Skip replay if too recent
    }
    
    lastReplayTime_ = currentTime;
    auto episodesToReplay = episodicMemory_->getEpisodesForReplay(episodeCount);
    
    for (const auto* episode : episodesToReplay) {
        episodicMemory_->replayEpisode(episode);
    }
    
    NLM_LOG_INFO("Memory replay completed for " + std::to_string(episodesToReplay.size()) + " episodes");
}

void MemorySystem::clear() {
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    
    activeNeurons_.clear();
    activations_.clear();
    
    NLM_LOG_INFO("Memory system cleared");
}

void MemorySystem::setCapacity(size_t capacity) {
    workingMemoryCapacity_ = capacity;
    if (workingMemory_) {
        workingMemory_->setCapacity(capacity);
    }
}

void MemorySystem::setMaxEpisodes(size_t maxEpisodes) {
    maxEpisodicMemorySize_ = maxEpisodes;
    if (episodicMemory_) {
        episodicMemory_->setMaxEpisodes(maxEpisodes);
    }
}

void MemorySystem::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodicMemory_) {
        episodicMemory_->storeEpisode(episode);
    }
}

std::vector<const EpisodicMemoryItem*> MemorySystem::getEpisodesForReplay(size_t count) const {
    if (episodicMemory_) {
        return episodicMemory_->getEpisodesForReplay(count);
    }
    return std::vector<const EpisodicMemoryItem*>();
}

float MemorySystem::getMemoryEfficiency() const {
    if (activeNeurons_.empty()) return 0.0f;
    
    float totalActivation = std::accumulate(activations_.begin(), activations_.end(), 0.0f);
    float maxPossibleActivation = static_cast<float>(activations_.size()) * 1.0f; // Assuming max activation of 1.0
    
    return totalActivation / maxPossibleActivation;
}

size_t MemorySystem::getTotalMemoryUsage() const {
    size_t total = 0;
    if (workingMemory_) total += workingMemory_->getActiveTraces() * sizeof(NeuronId);
    if (episodicMemory_) total += episodicMemory_->getEpisodeCount() * sizeof(EpisodicMemoryItem);
    if (associativeMemory_) total += associativeMemory_->getPatternCount() * sizeof(std::vector<NeuronId>);
    
    return total;
}

void MemorySystem::configure(size_t workingMemoryCapacity, size_t maxEpisodicMemorySize) {
    setCapacity(workingMemoryCapacity);
    setMaxEpisodes(maxEpisodicMemorySize);
    
    // Set reasonable defaults
    consolidationThreshold_ = 0.3f;
    replayFrequency_ = 0.1f;
}

void MemorySystem::maintainWorkingMemory() {
    if (!workingMemory_) return;
    
    // Update working memory with current activity
    workingMemory_->update(0.001); // Default timestep
}

void MemorySystem::updateEpisodicMemory(double currentTime) {
    if (!episodicMemory_) return;
    
    // Check if it's time to store a new episode
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10) { // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Create episodic memory item from current brain state
        EpisodicMemoryItem episode;
        episode.timestamp = static_cast<uint64_t>(currentTime * 1000.0); // Convert to milliseconds
        episode.reward = 0.0f; // Would be set by neuromodulation
        
        // Store active neurons
        if (brain_) {
            for (const auto& region : brain_->regions) {
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
        }
        
        storeEpisode(episode);
    }
}

void MemorySystem::applyPatternCompletion(const std::vector<NeuronId>& patterns) {
    if (associativeMemory_) {
        associativeMemory_->completePattern(patterns);
    }
}

void MemorySystem::compressMemoryIfNeeded() {
    // Simple memory compression based on efficiency
    float efficiency = getMemoryEfficiency();
    
    if (efficiency < 0.1f && episodicMemory_) {
        // If efficiency is low, consolidate and remove weak episodes
        episodicMemory_->consolidate(0.5f);
    }
}

} // namespace nlm
