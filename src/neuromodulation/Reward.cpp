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
    const auto* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const auto& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Parse sensory data based on type
    const char* type = sensoryInput->getType();
    float reward = 0.0f;
    
    // Vision reward (most common for navigation tasks)
    if (strcmp(type, "Vision") == 0) {
        // Vision data structure: 16 rays of vision + 8 touch + 4 internal + 6 proprioception
        // First 16 values are vision rays (0-1, where 1 = empty, 0 = object)
        // Vision values above 0.5 indicate resources (bright objects)
        // Vision values below 0.35 indicate hazards (dark objects/walls)
        
        // Look for resources (bright objects in forward vision)
        float forwardVisionSum = 0.0f;
        int forwardVisionCount = 0;
        
        // Forward cone rays: 5-10 (center rays in 16-ray vision)
        for (int i = 5; i <= 10 && i < static_cast<int>(data.size()); ++i) {
            if (data[i] < 0.5f) {  // Object detected
                forwardVisionSum += (0.5f - data[i]);  // Stronger reward for darker (more resource-like) objects
                forwardVisionCount++;
            }
        }
        
        if (forwardVisionCount > 0) {
            float avgForwardVision = forwardVisionSum / forwardVisionCount;
            reward += avgForwardVision * 10.0f;  // Resource reward
        }
        
        // Hazard penalty (dark objects that could be hazards)
        for (int i = 0; i < 16 && i < static_cast<int>(data.size()); ++i) {
            if (data[i] < 0.35f) {  // Dark object detected
                reward -= (0.35f - data[i]) * 5.0f;  // Hazard penalty
            }
        }
        
        // Bonus for being close to resources (vision < 0.3 in forward cone)
        for (int i = 5; i <= 10 && i < static_cast<int>(data.size()); ++i) {
            if (data[i] < 0.3f) {  // Very close to bright resource
                reward += 15.0f;  // Proximity bonus
                break;
            }
        }
    }
    
    // Internal signals reward - based on internal brain state
    else if (strcmp(type, "InternalSignals") == 0) {
        // Internal signals might represent things like energy, need, etc.
        // For now, reward based on positive internal states
        for (float value : data) {
            if (value > 0.5f) {  // Positive internal state
                reward += value * 5.0f;
            }
        }
    }
    
    // Audio or other sensory modalities could be added here
    
    // Clamp reward to reasonable range
    reward = std::clamp(reward, -5.0f, 20.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
