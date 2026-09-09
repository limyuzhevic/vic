#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

struct Reward::Impl {
    static constexpr float MIN_VALUE = -1000.0f;
    static constexpr float MAX_VALUE = 1000.0f;
    static constexpr size_t MAX_HISTORY_SIZE = 1000;
    
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
    // Validate and clamp to safe bounds
    if (value > Impl::MAX_VALUE || value < Impl::MIN_VALUE) {
        NLM_LOG_WARNING("Reward: Attempt to set value " + std::to_string(value) + 
                       " outside safe bounds (" + std::to_string(Impl::MIN_VALUE) + 
                       " to " + std::to_string(Impl::MAX_VALUE) + ")");
        value = std::clamp(value, Impl::MIN_VALUE, Impl::MAX_VALUE);
    }
    
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    // Validate input
    if (delta > MAX_VALUE || delta < MIN_VALUE) {
        NLM_LOG_WARNING("Reward: Attempt to add delta " + std::to_string(delta) + 
                       " outside safe bounds (" + std::to_string(MIN_VALUE) + 
                       " to " + std::to_string(MAX_VALUE) + ")");
        delta = std::clamp(delta, MIN_VALUE, MAX_VALUE);
    }
    
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    
    // Update history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > MAX_HISTORY_SIZE) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    // Reset with bounds checking
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
