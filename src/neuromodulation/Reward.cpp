#include "Reward.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float baseline;
    float decayRate;
    float learningRate;
    std::vector<float> history;
    static constexpr size_t MAX_HISTORY = 100;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baseline(0.0f), 
             decayRate(0.1f), learningRate(0.01f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->accumulatedReward += value;
}

void Reward::add(float delta) {
    pImpl->currentValue += delta;
    pImpl->accumulatedReward += delta;
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

} // namespace nlm