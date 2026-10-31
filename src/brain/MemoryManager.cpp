// MemoryManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemoryManager::MemoryManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config) {
    // Initialize memory systems
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
}

bool MemoryManager::initialize() {
    NLM_LOG_INFO("Initializing MemoryManager...");
    
    // Initialize memory systems with brain reference
    workingMemory_->initialize(brain_.get());
    workingMemory_->setCapacity(config_.workingMemoryCapacity);
    
    episodicMemory_->initialize(brain_.get());
    episodicMemory_->setMaxEpisodes(config_.maxEpisodes);
    
    associativeMemory_->initialize(brain_.get());
    
    NLM_LOG_INFO("MemoryManager initialized successfully");
    return true;
}

void MemoryManager::reset() {
    NLM_LOG_INFO("Resetting MemoryManager...");
    workingMemory_->clear();
    episodicMemory_->clear();
    associativeMemory_->clear();
    NLM_LOG_INFO("MemoryManager reset complete");
}

void MemoryManager::update() {
    // Memory updates are handled during simulation steps
}

void MemoryManager::logStatus() const {
    NLM_LOG_INFO("=== MemoryManager Status ===");
    if (workingMemory_) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory_->getActiveTraces()));
    }
    if (episodicMemory_) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory_->getEpisodeCount()));
    }
    if (associativeMemory_) {
        // Note: AssociativeMemory may not have getSize() method, using placeholder
        NLM_LOG_INFO("Associative memory active: " + std::to_string(associativeMemory_ != nullptr));
    }
}

void MemoryManager::storeExperience(const ExperienceItem& experience) {
    if (associativeMemory_) {
        associativeMemory_->storePattern(experience.pattern, experience.association);
    }
}

void MemoryManager::updateWorkingMemory(SimulationStep step, Timestamp time) {
    if (workingMemory_) {
        workingMemory_->update(time);
    }
}

void MemoryManager::updateEpisodicMemory(SimulationStep step, Timestamp time) {
    stepsSinceLastEpisode_++;
    if (stepsSinceLastEpisode_ >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode_ = 0;
        
        if (episodicMemory_) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = (brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f);
            
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
            episode.reward = (brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f);
            
            episodicMemory_->storeEpisode(episode);
        }
    }
}

void MemoryManager::updateAssociativeMemory(SimulationStep step, Timestamp time) {
    if (associativeMemory_) {
        associativeMemory_->update(time);
    }
}

void MemoryManager::replayMemory(SimulationStep step, Timestamp time) {
    if (episodicMemory_) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory_->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory_->replayEpisode(episode);
        }
    }
}

void MemoryManager::consolidateMemory(SimulationStep step, Timestamp time) {
    if (episodicMemory_ && step % config_.consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory_->consolidate(0.3f);
    }
}

// Static member initialization for MemoryManager
SimulationStep MemoryManager::stepsSinceLastEpisode_ = 0;

} // namespace nlm