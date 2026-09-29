#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Checkpoint management and persistence
 * Handles save/load operations for experiment state
 */
class CheckpointManager {
public:
    CheckpointManager() = default;
    ~CheckpointManager() = default;
    
    /**
     * Save brain state to checkpoint file
     */
    bool saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /**
     * Load brain state from checkpoint file
     */
    bool loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /**
     * Validate checkpoint file exists and is loadable
     */
    bool validateCheckpoint(const std::string& filepath) const;
    
    /**
     * Get checkpoint file size
     */
    size_t getCheckpointFileSize(const std::string& filepath) const;
    
    /**
     * Create backup of existing checkpoint
     */
    bool backupCheckpoint(const std::string& filepath);
    
    /**
     * Clean up old checkpoint files
     */
    bool cleanupOldCheckpoints(const std::string& pattern, int maxFiles = 5);
};

} // namespace nlm
