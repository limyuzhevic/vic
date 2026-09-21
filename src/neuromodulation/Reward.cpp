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
    // REAL PHASE 2: Implement real reward computation from observation
    // Realistic reward computation based on task goals:
    // - Proximity to target
    // - Action success/failure
    // - Resource availability
    // - Prediction errors
    
    // Extract features from observation for reward computation
    const auto& values = observation.getData();
    if (values.empty()) {
        return 0.0f;
    }
    
    // Basic reward computation based on observation features
    // For now, implement a simple heuristic that can be extended
    float reward = 0.0f;
    
    // Example: positive values contribute to reward (adaptation)
    for (float value : values) {
        // Scale observation values to reward range [-1, 1]
        reward += std::tanh(value * 0.1f);  // Simple scaling
    }
    
    // Normalize by number of observations
    reward /= static_cast<float>(values.size());
    
    // Add history-based reward shaping (learning from past rewards)
    if (!pImpl->history.empty()) {
        // Simple reward smoothing: average with recent history
        float avgHistory = 0.0f;
        for (float h : pImpl->history) {
            avgHistory += h;
        }
        avgHistory /= static_cast<float>(pImpl->history.size());
        
        // Small incentive to maintain consistent reward patterns
        reward = reward * 0.8f + avgHistory * 0.2f;
    }
    
    // Store in history for future reference
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {  // Keep last 100 rewards
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
