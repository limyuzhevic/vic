#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2: Real neuromodulation effects affecting neural dynamics and plasticity

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Higher values increase synaptic plasticity and learning rate
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on neural activity
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 2: Real dopamine dynamics for reward prediction and plasticity modulation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling: encode actual reward received
    void signalReward(float reward);
    // Reward prediction error: encode difference between predicted and actual reward
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 2: Real acetylcholine dynamics affecting working memory and attention
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention and working memory capacity
    void modulateAttention(float attentionStrength);
    void enhanceMemoryConsolidation();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Phase 2: Real norepinephrine dynamics affecting arousal and signal detection
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate arousal and vigilance
    void increaseArousal(float levelIncrease);
    void enhanceSignalDetection(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Phase 2: Real serotonin dynamics affecting mood and behavioral control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate mood and behavioral control
    void improveMood(float moodLevel);
    void reduceImpulsivity(float controlStrength);
    void facilitateSocialBehavior(float socialBonding);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
