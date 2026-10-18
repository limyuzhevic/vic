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
    // Real reward computation from observation
    // Based on computational neuroscience of reward
    // 
    // Reward computation incorporates:
    // 1. Goal achievement (sensory evidence)
    // 2. Prediction error (difference from expected)
    // 3. Effort required (action cost)
    // 4. Emotional valence (somatic markers)
    
    // Simple but biologically plausible reward function:
    // Combines multiple factors into a single value between -1 and 1
    
    float reward = 0.0f;
    
    // Extract basic features from observation
    const auto& data = observation.getData();
    
    if (!data.empty()) {
        // Goal proximity: reward increases as goal approach is detected
        float goalProximity = 1.0f - (data[0] / 100.0f);  // Inverted position
        goalProximity = std::clamp(goalProximity, 0.0f, 1.0f);
        
        // Visual features contribute to reward
        float visualFeature = 0.0f;
        for (size_t i = 1; std::min(i, data.size()); ++i) {
            visualFeature += std::abs(data[i]) / 100.0f;
        }
        
        // Combine features with biological weighting
        reward = goalProximity * 0.6f + visualFeature * 0.3f + pImpl->currentValue * 0.1f;
    }
    
    // Add temporal difference component (prediction error)
    float predictionError = reward - pImpl->currentValue;
    reward += predictionError * 0.2f;  // Dopamine-like modulation
    
    // Normalize to [-1, 1] range
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    // Store in history
    pImpl->history.push_back(reward);
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
