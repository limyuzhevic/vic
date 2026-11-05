#include "BrainMemory.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void BrainMemory::initializeAllMemorySystems(Brain* brain) {
    if (!brain) return;
    
    // Initialize working memory
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    workingMemory_->initialize(brain);
    workingMemory_->setCapacity(workingMemoryCapacity_);
    
    // Initialize episodic memory
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory_->initialize(brain);
    episodicMemory_->setMaxEpisodes(episodicMemoryMaxEpisodes_);
    
    // Initialize associative memory
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory_->initialize(brain);
    
    NLM_LOG_INFO("All memory systems initialized:");
    NLM_LOG_INFO("  Working memory capacity: " + std::to_string(workingMemoryCapacity_));
    NLM_LOG_INFO("  Episodic memory max episodes: " + std::to_string(episodicMemoryMaxEpisodes_));
}

void BrainMemory::configureMemoryFromConfig(const Config* config) {
    if (!config) return;
    
    workingMemoryCapacity_ = static_cast<size_t>(config->getOr<int64_t>("working_memory_capacity", 100));
    episodicMemoryMaxEpisodes_ = static_cast<size_t>(config->getOr<int64_t>("episodic_memory_max_episodes", 1000));
    memoryReplayEnabled_ = config->getOr<bool>("memory_replay_enabled", false);
    replayInterval_ = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
    
    NLM_LOG_INFO("Memory systems configured from config:");
    NLM_LOG_INFO("  Working memory capacity: " + std::to_string(workingMemoryCapacity_));
    NLM_LOG_INFO("  Episodic memory max episodes: " + std::to_string(episodicMemoryMaxEpisodes_));
    NLM_LOG_INFO("  Memory replay enabled: " + std::string(memoryReplayEnabled_ ? "yes" : "no"));
    NLM_LOG_INFO("  Replay interval: " + std::to_string(replayInterval_));
}

void BrainMemory::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (!brain || !workingMemory_) return;
    
    workingMemory_->update(timestep);
}

void BrainMemory::updateEpisodicMemory(Brain* brain, SimulationStep currentStep) {
    if (!brain || !episodicMemory_) return;
    
    // Store episode periodically
    if (memoryReplayEnabled_ && currentStep > 0 && currentStep % replayInterval_ == 0) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        
        // Store active neurons
        for (auto& region : brain->getRegions()) {
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
        auto* dopamine = brain->getDopamine();
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        // Store position if available
        if (auto* agent = brain->getAgentBody()) {
            episode.positionX = agent->x;
            episode.positionY = agent->y;
            episode.orientation = agent->orientation;
        }
        
        // Store in episodic memory
        episodicMemory_->storeEpisode(episode);
        
        // Trigger replay if enabled
        if (memoryReplayEnabled_ && currentStep % (replayInterval_ * 2) == 0) {
            replayRecentMemories(brain);
        }
    }
}

void BrainMemory::updateAssociativeMemory(Brain* brain, TimestepDuration timestep) {
    if (!brain || !associativeMemory_) return;
    
    associativeMemory_->update(brain, timestep);
}

void BrainMemory::updateAllMemorySystems(Brain* brain, SimulationStep currentStep, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update all memory systems
    updateWorkingMemory(brain, timestep);
    updateEpisodicMemory(brain, currentStep);
    updateAssociativeMemory(brain, timestep);
    
    // Process memory consolidation periodically
    if (currentStep > 0 && currentStep % 1000 == 0) {
        processMemoryConsolidation(brain, 0.3f);
    }
}

void BrainMemory::storeInWorkingMemory(Brain* brain, NeuronId neuron, float activation) {
    if (!brain || !workingMemory_) return;
    
    workingMemory_->storeToNeuron(neuron, activation);
}

void BrainMemory::storeEpisode(Brain* brain, const EpisodicMemoryItem& episode) {
    if (!brain || !episodicMemory_) return;
    
    episodicMemory_->storeEpisode(episode);
}

void BrainMemory::consolidateMemories(Brain* brain, float threshold) {
    if (!brain || !episodicMemory_) return;
    
    episodicMemory_->consolidate(threshold);
}

void BrainMemory::collectMemoryStats(Brain* brain) {
    if (!brain) return;
    
    workingMemorySize_ = workingMemory_ ? workingMemory_->getActiveTraces() : 0;
    episodicMemorySize_ = episodicMemory_ ? episodicMemory_->getEpisodeCount() : 0;
    associativeMemorySize_ = associativeMemory_ ? 0 : 0; // Placeholder - actual count would depend on implementation
    
    memoryEfficiency_ = 0.0f;
    if (workingMemorySize_ + episodicMemorySize_ + associativeMemorySize_ > 0) {
        memoryEfficiency_ = static_cast<float>(workingMemorySize_) / 
                           static_cast<float>(workingMemorySize_ + episodicMemorySize_ + associativeMemorySize_);
    }
}

