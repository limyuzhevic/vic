#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>

namespace nlm {

// Reward signal for reinforcement learning
// Computes rewards from environmental observations for reinforcement learning

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
    // Uses sensory input data to calculate meaningful rewards based on context
    float computeReward(const class Observation& observation) const;
    
    // Reward history management
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get average reward from history
    float getAverageReward(size_t window = 100) const;
    
    // Check if reward is positive
    bool isPositive() const;
    
    // Get reward magnitude (absolute value)
    float getMagnitude() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
