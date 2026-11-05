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

void Reward::computeReward(const Observation& observation) {
        // TODO PHASE 2: Implement real reward computation from observation
        // PLACEHOLDER: Returns 0
        return 0.0f;
    }
    
    void Reward::recordObservation(const Observation& observation) {
        // TODO PHASE 2: Implement observation recording for reward computation
        // PLACEHOLDER: Just record timestamp
        pImpl->history.push_back(observation.getTimestamp());
    }
    
    float Reward::getRecentReward(int lookbackSteps) const {
        // TODO PHASE 2: Implement recent reward calculation
        // PLACEHOLDER: Return current value
        if (lookbackSteps <= 0 || pImpl->history.empty()) return getValue();
        return getValue();
    }
    
    bool Reward::shouldReinforce() const {
        // TODO PHASE 2: Implement reinforcement decision logic
        // PLACEHOLDER: Always return true for testing
        return getValue() > 0.1f;
    }

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
