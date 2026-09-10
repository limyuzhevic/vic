#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baseline;
    float maxValue;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baseline(0.0f), maxValue(1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, pImpl->baseline, pImpl->maxValue);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue = std::clamp(pImpl->currentValue + delta, pImpl->baseline, pImpl->maxValue);
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = pImpl->baseline;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Compute reward from observation features
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Simple heuristic based on observation data
    
    const auto& data = observation.getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Simple reward: average of observation values (normalized to [0, 1])
    float sum = 0.0f;
    for (float val : data) {
        sum += std::abs(val);
    }
    return sum / static_cast<float>(data.size()) / 10.0f; // Scale to reasonable range
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
