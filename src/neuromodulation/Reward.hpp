#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>
#include <vector>

namespace nlm {

// Reward signal for reinforcement learning
// Computes reward from environmental observations and learned value functions
// Implements temporal difference learning and reward prediction
// Uses dopamine-like signals for credit assignment across time

// Constants for reward computation and learning
namespace {
    constexpr float DEFAULT_DISCOUNT_FACTOR = 0.95f;      // γ - temporal discount (0.9-0.99 typical)
    constexpr float DEFAULT_LEARNING_RATE = 0.1f;         // α - TD learning rate (0.01-0.5 typical)
    constexpr float DEFAULT_EXPLORATION_RATE = 0.01f;     // ε - random exploration probability
    constexpr float DEFAULT_BASELINE_REWARD = 0.0f;       // Default reward when no pattern matches
    constexpr float DEFAULT_REWARD_RANGE = 10.0f;         // Scales normalized reward to actual values
    constexpr size_t DEFAULT_HISTORY_LIMIT = 1000;       // Maximum reward history entries
    constexpr float MIN_DISCOUNT_FACTOR = 0.1f;          // Lower bound for discount factor
    constexpr float MAX_DISCOUNT_FACTOR = 1.0f;          // Upper bound for discount factor
    constexpr float MIN_LEARNING_RATE = 0.0f;            // Lower bound for learning rate
    constexpr float MAX_LEARNING_RATE = 1.0f;            // Upper bound for learning rate
}

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward over time
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Based on observation features and learned value function
    float computeReward(const class Observation& observation) const;
    
    // Compute reward prediction error
    // delta = r + γ * V(s') - V(s) - Temporal difference learning
    float computePredictionError(float reward, float currentValue, float nextValue, float discountFactor = DEFAULT_DISCOUNT_FACTOR) const;
    
    // Get learned value for an observation
    float getValueForObservation(const Observation& observation) const;
    void updateValueForObservation(const Observation& observation, float value);
    
    // Reward history management
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get recent reward for curiosity computation and feedback
    float getRecentReward() const;
    
    // Get reward statistics
    float getMeanReward() const;
    float getMaxReward() const;
    float getMinReward() const;
    
    // Configuration parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setDiscountFactor(float factor);
    float getDiscountFactor() const;
    
    void setExplorationRate(float rate);
    float getExplorationRate() const;
    
    void setBaselineReward(float reward);
    float getBaselineReward() const;
    
    void setRewardRange(float range);
    float getRewardRange() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // State variables
    float currentValue_;
    float accumulatedReward_;
    float recentReward_;
    float predictionError_;
    
    // Statistics
    float minObservedReward_;
    float maxObservedReward_;
    float sumObservedReward_;
    size_t countObservedReward_;
};

} // namespace nlm
