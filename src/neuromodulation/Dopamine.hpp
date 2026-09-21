#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <string>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Implements real dopamine dynamics based on reward prediction errors

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real neuromodulation effects
    float getPlasticityFactor() const;
    
    // Update neuromodulator state with realistic dynamics
    void update(TimestepDuration dt);
    
    // Reward signaling - phasic responses to rewards
    void signalReward(float reward);
    
    // Reward prediction error signaling - phasic responses to prediction errors
    void signalRewardPredictionError(float error);
    
    // Get baseline (tonic) dopamine level
    float getBaseline() const;
    
    // Get peak (phasic) dopamine level
    float getPeak() const;
    
    // Get decay rate
    float getDecayRate() const;
    
    // Get release rate
    float getReleaseRate() const;
    
    // Configure parameters for realistic dynamics
    void configure(float baseline, float peak, float decayRate, float releaseRate);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
