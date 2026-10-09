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
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Neuromodulator interface implementation
const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->currentValue;
}

void Reward::setLevel(float level) {
    pImpl->currentValue = level;
}

float Reward::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real reward-modulated plasticity factor
    // PLACEHOLDER: Higher reward increases plasticity
    return 0.5f + 0.5f * pImpl->currentValue;
}

void Reward::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real reward dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->currentValue = std::max(0.0f, pImpl->currentValue - 0.1f * static_cast<float>(dt));
}

} // namespace nlm
