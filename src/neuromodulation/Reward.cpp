#include "Reward.hpp"
#include "Neuromodulator.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward components
    float outcomeRewardComponent;
    float predictionErrorRewardComponent;
    float noveltyRewardComponent;
    float curiosityRewardComponent;
    float temporalRewardComponent;
    float confidenceWeight;
    
    // Configuration
    float outcomeWeight;
    float predictionErrorWeight;
    float noveltyWeight;
    float curiosityWeight;
    float temporalWeight;
    float predictionConfidenceWeight;
    float rewardScalingFactor;
    float rewardOffset;
    
    // State
    bool initialized;
    float maxObservedReward;
    float minObservedReward;
    float historicalMeanReward;
    
    Impl() : 
        currentValue(0.0f), accumulatedReward(0.0f),
        outcomeRewardComponent(0.0f), predictionErrorRewardComponent(0.0f),
        noveltyRewardComponent(0.0f), curiosityRewardComponent(0.0f),
        temporalRewardComponent(0.0f), confidenceWeight(1.0f),
        outcomeWeight(0.4f), predictionErrorWeight(0.2f),
        noveltyWeight(0.15f), curiosityWeight(0.15f), temporalWeight(0.1f),
        predictionConfidenceWeight(0.5f), rewardScalingFactor(10.0f),
        rewardOffset(0.0f), initialized(false),
        maxObservedReward(0.0f), minObservedReward(0.0f),
        historicalMeanReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->history.push_back(value);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(pImpl->currentValue);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    
    // Reset component states
    pImpl->outcomeRewardComponent = 0.0f;
    pImpl->predictionErrorRewardComponent = 0.0f;
    pImpl->noveltyRewardComponent = 0.0f;
    pImpl->curiosityRewardComponent = 0.0f;
    pImpl->temporalRewardComponent = 0.0f;
}

float Reward::computeReward(const Observation& observation,
                           const PredictionError& predictionError,
                           const Curiosity& curiosity,
                           const Novelty& novelty,
                           const NeuralPrediction& predictionSystem,
                           float predictedReward,
                           float outcomeReward) const {
    // Get observation data
    const std::vector<float>& obsData = observation.getData();
    
    // Compute prediction error component
    float predictionErrorValue = predictionError.getError();
    float predictionErrorReward = -std::abs(predictionErrorValue) * pImpl->predictionErrorWeight;
    
    // Compute novelty component
    float noveltyLevel = novelty.getLevel();
    float noveltyReward = noveltyLevel * pImpl->noveltyWeight;
    
    // Compute curiosity component
    float curiosityLevel = curiosity.getExplorationDrive();
    float curiosityReward = curiosityLevel * pImpl->curiosityWeight;
    
    // Compute temporal reward from prediction system
    float predictionConfidence = predictionSystem.getPredictionConfidence();
    float temporalReward = (1.0f - predictionConfidence) * pImpl->temporalWeight;
    
    // Update accumulated components
    pImpl->predictionErrorRewardComponent = predictionErrorReward;
    pImpl->noveltyRewardComponent = noveltyReward;
    pImpl->curiosityRewardComponent = curiosityReward;
    pImpl->temporalRewardComponent = temporalReward;
    
    // Normalize outcome reward if provided
    float normalizedOutcomeReward = outcomeReward;
    if (pImpl->initialized && std::abs(pImpl->maxObservedReward - pImpl->minObservedReward) > 0.001f) {
        normalizedOutcomeReward = 2.0f * ((outcomeReward - pImpl->minObservedReward) / 
                                           (pImpl->maxObservedReward - pImpl->minObservedReward)) - 1.0f;
        normalizedOutcomeReward = normalizedOutcomeReward * pImpl->outcomeWeight;
    } else {
        normalizedOutcomeReward = outcomeReward * pImpl->outcomeWeight;
    }
    
    // Compute prediction error between predicted and actual reward
    float predictionError = predictedReward - outcomeReward;
    pImpl->predictionErrorRewardComponent = -std::abs(predictionError) * pImpl->predictionConfidenceWeight;
    
    // Combine all reward components
    float totalReward = normalizedOutcomeReward + predictionErrorReward + 
                       noveltyReward + curiosityReward + temporalReward;
    
    // Apply scaling and offset
    totalReward = totalReward * pImpl->rewardScalingFactor + pImpl->rewardOffset;
    
    // Update historical statistics for normalization
    updateRewardStatistics(totalReward);
    
    // Clamp to reasonable bounds
    totalReward = std::clamp(totalReward, -10.0f, 10.0f);
    
    // Store the computed reward
    pImpl->currentValue = totalReward;
    pImpl->history.push_back(totalReward);
    
    return totalReward;
}

