#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Phase 6: Real reward computation implemented
// Computes reward from environmental observations and agent actions

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
    // Phase 6: Real reward computation from observation implemented
    float computeReward(const Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

