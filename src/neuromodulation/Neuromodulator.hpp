#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Neuromodulators are chemicals that modulate neural activity and plasticity
// They carry information about rewards, prediction errors, novelty, and other
// contextual signals that influence learning and behavior

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name (e.g., "Dopamine", "Curiosity", "Novelty")
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 typical range)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a multiplicative factor that scales learning rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on recent neural activity
    // dt: timestep duration
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Dopamine encodes reward prediction errors and drives reward-seeking behavior
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal a reward event (positive prediction error)
    void signalReward(float reward);
    
    // Signal a prediction error (difference between expected and actual reward)
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// ACh enhances attention, sensory processing, and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// NE regulates arousal, vigilance, and stress responses
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT regulates mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
