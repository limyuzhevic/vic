#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return getValue();
}

void Reward::setLevel(float level) {
    setValue(level);
}

float Reward::getPlasticityFactor() const {
    // Reward modulates plasticity - higher reward increases plasticity
    return 0.5f + 0.5f * currentValue;  // Range [0.5, 1.0]
}

void Reward::update(TimestepDuration dt) {
    // Reward decays over time towards baseline
    pImpl->currentValue = std::max(0.0f, pImpl->currentValue - 0.1f * static_cast<float>(dt));
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::max(0.0f, value);
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
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns current value
    return getValue();
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
