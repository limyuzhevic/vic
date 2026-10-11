#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cmath>

namespace nlm {

// Sleep stage enumeration
enum class SleepStage {
    Awake,
    LightSleep,      // Stage 1 & 2
    DeepSleep,       // Stage 3 & 4 (slow-wave sleep)
    REM              // Rapid Eye Movement
};

// Sleep cycle state
struct SleepCycleState {
    SleepStage stage;
    SimulationStep startTime;
    SimulationStep duration;
    float arousalLevel;          // 0.0 to 1.0
    float stressLevel;            // 0.0 to 1.0 (stress signals)
    float attentionLevel;         // 0.0 to 1.0 (attention during sleep)
    float predictionError;        // Prediction error signals
    float neuromodulation[3];     // DA, Ach, NE levels
    float memoryReversalScore;    // Pattern reversal during sleep
    bool isActive;               // Whether currently in sleep
    bool shouldWake;             // Wake-up condition met
};

// Stress signals for memory prioritization
struct StressSignals {
    float cortisolLevel;          // Stress hormone
    float noradrenalineLevel;     // Arousal/stress signal
    float predictionError;        // Learning signal
    float noveltyLevel;           // Novelty detection
    float curiosityLevel;         // Exploration motivation
    float rewardPrediction;       // Expected reward signal
};

// Attention guidance during sleep
struct AttentionGuidance {
    float globalAttention;        // Global attentional focus
    float sensoryFocus;           // Which sensory modalities are attended
    float workingMemoryFocus;     // Which working memory traces are attended
    float episodicFocus;          // Which episodic memories are attended
};

// SleepManager: Manages sleep/rest cycles and coordinates memory consolidation
class SleepManager {
public:
    SleepManager();
    ~SleepManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Main sleep management in step()
    void update(SimulationStep currentStep, float globalArousal);
    
    // Sleep cycle detection based on neural activity
    void detectSleepNeed(const std::vector<NeuronId>& firingNeurons,
                        const std::vector<float>& neuronActivities);
    
    // Wake-up condition management
    void checkWakeConditions(const SensoryInput& input,
                            const StressSignals& stress,
                            const AttentionGuidance& attention);
    
    // Sleep stage progression
    void advanceSleepStage(SimulationStep currentStep);
    
    // Get current sleep state
    SleepStage getCurrentStage() const { return cycleState_.stage; }
    bool isSleeping() const { return cycleState_.isActive; }
    float getStressLevel() const { return cycleState_.stressLevel; }
    float getAttentionLevel() const { return cycleState_.attentionLevel; }
    
    // Stress signal management
    void updateStressSignals(const StressSignals& stress);
    void applyStressToConsolidation();
    
    // Attention guidance management
    void updateAttentionGuidance(const AttentionGuidance& attention);
    void applyAttentionToConsolidation();
    
    // Memory consolidation coordination
    void coordinateMemoryConsolidation(SimulationStep currentStep);
    void triggerSleepReplay(SimulationStep currentStep);
    void triggerOfflineLearning(SimulationStep currentStep);
    
    // Get consolidated stress-based memory priorities
    std::vector<float> getStressBasedPriorities() const;
    
    // Get attention-guided consolidation targets
    std::vector<NeuronId> getAttentionGuidedTargets() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    SleepCycleState cycleState_;
    StressSignals stressSignals_;
    AttentionGuidance attentionGuidance_;
    
    // Sleep timing
    SimulationStep lastSleepStart_;
    SimulationStep sleepStartTime_;
    SimulationStep wakeUpTime_;
    SimulationStep currentStep_;
    
    // State tracking
    size_t wakeCount_;            // Number of wake-ups
    size_t sleepCount_;           // Number of sleep cycles
    float totalSleepTime_;        // Total time spent sleeping
    
    // Sleep stage durations
    SimulationStep lightSleepDuration_;
    SimulationStep deepSleepDuration_;
    SimulationStep remSleepDuration_;
    
    // Detection thresholds
    float firingThreshold_;       // Neural firing threshold for sleep detection
    float arousalThreshold_;      // Arousal threshold for wake-up
    float stressThreshold_;       // Stress threshold for wake-up
    
    // Sleep stage progression
    SimulationStep timeInCurrentStage_;
    size_t stageProgressionCounter_;
    
    // Memory consolidation state
    std::unordered_map<SimulationStep, std::vector<NeuronId>> sleepConsolidationTargets_;
    std::unordered_map<SimulationStep, float> attentionWeights_;
};