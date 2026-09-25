#pragma once

#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

// BrainMemoryManager manages memory systems
class BrainMemoryManager {
public:
    BrainMemoryManager() = default;
    ~BrainMemoryManager() = default;
    
    // Initialize memory systems
    void initialize(Brain* brain);
    
    // Update working memory
    void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    
    // Update episodic memory with current step
    void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Get memory statistics
    void collectMemoryStats(Brain* brain) const;
    
    // Reset all memory systems
    void reset(Brain* brain);
    
    // Store current brain state as an episode
    void storeEpisode(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Replay episodes for memory consolidation
    void replayEpisodes(Brain* brain, SimulationStep currentStep);
    
    // Consolidate memories
    void consolidateMemories(Brain* brain, SimulationStep currentStep);
};

} // namespace nlm
