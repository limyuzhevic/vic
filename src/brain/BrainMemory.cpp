// BrainMemory.cpp - Memory system integration implementation
#include "BrainMemory.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainMemory::BrainMemory() : brain_(nullptr), workingMemory_(nullptr), 
                            episodicMemory_(nullptr), associativeMemory_(nullptr) {}

BrainMemory::~BrainMemory() = default;

void BrainMemory::initializeMemorySystems(Brain* brain) {
    brain_ = brain;
    
    if (!brain_) {
        NLM_LOG_ERROR("BrainMemory: Cannot initialize - no brain reference!");
        return;
    }
    
    // Get pointers to memory systems from brain
    workingMemory_ = brain_->getWorkingMemory();
    episodicMemory_ = brain_->getEpisodicMemory();
    associativeMemory_ = brain_->getAssociativeMemory();
    
    if (workingMemory_) {
        NLM_LOG_INFO("BrainMemory: Working memory system initialized");
    }
    if (episodicMemory_) {
        NLM_LOG_INFO("BrainMemory: Episodic memory system initialized");
    }
    if (associativeMemory_) {
        NLM_LOG_INFO("BrainMemory: Associative memory system initialized");
    }
    
    NLM_LOG_INFO("BrainMemory: All memory systems connected to brain");
}

void BrainMemory::updateWorkingMemory(double timestep) {
    if (!workingMemory_) {
        NLM_LOG_WARNING("BrainMemory: Working memory not available");
        return;
    }
    
    // Update working memory with current neural activity
    workingMemory_->update(timestep);
    
    size_t activeTraces = workingMemory_->getActiveTraces();
    if (activeTraces > 0) {
        NLM_LOG_DEBUG("BrainMemory: Working memory updated with " + 
                   std::to_string(activeTraces) + " active traces");
    }
}

void BrainMemory::storeEpisode(EpisodicMemoryItem& episode, SimulationStep step, Timestamp time) {
    if (!episodicMemory_) {
        NLM_LOG_WARNING("BrainMemory: Episodic memory not available");
        return;
    }
    
    // Set episode metadata
    episode.timestamp = step;
    
    // Store the episode
    episodicMemory_->storeEpisode(episode);
    
    NLM_LOG_DEBUG("BrainMemory: Episode stored with " + 
               std::to_string(episode.activeNeurons.size()) + " active neurons");
}

void BrainMemory::consolidateMemory(float threshold) {
    if (!episodicMemory_) {
        NLM_LOG_WARNING("BrainMemory: Episodic memory not available for consolidation");
        return;
    }
    
    // Consolidate important memories, remove weak ones
    episodicMemory_->consolidate(threshold);
    
    NLM_LOG_INFO("BrainMemory: Memory consolidation completed with threshold " + 
               std::to_string(threshold));
}

size_t BrainMemory::getWorkingMemoryActiveTraces() const {
    if (!workingMemory_) return 0;
    return workingMemory_->getActiveTraces();
}

size_t BrainMemory::getEpisodicMemoryEpisodeCount() const {
    if (!episodicMemory_) return 0;
    return episodicMemory_->getEpisodeCount();
}

NeuralWorkingMemory* BrainMemory::getWorkingMemory() {
    return workingMemory_;
}

NeuralEpisodicMemory* BrainMemory::getEpisodicMemory() {
    return episodicMemory_;
}

NeuralAssociativeMemory* BrainMemory::getAssociativeMemory() {
    return associativeMemory_;
}

} // namespace nlm