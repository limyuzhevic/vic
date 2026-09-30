#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float decayRate;
    float learningRate;
    float discountFactor;
    float predictionErrorRelationship;
    float temporalIntegration;
    float rewardPrecision;
    SimulationStep lastProcessed;
    float rewardHistory[20];  // Store recent rewards for temporal analysis
    int historyIndex;
    float rewardStability;
    float behavioralModulation;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), decayRate(0.03f),
             learningRate(0.2f), discountFactor(0.95f), predictionErrorRelationship(0.0f),
             temporalIntegration(0.0f), rewardPrecision(1.0f), lastProcessed(0),
             historyIndex(0), rewardStability(0.0f), behavioralModulation(0.0f) {
        for (int i = 0; i < 20; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
};

Reward::Reward() : pImpl(new Impl) {
    NLM_LOG_INFO("Reward system enhanced initialized");
}

Reward::~Reward() {
    NLM_LOG_INFO("Reward system enhanced destroyed");
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
    
    // Store in history
    pImpl->history.push_back(pImpl->currentValue);
    
    // Store in temporal window
    pImpl->rewardHistory[pImpl->historyIndex] = pImpl->currentValue;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    
    // Update reward stability based on recent changes
    if (pImpl->history.size() > 10) {
        float recentAvg = 0.0f;
        for (size_t i = std::max(0, static_cast<int>(pImpl->history.size()) - 10); 
             i < pImpl->history.size(); ++i) {
            recentAvg += pImpl->history[i];
        }
        recentAvg /= 10.0f;
        
        // Stability is inverse of recent variance
        float recentStdDev = 0.0f;
        for (size_t i = std::max(0, static_cast<int>(pImpl->history.size()) - 10); 
             i < pImpl->history.size(); ++i) {
            float diff = pImpl->history[i] - recentAvg;
            recentStdDev += diff * diff;
        }
        recentStdDev = std::sqrt(recentStdDev / 10.0f);
        
        pImpl->rewardStability = std::max(0.0f, 1.0f - recentStdDev);
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->temporalIntegration = 0.0f;
    pImpl->rewardPrecision = 1.0f;
    pImpl->rewardStability = 0.0f;
    pImpl->behavioralModulation = 0.0f;
    for (int i = 0; i < 20; ++i) {
        pImpl->rewardHistory[i] = 0.0f;
    }
}

float Reward::computeReward(const Observation& observation) const {
    // Enhanced reward computation from observation
    // Placeholder for observation-based reward computation
    // Should integrate with brain's evaluation system
    
    // For placeholder, return a stable positive value
    return 0.5f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->rewardStability = 0.0f;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

void Reward::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.5f);
}

void Reward::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Reward::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.5f, 1.0f);
}

void Reward::enhanceLearning(float learningMultiplier) {
    // Enhanced reward specifically for learning acceleration
    pImpl->learningRate = std::min(pImpl->learningRate + learningMultiplier * 0.1f, 1.0f);
    pImpl->currentValue = std::min(pImpl->currentValue + learningMultiplier * 0.3f, 10.0f);
    
    // Improve reward precision for better learning
    pImpl->rewardPrecision = std::min(pImpl->rewardPrecision + learningMultiplier * 0.15f, 1.0f);
}

void Reward::enhanceTemporalIntegration(float timeMultiplier) {
    // Enhanced reward specifically for temporal integration
    pImpl->temporalIntegration = std::min(pImpl->temporalIntegration + timeMultiplier * 0.3f, 1.0f);
    
    // Apply discount factor for temporal consistency
    pImpl->currentValue = std::min(pImpl->currentValue * pImpl->discountFactor, 10.0f);
}

void Reward::enhanceBehavioralModulation(float modulationStrength) {
    // Enhanced reward specifically for behavioral modulation
    pImpl->behavioralModulation = std::min(pImpl->behavioralModulation + modulationStrength * 0.4f, 1.0f);
    
    // Adjust current value based on behavioral modulation
    pImpl->currentValue = std::min(pImpl->currentValue * (1.0f + modulationStrength * 0.1f), 10.0f);
}

float Reward::getLearningEfficiency() const {
    // Compute learning efficiency based on reward stability and precision
    float stabilityEfficiency = pImpl->rewardStability;
    float precisionEfficiency = pImpl->rewardPrecision;
    float temporalEfficiency = pImpl->temporalIntegration;
    
    return std::min(1.0f, (stabilityEfficiency + precisionEfficiency + temporalEfficiency) / 3.0f);
}

float Reward::getTemporalValue() const {
    // Compute temporally discounted value
    return pImpl->currentValue * pImpl->discountFactor;
}

float Reward::getRewardConsistency() const {
    // Compute reward consistency (inverse of variance)
    if (pImpl->history.size() < 2) return 1.0f;
    
    float mean = 0.0f;
    for (float value : pImpl->history) {
        mean += value;
    }
    mean /= pImpl->history.size();
    
    float variance = 0.0f;
    for (float value : pImpl->history) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= pImpl->history.size();
    
    return std::max(0.0f, 1.0f - variance);
}

} // namespace nlm