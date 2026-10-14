#include "Reward.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baseline;
    float maxHistorySize;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             baseline(0.0f), maxHistorySize(1000) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, pImpl->baseline, 10.0f);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    
    // Clamp to reasonable bounds
    pImpl->currentValue = std::clamp(pImpl->currentValue, pImpl->baseline, 10.0f);
    
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = pImpl->baseline;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Simple sum-based reward
    if (observation.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float val : observation) {
        total += val;
    }
    return total / observation.size();
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
