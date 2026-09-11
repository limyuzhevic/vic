#pragma once

#include "Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <chrono>
#include <fstream>
#include <memory>

namespace nlm {

class SimulationManager {
public:
    SimulationManager() = default;
    
    void initialize(Brain* brain);
    
    // Time management
    void update(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void advanceTime(Brain& brain, TimestepDuration dt);
    Timestamp getCurrentTime(Brain& brain) const;
    SimulationStep getCurrentStep(Brain& brain) const;
    
    // Checkpointing and persistence
    bool saveState(Brain& brain, const std::string& filepath) const;
    bool loadState(Brain& brain, const std::string& filepath);
    void createCheckpoint(Brain& brain, const std::string& directory) const;
    
    // Replay and consolidation
    void replayImportantMemories(Brain& brain, size_t count);
    void consolidateMemories(Brain& brain, float threshold);
    
    // State management
    void resetState(Brain& brain);
    void pauseSimulation(Brain& brain, bool pause);
    bool isPaused(Brain& brain) const;
    
    // Performance and statistics
    void updateStatistics(Brain& brain);
    void logPerformanceStats(Brain& brain) const;
    float getSimulationEfficiency(Brain& brain) const;
    
    // Time-based operations
    void executeScheduledOperations(Brain& brain, SimulationStep currentStep);
    void executeScheduledEvents(Brain& brain, Timestamp currentTime);
    
    void logSimulationManagerStatus(Brain& brain) const;
    
private:
    // Time management
    void updateTime(Brain& brain, Timestamp deltaTime);
    Timestamp calculateTheoreticalTime(SimulationStep step) const;
    
    // Checkpoint management
    std::string generateCheckpointFilename(const std::string& directory,
                                          SimulationStep step) const;
    bool verifyCheckpointIntegrity(const std::string& filepath) const;
    
    // Memory operations
    void executeReplay(Brain& brain, size_t episodeCount);
    void executeConsolidation(Brain& brain, float threshold);
    
    // State restoration
    void restoreStateFromCheckpoint(Brain& brain, const std::string& filepath);
    
    // Performance monitoring
    void measureMemoryUsage(Brain& brain);
    void measureCPUUsage(Brain& brain);
    void measureTimestepPerformance(Brain& brain, TimestepDuration actualStepTime);
    
    // Timing and scheduling
    struct ScheduledOperation {
        SimulationStep step;
        std::function<void()> operation;
        bool executed;
    };
    
    std::vector<ScheduledOperation> scheduledOperations_;
    
    // Performance metrics
    struct PerformanceMetrics {
        TimestepDuration averageTimestepTime;
        size_t totalSpikesProcessed;
        size_t memoryUsageBytes;
        double cpuUsagePercentage;
        float simulationEfficiency;
    };
    
    PerformanceMetrics metrics_;
    bool simulationPaused_;
};

} // namespace nlm
