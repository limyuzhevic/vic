// MemorySystem implementation - Handles working memory, episodic memory, and attention

#include "MemorySystem.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {

MemorySystem::MemorySystem(std::shared_ptr<Brain> brain)
    : brain_(brain), stepsSinceLastEpisode_(0), replayInterval_(100), consolidationInterval_(1000) {
}

MemorySystem::~MemorySystem() = default;

bool MemorySystem::initialize() {
    // Initialize memory systems with brain reference
    if (brain_) {
        workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        workingMemory_->initialize(brain_.get());
        
        episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory_->initialize(brain_.get());
        
        associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
        associativeMemory_->initialize(brain_.get());
    }
    
    return true;
}

void MemorySystem::step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    // Update working memory
    updateWorkingMemory(timestep);
    
    // Update episodic memory
    updateEpisodicMemory(currentStep, currentTime);
    
    // Update attention system
    updateAttentionSystem();
    
    // Update concept formation
    updateConceptFormation();
    
    // Update prediction system
    updatePredictionSystem();
    
    // Update memory replay
    updateMemoryReplay(currentStep);
    
    // Consolidate memories periodically
    consolidateMemories(currentStep);
}

void MemorySystem::updateWorkingMemory(TimestepDuration timestep) {
    if (workingMemory_) {
        workingMemory_->update(timestep);
    }
}

void MemorySystem::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    // Store episode every 10 steps
    stepsSinceLastEpisode_++;
    if (stepsSinceLastEpisode_ >= 10) {
        stepsSinceLastEpisode_ = 0;
        
        if (episodicMemory_) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            
            // Store active neurons
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            if (brain_->getDopamine()) {
                episode.reward = brain_->getDopamine()->getLevel();
            }
            
            episodicMemory_->storeEpisode(episode);
        }
    }
}

void MemorySystem::updateAttentionSystem() {
    if (attention_) {
        attention_->update(0.001f);  // Use default timestep
        
        // Apply attention to working memory winners
        if (workingMemory_ && !workingMemory_->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory_->getMemoryNeurons();
            attention_->processCompetition(competitors);
        }
    }
}

void MemorySystem::updateConceptFormation() {
    if (conceptFormation_) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding - can be implemented later
    }
}

void MemorySystem::updatePredictionSystem() {
    if (brain_->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void MemorySystem::updateMemoryReplay(SimulationStep currentStep) {
    if (episodicMemory_ && currentStep % replayInterval_ == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory_->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory_->replayEpisode(episode);
        }
    }
}

void MemorySystem::consolidateMemories(SimulationStep currentStep) {
    if (episodicMemory_ && currentStep % consolidationInterval_ == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory_->consolidate(0.3f);
    }
}

void MemorySystem::updateAttention() {
    if (attention_) {
        attention_->update(0.001f);
    }
}

void MemorySystem::updateConceptFormation() {
    if (conceptFormation_) {
        // Would process current neural activity patterns to form concepts
    }
}

void MemorySystem::updatePredictionSystem() {
    if (brain_->getPredictionSystem()) {
        // Update prediction system with current state
    }
}

} // namespace nlm
