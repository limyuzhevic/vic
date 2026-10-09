#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include "Observation.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float explorationBonus;
    float intrinsicWeight;
    float extrinsicWeight;
    float predictionErrorBaseline;
    
    // State tracking
    float lastPredictedReward;
    float rewardPredictionError;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             explorationBonus(0.1f), intrinsicWeight(0.5f), extrinsicWeight(0.5f),
             predictionErrorBaseline(0.0f), lastPredictedReward(0.0f), 
             rewardPredictionError(0.0f) {}
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
    pImpl->history.push_back(delta);
    // Keep history manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation, float predictionError) const {
    // Implement comprehensive reward computation
    float reward = 0.0f;
    
    // Extrinsic reward from observation (e.g., food, survival, goal achievement)
    reward += observation.getIntrinsicValue() * pImpl->extrinsicWeight;
    
    // Intrinsic curiosity reward
    float novelty = observation.getNovelty();
    float curiosityReward = novelty * pImpl->explorationBonus * pImpl->intrinsicWeight;
    reward += curiosityReward;
    
    // Prediction error contribution (Rescorla-Wagner style)
    reward += predictionError * 0.1f;
    
    // Environmental constraints and penalties
    if (observation.isNegative()) {
        reward *= 0.1f;  // Reduce negative rewards
    }
    
    // Apply exploration bonus for novel states
    if (novelty > 0.5f) {
        reward += 0.05f * pImpl->intrinsicWeight;
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::applyDecay(float decayRate) {
    // Apply exponential decay to reward history
    for (auto& reward : pImpl->history) {
        reward *= (1.0f - decayRate);
    }
}

float Reward::computePredictionError(float actual, float predicted) const {
    // Implement reward prediction error (delta rule)
    return actual - predicted;
}

void Reward::updatePrediction(float predicted) {
    // Update prediction model based on actual rewards
    pImpl->lastPredictedReward = predicted;
    pImpl->rewardPredictionError = computePredictionError(pImpl->currentValue, predicted);
}

void Reward::configure(float explorationBonus, float intrinsicWeight, float extrinsicWeight) {
    // Configure reward computation with biologically plausible parameters
    pImpl->explorationBonus = explorationBonus;
    pImpl->intrinsicWeight = std::clamp(intrinsicWeight, 0.0f, 1.0f);
    pImpl->extrinsicWeight = std::clamp(extrinsicWeight, 0.0f, 1.0f);
    
    // Ensure weights sum to 1.0
    float totalWeight = pImpl->intrinsicWeight + pImpl->extrinsicWeight;
    if (totalWeight > 0.0f) {
        pImpl->intrinsicWeight /= totalWeight;
        pImpl->extrinsicWeight /= totalWeight;
    }
}

} // namespace nlm
