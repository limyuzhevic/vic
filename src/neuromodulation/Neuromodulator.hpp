#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implements realistic concentration-based neuromodulation

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Update neuromodulator dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulatory effect on plasticity (1.0 = no change, <1.0 = suppression, >1.0 = enhancement)
    virtual float getPlasticityFactor() const = 0;
    
    // Get neuromodulatory effect on neural excitability (0.0 = hypo-excitable, 1.0 = baseline, >1.0 = hyper-excitable)
    virtual float getExcitabilityModifier() const = 0;
    
    // Get neuromodulatory effect on learning rate (1.0 = baseline)
    virtual float getLearningRateModifier() const = 0;
    
    // Get neuromodulatory effect on memory consolidation
    virtual float getMemoryConsolidationStrength() const = 0;
    
    // Reset neuromodulator to baseline state
    virtual void reset() = 0;
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements cholinergic modulation of attention and memory processes
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModifier() const override;
    float getLearningRateModifier() const override;
    float getMemoryConsolidationStrength() const override;
    void reset() override;
    
    // Attention and memory signaling
    void signalAttention(float attentionStrength);
    void signalMemoryDemand(float memoryLoad);
    void coupleWithArousal(float arousalLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements noradrenergic modulation of arousal and attention
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModifier() const override;
    float getLearningRateModifier() const override;
    float getMemoryConsolidationStrength() const override;
    void reset() override;
    
    // Arousal and vigilance signaling
    void signalVigilance(float vigilanceLevel);
    void signalAlertness(float alertness);
    void signalStressResponse(float stressLevel);
    void adaptToInputLoad(float inputLoad);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements serotonergic modulation of mood and behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getExcitabilityModifier() const override;
    float getLearningRateModifier() const override;
    float getMemoryConsolidationStrength() const override;
    void reset() override;
    
    // Mood and behavior signaling
    void signalMoodChange(float moodChange);
    void signalImpulsivity(float impulsivityLevel);
    void signalSocialBonding(float bondingLevel);
    void adjustHomeostasis(float energyLevel, float nutrients);
    
    // Homeostatic regulation
    float getSatietyLevel() const;
    void incrementSatiety(float amount);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
