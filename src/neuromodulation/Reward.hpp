#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Modern implementation with prediction error computation
class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state (Phase 2 implementation)
    // Uses observation and prediction error to compute intrinsic/extrinsic rewards
    float computeReward(const class Observation& observation, float predictionError = 0.0f) const;
    
    // Reward history with decay
    const std::vector<float>& getHistory() const;
    void clearHistory();
    void applyDecay(float decayRate);
    
    // Reward prediction error computation
    float computePredictionError(float actual, float predicted) const;
    void updatePrediction(float predicted);
    
    // Configuration
    void configure(float explorationBonus, float intrinsicWeight, float extrinsicWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
