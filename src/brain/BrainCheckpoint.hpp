// Brain checkpoint module - handles brain state persistence
// 
// This module provides functions for saving and loading brain state, checkpointing,
// and managing persistent storage of neural simulation data. Checkpointing is essential
// for recovery, continuation of learning, and experimental reproducibility.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainCheckpoint
 * @brief Manages brain state persistence and checkpointing
 * 
 * The BrainCheckpoint class provides functions for saving and loading brain state,
 * checkpointing, and managing persistent storage of neural simulation data. Checkpointing
 * is essential for recovery, continuation of learning, and experimental reproducibility.
 * 
 * @details This class implements checkpointing systems required for the integrated
 * brain architecture:
 * - Save/load brain state to/from files
 * - Checkpoint directory management
 * - Compression and efficient storage
 * - Recovery and continuation capabilities
 * 
 * @note Checkpointing is fundamental for long-running simulations, experimental
 * reproducibility, and the brain's ability to continue learning from previous states.
 */
class BrainCheckpoint {
public:
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * This method saves the complete brain state to a file for persistence, recovery,
     * or continuation of learning. The checkpoint includes neural connectivity,
     * memory states, and simulation parameters.
     * 
     * @param brain Pointer to Brain instance to save
     * @param filepath Path to save checkpoint file
     * @return True if successful, false otherwise
     * 
     * @pre brain must not be null
     * @post Brain state saved to file (if successful)
     */
    static bool save(Brain* brain, const std::string& filepath);
    
    /**
     * @brief Load brain state from file
     * 
     * This method loads a previously saved brain state from a file, restoring
     * neural connectivity, memory states, and simulation parameters. Loading enables
     * recovery from previous states or continuation of learning.
     * 
     * @param brain Pointer to Brain instance to load into
     * @param filepath Path to checkpoint file to load
     * @return True if successful, false otherwise
     * 
     * @pre brain must not be null, filepath must point to valid checkpoint
     * @post Brain state loaded from file (if successful)
     */
    static bool load(Brain* brain, const std::string& filepath);
    
    /**
     * @brief Reset brain to initial state
     * 
     * This method resets the brain to its initial state, clearing all neural connections,
     * memory contents, and simulation parameters. Reset is useful for starting fresh
     * or cleaning up after experiments.
     * 
     * @param brain Pointer to Brain instance to reset
     * 
     * @pre brain must not be null
     * @post Brain reset to initial state
     */
    static void reset(Brain* brain);
    
    /**
     * @brief Get checkpoint directory from configuration
     * 
     * This method returns the checkpoint directory path from the brain's configuration,
     * allowing for organized storage and management of checkpoint files.
     * 
     * @param brain Pointer to Brain instance
     * @return Checkpoint directory path (empty string if not configured)
     * 
     * @pre brain must not be null
     * @post Checkpoint directory path returned
     */
    static std::string getCheckpointDirectory(Brain* brain);
    
    /**
     * @brief Create checkpoint directory if it doesn't exist
     * 
     * This method creates the specified directory if it doesn't already exist,
     * ensuring that checkpoint storage directories are available for saving.
     * 
     * @param directory Directory path to create
     * @return True if directory created or already exists, false on error
     * 
     * @pre directory path must be valid
     * @post Directory created (if it didn't exist)
     */
    static bool createCheckpointDirectory(const std::string& directory);
};

} // namespace nlm