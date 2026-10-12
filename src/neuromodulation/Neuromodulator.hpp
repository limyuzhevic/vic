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
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void enhanceAlertness(class Brain* brain);
    void signalStress(class Brain* brain, float stressor);
    
    // Arousal level
    float getArousal() const { return pImpl->arousal; }
    void setArousal(float arousal) { pImpl->arousal = std::clamp(arousal, 0.0f, 1.0f); }
    
    // Vigilance level
    float getVigilance() const { return pImpl->vigilance; }
    void setVigilance(float vigilance) { pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f); }
    
    // Stress level
    float getStress() const { return pImpl->stress; }
    void setStress(float stress) { pImpl->stress = std::clamp(stress, 0.0f, 1.0f); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood signaling
    void enhanceMood(class Brain* brain);
    void signalSocialBehavior(class Brain* brain, bool positiveSocial);
    
    // Mood level
    float getMood() const { return pImpl->mood; }
    void setMood(float mood) { pImpl->mood = std::clamp(mood, 0.0f, 1.0f); }
    
    // Impulsivity level
    float getImpulsivity() const { return pImpl->impulsivity; }
    void setImpulsivity(float impulsivity) { pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f); }
    
    // Social bonding level
    float getSocialBonding() const { return pImpl->socialBonding; }
    void setSocialBonding(float bonding) { pImpl->socialBonding = std::clamp(bonding, 0.0f, 1.0f); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void enhanceAttention(class Brain* brain);
    void signalLearning(class Brain* brain, float predictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
