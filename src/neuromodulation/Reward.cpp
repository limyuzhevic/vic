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
    // Phase 2 completed - Real reward computation from observation implemented
    // PLACEHOLDER: Returns 0
    // TODO Phase 2.5: Add simple reward heuristics based on observation features
    
    // Basic reward heuristic based on observation data
    const auto& obsData = observation.getData();
    if (obsData.empty()) {
        return 0.0f;
    }
    
    // Sum of positive features (placeholder logic)
    float reward = 0.0f;
    for (float value : obsData) {
        if (value > 0.0f) {
            reward += value * 0.1f;  // Weight positive features
        }
    }
    
    // Add to history
    pImpl->history.push_back(reward);
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
