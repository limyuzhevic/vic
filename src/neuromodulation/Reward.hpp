#pragma once

#include "Neuromodulator.hpp"
#include "../world/Observation.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// Reward signal for reinforcement learning
// Implements real reward computation based on environmental interaction
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
    // Uses sophisticated reward functions based on observation
    float computeReward(const Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set reward function for custom reward computation
    void setRewardFunction(std::function<float(const Observation&)> func) {
        rewardFunction_ = func;
    }
    
    // Configure reward shaping
    void configure(float baseReward, float maxReward, float minReward);
    void setResourceReward(float value) { resourceReward_ = value; }
    void setHazardReward(float value) { hazardReward_ = value; }
    void setMovementReward(float value) { movementReward_ = value; }
    void setExplorationReward(float value) { explorationReward_ = value; }
    
    // Get total reward statistics
    float getTotalReward() const { return totalReward_; }
    size_t getSampleCount() const { return sampleCount_; }
    
    // Check if reward is above threshold
    bool isHighReward(float threshold = 0.5f) const;
    bool isLowReward(float threshold = -0.1f) const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Reward shaping parameters
    float resourceReward_;
    float hazardReward_;
    float movementReward_;
    float explorationReward_;
    float baseReward_;
    float totalReward_;
    size_t sampleCount_;
    std::function<float(const Observation&)> rewardFunction_;
};

} // namespace nlm
