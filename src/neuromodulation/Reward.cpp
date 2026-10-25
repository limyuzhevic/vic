#include "Reward.hpp"
#include "../../environment/SensoryInput.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float maxHistorySize;
    float predictionError;
    float learningRate;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             maxHistorySize(1000.0f), predictionError(0.0f), learningRate(0.01f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    // Apply smoothing to prevent sudden changes
    pImpl->currentValue = 0.9f * pImpl->currentValue + 0.1f * value;
    pImpl->accumulatedReward += value;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    setValue(pImpl->currentValue + delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->predictionError = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Implement real reward computation based on observation
    // This is a simplified version with multiple factors
    
    float reward = 0.0f;
    
    // Get sensory input from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return reward;
    }
    
    // Reward components based on different factors
    
    // 1. Exploration reward: based on sensory input novelty
    float explorationBonus = computeExplorationReward(sensoryInput);
    reward += explorationBonus;
    
    // 2. Consummatory reward: based on specific inputs
    float consummatoryReward = computeConsummatoryReward(sensoryInput);
    reward += consummatoryReward;
    
    // 3. Predictive reward: based on prediction error
    reward += pImpl->predictionError;
    
    // 4. Survival reward: negative for damaging inputs
    float survivalCost = computeSurvivalCost(sensoryInput);
    reward -= survivalCost;
    
    return reward;
}

// Helper methods for reward computation
float Reward::computeExplorationReward(const SensoryInput* sensoryInput) const {
    if (!sensoryInput) return 0.0f;
    
    // Simple exploration reward based on sensory data statistics
    const auto& data = sensoryInput->getData();
    if (data.empty()) return 0.0f;
    
    // Calculate variance of sensory data (measures novelty/unpredictability)
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float variance = 0.0f;
    for (float value : data) {
        variance += (value - mean) * (value - mean);
    }
    variance /= data.size();
    
    // Return variance-based exploration reward
    return variance * 0.01f;
}

float Reward::computeConsummatoryReward(const SensoryInput* sensoryInput) const {
    if (!sensoryInput) return 0.0f;
    
    const char* inputType = sensoryInput->getType();
    float value = 0.0f;
    
    // Map input type string to reward values
    if (std::string(inputType) == "Vision") {
        value = 0.01f;  // Vision is generally neutral
    } else if (std::string(inputType) == "Audio") {
        value = 0.005f;  // Audio is neutral but potentially informative
    } else if (std::string(inputType) == "InternalSignals") {
        value = 0.002f;  // Internal signals are subtle indicators
    } else {
        value = 0.001f;  // Unknown type: small neutral reward
    }
    
    return value;
}

float Reward::computeSurvivalCost(const SensoryInput* sensoryInput) const {
    if (!sensoryInput) return 0.0f;
    
    float cost = 0.0f;
    const char* inputType = sensoryInput->getType();
    
    // Always apply a small base cost for sensory processing
    cost = 0.0001f;
    
    // Higher cost for intense or novel inputs
    const auto& data = sensoryInput->getData();
    if (!data.empty()) {
        float maxValue = *std::max_element(data.begin(), data.end());
        float minValue = *std::min_element(data.begin(), data.end());
        float intensity = (std::abs(maxValue) + std::abs(minValue)) / 2.0f;
        cost += intensity * 0.0005f;
    }
    
    return cost;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
