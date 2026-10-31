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
    // TODO PHASE 3: Implement real reward computation based on goal achievement
    // PLACEHOLDER: Simple reward computation from observation metrics
    
    // Extract relevant features from observation (e.g., position, target proximity)
    float totalReward = 0.0f;
    
    // Example reward components (to be implemented based on specific task):
    // - Proximity reward based on goal distance
    // - Action reward for successful behaviors  
    // - Penalty for errors or undesirable states
    
    // For now, return a small baseline reward to encourage exploration
    // TODO: Replace with task-specific reward computation
    totalReward += 0.01f;  // Small exploration incentive
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
