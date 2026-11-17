#include "Reward.hpp"

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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Implement real reward computation from observation
    // Extract meaningful features from sensory data and compute reward
    if (!observation.isValid()) return 0.0f;
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    const std::vector<float>& data = sensoryInput->getData();
    
    // Compute reward based on sensory pattern analysis
    float reward = computeIntrinsicReward(data);
    
    // Apply extrinsic rewards based on learned preferences
    float extrinsic = getExtrinsicReward();
    
    // Combine intrinsic and extrinsic rewards
    reward = (reward + extrinsic) * getRewardScale();
    
    // Record reward in history for statistics
    pImpl->history.push_back(reward);
    
    return reward;
}

// Compute intrinsic reward from sensory pattern
float Reward::computeIntrinsicReward(const std::vector<float>& data) const {
    float reward = 0.0f;
    
    // Check for salient features in sensory data
    // Based on SimpleWorld environment rewards:
    // - Resource consumption gives positive reward
    // - Hazard hits give negative reward
    
    // Example implementation: analyze pattern characteristics
    if (!data.empty()) {
        // Check if pattern indicates resource proximity (high sensory values)
        float maxValue = 0.0f;
        float minValue = 100000.0f;
        
        for (float value : data) {
            maxValue = std::max(maxValue, value);
            minValue = std::min(minValue, value);
        }
        
        // High contrast patterns might indicate resource presence
        float contrast = maxValue - minValue;
        
        // Moderate contrast with positive bias suggests reward
        if (contrast > 5.0f && maxValue > 3.0f) {
            reward += 2.0f;  // Resource-like signal
        } else if (contrast < 2.0f && minValue < -1.0f) {
            reward -= 3.0f;  // Hazard-like signal (negative values)
        }
    }
    
    // Small positive reward for active sensing
    if (data.size() > 0) reward += 0.1f;
    
    return reward;
}

// Get extrinsic reward from learned preferences
float Reward::getExtrinsicReward() const {
    // In a real implementation, this would look up learned preferences
    // from associative memory or concept formation
    // For now, return 0.0f (no learned extrinsic rewards)
    return 0.0f;
}

// Get reward scaling factor
float Reward::getRewardScale() const {
    // Scale rewards to appropriate range for neural learning
    return 1.0f;
}

// Enhanced reward computation for advanced users
float Reward::computeAdvancedReward(const Observation& observation,
                                    const std::vector<float>& actionHistory) const {
    float reward = computeReward(observation);
    
    // Apply additional factors based on action history
    for (float actionReward : actionHistory) {
        reward += actionReward * 0.5f;  // Weighted contribution
    }
    
    return reward;
}

float Reward::computeIntrinsicReward(const std::vector<float>& data) const {
    float reward = 0.0f;
    
    // Check for salient features in sensory data
    // Based on SimpleWorld environment rewards:
    // - Resource consumption gives positive reward
    // - Hazard hits give negative reward
    
    // Example implementation: analyze pattern characteristics
    if (!data.empty()) {
        // Check if pattern indicates resource proximity (high sensory values)
        float maxValue = 0.0f;
        float minValue = 100000.0f;
        
        for (float value : data) {
            maxValue = std::max(maxValue, value);
            minValue = std::min(minValue, value);
        }
        
        // High contrast patterns might indicate resource presence
        float contrast = maxValue - minValue;
        
        // Moderate contrast with positive bias suggests reward
        if (contrast > 5.0f && maxValue > 3.0f) {
            reward += 2.0f;  // Resource-like signal
        } else if (contrast < 2.0f && minValue < -1.0f) {
            reward -= 3.0f;  // Hazard-like signal (negative values)
        }
    }
    
    // Small positive reward for active sensing
    if (data.size() > 0) reward += 0.1f;
    
    return reward;
}

// Get extrinsic reward from learned preferences
float Reward::getExtrinsicReward() const {
    // In a real implementation, this would look up learned preferences
    // from associative memory or concept formation
    // For now, return 0.0f (no learned extrinsic rewards)
    return 0.0f;
}

// Get reward scaling factor
float Reward::getRewardScale() const {
    // Scale rewards to appropriate range for neural learning
    return 1.0f;
}

// Get reward history statistics
float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float sum = 0.0f;
    for (float r : pImpl->history) sum += r;
    return sum / pImpl->history.size();
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float max = pImpl->history[0];
    for (float r : pImpl->history) max = std::max(max, r);
    return max;
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float min = pImpl->history[0];
    for (float r : pImpl->history) min = std::min(min, r);
    return min;
}

} // namespace nlm
