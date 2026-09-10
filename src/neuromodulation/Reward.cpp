#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float positiveWeight;
    float negativeWeight;
    float baselineReward;
    float maxReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             positiveWeight(1.0f), negativeWeight(0.5f), 
             baselineReward(0.0f), maxReward(10.0f) {}
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
    pImpl->currentValue = std::clamp(pImpl->currentValue, -pImpl->maxReward, pImpl->maxReward);
    pImpl->history.push_back(pImpl->currentValue);
    NLM_LOG_INFO("Reward: Added " + std::to_string(delta) + " (current: " + std::to_string(pImpl->currentValue) + ")");
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    NLM_LOG_INFO("Reward: Reset");
}

float Reward::computeReward(const Observation& observation) const {
    const auto& data = observation.getData();
    
    if (data.empty()) {
        return pImpl->baselineReward;
    }
    
    // Simple reward heuristic based on observation data
    // In a real implementation, this would integrate with:
    // - Action effects on environment
    // - Predicted outcomes
    // - Value function estimation
    // - Safety constraints
    
    float reward = pImpl->baselineReward;
    
    // Calculate simple features from observation
    float mean = 0.0f, variance = 0.0f;
    for (float val : data) {
        mean += val;
    }
    mean /= static_cast<float>(data.size());
    
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(data.size());
    
    // Reward positive deviations from mean with stability penalty
    // (This is a simplified model - real implementation would be more sophisticated)
    for (float val : data) {
        float deviation = val - mean;
        if (deviation > 0) {
            reward += deviation * pImpl->positiveWeight * 0.01f;
        } else {
            reward += deviation * pImpl->negativeWeight * 0.01f;
        }
    }
    
    // Add a small stability bonus for consistent observations
    reward -= variance * 0.05f;
    
    // Clamp to reasonable bounds
    reward = std::clamp(reward, -pImpl->maxReward, pImpl->maxReward);
    
    // Store reward for learning
    pImpl->history.push_back(reward);
    
    NLM_LOG_INFO("Reward: Computed " + std::to_string(reward) + " from observation of size " + 
                 std::to_string(data.size()));
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
