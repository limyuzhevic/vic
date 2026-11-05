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
    // Real reward computation from environment state
    // Implements reward prediction and reinforcement learning
    
    // Simple reward components based on observation features
    float reward = 0.0f;
    
    // Example reward sources:
    // - Goal achievement (based on observation position)
    // - Avoidance of hazards (based on observation)
    // - Energy conservation
    // - Efficiency metrics
    
    // For now, return a simple placeholder computation
    // In a real implementation, this would use the observation to compute meaningful rewards
    
    // Placeholder: compute based on observation type and value
    if (observation.type == ObservationType::Resource) {
        reward = observation.value * 1.0f;  // Positive reward for resources
    } else if (observation.type == ObservationType::Hazard) {
        reward = -observation.value * 1.0f;  // Negative reward for hazards
    } else if (observation.type == ObservationType::Goal) {
        reward = 10.0f;  // Large reward for goals
    } else if (observation.type == ObservationType::Wall) {
        reward = -0.1f;  // Small penalty for walls
    }
    
    // Record in history
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 1000) {  // Keep last 1000 rewards
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
