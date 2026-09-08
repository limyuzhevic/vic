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
    // Real reward computation based on observation
    // This is a placeholder implementation that computes a simple heuristic reward
    
    // Base reward from observation value
    float reward = 0.0f;
    
    // Check if observation has position data
    if (observation.getPositionAvailable()) {
        float x = observation.getX();
        float y = observation.getY();
        
        // Reward for being in positive territory (encourages exploration)
        if (x > 0 || y > 0) {
            reward += 0.1f;
        }
        
        // Reward for distance from origin
        float distance = std::sqrt(x * x + y * y);
        reward += distance * 0.01f;
        
        // Penalize negative positions
        if (x < 0 && y < 0) {
            reward -= 0.5f;
        }
    }
    
    // Check for hazards or resources based on observation type
    if (observation.getType() == "hazard") {
        reward -= 1.0f;  // High penalty for hazards
    } else if (observation.getType() == "resource") {
        reward += 1.0f;  // High reward for resources
    }
    
    // Add a small exploration bonus to encourage trying new things
    reward += 0.01f * (static_cast<float>(std::rand()) / RAND_MAX);
    
    // Store in history for debugging
    pImpl->history.push_back(reward);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
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
