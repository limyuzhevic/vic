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
    // Real reward computation based on observation
    // Simple heuristic: sum of resource values minus hazards
    float reward = 0.0f;
    
    for (const auto& obj : observation.getObjects()) {
        if (obj.type == WorldObjectType::Resource) {
            // Positive reward for resources
            reward += obj.value;
        } else if (obj.type == WorldObjectType::Hazard) {
            // Negative reward for hazards
            reward -= std::abs(obj.value);
        }
    }
    
    // Add energy bonus for high energy
    reward += std::min(10.0f, pImpl->currentValue * 0.5f);
    
    // Ensure reward is within reasonable bounds
    return std::max(-100.0f, std::min(100.0f, reward));
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
