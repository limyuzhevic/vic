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
    // PLACEHOLDER: Simple heuristic-based reward calculation
    
    // For now, return a simple heuristic-based reward
    // In a real implementation, this would analyze the observation
    // and compute an appropriate reward signal based on goal achievement
    
    // Extract observation components
    float x = observation.position.x;
    float y = observation.position.y;
    std::string type = observation.type;
    
    // Simple heuristic: positive reward for getting closer to origin
    float distance = std::sqrt(x * x + y * y);
    float reward = std::max(0.0f, 1.0f - distance / 10.0f);
    
    // Negative reward for hazards
    if (type == "HAZARD") {
        reward -= 1.0f;
    }
    
    // Add some noise to make it more realistic
    float noise = ((static_cast<float>(std::rand()) / RAND_MAX) - 0.5f) * 0.1f;
    reward += noise;
    
    // Store in history for analysis
    pImpl->history.push_back(reward);
    
    return std::max(-1.0f, std::min(1.0f, reward));  // Clamp to [-1, 1]
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
