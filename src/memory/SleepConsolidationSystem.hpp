#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// SleepState: Represents different sleep phases and states
struct SleepState {
    enum class State {
        Awake,
        LightSleep,
        DeepSleep,
        REM,
        TransitionAwake,
        TransitionSleep
    };
    
    State currentState;
    Timestamp sleepStartTime;
    Timestamp wakeUpTime;
    float sleepDepth;
    float homeostasisScore;
    float synapticDownscalingLevel;
    float memoryPrioritizationIndex;
    
    // State-specific parameters
    float slowWaveActivity;  // For deep sleep
    float spindleDensity;     // For light sleep
    float thetaPower;        // For REM sleep
    float coherenceLevel;    // Across brain regions
    
    SleepState()
        : currentState(State::Awake)
        , sleepStartTime(0.0)
        , wakeUpTime(0.0)
        , sleepDepth(0.0f)
        , homeostasisScore(1.0f)
        , synapticDownscalingLevel(1.0f)
        , memoryPrioritizationIndex(0.0f)
        , slowWaveActivity(0.0f)
        , spindleDensity(0.0f)
        , thetaPower(0.0f)
        , coherenceLevel(1.0f) {}
};

// MemoryReplayEvent: Represents a single memory replay event
struct MemoryReplayEvent {
    SimulationStep timestamp;
    SimulationStep episodeId;
    float replayStrength;
    float replayDuration;
    std::vector<NeuronId> reactivatedNeurons;
    float synapticWeightChange;
    
    MemoryReplayEvent()
        : timestamp(0)
        , episodeId(0)
        , replayStrength(0.0f)
        , replayDuration(0.0f)
        , synapticWeightChange(0.0f) {}
};

// SynapticDownscalingTarget: Target for synaptic normalization
struct SynapticDownscalingTarget {
    NeuronId neuronId;
    float targetWeight;
    float currentWeight;
    float downscalingFactor;
    float homeostaticError;
    
    SynapticDownscalingTarget()
        : neuronId()
        , targetWeight(0.1f)
        , currentWeight(0.0f)
        , downscalingFactor(1.0f)
        , homeostaticError(0.0f) {}
};

// HomeostaticRegulation: Maintains neural excitability balance
struct HomeostaticRegulation {
    float synapticExcitability;
    float intrinsicConductance;
    float inhibitionBalance;
    float metabolicLoad;
    float firingRateTarget;
    float adaptationThreshold;
    
    HomeostaticRegulation()
        : synapticExcitability(1.0f)
        , intrinsicConductance(1.0f)
        , inhibitionBalance(1.0f)
        , metabolicLoad(0.5f)
        , firingRateTarget(5.0f)
        , adaptationThreshold(0.1f) {}
};

// NeuromodulatorySleepControl: Controls sleep-wake transitions
struct NeuromodulatorySleepControl {
    float acetylcholine;
    float norepinephrine;
    float serotonin;
    float dopamine;
    float orexin;
    float melatonin;
    
    NeuromodulatorySleepControl()
        : acetylcholine(0.0f)
        , norepinephrine(0.0f)
        , serotonin(0.0f)
        , dopamine(0.0f)
        , orexin(0.0f)
        , melatonin(0.0f) {}
};

// MemoryPriorityEntry: Priority scoring for memory consolidation
struct MemoryPriorityEntry {
    SimulationStep episodeId;
    float replayFrequency;
    float noveltyScore;
    float rewardAssociation;
    float predictionError;
    float temporalRecency;
    float emotionalValence;
    float consolidationNeed;
    
    MemoryPriorityEntry()
        : episodeId(0)
        , replayFrequency(0.0f)
        , noveltyScore(0.0f)
        , rewardAssociation(0.0f)
        , predictionError(0.0f)
        , temporalRecency(0.0f)
        , emotionalValence(0.0f)
        , consolidationNeed(0.0f) {}
};

class SleepConsolidationSystem {
public:
    SleepConsolidationSystem();
    ~SleepConsolidationSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update sleep system with neuromodulatory inputs
    void update(TimestepDuration dt, const NeuromodulatorySleepControl& neuromodulators);
    
    // Enter sleep state with specific characteristics
    void enterSleep(SleepState::State state, float depth = 0.5f);
    
    // Exit sleep state (wake up)
    void exitSleep();
    
    // Trigger spontaneous replay during rest/sleep
    void triggerReplay();
    
    // Apply synaptic downscaling for homeostatic regulation
    void applySynapticDownscaling(const std::vector<SynapticDownscalingTarget>& targets);
    
    // Perform memory pruning of weak traces
    void pruneWeakMemories(float threshold);
    
    // Consolidate memories based on priority
    void consolidateMemories(const std::vector<MemoryPriorityEntry>& priorities);
    
    // Update homeostatic regulation
    void updateHomeostasis(const std::vector<NeuronId>& activeNeurons, float averageFiringRate);
    
    // Get current sleep state
    const SleepState& getSleepState() const { return sleepState_; }
    
    // Get replay events from this cycle
    const std::vector<MemoryReplayEvent>& getReplayEvents() const { return replayEvents_; }
    
    // Get homeostatic regulation state
    const HomeostaticRegulation& getHomeostaticRegulation() const { return homeostasis_; }
    
    // Get synaptic downscaling targets
    const std::vector<SynapticDownscalingTarget>& getDownscalingTargets() const { return downscalingTargets_; }
    
    // Force sleep-wake cycle for consolidation
    void forceConsolidationCycle(TimestepDuration dt);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    SleepState sleepState_;
    HomeostaticRegulation homeostasis_;
    NeuromodulatorySleepControl neuromodulators_;
    
    std::vector<MemoryReplayEvent> replayEvents_;
    std::vector<SynapticDownscalingTarget> downscalingTargets_;
    std::vector<MemoryPriorityEntry> memoryPriorities_;
    
    // Sleep timing parameters
    float sleepDuration_;
    float lightSleepDuration_;
    float deepSleepDuration_;
    float remSleepDuration_;
    
    // Consolidation parameters
    float downscalingFactor_;
    float pruningThreshold_;
    float replayStrengthMultiplier_;
    float homeostasisGain_;
    
    // Statistics
    size_t totalSleepCycles_;
    size_t successfulReplays_;
    size_t synapticPrunings_;
    float averageHomeostasisScore_;
    
    // Internal methods
    void updateSleepState(TimestepDuration dt);
    void generateDownscalingTargets();
    void calculateMemoryPriorities();
    void scheduleReplayEvents();
    void applyHomeostaticPlasticity();
    void updateNeuromodulatoryControl();
    void computeSynapticTargetWeights();
};

} // namespace nlm
