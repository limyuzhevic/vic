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
    // TODO PHASE 2: Implement real reward computation from observation
    // Real reward computation based on prediction error and novelty
    // Reward = prediction_error + novelty_bonus * learning_rate
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput || !sensoryInput->isValid()) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    
    // Compute reward from sensory data
    // Higher magnitude signals = higher potential reward
    // Sign of signal indicates approach/avoidance direction
    float magnitude = 0.0f;
    float sign = 0.0f;
    
    for (float value : data) {
        magnitude += std::abs(value);
        sign += value;
    }
    
    // Normalize by dimensionality
    float norm = static_cast<float>(data.size()) * 0.5f;  // Assuming [-0.5, 0.5] typical
    if (norm > 0.0f) {
        magnitude /= norm;
        sign /= norm;
    }
    
    // Base reward from magnitude (stronger signals = stronger reward)
    float baseReward = std::min(magnitude * 10.0f, 1.0f);  // Scale to [0, 1]
    
    // Reward direction based on sign (positive for approach, negative for avoid)
    float directedReward = sign * baseReward * 0.5f;
    
    // Apply nonlinear transformation (saturating function)
    float totalReward = std::tanh(baseReward + directedReward * 0.5f);
    
    // Add small random noise for exploration (will be averaged over time)
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
