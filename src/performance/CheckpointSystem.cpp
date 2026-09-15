#include "Checkpointsystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct CheckpointManager::Impl {
    std::string checkpointDir_;
    size_t maxCheckpoints_;
    size_t retentionCount_;
    bool compressionEnabled_;
    
    std::vector<std::string> checkpointPaths_;
    size_t currentCheckpointIndex_;
};

CheckpointManager::CheckpointManager()
    : pImpl(std::make_unique<Impl>()) {
    pImpl->maxCheckpoints_ = 10;
    pImpl->retentionCount_ = 5;
    pImpl->compressionEnabled_ = false;
    pImpl->currentCheckpointIndex_ = 0;
}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& dir, size_t maxCheckpoints,
                                 size_t retentionCount, bool compression) {
    pImpl->checkpointDir_ = dir;
    pImpl->maxCheckpoints_ = maxCheckpoints;
    pImpl->retentionCount_ = retentionCount;
    pImpl->compressionEnabled_ = compression;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(dir);
    
    NLM_LOG_INFO("Checkpoint manager configured: " + dir + ", max: " + 
                 std::to_string(maxCheckpoints) + ", retention: " + 
                 std::to_string(retentionCount));
}

void CheckpointManager::update(size_t step, Timestamp time) {
    // Auto-save at specified intervals
    if (step % 1000 == 0) {
        std::string path = pImpl->checkpointDir_ + "/checkpoint_step_" + 
                          std::to_string(step) + ".bin";
        save(path);
    }
    
    // Clean up old checkpoints
    cleanupOldCheckpoints();
}

bool CheckpointManager::save(const std::string& filepath, const Brain* brain) {
    NLM_LOG_INFO("Saving checkpoint to " + filepath);
    
    // Create directory if needed
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot create checkpoint file: " + filepath);
        return false;
    }
    
    try {
        // Save checkpoint metadata
        CheckpointMetadata metadata;
        metadata.version = "1.0";
        metadata.timestamp = std::chrono::system_clock::now();
        metadata.step = brain ? brain->getCurrentStep() : 0;
        metadata.synapseCount = brain ? brain->getTotalSynapseCount() : 0;
        metadata.neuronCount = brain ? brain->getTotalNeuronCount() : 0;
        
        // Write metadata
        file.write(reinterpret_cast<const char*>(&metadata), sizeof(metadata));
        
        if (brain) {
            // Save brain state
            if (!brain->saveState(file)) {
                NLM_LOG_ERROR("Failed to save brain state to checkpoint");
                return false;
            }
            
            // Save memory systems
            if (brain->getWorkingMemory()) {
                if (!brain->getWorkingMemory()->saveState(file)) {
                    NLM_LOG_ERROR("Failed to save working memory");
                    return false;
                }
            }
            
            if (brain->getEpisodicMemory()) {
                if (!brain->getEpisodicMemory()->saveState(file)) {
                    NLM_LOG_ERROR("Failed to save episodic memory");
                    return false;
                }
            }
        }
        
        // Add to checkpoint list
        pImpl->checkpointPaths_.push_back(filepath);
        
        // Trim if too many
        if (pImpl->checkpointPaths_.size() > pImpl->maxCheckpoints_) {
            pImpl->checkpointPaths_.erase(pImpl->checkpointPaths_.begin());
        }
        
        NLM_LOG_INFO("Checkpoint saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint save: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::load(const std::string& filepath, Brain* brain) {
    NLM_LOG_INFO("Loading checkpoint from " + filepath);
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot open checkpoint file: " + filepath);
        return false;
    }
    
    try {
        // Read metadata
        CheckpointMetadata metadata;
        file.read(reinterpret_cast<char*>(&metadata), sizeof(metadata));
        
        if (brain) {
            // Load brain state
            if (!brain->loadState(file)) {
                NLM_LOG_ERROR("Failed to load brain state from checkpoint");
                return false;
            }
            
            // Load memory systems if brain has them
            if (brain->getWorkingMemory()) {
                if (!brain->getWorkingMemory()->loadState(file)) {
                    NLM_LOG_ERROR("Failed to load working memory");
                    return false;
                }
            }
            
            if (brain->getEpisodicMemory()) {
                if (!brain->getEpisodicMemory()->loadState(file)) {
                    NLM_LOG_ERROR("Failed to load episodic memory");
                    return false;
                }
            }
        }
        
        NLM_LOG_INFO("Checkpoint loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint load: " + std::string(e.what()));
        return false;
    }
}

void CheckpointManager::cleanupOldCheckpoints() {
    if (pImpl->checkpointPaths_.size() <= pImpl->retentionCount_) {
        return;
    }
    
    // Remove oldest checkpoints
    size_t toRemove = pImpl->checkpointPaths_.size() - pImpl->retentionCount_;
    for (size_t i = 0; i < toRemove; ++i) {
        try {
            std::filesystem::remove(pImpl->checkpointPaths_[i]);
            NLM_LOG_DEBUG("Removed old checkpoint: " + pImpl->checkpointPaths_[i]);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to remove checkpoint: " + std::string(e.what()));
        }
    }
    
    // Update list
    pImpl->checkpointPaths_.erase(pImpl->checkpointPaths_.begin(),
                                 pImpl->checkpointPaths_.begin() + toRemove);
}

std::vector<std::string> CheckpointManager::getCheckpointPaths() const {
    return pImpl->checkpointPaths_;
}

} // namespace nlm