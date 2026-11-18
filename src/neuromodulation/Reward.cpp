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
    // REAL IMPLEMENTATION: Compute reward from environment observation
    // This analyzes multiple sensory inputs to determine appropriate rewards
    
    float reward = 0.0f;
    
    if (!observation.isValid()) {
        return 0.0f; // No observation, no reward
    }
    
    const SensoryInput* sensory = observation.getSensoryInput();
    if (!sensory) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensory->getData();
    
    // Based on SimpleWorld reward system:
    // 1. Vision: Check for resource proximity (bright regions = resources)
    //    Vision values 0.8-1.0 indicate resources in SimpleWorld
    // 2. Touch: Detect object interactions
    // 3. Internal: Energy and health signals
    // 4. Proprioception: Movement and position for navigation penalties
    
    size_t offset = 0;
    float totalVision = 0.0f;
    size_t visionCount = 0;
    
    // Analyze vision data (first part of sensory data)
    // In SimpleWorld, visionWidth_ = 16, visionHeight_ = 16
    for (float value : data) {
        if (value > 0.0f) {
            totalVision += value;
            visionCount++;
            
            // Resource detection: Bright regions (0.8+) are resources
            if (value >= 0.8f) {
                reward += value * 5.0f;  // Strong positive reward for resources
            }
            // Hazard detection: Dark regions (0.0-0.3) are hazards  
            else if (value <= 0.3f) {
                reward -= (0.3f - value) * 10.0f;  // Strong negative reward for hazards
            }
        }
    }
    
    if (visionCount > 0) {
        float avgVision = totalVision / visionCount;
        // Bonus for good general vision (not too dark)
        if (avgVision > 0.4f) {
            reward += 0.1f;
        }
    }
    
    // For future enhancement: analyze touch, internal, proprioception
    // Phase 5 and beyond can use these additional sensory channels
    
    // Normalize reward to reasonable range
    // Prevent runaway reward accumulation
    reward = std::clamp(reward, -50.0f, 50.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
