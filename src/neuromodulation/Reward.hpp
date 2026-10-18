#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation

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
    
    // Compute reward from environment state
    // Implements real reward computation with multiple components:
    // - External rewards from environmental resources/hazards
    // - Prediction error (surprise) for learning
    // - Novelty rewards for exploration
    // - Action outcome rewards
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
