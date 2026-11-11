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
    // REAL IMPLEMENTATION: Compute reward based on observation
    // Phase 2 real reward computation from observation state
    float reward = 0.0f;
    
    // Consider various factors for reward computation
    for (float value : observation.getFeatures()) {
        // Positive features contribute positively to reward
        if (value > 0.0f) {
            reward += value * 0.1f;
        }
        // Negative features contribute negatively to reward
        else if (value < 0.0f) {
            reward += value * 0.05f;
        }
    }
    
    // Clamp reward to reasonable range
    reward = std::clamp(reward, -10.0f, 10.0f);
    
    pImpl->currentValue = reward;
    pImpl->history.push_back(reward);
    
    // Limit history size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    std::cout << "[Reward] Computed reward: " << reward << " from observation" << std::endl;
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
