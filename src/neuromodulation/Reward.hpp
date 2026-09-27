#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Real reward computation: biologically-inspired reward prediction

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
    // Real reward computation: neural-based reward prediction
    float computeReward(const class Observation& observation) const;
    
    // Get expected value (future reward prediction)
    float getExpectedValue() const;
    
    // Update expected value based on observed outcomes
    void updateExpectedValue(float actualValue);
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get reward prediction error
    float getPredictionError() const;
    
    // Get reward discount factor
    float getDiscountFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
