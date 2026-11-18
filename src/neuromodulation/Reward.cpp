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
    // Implement real reward computation from observation
    // Reward should be computed based on sensory input patterns
    
    float reward = 0.0f;
    
    // Check if observation is valid
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    const std::vector<float>& data = sensoryInput->getData();
    if (data.empty()) {
        return 0.0f;
    }
    
    // TODO: In Phase 4, the observation space was flattened to a single vector
    // for simplicity. In this implementation, we'll treat the data as
    // containing a mix of sensory signals. For now, implement a simple
    // reward based on energy and health patterns (derived from internal signals)
    
    // For now, compute a placeholder reward based on observation data
    // In a real implementation, this would look at:
    // - Vision patterns (resources vs hazards)
    // - Internal signals (energy, health)
    // - Action outcomes
    // - Prediction errors
    
    // Simple placeholder: compute based on variance in observation data
    // This encourages the agent to seek novel/meaningful patterns
    float mean = 0.0f;
    for (float value : data) {
        mean += value;
    }
    mean /= data.size();
    
    float variance = 0.0f;
    for (float value : data) {
        variance += (value - mean) * (value - mean);
    }
    variance /= data.size();
    
    // Reward for novel patterns (higher variance)
    reward = variance * 0.1f;
    
    // Store in history for debugging
    pImpl->history.push_back(reward);
    
    // Cap the reward to reasonable values
    if (reward > 1.0f) reward = 1.0f;
    if (reward < -1.0f) reward = -1.0f;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
