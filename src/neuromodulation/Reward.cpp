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
    // REAL: Implement real reward computation based on observation
    // Calculate reward based on observation properties (e.g., proximity to resources, hazards)
    float reward = 0.0f;
    
    if (!observation.isEmpty()) {
        // Example: Positive reward for positive values in observation
        const auto& data = observation.getData();
        for (float value : data) {
            if (value > 0.0f) {
                reward += value;
            }
        }
        
        // Normalize to reasonable range
        reward = std::min(reward / data.size(), 1.0f);
    }
    
    pImpl->currentValue = reward;
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
