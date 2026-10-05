// Brain Memory System - Manages all memory subsystems
#ifndef BRAIN_MEMORY_SYSTEM_HPP
#define BRAIN_MEMORY_SYSTEM_HPP

#include "Brain.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainMemorySystem {
public:
    explicit BrainMemorySystem(Brain* brain);
    
    // Initialize memory systems
    void initializeMemorySystems();
    
    // Update working memory
    void updateWorkingMemory(TimestepDuration timestep);
    
    // Update episodic memory with current experience
    void updateEpisodicMemory(SimulationStep currentStep, float reward);
    
    // Store episode with current brain state
    void storeCurrentStateAsEpisode(SimulationStep currentStep, float reward);
    
    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    
    // Replay episodes
    void replayEpisodes(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // Memory consolidation
    void consolidateMemories(float threshold);
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Statistics
    size_t getActiveMemoryTraces() const { return activeMemoryTraces; }
    size_t getTotalEpisodes() const { return totalEpisodes; }
    
private:
    Brain* brain;
    
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Statistics
    size_t activeMemoryTraces;
    size_t totalEpisodes;
    
    // Helper methods
    void captureCurrentBrainState(EpisodicMemoryItem& episode);
    bool shouldStoreEpisode(SimulationStep currentStep);
};

} // namespace nlm

#endif // BRAIN_MEMORY_SYSTEM_HPP