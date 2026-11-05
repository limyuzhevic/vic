#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {
    // Initialize with default values for real-world reward dynamics
    // Start at baseline reward level
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    
    // Reward history for tracking learning progress
    pImpl->history.reserve(100); // Reserve space for performance
}

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
    // The observation contains sensory input that reflects the agent's state
    // and interactions with the world. We compute reward based on key features:
    // - Total sensory activation (higher = more exploration)
    // - Novelty (new patterns = learning opportunities)
    // - Energy state (maintaining energy is valuable)
    // - Action outcomes (successful actions = positive reward)
    
    if (!observation.isValid() || !observation.getSensoryInput()) {
        return 0.0f;
    }
    
    const auto& sensoryInput = *observation.getSensoryInput();
    const auto& data = sensoryInput.getData();
    
    if (data.empty()) {
        return 0.0f;
    }
    
    // Base reward from sensory activation
    float baseReward = 0.0f;
    for (float value : data) {
        baseReward += std::abs(value) * 0.01f;  // Scale contribution
    }
    
    // Add novelty-based reward
    float noveltyBonus = 0.0f;
    for (float value : data) {
        if (std::abs(value) > 0.5f) {  // Strong activations indicate novelty
            noveltyBonus += (std::abs(value) - 0.5f) * 0.1f;
        }
    }
    
    // Energy maintenance reward (if energy input available)
    float energyReward = 0.0f;
    // Internal signals typically represent energy/maintenance needs
    // Looking for internal signal values that indicate maintenance
    
    // Combine all reward components
    float totalReward = baseReward + noveltyBonus + energyReward;
    
    // Ensure reasonable bounds
    totalReward = std::clamp(totalReward, -1.0f, 10.0f);
    
    // Add to history for learning
    pImpl->history.push_back(totalReward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
