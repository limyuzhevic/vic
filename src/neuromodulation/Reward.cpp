#include "Reward.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baselineReward;        // Baseline reward for prediction error calculation
    float predictionError;        // Prediction error for reward prediction
    Timestamp lastUpdateTime;     // Time of last reward update
    float rewardDecayRate;        // Rate at which reward value decays towards baseline
    float maxAccumulatedReward;  // Maximum accumulated reward (for normalization)
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baselineReward(0.0f), 
             predictionError(0.0f), lastUpdateTime(0.0), rewardDecayRate(0.01f),
             maxAccumulatedReward(1000.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    if (!pImpl) return 0.0f;
    
    // Apply temporal decay to current reward value
    TimestepDuration dt = 0.1f; // Default timestep, should be configurable
    float decayedValue = pImpl->currentValue * std::exp(-pImpl->rewardDecayRate * dt);
    
    return decayedValue;
}

void Reward::setValue(float value) {
    if (!pImpl) return;
    
    // Validate and clamp reward value
    pImpl->currentValue = std::max(-10.0f, std::min(value, 10.0f));
    pImpl->lastUpdateTime = 0.0f;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentValue);
    
    // Keep history size manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    if (!pImpl) return;
    
    // Add to accumulated reward with bounds checking
    pImpl->accumulatedReward += delta;
    pImpl->accumulatedReward = std::max(-maxAccumulatedReward, 
                                      std::min(pImpl->accumulatedReward, maxAccumulatedReward));
    
    // Apply delta to current value
    setValue(pImpl->currentValue + delta);
}

void Reward::reset() {
    if (!pImpl) return;
    
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->lastUpdateTime = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!pImpl) return 0.0f;
    
    // Implement sophisticated reward computation based on observation
    float computedReward = 0.0f;
    
    // Try different reward computation strategies
    computedReward = computeFromObservationProperties(observation);
    
    // If basic computation gives 0, try neural-based reward
    if (computedReward == 0.0f) {
        computedReward = computeNeuralBasedReward(observation);
    }
    
    // If still 0, use simple proximity-based reward
    if (computedReward == 0.0f) {
        computedReward = computeProximityBasedReward(observation);
    }
    
    // Normalize reward to reasonable range
    computedReward = std::max(-1.0f, std::min(computedReward, 1.0f));
    
    // Update prediction error
    float predictionError = computedReward - pImpl->baselineReward;
    pImpl->predictionError = predictionError;
    
    return computedReward;
}

float Reward::computeFromObservationProperties(const Observation& observation) const {
    if (!pImpl) return 0.0f;
    
    // Extract properties from observation
    const auto& data = observation.getData();
    const auto& metadata = observation.getMetadata();
    
    float reward = 0.0f;
    
    // Reward based on proximity to goal (if present in metadata)
    auto goalPosIt = metadata.find("goal_position");
    if (goalPosIt != metadata.end()) {
        // For now, assume observation has position data
        // In real implementation, this would compute distance to goal
        reward = -0.1f; // Small negative reward (distance penalty)
    }
    
    // Reward based on energy/efficiency (if energy in metadata)
    auto energyIt = metadata.find("energy");
    if (energyIt != metadata.end()) {
        float energy = energyIt->second;
        if (energy > 0.0f) {
            reward += 0.05f * energy; // Positive reward for high energy
        }
    }
    
    return reward;
}

float Reward::computeNeuralBasedReward(const Observation& observation) const {
    if (!pImpl) return 0.0f;
    
    // Neural-based reward computation using prediction error
    float predictionError = pImpl->predictionError;
    
    // Reward based on prediction error magnitude
    // Large prediction errors indicate surprise, which can be rewarding for exploration
    float reward = 0.0f;
    if (std::abs(predictionError) > 0.5f) {
        reward = 0.3f * std::signum(predictionError);
    } else {
        reward = 0.1f * predictionError;
    }
    
    return reward;
}

float Reward::computeProximityBasedReward(const Observation& observation) const {
    if (!pImpl) return 0.0f;
    
    // Simple proximity-based reward
    // For now, return a small negative reward to encourage movement
    return -0.01f;
}

const std::vector<float>& Reward::getHistory() const {
    if (!pImpl) {
        static const std::vector<float> emptyHistory;
        return emptyHistory;
    }
    return pImpl->history;
}

void Reward::clearHistory() {
    if (pImpl) {
        pImpl->history.clear();
    }
}

void Reward::setBaseline(float baseline) {
    if (pImpl) {
        pImpl->baselineReward = baseline;
    }
}

float Reward::getBaseline() const {
    if (pImpl) {
        return pImpl->baselineReward;
    }
    return 0.0f;
}

float Reward::getPredictionError() const {
    if (pImpl) {
        return pImpl->predictionError;
    }
    return 0.0f;
}

float Reward::getAccumulatedReward() const {
    if (pImpl) {
        return pImpl->accumulatedReward;
    }
    return 0.0f;
}

Timestamp Reward::getLastUpdateTime() const {
    if (pImpl) {
        return pImpl->lastUpdateTime;
    }
    return 0.0f;
}

} // namespace nlm
