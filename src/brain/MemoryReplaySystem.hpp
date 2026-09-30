#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/SleepState.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "NeuralWorkingMemoryWithSleep.hpp"
#include "NeuralEpisodicMemoryWithSleep.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

// Memory replay scheduling and timing management
class MemoryReplaySystem {
public:
    MemoryReplaySystem();
    ~MemoryReplaySystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Comprehensive memory replay coordination
    void executeReplay(SimulationStep currentStep, SleepState sleepState);
    
    // State-specific replay algorithms
    void executeQuietSleepReplay();
    void executeDeepSleepReplay();
    void executeREMSleepReplay();
    void executeWakeReplay();  // No replay during wake
    
    // Working memory and episodic memory integration
    void replayWorkingMemory(SleepState sleepState);
    void replayEpisodicMemory(SleepState sleepState);
    
    // Replay scheduling and timing management
    void scheduleReplay(SleepState sleepState, SimulationStep currentStep);
    void updateReplayTiming(SleepState sleepState, float dt);
    
    // State-specific replay characteristics
    struct ReplayParameters {
        float replayInterval;           // Time between replay events (steps)
        float replayStrength;           // Strength of replay (0.0 to 1.0)
        float reconstructionFactor;     // Pattern reconstruction factor
        float consolidationWeight;      // Weight for consolidation
        size_t tracesPerReplay;          // Number of traces to replay
        bool integrateWithWorkingMemory; // Whether to integrate with working memory
    };
    
    const ReplayParameters& getReplayParameters(SleepState state) const;
    void setReplayParameters(SleepState state, const ReplayParameters& params);
    
    // Replay statistics and monitoring
    size_t getTotalReplayCount() const { return totalReplayCount_; }
    float getAverageReplayStrength() const { return averageReplayStrength_; }
    float getReplayEfficiency() const { return replayEfficiency_; }
    
    // Real-time monitoring
    float getCurrentReplayProgress() const { return currentReplayProgress_; }
    SleepState getCurrentReplayState() const { return currentReplayState_; }
    size_t getActiveReplaySequences() const { return activeReplaySequences_.size(); }
    
    // Replay sequencing
    struct ReplaySequence {
        SleepState state;
        SimulationStep startTime;
        SimulationStep endTime;
        float strength;
        std::vector<size_t> memoryIndices;
        bool completed;
    };
    
    void startReplaySequence(SleepState state, SimulationStep startTime);
    void completeReplaySequence(size_t sequenceId);
    void pauseReplaySequence(size_t sequenceId);
    void resumeReplaySequence(size_t sequenceId);
    
    // Checkpointing support
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Integration with existing neuromodulation systems
    void integrateWithNeuromodulation(SleepState sleepState);
    void integrateWithPredictionSystem(SleepState sleepState);
    void integrateWithPlanningSystem(SleepState sleepState);
    
    // Error handling and recovery
    void handleReplayError(const std::string& errorMessage);
    void recoverFromReplayError();
    bool isInErrorState() const { return inErrorState_; }

private:
    // Core replay system components
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Replay parameters for each state
    std::map<SleepState, ReplayParameters> replayParameters_;
    
    // Statistics
    size_t totalReplayCount_;
    float averageReplayStrength_;
    float replayEfficiency_;
    
    // Current replay state
    SleepState currentReplayState_;
    float currentReplayProgress_;
    
    // Active replay sequences
    std::vector<ReplaySequence> activeReplaySequences_;
    
    // Error handling
    bool inErrorState_;
    std::string lastErrorMessage_;
    size_t consecutiveErrors_;
    
    // Replay timing
    SimulationStep nextReplayStep_;
    float replayInterval_;
    float replayStrength_;
    
    // State-specific replay characteristics
    struct StateReplayCharacteristics {
        float replayFrequency;        // Replay events per second
        float replayDuration;         // Duration of each replay (seconds)
        float replayOverlap;          // Overlap between replay sequences
        float reconstructionAccuracy; // Accuracy of pattern reconstruction
        float consolidationEfficiency; // Efficiency of consolidation
        float integrationDepth;       // Depth of memory integration
    };
    
    std::map<SleepState, StateReplayCharacteristics> stateCharacteristics_;
    
    // Helper functions
    void updateReplayParameters();
    void executeStateSpecificReplay(SleepState state);
    void integrateReplayWithWorkingMemory(SleepState state);
    void integrateReplayWithEpisodicMemory(SleepState state);
    void updateReplayStatistics(float strength, bool success);
    void resetReplayState();
    bool validateReplayParameters(const ReplayParameters& params) const;
    void scheduleNextReplay(SleepState state, SimulationStep currentStep);
};

} // namespace nlm
