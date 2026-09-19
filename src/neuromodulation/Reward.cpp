#include "Reward.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float maxValue;
    float decayRate;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), maxValue(1.0f), decayRate(0.01f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, -pImpl->maxValue, pImpl->maxValue);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Simple reward function based on observation values
    // In a real implementation, this would be replaced with domain-specific logic
    float reward = 0.0f;
    
    // Example: positive reward for large positive observations, negative for large negative
    for (float value : observation.getValues()) {
        if (value > 0.5f) {
            reward += value * 0.1f;
        } else if (value < -0.5f) {
            reward -= std::abs(value) * 0.1f;
        }
    }
    
    // Normalize to range [-1, 1]
    return std::clamp(reward, -1.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

void Reward::setMaxValue(float max) {
    pImpl->maxValue = std::max(0.0f, max);
}

float Reward::getMaxValue() const {
    return pImpl->maxValue;
}

void Reward::update(TimestepDuration dt) {
    // Decay accumulated reward over time (habituation)
    pImpl->accumulatedReward *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

} // namespace nlm
