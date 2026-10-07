#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    // Reward computation parameters
    float rewardHistorySize;
    float predictionThreshold;
    float noveltySensitivity;
    float consistencyWeight;
    float surpriseWeight;
    float predictionErrorHistory;
    
    // State tracking
    std::vector<float> stateHistory;
    std::vector<float> rewardHistory;
    float runningAverage;
    float consistencyScore;
    float predictionError;
    
    // Biological parameters
    float rewardTimeConstant;  // Time constant for reward integration
    float baselineReward;      // Baseline metabolic reward
    float maxReward;           // Maximum achievable reward
    float rewardSaturation;    // Saturation point for reward values
    
    Impl() : rewardHistorySize(100.0f), predictionThreshold(0.5f), 
             noveltySensitivity(1.0f), consistencyWeight(0.7f), 
             surpriseWeight(0.3f), predictionErrorHistory(0.0f),
             runningAverage(0.0f), consistencyScore(1.0f), predictionError(0.0f),
             rewardTimeConstant(1.5f), baselineReward(0.01f), 
             maxReward(2.0f), rewardSaturation(1.5f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, 0.0f, pImpl->maxReward);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue = std::clamp(pImpl->currentValue + delta, 
                                    pImpl->baselineReward, pImpl->maxReward);
    
    // Update running statistics
    pImpl->rewardHistory.push_back(pImpl->currentValue);
    if (pImpl->rewardHistory.size() > pImpl->rewardHistorySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Update running average
    if (!pImpl->rewardHistory.empty()) {
        float sum = std::accumulate(pImpl->rewardHistory.begin(), 
                                   pImpl->rewardHistory.end(), 0.0f);
        pImpl->runningAverage = sum / pImpl->rewardHistory.size();
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionErrorHistory = 0.0f;
    pImpl->consistencyScore = 1.0f;
    pImpl->stateHistory.clear();
    pImpl->rewardHistory.clear();
    pImpl->runningAverage = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on multiple factors
    std::vector<float> features = observation.getFeatures();
    
    // 1. Prediction-based reward (prediction error)
    float prediction = predictOutcome(features);
    float predictionError = std::abs(observation.getValue() - prediction);
    
    // Update prediction error history
    pImpl->predictionErrorHistory = 0.95f * pImpl->predictionErrorHistory + 
                                   0.05f * predictionError;
    
    // 2. Consistency-based reward
    float consistencyReward = computeConsistency(observation);
    
    // 3. Novelty-based reward
    float noveltyReward = computeNovelty(observation);
    
    // 4. Outcome-based reward
    float outcomeReward = observation.getValue();
    
    // 5. Surprise-based reward
    float surpriseReward = computeSurprise(observation);
    
    // Combine all components biologically realistically
    float totalReward = 0.0f;
    
    // Prediction error drives learning (error = unexpected outcomes)
    totalReward += pImpl->surpriseWeight * predictionError;
    
    // Consistency rewards stable performance
    totalReward += pImpl->consistencyWeight * consistencyReward;
    
    // Novelty rewards exploration
    totalReward += pImpl->noveltySensitivity * noveltyReward;
    
    // Outcome rewards basic survival/goal achievement
    totalReward += outcomeReward * 0.4f;  // Moderated outcome importance
    
    // Surprise rewards unexpected discoveries
    totalReward += surpriseReward * 0.2f;
    
    // Apply biological constraints
    totalReward = std::clamp(totalReward, pImpl->baselineReward, pImpl->rewardSaturation);
    
    // Temporal integration (exponential smoothing)
    pImpl->currentValue = 0.8f * pImpl->currentValue + 0.2f * totalReward;
    
    // Update state history
    pImpl->stateHistory.push_back(observation.getValue());
    if (pImpl->stateHistory.size() > pImpl->rewardHistorySize) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
    }
    
    // Log reward computation for debugging
    NLM_LOG_DEBUG("Reward computed: prediction_error=" + std::to_string(predictionError) +
                  " consistency=" + std::to_string(consistencyReward) +
                  " novelty=" + std::to_string(noveltyReward) +
                  " outcome=" + std::to_string(outcomeReward) +
                  " surprise=" + std::to_string(surpriseReward) +
                  " total=" + std::to_string(totalReward));
    
    return pImpl->currentValue;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->rewardHistory;
}

