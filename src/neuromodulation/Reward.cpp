#include "Reward.hpp"
#include "../world/Observation.hpp"
#include "../world/SimpleWorld.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
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
    // Add to history for learning
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Extract observation data (from world/Observation)
    const std::vector<float>& obsData = observation.getData();
    
    if (obsData.empty()) {
        return 0.0f;
    }
    
    // Basic reward computation based on observation patterns:
    // - Positive reward for finding resources/objects (high intensity in vision)
    // - Negative reward for hazards/obstacles (medium intensity)
    // - Small reward for exploration (change detection)
    
    float reward = 0.0f;
    
    // Process vision data (first 256 values for 16x16 vision grid)
    size_t visionSize = std::min<size_t>(256, obsData.size());
    
    // Check for resources (high vision values)
    float maxVision = 0.0f;
    float totalVision = 0.0f;
    for (size_t i = 0; i < visionSize; ++i) {
        maxVision = std::max(maxVision, obsData[i]);
        totalVision += obsData[i];
    }
    
    // Resource reward: proportional to maximum vision intensity
    if (maxVision > 0.5f) {
        reward += maxVision * 2.0f;  // Max 2.0 for resource
    }
    
    // Exploration reward: based on scene complexity/variation
    if (visionSize > 0) {
        float variation = std::abs(totalVision / visionSize - 0.5f);
        reward += variation * 0.5f;  // Up to 0.5 for exploration
    }
    
    // Negative reward for hazards (medium but not too high vision)
    for (size_t i = 0; i < visionSize; ++i) {
        if (obsData[i] > 0.3f && obsData[i] < 0.7f) {
            // Medium intensity might be a hazard
            reward -= 0.5f;
            break;  // Only penalize once per observation
        }
    }
    
    // Reward for survival (energy/health would be passed via neuromodulators)
    // For now, small baseline reward to encourage persistence
    reward += 0.01f;
    
    // Clamp to reasonable range
    reward = std::clamp(reward, -2.0f, 5.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
