#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include <queue>

namespace nlm {

/**
 * SleepConsolidationSystem: Implements sleep/rest-based memory consolidation
 * 
 * Key mechanisms:
 * - Multi-stage consolidation (fast -> slow)
 * - Sleep/rest cycles triggered by neuromodulators
 * - Replay-based strengthening during sleep
 * - Consolidation depth control (shallow vs deep)
 * - Development-stage-specific consolidation rules
 */
class SleepConsolidationSystem {
public:
    SleepConsolidationSystem();
    ~SleepConsolidationSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Start/rest cycle
    void startRestCycle();
    void endRestCycle();
    bool isResting() const { return isResting_; }
    
    // Trigger consolidation based on neuromodulator levels
    void triggerConsolidation();
    
    // Schedule consolidation event
    void scheduleConsolidation(size_t step, bool force = false);
    
    // Perform fast consolidation (immediate, weak)
    void performFastConsolidation();
    
    // Perform slow consolidation (delayed, strong)
    void performSlowConsolidation();
    
    // Set consolidation parameters
    void setConsolidationDepth(float depth) { consolidationDepth_ = depth; }
    void setFastConsolidationInterval(size_t interval) { fastConsolidationInterval_ = interval; }
    void setSlowConsolidationInterval(size_t interval) { slowConsolidationInterval_ = interval; }
    void setConsolidationThreshold(float threshold) { consolidationThreshold_ = threshold; }
    
    // Get consolidation state
    float getConsolidationProgress() const { return consolidationProgress_; }
    size_t getConsolidationStepsRemaining() const { return stepsRemaining_; }
    
    // Development stage detection
    enum class DevelopmentalStage {
        Embryonic,
        Infant,
        Juvenile,
        Adolescence,
        Adult
    };
    
    void updateDevelopment(DevelopmentalStage stage) {
        developmentalStage_ = stage;
        // Adjust consolidation parameters based on stage
        switch (stage) {
            case DevelopmentalStage::Embryonic:
                setConsolidationDepth(0.3f);
                setFastConsolidationInterval(100);
                setSlowConsolidationInterval(5000);
                break;
            case DevelopmentalStage::Infant:
                setConsolidationDepth(0.5f);
                setFastConsolidationInterval(200);
                setSlowConsolidationInterval(3000);
                break;
            case DevelopmentalStage::Juvenile:
                setConsolidationDepth(0.7f);
                setFastConsolidationInterval(300);
                setSlowConsolidationInterval(2000);
                break;
            case DevelopmentalStage::Adolescence:
                setConsolidationDepth(0.8f);
                setFastConsolidationInterval(500);
                setSlowConsolidationInterval(1000);
                break;
            case DevelopmentalStage::Adult:
                setConsolidationDepth(1.0f);
                setFastConsolidationInterval(1000);
                setSlowConsolidationInterval(5000);
                break;
        }
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    bool isResting_;
    float consolidationDepth_;
    size_t fastConsolidationInterval_;
    size_t slowConsolidationInterval_;
    float consolidationThreshold_;
    float consolidationProgress_;
    size_t stepsRemaining_;
    DevelopmentalStage developmentalStage_;
    
    // Queued consolidation events
    std::vector<std::pair<size_t, bool>> scheduledConsolidations_;
};

/**
 * ReplaySystem: Implements importance-based memory replay
 * 
 * Key mechanisms:
 * - Importance-based replay (not random)
 * - Replay timing optimization (right after learning)
 * - Replay strength control (important memories replayed more)
 * - Replay-dependent learning (triggers plasticity)
 * - Replay validation (checks if successful)
 * - Replay during sleep/rest cycles
 */
class ReplaySystem {
public:
    ReplaySystem();
    ~ReplaySystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update replay schedule and perform replay
    void update(float dt);
    
    // Trigger replay of specific episode
    void replayEpisode(size_t episodeId, float strength = 1.0f);
    
    // Get episodes for replay based on importance
    std::vector<size_t> getEpisodesForReplay(size_t count) const;
    
    // Replay timing optimization
    void recordLearningEvent(size_t episodeId, float importance);
    void adjustReplayTiming(size_t episodeId);
    
