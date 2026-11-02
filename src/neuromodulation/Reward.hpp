#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Implements real-time reward computation and prediction

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward with time step
    void add(float delta, float timestep = 1.0f);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state and agent's history
    float computeReward(const class Observation& observation, 
                       const std::vector<float>& recentRewards = std::vector<float>(),
                       float predictedReward = 0.0f) const;
    
    // Get reward prediction for next action
    float getPredictedReward() const;
    
    // Update prediction based on actual outcome
    void updatePrediction(float actualReward);
    
    // Get reward history for analysis
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get statistics on reward distribution
    struct RewardStats {
        float mean = 0.0f;
        float variance = 0.0f;
        float min = 0.0f;
        float max = 0.0f;
        size_t count = 0;
    };
    
    RewardStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
