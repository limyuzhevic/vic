#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float predictedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), predictedReward(0.0f) {}
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
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation, 
                           const std::vector<float>& recentRewards,
                           float predictedReward) const {
    // Compute reward based on observation and prediction error
    
    float baseReward = 0.0f;
    
    // TODO: Implement sophisticated reward computation based on observation
    // For now, compute based on observation type and proximity to goals
    // This is a placeholder that should be expanded for Phase 2+
    
    // Base reward from observation could come from:
    // - Resource proximity (positive)
    // - Hazard proximity (negative)
    // - Novelty (encourages exploration)
    // - Developmental appropriateness
    
    // Compute prediction error
    float predictionError = predictedReward - getValue();
    
    // Combine base reward with prediction error
    float totalReward = baseReward + predictionError * 0.1f; // Small weight on prediction error
    
    // Apply learning-rate-like scaling
    totalReward = totalReward * 0.1f; // Scale down for stability
    
    return totalReward;
}

float Reward::getPredictedReward() const {
    return pImpl->predictedReward;
}

void Reward::updatePrediction(float actualReward) {
    // Simple exponential moving average for prediction
    pImpl->predictedReward = 0.9f * pImpl->predictedReward + 0.1f * actualReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

Reward::RewardStats Reward::getStats() const {
    RewardStats stats;
    stats.count = pImpl->history.size();
    
    if (stats.count == 0) return stats;
    
    // Compute statistics
    float sum = 0.0f;
    float sumSquares = 0.0f;
    stats.min = pImpl->history[0];
    stats.max = pImpl->history[0];
    
    for (float reward : pImpl->history) {
        sum += reward;
        sumSquares += reward * reward;
        
        if (reward < stats.min) stats.min = reward;
        if (reward > stats.max) stats.max = reward;
    }
    
    stats.mean = sum / stats.count;
    
    if (stats.count > 1) {
        float variance = (sumSquares - (sum * sum) / stats.count) / (stats.count - 1);
        stats.variance = variance > 0.0f ? variance : 0.0f;
    }
    
    return stats;
}
