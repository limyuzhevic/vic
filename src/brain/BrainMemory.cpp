// NLM Brain Memory System Implementation
// Phase 6: Advanced Memory Integration

#include "BrainMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

void BrainMemory::initializeMemory() {
    NLM_LOG_INFO("Initializing brain memory systems...");
    
    // Initialize working memory - stores recent sensory inputs and actions
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    workingMemory_->setCapacity(100); // Store last 100 time steps
    workingMemory_->setDecayRate(0.95f);
    
    // Initialize episodic memory - stores complete experiences
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory_->setMaxEpisodes(1000);
    episodicMemory_->setConsolidationInterval(100); // Consolidate every 100 steps
    
    // Initialize associative memory - stores pattern relationships
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory_->setLearningRate(0.1f);
    associativeMemory_->setRecallThreshold(0.8f);
    
    NLM_LOG_INFO("Memory systems initialized successfully.");
}

void BrainMemory::updateMemory() {
    // Update working memory with decay
    if (workingMemory_) {
        workingMemory_->decay();
    }
    
    // Consolidate episodic memories periodically
    if (episodicMemory_) {
        episodicMemory_->consolidate();
    }
    
    // Update associative memory
    if (associativeMemory_) {
        associativeMemory_->update();
    }
    
    NLM_LOG_DEBUG("Memory systems updated.");
}

}} // namespace nlm