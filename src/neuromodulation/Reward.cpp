#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

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
    constexpr float MIN_REWARD_RANGE = 1.0f;             // Minimum reasonable reward range
    constexpr float MAX_REWARD_RANGE = 100.0f;           // Maximum reasonable reward range
    constexpr float MIN_OBSERVED_REWARD = -1000.0f;     // Reasonable lower bound for observed rewards
    constexpr float MAX_OBSERVED_REWARD = 1000.0f;      // Reasonable upper bound for observed rewards
    constexpr float MIN_VALUE_FOR_OBSERVATION = -1.0f;  // Default learned value for new observations
}

struct Reward::Impl {
    // Current reward value
    float currentValue;
    
    // Accumulated reward (sum of all rewards received)
    float accumulatedReward;
    
    // Reward history for statistics
    std::vector<float> history;
    
    // Learned value function for observations
    // Maps observation features to expected value using TD learning
    std::unordered_map<uint64_t, float> observationValueMap;
    
    // Temporal difference learning parameters
    float learningRate;           // α - TD learning rate
    float discountFactor;         // γ - future reward discount factor
    float explorationRate;        // ε - random exploration probability
    
    // Reward computation parameters
    float baselineReward;         // Default reward when no pattern matches
    float rewardRange;            // Normalizes reward to [0, 1] before scaling
    
    // Internal state for learning and feedback
    float recentReward;           // Most recent reward received
    float predictionError;        // Reward prediction error (TD error)
    
    // Statistics for tracking reward distribution
    float minObservedReward;
    float maxObservedReward;
    float sumObservedReward;
    size_t countObservedReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f),
             learningRate(DEFAULT_LEARNING_RATE), discountFactor(DEFAULT_DISCOUNT_FACTOR), explorationRate(DEFAULT_EXPLORATION_RATE),
             baselineReward(DEFAULT_BASELINE_REWARD), rewardRange(DEFAULT_REWARD_RANGE),
             recentReward(0.0f), predictionError(0.0f),
             minObservedReward(std::numeric_limits<float>::max()),
             maxObservedReward(std::numeric_limits<float>::lowest()),
             sumObservedReward(0.0f), countObservedReward(0) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->currentValue += delta;
    pImpl->accumulatedReward += delta;
    
    // Update statistics
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > DEFAULT_HISTORY_LIMIT) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->recentReward = delta;
    recentReward_ = delta;
    
    // Update min/max statistics with bounds checking
    if (delta < pImpl->minObservedReward) {
        pImpl->minObservedReward = delta;
    }
    if (delta > pImpl->maxObservedReward) {
        pImpl->maxObservedReward = delta;
    }
    pImpl->sumObservedReward += delta;
    pImpl->countObservedReward++;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->observationValueMap.clear();
    recentReward_ = 0.0f;
    
    // Reset statistics
    pImpl->minObservedReward = std::numeric_limits<float>::max();
    pImpl->maxObservedReward = std::numeric_limits<float>::lowest();
    pImpl->sumObservedReward = 0.0f;
    pImpl->countObservedReward = 0;
}

float Reward::computeReward(const Observation& observation) const {
    // Compute reward based on observation features
    // This is a placeholder that can be overridden with domain-specific reward functions
    
    // Default implementation: normalize observation values
    const auto& data = observation.getData();
    if (data.empty()) {
        return pImpl->baselineReward;
    }
    
    // Compute average observation value
    float sum = 0.0f;
    for (float val : data) {
        sum += val;
    }
    float avgValue = sum / data.size();
    
    // Normalize to reward range
    float normalizedReward = (avgValue + pImpl->rewardRange) / (2.0f * pImpl->rewardRange);
    normalizedReward = std::clamp(normalizedReward, 0.0f, 1.0f);
    
    // Scale to actual reward range
    return normalizedReward * pImpl->rewardRange;
}

float Reward::computePredictionError(float reward, float currentValue, float nextValue, float discountFactor) const {
    // Temporal difference (TD) learning: delta = r + γ * V(s') - V(s)
    // This is the reward prediction error signal used in reinforcement learning
    // Positive prediction error indicates better-than-expected reward (learning signal)
    // Negative prediction error indicates worse-than-expected reward (prediction error)
    
    return reward + discountFactor * nextValue - currentValue;
}

float Reward::getValueForObservation(const Observation& observation) const {
    // Get learned value for an observation from the value function
    const auto& data = observation.getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Create a simple hash from observation data
    uint64_t hash = 0;
    for (size_t i = 0; i < data.size() && i < 4; ++i) {
        hash = (hash << 8) | (static_cast<uint64_t>(data[i]) & 0xFF);
    }
    
    auto it = pImpl->observationValueMap.find(hash);
    if (it != pImpl->observationValueMap.end()) {
        return it->second;
    }
    
    return 0.0f; // Default value
}

void Reward::updateValueForObservation(const Observation& observation, float value) {
    // Update learned value for an observation using TD learning
    const auto& data = observation.getData();
    if (data.empty()) {
        return;
    }
    
    // Create a simple hash from observation data
    uint64_t hash = 0;
    for (size_t i = 0; i < data.size() && i < 4; ++i) {
        hash = (hash << 8) | (static_cast<uint64_t>(data[i]) & 0xFF);
    }
    
    // Apply learning rate to update value
    pImpl->observationValueMap[hash] = value;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getMeanReward() const {
    if (pImpl->countObservedReward == 0) {
        return 0.0f;
    }
    return pImpl->sumObservedReward / pImpl->countObservedReward;
}

float Reward::getMaxReward() const {
    if (pImpl->countObservedReward == 0) {
        return 0.0f;
    }
    return pImpl->maxObservedReward;
}

float Reward::getMinReward() const {
    if (pImpl->countObservedReward == 0) {
        return 0.0f;
    }
    return pImpl->minObservedReward;
}

} // namespace nlm

// Helper function to clamp float value between min and max bounds
inline float clamp(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

// Helper function to compute standard deviation from statistics
float computeStandardDeviation(float mean, float sumSq, size_t count) {
    if (count < 2) return 0.0f;
    float variance = (sumSq - count * mean * mean) / (count - 1);
    return std::sqrt(variance);
}

// Configuration methods implementation
void Reward::setLearningRate(float rate) {
    pImpl->learningRate = clamp(rate, MIN_LEARNING_RATE, MAX_LEARNING_RATE);
}

float Reward::getLearningRate() const {
    return pImpl->learningRate;
}

void Reward::setDiscountFactor(float factor) {
    pImpl->discountFactor = clamp(factor, MIN_DISCOUNT_FACTOR, MAX_DISCOUNT_FACTOR);
}

float Reward::getDiscountFactor() const {
    return pImpl->discountFactor;
}

void Reward::setExplorationRate(float rate) {
    pImpl->explorationRate = clamp(rate, 0.0f, 1.0f);
}

float Reward::getExplorationRate() const {
    return pImpl->explorationRate;
}

void Reward::setBaselineReward(float reward) {
    pImpl->baselineReward = reward;
}

float Reward::getBaselineReward() const {
    return pImpl->baselineReward;
}

void Reward::setRewardRange(float range) {
    pImpl->rewardRange = clamp(range, MIN_REWARD_RANGE, MAX_REWARD_RANGE);
}

float Reward::getRewardRange() const {
    return pImpl->rewardRange;
}

} // namespace nlm
