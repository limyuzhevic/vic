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
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    
    // Simple reward computation based on observation properties
    // In a real implementation, this would consider:
    // 1. Progress towards goals
    // 2. Novelty and surprise
    // 3. Stability and coherence
    // 4. Learning efficiency
    
    // For now, implement a basic reward signal
    float reward = 0.0f;
    
    // Check for salient features in observation
    if (observation.positionX) {
        // Reward for reaching interesting positions
        float distance = std::abs(observation.positionX - 8.0f); // Target position
        reward += (10.0f - distance) / 10.0f; // Max reward at target position
    }
    
    if (observation.positionY) {
        // Reward for vertical exploration
        reward += std::abs(observation.positionY - 8.0f) / 16.0f;
    }
    
    if (observation.orientation) {
        // Reward for orientation changes (exploration)
        float orientationChange = std::abs(observation.orientation - 0.0f);
        reward += orientationChange / 6.28f; // Normalize to [0,1]
    }
    
    if (observation.reward) {
        // Add external reward if provided
        reward += observation.reward;
    }
    
    // Apply neuromodulation factors
    // Simple novelty: small reward for state changes
    if (pImpl->currentValue != observation.positionX || 
        pImpl->accumulatedReward != observation.positionY) {
        reward += 0.1f; // Small novelty bonus
    }
    
    // Clamp reward to reasonable bounds
    reward = std::max(-1.0f, std::min(10.0f, reward));
    
    // Record the reward
    pImpl->history.push_back(reward);
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
