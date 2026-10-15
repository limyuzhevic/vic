#pragma once

#include "Neuromodulator.hpp"
#include <vector>
#include <string>

namespace nlm {

// Reward signal for reinforcement learning

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
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward configuration
    void setExtrinsicWeight(float weight);
    void setIntrinsicWeight(float weight);
    void setExplorationWeight(float weight);
    void setCuriosityWeight(float weight);
    void setRewardFunction(enum RewardFunction func);
    
    // Get reward configuration
    float getExtrinsicWeight() const;
    float getIntrinsicWeight() const;
    float getExplorationWeight() const;
    float getCuriosityWeight() const;
    enum RewardFunction getRewardFunction() const;
    
    // Statistics
    float getStatisticsMean() const;
    float getStatisticsVariance() const;
    float getStatisticsMin() const;
    float getStatisticsMax() const;
    int getStatisticsCount() const;
    
    // State tracking
    float getConsecutiveSuccesses() const;
    float getMaxRecentReward() const;
    float getMinRecentReward() const;
    
    // Action tracking for curiosity
    void addAction(float actionValue);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward function types
enum RewardFunction {
    Additive,
    Multiplicative,
    TanhScaling,
    Sigmoid,
    Max,
    WeightedSum
};

} // namespace nlm
