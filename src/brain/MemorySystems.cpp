// Memory systems implementation for Brain.cpp
// Contains: Working memory, episodic memory, associative memory integration

#pragma once

#include "MemorySystems.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void MemorySystems::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    config_ = config;
    brain_ = brain;
    
    // Initialize working memory
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    workingMemory_->initialize(brain.get());
    
    // Initialize episodic memory
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory_->initialize(brain.get());
    
    // Initialize associative memory
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory_->initialize(brain.get());
    
    // Configure memory parameters
    workingMemoryCapacity_ = config->getOr<size_t>("working_memory_capacity", 100);
    episodicMemoryMaxEpisodes_ = config->getOr<size_t>("episodic_memory_max_episodes", 1000);
    
    workingMemory_->setCapacity(workingMemoryCapacity_);
    episodicMemory_->setMaxEpisodes(episodicMemoryMaxEpisodes_);
    
    NLM_LOG_INFO("MemorySystems initialized");
}

void MemorySystems::processStep(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_) return;
    
    // Update working memory
    if (workingMemory_) {
        workingMemory_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Store episodic memories periodically
    if (currentStep % episodeStorageInterval_ == 0) {
        storeEpisodicMemory(currentStep, currentTime);
    }
    
    // Update associative memory
    if (associativeMemory_) {
        associativeMemory_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}

void MemorySystems::storeEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    if (!episodicMemory_ || !brain_) return;
    
    // Create episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = getCurrentReward();
    
    // Store active neurons and their activations
    for (auto& region : brain_->getRegions()) {
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
    
    episodicMemory_->storeEpisode(episode);
}

void MemorySystems::replayMemories() {
    if (!episodicMemory_ || !brain_) return;
    
    // Get episodes for replay
    auto episodesToReplay = episodicMemory_->getEpisodesForReplay(memoryReplayCount_);
    
    for (const auto* episode : episodesToReplay) {
        episodicMemory_->replayEpisode(episode);
    }
}

void MemorySystems::consolidateMemories(float threshold) {
    if (!episodicMemory_) return;
    
    episodicMemory_->consolidate(threshold);
}

float MemorySystems::getCurrentReward() const {
    if (!brain_ || !brain_->getDopamine()) return 0.0f;
    return brain_->getDopamine()->getLevel();
}

NeuralWorkingMemory* MemorySystems::getWorkingMemory() {
    return workingMemory_.get();
}

NeuralEpisodicMemory* MemorySystems::getEpisodicMemory() {
    return episodicMemory_.get();
}

NeuralAssociativeMemory* MemorySystems::getAssociativeMemory() {
    return associativeMemory_.get();
}

} // namespace nlm
