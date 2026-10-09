#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2: Implement real neuromodulation effects with biological models

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (phasic release)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get tonic baseline level
    virtual float getBaselineLevel() const = 0;
    
    // Get receptor activation state
    virtual float getReceptorActivation() const = 0;
    
    // Get neuromodulatory effect on neural excitability
    virtual float getExcitabilityModulation() const = 0;
    
    // Get neuromodulatory effect on working memory stability
    virtual float getWorkingMemoryModulation() const = 0;
    
    // Get neuromodulatory effect on plasticity eligibility traces
    virtual float getEligibilityTraceModulation() const = 0;
    
    // Get neuromodulatory effect on attention
    virtual float getAttentionModulation() const = 0;
    
    // Get neuromodulatory effect on learning rate
    virtual float getLearningRateModulation() const = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state (phasic/tonic dynamics)
    virtual void update(TimestepDuration dt) = 0;
    
    // Signal specific events (reward prediction error, learning, attention shifts)
    virtual void signalEvent(float value, const std::string& eventType) = 0;
    
    // Get neurotransmitter type
    virtual std::string getNeurotransmitterType() const = 0;
    
    // Cross-modulation with other neurotransmitters
    virtual void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) = 0;
    
    // Get activity-dependent release factor
    virtual float getActivityDependentRelease(float neuralActivity) const = 0;
    
protected:
    Neuromodulator() = default;
    
    // Helper: Clamp level between 0 and 1
    static float clampLevel(float level) { return std::clamp(level, 0.0f, 1.0f); }
};

// Dopamine: Reward and reinforcement learning signal
// Phase 2: Enhanced biological model with eligibility traces and working memory integration
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getBaselineLevel() const override;
    float getReceptorActivation() const override;
    float getExcitabilityModulation() const override;
    float getWorkingMemoryModulation() const override;
    float getEligibilityTraceModulation() const override;
    float getAttentionModulation() const override;
    float getLearningRateModulation() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float value, const std::string& eventType) override;
    std::string getNeurotransmitterType() const override;
    void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) override;
    float getActivityDependentRelease(float neuralActivity) const override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Learning-related signaling
    void signalLearning(float predictionError, float confidence);
    void signalWorkingMemoryUpdate(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 2: Biological model with attention gating and memory stabilization
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getBaselineLevel() const override;
    float getReceptorActivation() const override;
    float getExcitabilityModulation() const override;
    float getWorkingMemoryModulation() const override;
    float getEligibilityTraceModulation() const override;
    float getAttentionModulation() const override;
    float getLearningRateModulation() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float value, const std::string& eventType) override;
    std::string getNeurotransmitterType() const override;
    void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) override;
    float getActivityDependentRelease(float neuralActivity) const override;
    
    // Attention-related signaling
    void signalAttentionShift(float newFocusStrength);
    void signalWorkingMemoryEncoding(float memoryContent);
    void signalWorkingMemoryConsolidation(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal, vigilance, and cognitive flexibility
// Phase 2: Biological model with locus coeruleus dynamics and adaptive gain
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getBaselineLevel() const override;
    float getReceptorActivation() const override;
    float getExcitabilityModulation() const override;
    float getWorkingMemoryModulation() const override;
    float getEligibilityTraceModulation() const override;
    float getAttentionModulation() const override;
    float getLearningRateModulation() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float value, const std::string& eventType) override;
    std::string getNeurotransmitterType() const override;
    void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) override;
    float getActivityDependentRelease(float neuralActivity) const override;
    
    // Cognitive function signaling
    void signalNoveltyDetection(float noveltyStrength);
    void signalCognitiveFlexibility(float flexibilityLevel);
    void signalVigilance(float alertnessLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood regulation, impulsivity, and social behavior
// Phase 2: Biological model with raphe nucleus dynamics and behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getBaselineLevel() const override;
    float getReceptorActivation() const override;
    float getExcitabilityModulation() const override;
    float getWorkingMemoryModulation() const override;
    float getEligibilityTraceModulation() const override;
    float getAttentionModulation() const override;
    float getLearningRateModulation() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float value, const std::string& eventType) override;
    std::string getNeurotransmitterType() const override;
    void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) override;
    float getActivityDependentRelease(float neuralActivity) const override;
    
    // Behavioral regulation signaling
    void signalMoodChange(float moodLevel);
    void signalBehavioralInhibition(float inhibitionStrength);
    void signalSocialBehavior(float socialReward);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
