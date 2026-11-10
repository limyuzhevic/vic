#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Parameters for reward computation
    float movementReward;
    float reachingGoalReward;
    float negativeStepPenalty;
    float noveltyRewardFactor;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             movementReward(0.1f), reachingGoalReward(1.0f),
             negativeStepPenalty(-0.01f), noveltyRewardFactor(0.5f) {}
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
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: This is a placeholder implementation
    // For now, return a small default reward
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
