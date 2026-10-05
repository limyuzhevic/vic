#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float discountFactor;
    float predictionError;
    float predictedValue;
    float learningRate;
    Timestamp lastUpdate;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , history()
        , discountFactor(0.99f)
        , predictionError(0.0f)
        , predictedValue(0.0f)
        , learningRate(0.1f)
        , lastUpdate(-1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {
    NLM_LOG_INFO("Reward system initialized");
}

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
    pImpl->history.push_back(pImpl->currentValue);
    
    // Keep history size manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->predictedValue = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on observation
    // This is a simplified version that could be overridden by specific implementations
    
    float reward = 0.0f;
    
    // Extract features from observation
    // This would depend on the specific observation structure
    // For now, return a simple value based on observation content
    
    // Check if observation has reward-related information
    // This is placeholder - real implementation would analyze observation
    
    // Use prediction error as reward signal
    float predictionError = pImpl->predictionError;
    if (std::abs(predictionError) > 0.0f) {
        reward = predictionError;
    }
    
    // Update prediction error for next computation
    updatePredictionError(reward);
    
    NLM_LOG_INFO(std::string("Computed reward: ") + std::to_string(reward));
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::updatePredictionError(float actualReward) {
    // Compute reward prediction error using temporal difference learning
    float predictionError = actualReward - pImpl->predictedValue;
    
    // Update predicted value using learning rule
    pImpl->predictedValue += pImpl->learningRate * predictionError;
    pImpl->predictionError = predictionError;
    
    // Apply discount factor to future predictions
    pImpl->predictedValue *= pImpl->discountFactor;
}

} // namespace nlm
