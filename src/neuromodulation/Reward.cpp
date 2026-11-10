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
    // PLACEHOLDER: Returns 0
    // 
    // Basic implementation: Use sensory input to compute reward
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    // Get sensory data based on input type
    const auto& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // Simple reward computation based on sensory values:
    // - Positive reward for bright signals (resources)
    // - Negative reward for dark signals (hazards)  
    // - Neutral reward for intermediate values
    
    float reward = 0.0f;
    for (float value : data) {
        if (value > 0.7f) {
            // Bright signal: resource found
            reward += 10.0f;
        } else if (value < 0.3f && value > 0.0f) {
            // Dark signal: potential hazard
            reward -= 5.0f;
        } else if (value > 0.3f && value <= 0.7f) {
            // Intermediate signal: neutral
            reward += 0.5f;
        }
        // value == 0: no signal, no reward
    }
    
    // Small penalty for taking too long (encourage efficiency)
    reward -= 0.01f;
    
    // Clamp to reasonable range
    if (reward > 20.0f) reward = 20.0f;
    if (reward < -20.0f) reward = -20.0f;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
