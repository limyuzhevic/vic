#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

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

} // namespace nlm