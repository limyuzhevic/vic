#include "Reward.hpp"

namespace nlm {

Reward::Reward() : pImpl(std::make_unique<Impl>()) {}

Reward::~Reward() = default;

Reward::Reward(Reward&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Reward& Reward::operator=(Reward&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    // Implement real reward computation from sensory input
    // Reward signals represent value of current state or action
    // Computed based on sensory features and learned value functions
    
    // Extract relevant features from observation
    std::vector<float> features;
    
    // Try to get vision data
    const auto* visionInput = dynamic_cast<const Vision*>(&observation);
    if (visionInput) {
        features = visionInput->getData();
    } else {
        features = observation.getInternal();
    }
    
    // Compute reward based on feature values
    // In a real implementation, this would use:
    // - Learned value functions (model-based)
    // - heuristics based on features (model-free)
    // - combination of both
    
    float reward = 0.0f;
    
    // Example reward heuristics (simplified)
    // Positive features -> positive reward
    // Negative features -> negative reward
    
    // Sum of feature values with weights
    for (float feature : features) {
        // Feature weights (could be learned)
        float weight = 0.1f;  // Simplified: uniform weights
        reward += feature * weight;
    }
    
    // Apply homeostatic regulation
    // Prevents extreme reward values
    float homeostaticFactor = 1.0f - 0.1f * std::abs(reward);
    reward *= homeostaticFactor;
    
    // Add stochastic element for exploration
    // Makes reward computation less deterministic
    float noise = (std::rand() % 100 - 50) / 500.0f;  // -0.1 to 0.1
    reward += noise;
    
    // Store in history for temporal analysis
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {
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
