#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include "../environment/Observation.hpp"

namespace nlm {

// Enhanced Reward signal for reinforcement learning with temporal difference learning
// Implements real reward computation, reward prediction error (RPE), and eligibility traces

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
    
    // Reset reward history and statistics
    void resetHistory();
    
    // Compute reward from environment state
    float computeReward(const Observation& observation) const;
    
    // Compute temporal difference update
    float computeTemporalDifference(float currentReward, float discountFactor = 0.95f, float learningRate = 0.1f);
    
    // Update eligibility trace
    void updateEligibilityTrace(float traceDecay = 0.9f);
    
    // Get reward statistics
    float getMeanReward() const;
    float getRewardVariance() const;
    float getMaxReward() const;
    float getMinReward() const;
    float getRecentAverage() const;
    
    // Get internal state
    float getPredictionError() const;
    float getEligibilityTrace() const;
    float getValuePrediction() const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
