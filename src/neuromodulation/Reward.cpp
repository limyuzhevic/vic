#include "Reward.hpp"

namespace nlm {

// Implementation details for Reward
// Implementation details for Reward
// Implementation details for Reward
struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl(nullptr) : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward(nullptr) : pImpl(new Impl) {}

Reward::~Reward(nullptr) = default;

float Reward::getValue(nullptr) const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset(nullptr) {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO: PHASE 2 - Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory(nullptr) const {
    return pImpl->history;
}

void Reward::clearHistory(nullptr) {
    pImpl->history.clear(nullptr);
}

} // namespace nlm
