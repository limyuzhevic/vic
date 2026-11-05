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
    // Phase 3+ IMPLEMENTATION: Real reward computation from observation
    // Check if reward was already set (e.g., from world interaction)
    float reward = pImpl->currentValue;
    
    if (reward != 0.0f) {
        // Use explicit reward if set
        return reward;
    }
    
    // Default reward computation from observation features
    // Look for reward-related features in observation
    const auto& signals = observation.getSignals();
    float totalReward = 0.0f;
    
    for (size_t i = 0; i < signals.size(); ++i) {
        float signal = signals[i];
        
        // Energy-based reward: maintain energy level
        if (i == 0 && signal > 0.8f) {  // High energy level
            totalReward += 0.1f;
        } else if (i == 0 && signal < 0.2f) {  // Low energy level
            totalReward -= 0.2f;
        }
        
        // Health-based reward
        if (i == 1 && signal > 0.5f) {  // Good health
            totalReward += 0.05f;
        } else if (i == 1 && signal < 0.3f) {  // Poor health
            totalReward -= 0.1f;
        }
        
        // Novelty-based reward: explore new areas (detect through signal changes)
        // This will be enhanced by the curiosity system
        
        // Position-based reward: move toward positive targets
        if (i >= 2 && i < signals.size() - 1) {
            // Reward for approaching goal positions
            float targetProximity = 1.0f - std::abs(signal - 1.0f);
            if (targetProximity > 0.7f) {
                totalReward += 0.05f * targetProximity;
            }
        }
    }
    
    // Add movement-based reward for exploration
    if (signals.size() >= 6) {
        float movementActivity = (signals[2] + signals[3] + signals[4] + signals[5]) / 4.0f;
        if (movementActivity > 0.3f) {
            totalReward += 0.02f * movementActivity;
        }
    }
    
    // Small exploration bonus to encourage discovery
    totalReward += 0.001f;
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
