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

    // Store current reward in history
    pImpl->history.push_back(pImpl->currentValue);
    
    // Apply exploration bonus based on curiosity level
    float curiosityBonus = 0.0f;
    if (pImpl->currentValue > 0.0f) {
        curiosityBonus = pImpl->currentValue * 0.1f;  // 10% exploration bonus
        pImpl->currentValue += curiosityBonus;
    }
    
    // Apply unexpected reward bonus
    if (pImpl->currentValue > 1.0f) {
        float unexpectedBonus = (pImpl->currentValue - 1.0f) * 0.5f;
        pImpl->currentValue += unexpectedBonus;
    }
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
