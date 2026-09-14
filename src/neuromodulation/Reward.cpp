#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <limits>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    std::unordered_map<ActionType, float> actionRewards;
    float baselineReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baselineReward(0.0f) {}
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
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Simple reward computation based on sensory input
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return pImpl->baselineReward;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    if (data.empty()) {
        return pImpl->baselineReward;
    }
    
    // Compute reward based on sensory signal statistics
    float sum = 0.0f;
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    
    for (float value : data) {
        sum += value;
        minVal = std::min(minVal, value);
        maxVal = std::max(maxVal, value);
    }
    
    float mean = sum / data.size();
    
    // Reward for being in active/extreme states
    float reward = 0.0f;
    
    // Positive reward for high variance (active environment)
    float variance = 0.0f;
    for (float value : data) {
        variance += (value - mean) * (value - mean);
    }
    variance /= data.size();
    reward += std::min(variance * 0.5f, 1.0f);
    
    // Reward for extreme values (e.g., high intensity)
    float intensity = (std::max(std::abs(maxVal), std::abs(minVal)));
    reward += std::min(intensity * 0.3f, 1.0f);
    
    // Positive reward for active changes
    float changeRate = 0.0f;
    if (!pImpl->history.empty()) {
        // Look at recent changes in sensory input
        changeRate = std::abs(data[0] - mean) * 0.1f;
    }
    reward += changeRate;
    
    // Apply action-specific rewards if available
    if (data.size() > 0) {
        // For now, simple reward based on first observation
        // Cast index to ActionType safely
        ActionType actionType = static_cast<ActionType>(data[0] * 100.0f) % ActionType::Custom;
        auto actionIt = pImpl->actionRewards.find(actionType);
        if (actionIt != pImpl->actionRewards.end()) {
            reward += actionIt->second * 0.5f;
        }
    }
    
    // Clamp reward to reasonable range
    reward = std::max(std::min(reward, 2.0f), -1.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
