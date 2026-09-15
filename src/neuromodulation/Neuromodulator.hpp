#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for other neuromodulators that can be used in callbacks
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Novelty;
class Curiosity;
class PredictionError;

// Neuromodulator: Abstract base class for neuromodulatory signals
// Neuromodulators are chemical substances that affect neural activity, plasticity, and behavior
// They include dopamine (reward, reinforcement), acetylcholine (attention, memory), etc.
// All neuromodulators must implement:
// - getName(): returns the neuromodulator's identifier (e.g., "DA", "ACh", "NE")
// - getLevel(): current concentration or activation level
// - setLevel(): modify the current level
// - getPlasticityFactor(): influence on synaptic plasticity (1.0 = normal, >1.0 = enhanced)
// - update(): update state based on time step

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get the neuromodulator's name/identifier
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (typically 0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get plasticity modulation factor
    // Returns how much this neuromodulator affects synaptic changes
    // 1.0 = normal plasticity, >1.0 = enhanced, <1.0 = reduced
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state (called every simulation timestep)
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine (DA): Reward and reinforcement learning signal
// Mediates reward prediction, reinforcement learning, and motivation
// High levels increase plasticity for rewarded behaviors
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal reward event (increases dopamine)
    void signalReward(float reward);
    
    // Signal reward prediction error (dopamine response to surprise)
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine (ACh): Attention and memory consolidation
// Modulates attention, arousal, and working memory
// Enhances synaptic plasticity for attended stimuli
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.5f; }  // Enhances plasticity
    void update(TimestepDuration dt) override {}
};

// Norepinephrine (NE): Arousal and vigilance
// Controls alertness, attention, and stress response
// Improves signal-to-noise ratio and memory consolidation
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.2f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin (5-HT): Mood regulation and impulsivity
// Modulates mood, sleep, appetite, and impulse control
// Can both enhance and reduce plasticity depending on concentration
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 0.8f; }  // Reduces plasticity at high levels
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
