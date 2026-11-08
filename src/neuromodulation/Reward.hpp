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
    
    // Compute reward from environment directly
    float computeReward(const class Environment& environment) const;
    
    // Compute reward from environment and observation
    float computeReward(const class Environment& environment, const class Observation& observation) const;
    
    // Enhanced reward computation that considers goal achievement
    // For now, returns a simple placeholder that can be overridden by subclasses
    float computeGoalBasedReward(float goalProgress, float currentPerformance) const;
    
    // Reward computation with temporal difference components
    float computeTemporalReward(float immediateReward, float futureValueEstimate, float discountFactor) const;
    
    // Compute reward based on action outcomes
    float computeActionBasedReward(const class Action& action, const class Environment& environment) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
