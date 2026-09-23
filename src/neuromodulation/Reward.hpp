#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Real reward computation and accumulation for reinforcement learning

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
    // Real reward computation using sensory and internal signals
    float computeReward(const class Observation& observation) const;
    
    // Reward history for replay and consolidation
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
