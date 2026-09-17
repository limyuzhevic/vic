#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Neuromodulator: Abstract base for neuromodulatory signals
// Integrates prediction error with synaptic eligibility traces for learning

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
// Integrates prediction error with synaptic eligibility traces for learning

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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Get prediction error
    float getPredictionError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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

// Novelty: Novelty detection and exploration driver
// Detects unexpected sensory input and drives exploration behavior

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Detect novelty from sensory input
    void detectNovelty(const SensoryInput& input);
    
    // Get current novelty level
    float getLevel() const;
    
    // Get contribution to prediction error
    float getPredictionErrorContribution() const;
    
    // Update novelty system state
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity: Exploration motivation system
// Combines novelty detection and prediction error to drive exploration

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get current curiosity level
    float getLevel() const;
    
    // Get exploration bias for action selection
    float getExplorationBias() const;
    
    // Get plasticity factor for learning
    float getPlasticityFactor() const;
    
    // Update curiosity system state
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionError: Prediction error signal for learning
// PLACEHOLDER - Phase 2
class PredictionError {
public:
    const char* getName() const { return "PredError"; }
    float getLevel() const { return 0.0f; }
    void setLevel(float level) {}
    float getPlasticityFactor() const { return 1.0f; }
    void update(TimestepDuration dt) {}
};

} // namespace nlm