    // Replay validation
    bool validateReplay(const EpisodicMemoryItem& episode) const;
    
    // Replay strength control
    void setReplayStrengthMultiplier(float multiplier) { replayStrengthMultiplier_ = multiplier; }
    void setImportanceWeight(float weight) { importanceWeight_ = weight; }
    void setRecencyWeight(float weight) { recencyWeight_ = weight; }
    void setRewardWeight(float weight) { rewardWeight_ = weight; }
    
    // Get replay statistics
    size_t getTotalReplays() const { return totalReplays_; }
    size_t getSuccessfulReplays() const { return successfulReplays_; }
    float getReplaySuccessRate() const {
        return totalReplays_ > 0 ? (float)successfulReplays_ / totalReplays_ : 0.0f;
    }
    
    // Enable/disable replay during rest
    void enableReplayDuringRest(bool enable) { replayDuringRest_ = enable; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float replayStrengthMultiplier_;
    float importanceWeight_;
    float recencyWeight_;
    float rewardWeight_;
    bool replayDuringRest_;
    
    // Learning event buffer for timing optimization
    struct LearningEvent {
        size_t episodeId;
        float importance;
        size_t timestamp;
    };
    std::deque<LearningEvent> learningEvents_;
    
    // Replay statistics
    size_t totalReplays_;
    size_t successfulReplays_;
};

/**
 * NeuromodulatorMemoryIntegration: Integrates neuromodulation with memory systems
 * 
 * Key mechanisms:
 * - Dopamine control of reward-based consolidation and replay
 * - Acetylcholine control of attention and memory encoding
 * - Norepinephrine control of arousal and memory strength
 * - Serotonin control of mood and memory stability
 * - Integration with memory strength modulation
 */
class NeuromodulatorMemoryIntegration {
public:
    NeuromodulatorMemoryIntegration();
    ~NeuromodulatorMemoryIntegration();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update neuromodulator effects on memory
    void update(float dt);
    
    // Get neuromodulator influence on consolidation
    float getConsolidationModulation() const { return consolidationModulation_; }
    float getReplayModulation() const { return replayModulation_; }
    
    // Set neuromodulator weights
    void setDopamineWeight(float weight) { dopamineWeight_ = weight; }
    void setAcetylcholineWeight(float weight) { acetylcholineWeight_ = weight; }
    void setNorepinephrineWeight(float weight) { norepinephrineWeight_ = weight; }
    void setSerotoninWeight(float weight) { serotoninWeight_ = weight; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float consolidationModulation_;
    float replayModulation_;
    float dopamineWeight_;
    float acetylcholineWeight_;
    float norepinephrineWeight_;
    float serotoninWeight_;
};

/**
 * DevelopmentalMemoryIntegration: Developmental-stage-specific memory consolidation
 * 
 * Key mechanisms:
 * - Different consolidation rules at different developmental stages
 * - Changing memory capacity and stability with development
 * - Critical periods for specific memory types
 * - Integration with brain development
 */
class DevelopmentalMemoryIntegration {
public:
    DevelopmentalMemoryIntegration();
    ~DevelopmentalMemoryIntegration();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update developmental effects on memory
    void update(float dt);
    
    // Get developmental stage
    enum class DevelopmentalStage {
        Embryonic,
        Infant,
        Juvenile,
        Adolescence,
        Adult
    };
    
    DevelopmentalStage getStage() const { return stage_; }
    void setStage(DevelopmentalStage stage) { stage_ = stage; }
    
    // Get developmental plasticity factor
    float getPlasticityFactor() const { return plasticityFactor_; }
    float getStabilityFactor() const { return stabilityFactor_; }
    
    // Get developmental constraints
    bool isCriticalPeriod() const { return isCriticalPeriod_; }
    float getMemoryCapacityMultiplier() const { return memoryCapacityMultiplier_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    DevelopmentalStage stage_;
    float plasticityFactor_;
    float stabilityFactor_;
    bool isCriticalPeriod_;
    float memoryCapacityMultiplier_;
    size_t criticalPeriodStepsRemaining_;
};

} // namespace nlm