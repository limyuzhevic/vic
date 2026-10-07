#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class Brain;
class Dopamine;
class AttentionalSelection;

// Memory statistics for diagnostics and monitoring
struct MemoryStats {
    size_t workingMemoryNeurons = 0;
    size_t episodicMemoryCount = 0;
    size_t associativeMemoryCount = 0;
    size_t workingMemoryTraces = 0;
    float workingMemoryActivity = 0.0f;
    float averageEpisodicReward = 0.0f;
    size_t replayCount = 0;
    size_t consolidationCount = 0;
    SimulationStep lastConsolidationStep = 0;
    SimulationStep lastReplayStep = 0;
    SimulationStep lastEpisodicStep = 0;
    float totalMemoryMaintenance = 0.0f;
    float totalCompetition = 0.0f;
};

// MemoryManager: Central coordinator for all memory systems
// Handles working memory updates, episodic memory recording,
// periodic consolidation, replay, and provides statistics
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Initialize with brain reference and memory systems
    void initialize(Brain* brain,
                    std::unique_ptr<NeuralWorkingMemory> workingMemory,
                    std::unique_ptr<NeuralEpisodicMemory> episodicMemory,
                    std::unique_ptr<NeuralAssociativeMemory> associativeMemory);
    
    // Update all memory systems with current brain state
    // This is called every simulation step
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Update working memory (maintenance and competition)
    void updateWorkingMemory();
    
    // Update episodic memory with current experience
    void updateEpisodicMemory(SimulationStep currentStep);
    
    // Handle memory consolidation periodically
    void handleConsolidation(SimulationStep currentStep);
    
    // Manage memory statistics and diagnostics
    void updateStatistics();
    const MemoryStats& getStatistics() const { return stats_; }
    
    // Get consolidated memory statistics report
    std::string getMemoryReport() const;
    
    // Force consolidation of all memories
    void forceConsolidation(float threshold = 0.3f);
    
    // Force replay of important memories
    void forceReplay(size_t count = 3);
    
    // Clear all memories (reset)
    void clear();
    
    // Enable/disable memory operations
    void setWorkingMemoryEnabled(bool enabled) { workingMemoryEnabled_ = enabled; }
    void setEpisodicMemoryEnabled(bool enabled) { episodicMemoryEnabled_ = enabled; }
    void setReplayEnabled(bool enabled) { replayEnabled_ = enabled; }
    void setConsolidationEnabled(bool enabled) { consolidationEnabled_ = enabled; }
    
    // Get memory system status
    bool isWorkingMemoryEnabled() const { return workingMemoryEnabled_; }
    bool isEpisodicMemoryEnabled() const { return episodicMemoryEnabled_; }
    bool isReplayEnabled() const { return replayEnabled_; }
    bool isConsolidationEnabled() const { return consolidationEnabled_; }
    
private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Configuration and settings
    SimulationStep consolidationInterval_;
    SimulationStep replayInterval_;
    SimulationStep episodicMemoryInterval_;
    
    // State tracking
    SimulationStep stepsSinceLastEpisodic_;
    SimulationStep stepsSinceLastConsolidation_;
    SimulationStep stepsSinceLastReplay_;
    
    // Statistics
    MemoryStats stats_;
    
    // Configuration flags
    bool workingMemoryEnabled_;
    bool episodicMemoryEnabled_;
    bool replayEnabled_;
    bool consolidationEnabled_;
    
    // Helper methods
    void createEpisodicMemoryItem(SimulationStep currentStep, float reward);
    void recordEpisodicExperience(SimulationStep currentStep);
    void performMemoryMaintenance();
    void runMemoryCompetition();
};

} // namespace nlm