void Reward::updateRewardStatistics(float reward) {
    // Update running statistics for normalization
    if (!pImpl->initialized) {
        pImpl->initialized = true;
        pImpl->maxObservedReward = reward;
        pImpl->minObservedReward = reward;
        pImpl->historicalMeanReward = reward;
    } else {
        pImpl->maxObservedReward = std::max(pImpl->maxObservedReward, reward);
        pImpl->minObservedReward = std::min(pImpl->minObservedReward, reward);
        
        // Update moving average
        const float alpha = 0.1f; // Learning rate for moving average
        pImpl->historicalMeanReward = (1.0f - alpha) * pImpl->historicalMeanReward + 
                                    alpha * reward;
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Configuration methods
void Reward::setRewardWeights(float outcomeWeight, float predictionErrorWeight, 
                             float noveltyWeight, float curiosityWeight, float temporalWeight) {
    pImpl->outcomeWeight = outcomeWeight;
    pImpl->predictionErrorWeight = predictionErrorWeight;
    pImpl->noveltyWeight = noveltyWeight;
    pImpl->curiosityWeight = curiosityWeight;
    pImpl->temporalWeight = temporalWeight;
}

void Reward::setRewardScaling(float scalingFactor, float offset) {
    pImpl->rewardScalingFactor = scalingFactor;
    pImpl->rewardOffset = offset;
}

void Reward::setPredictionConfidenceWeight(float weight) {
    pImpl->predictionConfidenceWeight = weight;
}

// Implementation of the additional methods

float Reward::getOutcomeRewardComponent() const {
    return pImpl->outcomeRewardComponent;
}

float Reward::getPredictionErrorRewardComponent() const {
    return pImpl->predictionErrorRewardComponent;
}

float Reward::getNoveltyRewardComponent() const {
    return pImpl->noveltyRewardComponent;
}

float Reward::getCuriosityRewardComponent() const {
    return pImpl->curiosityRewardComponent;
}

float Reward::getTemporalRewardComponent() const {
    return pImpl->temporalRewardComponent;
}

float Reward::getMaxObservedReward() const {
    return pImpl->maxObservedReward;
}

float Reward::getMinObservedReward() const {
    return pImpl->minObservedReward;
}

float Reward::getHistoricalMeanReward() const {
    return pImpl->historicalMeanReward;
}

void Reward::setRewardWeights(float outcomeWeight, float predictionErrorWeight,
                             float noveltyWeight, float curiosityWeight, float temporalWeight) {
    pImpl->outcomeWeight = outcomeWeight;
    pImpl->predictionErrorWeight = predictionErrorWeight;
    pImpl->noveltyWeight = noveltyWeight;
    pImpl->curiosityWeight = curiosityWeight;
    pImpl->temporalWeight = temporalWeight;
}

void Reward::setRewardScaling(float scalingFactor, float offset) {
    pImpl->rewardScalingFactor = scalingFactor;
    pImpl->rewardOffset = offset;
}

void Reward::setPredictionConfidenceWeight(float weight) {
    pImpl->predictionConfidenceWeight = weight;
}

} // namespace nlm
