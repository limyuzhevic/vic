#include "Reward.hpp"
#include <algorithm>
#include <numeric>

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
    // Real reward computation based on prediction error theory
    // Implements reward as difference between predicted and actual outcomes
    // Uses neural prediction signals and prediction errors
    
    // Get prediction from prediction system (in real implementation)
    // For now, using simplified prediction based on observation features
    float predictedOutcome = 0.0f;
    
    // Extract features from observation
    // In real implementation, these would be processed through prediction system
    const auto& obsData = observation.getData();
    size_t dataSize = obsData.size();
    
    if (dataSize > 0) {
        // Calculate expected value based on observation patterns
        // This simulates what a prediction system would compute
        for (size_t i = 0; i < dataSize; ++i) {
            // Use historical patterns to predict
            float feature = obsData[i];
            predictedOutcome += feature * 0.1f;  // Simple feature weighting
        }
        
        // Normalize prediction
        predictedOutcome = predictedOutcome / dataSize;
    }
    
    // Calculate actual outcome from observation
    float actualOutcome = 0.0f;
    
    if (dataSize > 0) {
        // Reward is based on difference between actual and expected outcomes
        // Also modulated by observation salience and uncertainty
        actualOutcome = obsData[0] * 2.0f - 1.0f;  // Map to [-1, 1] range
        
        // Add bonus for positive outcomes
        float positiveBonus = 0.0f;
        for (size_t i = 1; i < dataSize; ++i) {
            if (obsData[i] > 0.5f) {
                positiveBonus += obsData[i] * 0.5f;
            }
        }
        
        actualOutcome += positiveBonus * 0.3f;
        
        // Add temporal discount factor
        float temporalDiscount = 1.0f / (1.0f + 0.1f * observation.getTimestamp());
        actualOutcome *= temporalDiscount;
    }
    
    // Calculate prediction error (what reward should be)
    float reward = actualOutcome - predictedOutcome;
    
    // Apply reward scaling and bounds
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    // Add exploration bonus for novel observations
    // This encourages the agent to explore new patterns
    if (hasNovelObservation(observation)) {
        reward += 0.5f;  // Exploration bonus
    }
    
    // Add motivational factors based on internal state
    // This simulates neuromodulatory influence on reward valuation
    float motivationalFactor = getMotivationalValue();
    reward *= (1.0f + motivationalFactor * 0.2f);
    
    // Ensure final reward is within bounds
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    // Update history for learning
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Accumulate total reward
    pImpl->accumulatedReward += reward;
    
    return reward;
}

bool Reward::hasNovelObservation(const Observation& observation) const {
    // Check if observation contains novel features
    // This encourages exploration of new patterns
    
    const auto& obsData = observation.getData();
    float noveltyScore = 0.0f;
    
    // Calculate statistics from history
    if (pImpl->history.size() > 10) {
        float avgReward = 0.0f;
        for (float r : pImpl->history) {
            avgReward += r;
        }
        avgReward /= pImpl->history.size();
        
        // Calculate deviation from mean
        for (float val : obsData) {
            float deviation = std::abs(val - avgReward);
            noveltyScore += deviation;
        }
    }
    
    return noveltyScore > 0.5f;  // Threshold for novelty
}

float Reward::getMotivationalValue() const {
    // Calculate motivational value based on internal state
    // Simulates neuromodulatory influence on reward valuation
    
    // Combine various factors that influence motivation
    float hungerFactor = std::min(1.0f, pImpl->currentValue / 10.0f);
    float arousalFactor = std::min(1.0f, pImpl->accumulatedReward / 5.0f);
    float attentionFactor = 0.5f;  // Simplified
    
    return (hungerFactor * 0.4f) + (arousalFactor * 0.3f) + (attentionFactor * 0.3f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
