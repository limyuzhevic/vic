#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Core neuromodulation system for brain regulation and plasticity

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns plasticity factor (1.0 = normal, >1 = enhanced, <1 = reduced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return getLevel() > 0.01f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Central to reward prediction error and learning modulation
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
    
    // Get prediction error
    float getPredictionError() const { return pImpl->predictionError; }
    
    // Check if dopamine signal is phasic
    bool isPhasic() const { return pImpl->phasicActive; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Modulates attention, arousal, and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get cholinergic tone for attention
    float getCholinergicTone() const { return getLevel(); }
    
    // Set attentional focus
    void setAttentionTarget(float target) { pImpl->attentionTarget = target; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Controls alertness, attention, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get arousal level
    float getArousalLevel() const { return getLevel(); }
    
    // Set stress response
    void setStressResponse(float stress) { pImpl->stressResponse = stress; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, social behavior, and impulse control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get mood signal
    float getMoodSignal() const { return getLevel(); }
    
    // Set social engagement level
    void setSocialEngagement(float engagement) { pImpl->socialEngagement = engagement; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
