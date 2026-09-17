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
    // Extract data from observation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) return 0.0f;
    
    const auto& values = sensoryInput->getData();
    if (values.empty()) return 0.0f;
    
    // Simple reward: positive if any input is above threshold
    for (float val : values) {
        if (val > 0.5f) return 1.0f;
    }
    
    // Simple reward: negative if any input is below threshold
    for (float val : values) {
        if (val < -0.5f) return -1.0f;
    }
    
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
