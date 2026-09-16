#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

/**
 * Memory system checkpoint integration
 * 
 * Provides methods to checkpoint and restore memory systems (working, episodic, associative)
 */
class MemoryCheckpoint {
public:
    MemoryCheckpoint();
    ~MemoryCheckpoint();
    
    /**
     * Checkpoint working memory state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointWorkingMemory(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Checkpoint episodic memory state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointEpisodicMemory(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Checkpoint associative memory state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointAssociativeMemory(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Restore working memory state
     * @param brain Brain instance
     * @param reader Checkpoint reader
     * @return true if successful
     */
    bool restoreWorkingMemory(Brain* brain, CheckpointReader& reader);
    
    /**
     * Restore episodic memory state
     * @param brain Brain instance
     * @param reader Checkpoint reader
     * @return true if successful
     */
    bool restoreEpisodicMemory(Brain* brain, CheckpointReader& reader);
    
    /**
     * Restore associative memory state
     * @param brain Brain instance
     * @param reader Checkpoint reader
     * @return true if successful
     */
    bool restoreAssociativeMemory(Brain* brain, CheckpointReader& reader);
    
    /**
     * Checkpoint prediction system state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointPredictionSystem(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Checkpoint cognitive system state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointCognitiveSystems(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Checkpoint neuromodulation state
     * @param brain Brain instance
     * @return true if successful
     */
    bool checkpointNeuromodulation(Brain* brain, CheckpointWriter& writer);
    
    /**
     * Get memory checkpoint version
     */
    static constexpr uint32_t getMemoryCheckpointVersion() { return 1; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm