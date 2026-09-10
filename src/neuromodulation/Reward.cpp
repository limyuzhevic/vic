#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
    
    ~Impl() {
        history.clear();
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    if (!std::isfinite(value)) {
        NLM_LOG_WARNING("Invalid reward value: " + std::to_string(value));
        return;
    }
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    if (!std::isfinite(delta)) {
        NLM_LOG_WARNING("Invalid reward delta: " + std::to_string(delta));
        return;
    }
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    
    // Add to history with bounds checking
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
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
