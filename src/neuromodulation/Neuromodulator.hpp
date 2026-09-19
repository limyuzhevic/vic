#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Advanced dopamine functionality
    void initialize(Brain* brain);
    void reset();
    void setLearningRate(float rate);
    void setBaseline(float baseline);
    void setTonicLevel(float level);
    float getPredictionError() const;
    float getLearnedValue() const;
    void getPlasticityFactors(float& ltpWeight, float& ltdWeight, 
                             float& hebbianRate, float& structuralRate) const;
    const std::vector<float>& getPhasicHistory() const;
    bool isBursting() const;
    bool isSlapping() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Phase 2 methods for real ACh implementation
    void initialize(Brain* brain);
    void enhanceAttention(float attentionStrength);
    void facilitateMemoryConsolidation(float consolidationRate);
    void reset();
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Phase 2 methods for real NE implementation
    void initialize(Brain* brain);
    void increaseArousal(float arousalLevel);
    void enhanceSensoryProcessing(float gainFactor);
    void reset();
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Phase 2 methods for real 5-HT implementation
    void initialize(Brain* brain);
    void modulateMood(float moodState);
    void regulateImpulsivity(float impulsivityLevel);
    void influenceSocialBehavior(float socialFactor);
    void reset();
};

// Novelty detection system
// This detects new or unexpected sensory input
class Novelty : public Neuromodulator {
public:
    const char* getName() const override { return "Novelty"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty-specific functionality
    void initialize(Brain* brain);
    void processSensoryInput(const SensoryInput& input);
    float getCuriosityDrive() const;
    bool isNoveltyHigh() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity system
// Drives exploration of novel environments
class Curiosity : public Neuromodulator {
public:
    const char* getName() const override { return "Curiosity"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity-specific functionality
    void initialize(Brain* brain);
    void update(float novelty, float predictionError, TimestepDuration dt);
    float getExplorationDrive() const;
    float getInformationGain() const;
    bool needsExploration() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal
// Monitors discrepancies between predicted and actual outcomes
class PredictionError : public Neuromodulator {
public:
    const char* getName() const override { return "PredictionError"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Prediction error-specific functionality
    void initialize(Brain* brain);
    void calculateError(const SensoryInput& predicted, const SensoryInput& actual);
    float getAbsoluteError() const;
    float getSquaredError() const;
    bool isHighPredictionError() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Sleep/rest system
// Manages consolidation and replay during rest periods
class SleepSystem : public Neuromodulator {
public:
    const char* getName() const override { return "SleepSystem"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Sleep-specific functionality
    void initialize(Brain* brain);
    void enterSleep(float sleepDepth);
    void exitSleep();
    bool isSleeping() const;
    float getSleepDepth() const;
    
    // Memory management
    void scheduleConsolidation();
    void performMemoryReplay();
    void updateDevelopmentSchedule();
    
    // Episode replay
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Synaptic regulation
    void applySynapticDownscaling();
    
    // State management
    void reset();
    
    // Configuration
    void setConsolidationThreshold(float threshold);
    void setMemoryReplayStrength(float strength);
    
    // Statistics
    float getConsolidationThreshold() const;
    float getMemoryReplayStrength() const;
    int getReplayCount() const;
    int getSynapticChangesCount() const;
    
    // Event tracking
    struct SleepEvent {
        SimulationStep timestamp;
        std::string type;       // "replay", "consolidation", "synaptic_change"
        NeuronId neuronId;      // For replay events
        float strength;        // Activation/strength level
    };
    
    std::vector<SleepEvent> getRecentEvents(SimulationStep fromStep) const;
    
private:
    void replayNeuronActivation(NeuronId neuronId, float strength);
    void strengthenReplayConnections(const EpisodicMemoryItem* episode);
    
    struct Impl;
    Impl* pImpl;
};

// Reward system
// Computes reward values from observations
class Reward : public Neuromodulator {
public:
    const char* getName() const override { return "Reward"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward-specific functionality
    void initialize(Brain* brain);
    float computeReward(const Observation& observation) const;
    void recordReward(float reward, float confidence = 1.0f);
    float getAccumulatedReward() const;
    float getAverageReward() const;
    std::vector<float> getRewardHistory() const;
    bool isHighReward() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Attention system
// Selects and amplifies relevant neural activity
class AttentionalSelection : public Neuromodulator {
public:
    const char* getName() const override { return "Attention"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention-specific functionality
    void initialize(Brain* brain);
    void allocateAttention(const std::vector<NeuronId>& neurons, float allocationStrength);
    void inhibitIrrelevantActivity(const std::vector<NeuronId>& neurons);
    void enhanceWorkingMemory(const std::vector<NeuronId>& neurons);
    void updateTopDownAttention(const SensoryInput& context);
    float getAttentionLevel() const;
    std::vector<NeuronId> getAttendedNeurons() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Development system
// Manages developmental stages and plasticity changes over time
class DevelopmentSystem : public Neuromodulator {
public:
    const char* getName() const override { return "Development"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Development-specific functionality
    void initialize(Brain* brain);
    void advanceDevelopmentalStage(TimestepDuration dt);
    DevelopmentalStage getCurrentStage() const;
    void setTargetStage(DevelopmentalStage stage);
    float getPlasticityModifier() const;
    void updateStructuralPlasticity();
    void scheduleSynaptogenesis();
    void schedulePruning();
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm