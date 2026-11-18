#include "Reward.hpp"
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float value;          // Current reward value
    float historyWeight;  // Weight for history computation
    std::vector<float> history;  // Reward history
    float maxHistory;     // Maximum history size
    
    Impl() : value(0.0f), historyWeight(0.1f), maxHistory(100.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->value;
}

void Reward::setValue(float value) {
    pImpl->value = std::clamp(value, -1.0f, 1.0f);
    pImpl->history.push_back(pImpl->value);
    if (pImpl->history.size() > static_cast<size_t>(pImpl->maxHistory)) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    setValue(pImpl->value + delta);
}

void Reward::reset() {
    pImpl->value = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const class Observation& observation) const {
    // TODO: Implement real reward computation based on observation
    // For now, return a simple value based on observation properties
    float reward = 0.0f;
    
    // Example: reward based on resource proximity, hazard avoidance, etc.
    // This would be replaced with environment-specific reward computation
    
    // Clamp to reasonable range
    reward = std::clamp(reward, -1.0f, 1.0f);
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
