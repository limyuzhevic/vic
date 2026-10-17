#include "Reward.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float baselineReward;
    float predictionError;
    float learningRate;
    float maxAccumulated;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), baselineReward(0.0f), 
             predictionError(0.0f), learningRate(0.01f), maxAccumulated(100.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, -1.0f, 1.0f);
}

void Reward::add(float delta) {
    // IMPLEMENTATION: Real reward computation with bounds and history tracking
    float newValue = pImpl->currentValue + delta;
    
    // Apply reward-specific scaling for better learning dynamics
    newValue = std::clamp(newValue, -1.0f, 1.0f);
    
    // Update accumulated reward with max tracking
    if (delta > 0.0f) {
        pImpl->accumulatedReward = std::min(pImpl->accumulatedReward + delta, pImpl->maxAccumulated);
    } else {
        pImpl->accumulatedReward += delta;
    }
    
    pImpl->currentValue = newValue;
    
    // Add to history
    pImpl->history.push_back(newValue);
    
    // Keep history manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update prediction error signal
    updatePredictionError(newValue);
}

void Reward::updatePredictionError(float actualReward) {
    // Update prediction error for learning
    float predictedReward = pImpl->baselineReward + 0.1f * pImpl->accumulatedReward;
    
    // Compute prediction error as difference between actual and predicted
    pImpl->predictionError = actualReward - predictedReward;
    
    // Apply reward to baseline for next prediction (TD learning style)
    pImpl->baselineReward = 0.9f * pImpl->baselineReward + 0.1f * actualReward;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->baselineReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // IMPLEMENTATION: Real reward computation from observation
    // This implements biologically plausible reward computation using:
    // - Prediction error
    // - Reward prediction and comparison
    // - Neuromodulation factors
    
    float computedReward = 0.0f;
    
    // Extract reward-relevant features from observation
    float novelty = observation.getNovelty();
    float predictionAccuracy = observation.getPredictionAccuracy();
    float goalProgress = observation.getGoalProgress();
    float threatLevel = observation.getThreatLevel();
    float rewardMagnitude = observation.getRewardMagnitude();
    
    // Apply reward computation based on observation
    
    // 1. Primary reward from goal progress (approach motivation)
    computedReward += goalProgress * 0.5f;
    
    // 2. Reward from prediction accuracy (surprise-based learning)
    computedReward += (predictionAccuracy - 0.5f) * 0.3f;
    
    // 3. Reward from novelty detection (exploration motivation)
    computedReward += novelty * 0.2f;
    
    // 4. Punishment from threat level (avoidance motivation)
    computedReward -= threatLevel * 0.7f;
    
    // 5. Direct reward magnitude modulation
    computedReward *= rewardMagnitude;
    
    // 6. Apply temporal difference prediction error integration
    float predictionError = observation.getPredictionError();
    computedReward += predictionError * 0.4f;
    
    // 7. Apply neuromodulatory scaling
    float neuromodulationScale = 1.0f + (predictionError * 0.1f); // Prediction error modulates reward sensitivity
    computedReward *= neuromodulationScale;
    
    // 8. Bound to biologically plausible range
    computedReward = std::clamp(computedReward, -1.0f, 1.0f);
    
    return computedReward;
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getBaselineReward() const {
    return pImpl->baselineReward;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
