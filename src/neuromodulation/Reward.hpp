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
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Compute advanced reward with action history
    // For advanced users who want custom reward functions
    float computeAdvancedReward(const class Observation& observation,
                               const std::vector<float>& actionHistory) const;
    
    // Get intrinsic reward from sensory data
    // Extracted for testing/debugging purposes
    float computeIntrinsicReward(const std::vector<float>& data) const;
    
    // Get extrinsic reward from learned preferences
    // Extracted for testing/debugging purposes
    float getExtrinsicReward() const;
    
    // Get reward scaling factor
    // Extracted for testing/debugging purposes
    float getRewardScale() const;
    
    // Get reward history statistics
    float getAverageReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
