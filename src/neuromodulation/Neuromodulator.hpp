#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Core system for brain state modulation and plasticity control
// Neuromodulators are chemical/biological signals that modulate neural excitability,
// synaptic plasticity, and learning processes. They are essential for adaptive behavior,
// reward learning, and exploration-exploitation balance in artificial brains.

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    // @return Human-readable name of this neuromodulator
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    // @return Current neuromodulator level (normalized to [0,1] or appropriate range)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Neuromodulators scale or modify plasticity rule parameters based on their levels
    // @return Plasticity scaling factor (1.0 = normal plasticity, >1.0 = enhanced, <1.0 = reduced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    // Called each simulation step to update neuromodulator levels based on brain state
    // @param dt Time step duration in seconds
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Positive neuromodulator that reinforces behaviors leading to rewards
// Implements reward prediction error (RPE) coding similar to biological dopamine neurons
// Drives learning toward rewarding outcomes and reduces exploration over time
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
    // Direct reward signal for experienced outcomes
    // @param reward Positive reward value (e.g., food, safety)
    void signalReward(float reward);
    
    // Reward prediction error signaling
    // Compares expected vs actual reward to guide learning
    // @param error Prediction error (actual - expected reward)
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Attention modulator that enhances signal-to-noise ratio in neural processing
// Promotes memory consolidation during wakeful states
// Supports focused attention and learning
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// arousal and alertness signal that increases neural gain
// Enhances signal detection in noisy environments
// Reduces with aging, affects attention and memory
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// mood regulation and impulse control signal
// influences social behavior and decision making
// affects learning rates and exploration
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Novelty: Novelty detection and curiosity
// detects new or unexpected sensory patterns
// drives exploration of novel stimuli
// higher levels increase learning and exploration
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute novelty from sensory input
    // Higher novelty for unexpected or rare sensory patterns
    // @param input Sensory input pattern to evaluate
    // @return Novelty score (higher = more novel)
    float computeNovelty(const class SensoryInput& input);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error: Mismatch between expected and actual outcomes
// core component of predictive coding and reinforcement learning
// drives model updating and adaptation
// negative prediction error signals need for learning, positive for stability
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Set prediction error from external source
    // Typically comes from reward prediction errors or sensory prediction errors
    // @param error Prediction error value (positive/negative)
    void setError(float error);
    
    // Reset prediction error to neutral state
    void reset();
    
    // Get current prediction error
    // @return Current prediction error value
    float getError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
