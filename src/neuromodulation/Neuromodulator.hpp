#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulatory systems that affect neural plasticity and behavior

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (normalized 0-1)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns plasticity modulation factor (1.0 = normal, <1.0 = reduced, >1.0 = enhanced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on current timestep
    virtual void update(TimestepDuration dt) = 0;
    
    // Get baseline level (steady-state concentration)
    virtual float getBaselineLevel() const = 0;
    
    // Check if neuromodulator is currently active
    virtual bool isActive() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements phasic and tonic dopamine dynamics with reward prediction error
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get baseline dopamine level (steady-state)
    float getBaselineLevel() const override;
    
    // Check if dopamine is in phasic burst state
    bool isActive() const override;
    
    // Reward signaling - phasic burst response to unexpected rewards
    void signalReward(float reward);
    
    // Reward prediction error signaling - core RL mechanism
    void signalRewardPredictionError(float error);
    
    // Set baseline dopamine level for tonic firing
    void setBaselineLevel(float level);
    
    // Configure dopamine dynamics parameters
    void configure(float baseline, float peak, float decayRate, float releaseRate);
    
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
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
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
};

} // namespace nlm
