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
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensory = observation.getSensoryInput();
    if (!sensory) {
        return 0.0f;
    }
    
    // Extract components from sensory input
    const auto& vision = sensory->getVision();
    const auto& touch = sensory->getTouch();
    const auto& internal = sensory->getInternal();
    const auto& proprio = sensory->getProprioception();
    
    float reward = 0.0f;
    
    // Vision component: reward for finding resources, punish for hazards
    // Vision is a vector of intensity values (0-1)
    for (size_t i = 0; i < vision.size(); ++i) {
        // High intensity (bright pixels) indicates potential resources
        // Medium intensity indicates empty space
        // Low intensity (dark pixels) indicates hazards or walls
        if (vision[i] > 0.7f) {
            reward += 0.1f;  // Resource reward
        } else if (vision[i] < 0.2f) {
            reward -= 0.05f;  // Hazard penalty
        }
    }
    
    // Touch component: reward for interacting with objects
    for (size_t i = 0; i < touch.size(); ++i) {
        if (touch[i] > 0.5f) {
            reward += 0.2f;  // Object interaction reward
        }
    }
    
    // Internal component: energy and health rewards
    // internal[0] = energy level, internal[1] = health
    if (internal.size() >= 2) {
        reward += (internal[0] * 0.1f);  // Energy reward
        reward += (internal[1] * 0.05f);  // Health reward
    }
    
    // Proprioception component: stability rewards
    // Higher stability (smooth movement) is rewarded
    if (proprio.size() >= 2) {
        float stability = 1.0f - std::abs(proprio[3] - 1.0f);  // Compare to expected speed
        stability = std::max(0.0f, stability);
        reward += stability * 0.05f;
    }
    
    // Apply neuromodulatory factors (dopamine, curiosity)
    // In a real implementation, these would come from AgentBrain
    // For now, we'll use internal state variables that can be set externally
    // reward *= (1.0f + dopamineFactor);  // Would be dopamine level from AgentBrain
    // reward *= (1.0f + noveltyFactor);  // Would be novelty level from AgentBrain
    
    // Clip to reasonable range
    reward = std::clamp(reward, -10.0f, 10.0f);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