void Reward::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->stateHistory.clear();
    pImpl->runningAverage = 0.0f;
    NLM_LOG_INFO("Reward history cleared");
}

// Helper methods for reward computation
float Reward::predictOutcome(const std::vector<float>& features) const {
    // Simple prediction model based on weighted sum of features
    float prediction = 0.0f;
    
    // Use recent state history for prediction
    if (!pImpl->stateHistory.empty()) {
        // Predict based on exponential moving average of recent states
        float decay = 0.9f;
        float predicted = pImpl->runningAverage;
        
        // Add contribution from features
        for (size_t i = 0; i < std::min(features.size(), pImpl->stateHistory.size()); ++i) {
            predicted += features[i] * (0.1f / (i + 1));
        }
        
        prediction = predicted;
    } else {
        // Fallback to simple feature sum
        prediction = std::accumulate(features.begin(), features.end(), 0.0f) / features.size();
    }
    
    return std::clamp(prediction, 0.0f, 1.0f);
}

float Reward::computeConsistency(const Observation& observation) const {
    // Reward for consistent, predictable behavior
    if (pImpl->stateHistory.size() < 3) {
        return 0.5f;  // Neutral consistency for insufficient history
    }
    
    // Calculate consistency as inverse of recent variance
    size_t recentSize = std::min(static_cast<size_t>(10), pImpl->stateHistory.size());
    float sum = 0.0f, sumSq = 0.0f;
    
    for (size_t i = pImpl->stateHistory.size() - recentSize; i < pImpl->stateHistory.size(); ++i) {
        float val = pImpl->stateHistory[i];
        sum += val;
        sumSq += val * val;
    }
    
    float mean = sum / recentSize;
    float variance = (sumSq / recentSize) - (mean * mean);
    
    // Consistency increases as variance decreases
    float consistency = std::max(0.0f, 1.0f - variance);
    
    return consistency;
}

float Reward::computeNovelty(const Observation& observation) const {
    // Reward for novel/exciting experiences
    std::vector<float> features = observation.getFeatures();
    
    // Novelty as deviation from expected feature distribution
    float novelty = 0.0f;
    
    // Compare with recent feature history (simplified)
    if (pImpl->rewardHistory.size() >= 5) {
        for (size_t i = 0; i < std::min(features.size(), static_cast<size_t>(5)); ++i) {
            float expected = 0.0f;
            size_t start = std::max(0, static_cast<int>(pImpl->rewardHistory.size()) - 5);
            for (size_t j = start; j < pImpl->rewardHistory.size(); ++j) {
                expected += pImpl->rewardHistory[j];
            }
            expected /= static_cast<float>(pImpl->rewardHistory.size() - start);
            
            novelty += std::abs(features[i] - expected);
        }
        novelty /= std::min(features.size(), static_cast<size_t>(5));
    } else {
        // If not enough history, default to moderate novelty
        novelty = 0.5f;
    }
    
    return std::clamp(novelty, 0.0f, 1.0f);
}

float Reward::computeSurprise(const Observation& observation) const {
    // Surprise reward for unexpected outcomes
    float outcome = observation.getValue();
    
    // Calculate prediction error
    float prediction = predictOutcome(observation.getFeatures());
    float error = std::abs(outcome - prediction);
    
    // Surprise increases with prediction error
    float surprise = error * (1.0f + pImpl->predictionErrorHistory);
    
    return std::clamp(surprise, 0.0f, 1.0f);
}

} // namespace nlm
