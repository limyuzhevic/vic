#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real implementation with actual dynamics
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor (modulation of learning rate)
    float getPlasticityFactor() const;
    void setPlasticityFactor(float factor);
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get reward prediction error
    float getPredictionError() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
