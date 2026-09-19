#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real implementation with proper P-impl pattern

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Initialize with brain reference
    virtual void initialize(class Brain* brain) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation with P-impl pattern

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine;

// Norepinephrine: Arousal and vigilance
class Norepinephrine;

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin;

// Reward signal for reinforcement learning
class Reward;

// Prediction error signal for curiosity and learning
class PredictionError;

// Curiosity drive: exploration motivation based on novelty and prediction error
class Curiosity;

// Novelty detection signal
class Novelty;
