#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulation effects implemented in Phase 2

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // PHASE 2: Real modulation implemented
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation with full integration into memory, attention, and learning systems
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling - burst release on reward receipt
    void signalReward(float reward);
    
    // Reward prediction error signaling - key for reinforcement learning
    void signalRewardPredictionError(float error);
    
    // Memory consolidation - modulates working and episodic memory
    void modulateMemoryConsolidation(float consolidationStrength);
    
    // Attention and focus enhancement - increases attentional selection
    void enhanceAttentionAndFocus(float focusGain);
    
    // Learning rate modulation - affects concept formation learning rates
    void modulateConceptFormationLearning(float learningRate);
    
    // Working memory enhancement - improves WM retention and capacity
    void enhanceWorkingMemory(float capacityBoost);
    
    // Goal-directed behavior modulation - biases actions toward goals
    void modulateGoalDirectedBehavior(float goalBias);

private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements real acetylcholine dynamics for attention, learning, and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation - acetylcholine enhances attentional focus
    void enhanceAttention(float focusStrength);
    
    // Memory encoding - modulates memory encoding and consolidation
    void signalMemoryEncoding(float encodingStrength);
    void promoteMemoryConsolidation();
    
    // Learning enhancement - increases learning rate in novel situations
    void enhanceLearningRate(float learningRateModulator);
    
    // Novelty detection integration - works with novelty detection systems
    void integrateWithNoveltyDetection(float noveltySignal);
    
    // Cortical plasticity - modulates cortical plasticity rules
    void modulateCorticalPlasticity(float plasticityRate);

private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements real norepinephrine dynamics for arousal, alertness, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation - norepinephrine increases alertness and arousal
    void signalArousal(float arousalLevel);
    
    // Vigilance enhancement - increases attention to salient stimuli
    void enhanceVigilance(float vigilanceStrength);
    
    // Stress response - responds to stressors with adaptive changes
    void signalStress(float stressLevel, float recoveryRate);
    
    // Sensory processing enhancement - increases sensory signal gain
    void enhanceSensoryProcessing(float gainFactor);
    
    // Working memory capacity modulation - affects WM retention
    void modulateWorkingMemoryCapacity(float capacityModulator);

private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements real serotonin dynamics for behavioral inhibition and mood regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation - higher serotonin improves mood, reduces negative affect
    void signalMoodChange(float moodDelta);
    
    // Behavioral inhibition - serotonin increases behavioral inhibition
    void signalInhibition(float inhibitionStrength);
    
    // Social behavior modulation - affects social approach/avoidance
    void signalSocialBehavior(float socialityLevel);
    
    // Attentional modulation - serotonin affects selective attention
    void applyAttentionalModulation(float attentionalBias);
    
    // Concept formation influence - modulates pattern learning rate
    void modulateConceptFormation(float learningRate);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
