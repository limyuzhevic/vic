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
    // Compute reward based on observation features
    // For now: simple magnitude-based reward with decay for small values
    const std::vector<float>* data = observation.getSensoryInput()->getData();
    
    if (!data || data->empty()) {
        return 0.0f;
    }
    
    // Compute sum of absolute values as magnitude
    float totalMagnitude = 0.0f;
    for (float value : *data) {
        totalMagnitude += std::abs(value);
    }
    
    // Normalize by number of elements
    float avgMagnitude = totalMagnitude / static_cast<float>(data->size());
    
    // Apply non-linear scaling: small values get penalized, large values rewarded
    float reward = 0.0f;
    if (avgMagnitude > 0.5f) {
        reward = std::pow(avgMagnitude, 2.0f) * 10.0f;
    } else if (avgMagnitude > 0.1f) {
        reward = avgMagnitude * 5.0f;
    } else if (avgMagnitude > 0.01f) {
        reward = avgMagnitude * 2.0f;
    }
    
    // Apply decay for small positive signals (encourage stronger signals)
    if (reward < 0.1f && reward > 0.0f) {
        reward = 0.0f;
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
