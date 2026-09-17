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
    // Real reward computation based on environmental factors
    // Combines multiple reward sources: goal achievement, survival, exploration
    
    float reward = 0.0f;
    
    // Get observation data
    const std::vector<float>& obsData = observation.getData();
    if (obsData.empty()) {
        return 0.0f;
    }
    
    // Base reward from observation properties
    // Simple heuristic: reward for positive signals, punish negative ones
    float observationValue = obsData[0];  // Use first value as primary signal
    
    // Normalize and apply reward function
    // Rewards larger positive values, punishes larger negative values
    // Uses a sigmoid-like function to keep rewards bounded
    reward = 2.0f / (1.0f + std::exp(-observationValue)) - 1.0f;
    
    // Add contribution from other observation dimensions if available
    for (size_t i = 1; i < obsData.size() && i < 5; ++i) {
        // Add smaller contributions from additional dimensions
        reward += obsData[i] * 0.1f;
    }
    
    // Scale based on energy/effort if available in observation
    float effort = observation.getEffort();
    if (effort > 0.0f) {
        // High effort should yield higher reward for same outcome
        reward *= (1.0f + 0.1f * effort);
    }
    
    // Add exploration bonus for novel observations
    float novelty = observation.getNovelty();
    if (novelty > 0.5f) {
        // Significant novelty provides exploration bonus
        reward += 0.5f * (novelty - 0.5f) * 2.0f;
    }
    
    // Add small negative reward for high risk situations
    float risk = observation.getRisk();
    if (risk > 0.8f) {
        reward -= 0.3f * (risk - 0.8f) * 5.0f;
    }
    
    // Store reward in history for learning
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update current and accumulated values
    pImpl->currentValue = reward;
    pImpl->accumulatedReward += reward;
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
