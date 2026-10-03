#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation

class Reward {
public:
    Reward(nullptr);
    ~Reward(nullptr);
    
    // Get current reward value
    float getValue(nullptr) const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset(nullptr);
    
    // Compute reward from environment state
    // TODO: PHASE 2 - Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory(nullptr) const;
    void clearHistory(nullptr);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
