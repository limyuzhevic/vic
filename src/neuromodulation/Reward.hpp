#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// Real reward computation with biological plausibility
// Integrates with neuromodulation for reward-based learning

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Get current reward value
    float getValue() const override;
    void setValue(float value) override;
    
    // Accumulate reward
    void add(float delta) override;
    
    // Reset accumulated reward
    void reset() override;
    
    // Compute reward from environment state
    // Real reward computation based on observation features
    float computeReward(const class Observation& observation) const override;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward statistics
    struct RewardStats {
        float currentValue;
        float accumulatedReward;
        float meanReward;
        float maxReward;
        float minReward;
        float rewardVariance;
        size_t updateCount;
    };
    RewardStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
