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
    // Improved reward computation from observation
    // PLACEHOLDER: More sophisticated reward based on energy, health, and environment
    float reward = 0.0f;
    
    // Base reward for survival
    if (observation.agentEnergy > 50.0f) {
        reward += 0.01f;
    }
    
    // Negative reward for low energy
    if (observation.agentEnergy < 20.0f) {
        reward -= 0.02f;
    }
    
    // Reward for good health
    if (observation.agentHealth > 80.0f) {
        reward += 0.005f;
    }
    
    // Large negative reward for low health
    if (observation.agentHealth < 30.0f) {
        reward -= 0.05f;
    }
    
    // Small time-based reward for exploration
    reward += 0.0005f * observation.timeStep;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
