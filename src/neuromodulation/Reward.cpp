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
    // PHASE 2: Compute reward based on sensory observation patterns
    const auto& data = observation.getData();
    
    if (data.empty()) {
        return 0.0f; // No observation, no reward
    }
    
    float totalReward = 0.0f;
    
    // Reward for positive sensory input (e.g., food, resources)
    for (float value : data) {
        if (value > 0.5f) {
            totalReward += value * 0.1f; // Scale positive values
        } else if (value < -0.5f) {
            totalReward += value * 0.05f; // Smaller penalty for negative values
        }
    }
    
    // Reward for consistency (stable observations)
    float variance = 0.0f;
    float mean = 0.0f;
    for (float value : data) {
        mean += value;
    }
    mean /= data.size();
    
    for (float value : data) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= data.size();
    
    // Higher reward for low variance (stable environment)
    totalReward += (1.0f - std::sqrt(variance)) * 0.2f;
    
    // Add some exploration bonus (randomness in reward)
    totalReward += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
    
    // Clamp reward to reasonable range
    return std::clamp(totalReward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
