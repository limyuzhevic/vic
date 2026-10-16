#pragma once

#include "Neuromodulator.hpp"
#include "core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Reward type enumeration
enum class RewardType : uint8_t {
    Extrinsic,       // External rewards (e.g., food, money)
    Intrinsic,       // Internal rewards (e.g., curiosity, exploration)
    Social,          // Social rewards (e.g., social approval)
    Novelty,         // Novelty-based rewards (e.g., discovering new things)
    PredictionError, // Reward prediction error (dopamine-like)
    Mixed            // Combination of multiple reward types
};

// Reward component for different aspects of learning
struct RewardComponent {
    RewardType type;
    float value;
    float weight;           // Weight for combining different reward types
    float decayRate;        // How quickly this reward component decays
    double timestamp;       // When this reward was computed
    
    RewardComponent(RewardType t = RewardType::Mixed, float val = 0.0f, float weight = 1.0f, 
                   float decay = 0.01f, double ts = 0.0)
        : type(t), value(val), weight(weight), decayRate(decay), timestamp(ts) {}
};

// Reward signal for reinforcement learning
// Enhanced implementation with sophisticated reward computation

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
    // Enhanced implementation with observation-based computation
    float computeReward(const class Observation& observation, RewardType source = RewardType::Extrinsic);
    
    // Compute reward prediction error (dopamine-like signal)
    float computePredictionError(float expected, float actual);
    
    // Update reward history with timestamp
    void updateHistory(float reward);
    
    // Get reward history with timestamps
    const std::vector<float>& getHistory() const;
    const std::vector<double>& getHistoryTimestamps() const;
    void clearHistory();
    
    // Reward normalization and thresholding
    float normalizeReward(float reward, float minExpected = -1.0f, float maxExpected = 1.0f);
    bool isSignificant(float reward, float threshold = 0.01f) const;
    
    // Reward decay (temporal difference)
    void applyDecay(float decayRate);
    
    // Reward modulation based on neuromodulation signals
    void modulateWithNeuromodulator(const Neuromodulator& signal);
    
    // Exploration vs exploitation consideration
    float computeExplorationBonus(float uncertainty = 0.0f, float explorationRate = 0.1f);
    
    // Reward type management
    void setRewardType(RewardType type, float weight = 1.0f);
    RewardType getDominantRewardType() const;
    
    // Internal state access
    const std::vector<RewardComponent>& getComponents() const;
    float getTotalWeightedReward() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
