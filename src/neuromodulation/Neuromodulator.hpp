#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2 implements real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0, normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor that modifies baseline plasticity (1.0 = normal)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state for current timestep
    virtual void update(TimestepDuration dt) = 0;
    
    // Get modifier for neural excitability (affects firing threshold)
    virtual float getExcitabilityModifier() const = 0;
    
    // Get modifier for memory consolidation
    virtual float getMemoryConsolidationModifier() const = 0;
    
    // Get modifier for attention
    virtual float getAttentionModifier() const = 0;
    
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
class Acetylcholine : public Neuromodulator {
public:
    ACh();
    ~ACh() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effects
    float getMemoryConsolidationFactor() const;
    
    // Attention modulation
    float getAttentionFocusFactor() const;
    
    // Enable/disable attention modulation
    void enableAttention(bool enable) { attentionEnabled_ = enable; }
    
    // Set memory consolidation rate
    void setMemoryConsolidationRate(float rate) { consolidationRate_ = rate; }
    
    // Signal for new learning episode
    void signalLearningEvent();
    
private:
    struct Impl;
    Impl* pImpl;
    bool attentionEnabled_;
    float consolidationRate_;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    NE();
    ~NE() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal effects (overall neural excitability)
    float getArousalLevel() const;
    
    // Vigilance modulation (attention to salient stimuli)
    float getVigilanceFactor() const;
    
    // Signal for unexpected stimuli
    void signalSurprise(float predictionError);
    
    // Enable/disable stress response
    void enableStressResponse(bool enable) { stressEnabled_ = enable; }
    
    // Set recovery rate (how quickly NE returns to baseline)
    void setRecoveryRate(float rate) { recoveryRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    bool stressEnabled_;
    float recoveryRate_;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation (affects reward sensitivity)
    float getMoodModifier() const;
    
    // Social behavior modulation
    float getSocialFactor() const;
    
    // Impulse control
    float getImpulseControl() const;
    
    // Signal for social interaction
    void signalSocialInteraction(float socialValue);
    
    // Enable/disable social learning
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    
    // Set reset rate (how quickly serotonin returns to baseline after social events)
    void setResetRate(float rate) { resetRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    bool socialLearningEnabled_;
    float resetRate_;
};

} // namespace nlm
