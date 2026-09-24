#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float decayRate;
    float baselineReward;
    
    Impl() 
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , decayRate(0.01f)
        , baselineReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() {
    delete pImpl;
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->accumulatedReward += value;
    pImpl->history.push_back(value);
}

void Reward::add(float delta) {
    setValue(pImpl->currentValue + delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedValue = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation based on observation
    // For now, return a simple placeholder value
    return 0.1f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
