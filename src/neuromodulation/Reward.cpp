#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    float baseline;
    float decayRate;
    float learningRate;
    float explorationBonus;
    float punishmentSensitivity;
    TimestepDuration lastUpdateTime;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baseline(0.0f), 
             decayRate(0.02f), learningRate(0.1f), explorationBonus(0.1f),
             punishmentSensitivity(0.5f), lastUpdateTime(0) {}
};

Reward::Reward() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Reward system initialized");
}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    // Add reward with optional exploration bonus
    if (delta > 0.0f) {
        // Positive reward increases value and exploration bonus
        pImpl->currentValue = std::max(0.0f, pImpl->currentValue + delta);
        pImpl->explorationBonus = std::min(1.0f, pImpl->explorationBonus + delta * 0.3f);
    } else {
        // Negative reward (punishment) decreases value
        // Punishment has stronger effect than reward gain
        float penalty = std::abs(delta) * pImpl->punishmentSensitivity;
        pImpl->currentValue = std::max(0.0f, pImpl->currentValue - penalty);
        // Reset exploration bonus on punishment
        pImpl->explorationBonus *= 0.5f;
    }
    
    pImpl->accumulatedReward += delta;
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->explorationBonus = 0.1f;
    pImpl->history.clear();
}

float Reward::computeReward(const class Observation& observation) const {
    // Compute reward based on observation properties
    float reward = 0.0f;
    
    // Reward based on observation type
    float obsType = static_cast<float>(observation.getType());
    reward += obsType * 0.5f;
    
    // Reward based on observation properties (would be real implementation)
    // reward += observation.getEnergy() * 0.3f;
    // reward += observation.getNovelty() * 0.2f;
    
    // Small exploration bonus to encourage discovery
    reward += pImpl->explorationBonus * 0.1f;
    
    return reward;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

float Reward::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

void Reward::setPunishmentSensitivity(float sensitivity) {
    pImpl->punishmentSensitivity = std::clamp(sensitivity, 0.1f, 2.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Natural decay of current value (spontaneous recovery)
    pImpl->currentValue = std::max(0.0f, pImpl->currentValue - pImpl->decayRate * static_cast<float>(timeSinceUpdate));
    
    // Natural decay of exploration bonus
    pImpl->explorationBonus = std::max(0.0f, pImpl->explorationBonus - pImpl->decayRate * 2.0f * static_cast<float>(timeSinceUpdate));
}

} // namespace nlm