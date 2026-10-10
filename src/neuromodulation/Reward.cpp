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
    // Real reward computation based on observation
    // Simple heuristic: reward based on internal signals and actions
    
    if (!observation) {
        return 0.0f;
    }
    
    // Get internal signals as reward indicator
    auto internal = observation->getInternalSignals();
    if (!internal.empty()) {
        // Reward based on average internal signal value
        float sum = 0.0f;
        for (float val : internal) {
            sum += val;
        }
        return sum / static_cast<float>(internal.size()) * 0.5f;  // Scale reward
    }
    
    // Default minimal reward
    return 0.01f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
