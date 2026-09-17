#include "Reward.hpp"

namespace nlm {

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
    float reward = 0.0f;
    
    // Positive reward for high vision brightness (finding something interesting)
    for (float pixel : observation.vision) {
        reward += std::max(0.0f, pixel * 0.5f);
    }
    
    // Reward for reaching target areas in vision
    for (float pixel : observation.vision) {
        if (pixel > 0.8f) reward += 0.5f;
    }
    
    // Moderate reward for touch (contact)
    for (float touch : observation.touch) {
        if (touch > 0.3f) reward += 0.1f;
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
