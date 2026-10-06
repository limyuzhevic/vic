#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real-time dopamine dynamics for reward prediction, prediction error processing,
// and integration with curiosity-driven exploration

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
    // Real dopamine modulation of synaptic plasticity rules
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward signaling for reinforcement learning
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void integrateCuriosity(float curiosityLevel);
    
    // Reset dopamine system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm