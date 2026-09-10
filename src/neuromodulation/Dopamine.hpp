#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Dopamine signal: reward prediction error and motivation
// Updates based on received rewards and prediction errors
// Levels modulate plasticity and learning rate

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Get dopamine system name
    const char* getName() const;
    
    // Get current dopamine level (0.0 to 1.0)
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor modulated by dopamine
    float getPlasticityFactor() const;
    
    // Update dopamine state based on time
    void update(TimestepDuration dt);
    
    // Signal a received reward
    void signalReward(float reward);
    
    // Signal a reward prediction error
    void signalRewardPredictionError(float error);
    
    // Get current prediction error signal
    float getPredictionError() const;
    
    // Get current reward signal
    float getRewardSignal() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm