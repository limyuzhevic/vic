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
    // This should compute reward based on:
    // 1. Sensory input patterns (novelty, goal proximity, etc.)
    // 2. Motor outcomes (actions taken)
    // 3. Environment state changes
    // 4. Neural activity patterns
    
    // For now, return a simple exploration-driven reward
    float reward = 0.0f;
    
    // Check for novel sensory patterns
    if (observation.getNovelty() > 0.1f) {
        reward += 0.1f;  // Small novelty reward
    }
    
    // Check for goal proximity (if available)
    if (observation.getGoalProximity() < 0.3f) {
        reward += 1.0f;  // Large reward for goal proximity
    }
    
    // Check for resource proximity
    if (observation.getResourceProximity() < 0.5f) {
        reward += 0.5f;  // Moderate reward for resource proximity
    }
    
    // Negative reward for hazards
    if (observation.getHazardProximity() < 0.3f) {
        reward -= 1.0f;  // Large negative reward for hazard proximity
    }
    
    // Small time step penalty to encourage efficiency
    reward -= 0.001f;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