void BrainMemory::clearAllMemories(Brain* brain) {
    if (!brain) return;
    
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    
    collectMemoryStats(brain);
}

void BrainMemory::backupMemories(Brain* brain) {
    if (!brain) return;
    
    // Create backup of current memory state
    // This is a simplified backup - actual implementation would save to file
    NLM_LOG_INFO("Memory backup created (simplified implementation)");
}

void BrainMemory::restoreMemories(Brain* brain) {
    if (!brain) return;
    
    // Restore memory state from backup
    // This is a simplified restore - actual implementation would load from file
    NLM_LOG_INFO("Memory state restored (simplified implementation)");
}

void BrainMemory::setWorkingMemoryCapacity(Brain* brain, size_t capacity) {
    workingMemoryCapacity_ = capacity;
    if (workingMemory_) {
        workingMemory_->setCapacity(capacity);
    }
}

void BrainMemory::setEpisodicMemoryMaxEpisodes(Brain* brain, size_t maxEpisodes) {
    episodicMemoryMaxEpisodes_ = maxEpisodes;
    if (episodicMemory_) {
        episodicMemory_->setMaxEpisodes(maxEpisodes);
    }
}

void BrainMemory::enableMemoryReplay(Brain* brain, bool enable) {
    memoryReplayEnabled_ = enable;
}

void BrainMemory::setReplayInterval(Brain* brain, size_t interval) {
    replayInterval_ = interval;
}

std::vector<NeuronId> BrainMemory::getActiveMemoryNeurons(Brain* brain) const {
    if (!brain || !workingMemory_) return {};
    
    return workingMemory_->getMemoryNeurons();
}

std::vector<EpisodicMemoryItem> BrainMemory::getRecentEpisodes(Brain* brain, size_t count) const {
    if (!brain || !episodicMemory_) return {};
    
    return episodicMemory_->getEpisodes(); // Would need to modify EpisodicMemory for this
}

float BrainMemory::getMemoryEfficiency(Brain* brain) const {
    return memoryEfficiency_;
}

void BrainMemory::replayRecentMemories(Brain* brain) {
    if (!brain || !episodicMemory_) return;
    
    // Get recent episodes for replay
    auto episodes = episodicMemory_->getEpisodes();
    if (episodes.empty()) return;
    
    // Replay most recent episodes
    size_t replayCount = std::min(size_t(3), episodes.size());
    for (size_t i = 0; i < replayCount; ++i) {
        const auto* episode = &episodes[episodes.size() - 1 - i]; // Most recent first
        episodicMemory_->replayEpisode(episode);
    }
}

void BrainMemory::setupMemoryConnections(Brain* brain) {
    if (!brain) return;
    
    // Establish connections between memory systems
    if (workingMemory_ && episodicMemory_) {
        workingMemory_->setEpisodicMemoryReference(episodicMemory_.get());
    }
    
    if (episodicMemory_ && associativeMemory_) {
        episodicMemory_->setAssociativeMemoryReference(associativeMemory_.get());
    }
}

void BrainMemory::validateMemoryState(Brain* brain) const {
    if (!brain) return;
    
    // Validate working memory state
    if (workingMemory_) {
        size_t capacity = workingMemory_->getCapacity();
        size_t activeTraces = workingMemory_->getActiveTraces();
        if (activeTraces > capacity) {
            NLM_LOG_WARNING("Working memory exceeds capacity: " + std::to_string(activeTraces) + "/" + std::to_string(capacity));
        }
    }
    
    // Validate episodic memory state
    if (episodicMemory_) {
        size_t maxEpisodes = episodicMemory_->getMaxEpisodes();
        size_t episodeCount = episodicMemory_->getEpisodeCount();
        if (episodeCount > maxEpisodes) {
            NLM_LOG_WARNING("Episodic memory exceeds capacity: " + std::to_string(episodeCount) + "/" + std::to_string(maxEpisodes));
        }
    }
}

void BrainMemory::optimizeMemoryLayout(Brain* brain) {
    if (!brain) return;
    
    // Optimize memory layout for better access patterns
    if (workingMemory_) {
        workingMemory_->optimizeTraceDistribution();
    }
}

void BrainMemory::processMemoryConsolidation(Brain* brain, float threshold) {
    if (!brain || !episodicMemory_) return;
    
    episodicMemory_->consolidate(threshold);
}

} // namespace nlm