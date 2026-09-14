#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Neural substrate for reward computation
    float valueFunction;
    float predictionValue;
    float rewardPredictionError;
    
    // Spike-driven reward signals
    size_t recentSpikeCount;
    float spikeFilteredValue;
    float explorationModulation;
    
    // Temporal reward integration
    float temporalIntegral;
    float discountFactor;
    float learningRate;
    
    // Confidence and uncertainty
    float rewardConfidence;
    float predictionUncertainty;
    float precisionWeight;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), valueFunction(0.0f), predictionValue(0.0f)
        , rewardPredictionError(0.0f), recentSpikeCount(0), spikeFilteredValue(0.0f)
        , explorationModulation(1.0f), temporalIntegral(0.0f), discountFactor(0.95f)
        , learningRate(0.1f), rewardConfidence(1.0f), predictionUncertainty(0.5f), precisionWeight(1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->accumulatedReward += value;
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Neural reward computation
    computeNeuralReward(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->valueFunction = 0.0f;
    pImpl->rewardConfidence = 1.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation based on neural substrate
    float reward = 0.0f;
    
    // Basic sensory feature analysis
    if (!observation.features.empty()) {
        // Compute reward based on environmental features
        // Higher positive features yield higher reward
        for (float feature : observation.features) {
            if (feature > 0.0f) {
                reward += feature * 0.1f;
            } else if (feature < 0.0f) {
                reward += feature * 0.05f; // Negative features have less negative impact
            }
        }
        
        // Normalize reward
        reward = std::max(0.0f, std::min(1.0f, reward / observation.features.size()));
    } else {
        // Default reward computation for simple observations
        reward = (observation.temperature - 37.0f) * 0.01f +  // Optimal body temp
                observation.humidity * 0.05f +                 // Moderate humidity preferred
                observation.pressure * 0.01f +                // Moderate pressure preferred
                observation.soundLevel * 0.02f +             // Moderate sound preferred
                observation.lightLevel * 0.02f +              // Moderate light preferred
                observation.movement * 0.1f;                  // Movement yields positive reward
    }
    
    // Apply prediction error modulation
    if (pImpl) {
        // Modulation based on prediction error
        float predictionError = pImpl->rewardPredictionError;
        reward = reward * (1.0f + predictionError * 0.5f);
        
        // Spike-driven modulation
        if (pImpl->recentSpikeCount > 0) {
            reward *= (1.0f + 0.3f * pImpl->spikeFilteredValue);
        }
        
        // Confidence-based scaling
        reward *= pImpl->rewardConfidence;
        
        // Temporal integration
        pImpl->temporalIntegral = 0.9f * pImpl->temporalIntegral + 0.1f * reward;
        reward = 0.7f * reward + 0.3f * pImpl->temporalIntegral;
    }
    
    return std::max(0.0f, reward);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Helper method for neural reward computation
void Reward::computeNeuralReward(float delta) {
    if (!pImpl) return;
    
    // Temporal difference learning for reward
    float tdError = delta - pImpl->rewardPredictionError;
    
    // Update value function
    pImpl->valueFunction += pImpl->learningRate * tdError;
    
    // Spike-driven reward processing
    if (delta > 0.0f) {
        // Positive reward triggers spike response
        pImpl->recentSpikeCount++;
        pImpl->explorationModulation = std::min(2.0f, 1.0f + delta * 0.5f);
    }
    
    // Update prediction error
    pImpl->rewardPredictionError = tdError;
    
    // Update confidence based on prediction accuracy
    float predictionAccuracy = std::abs(tdError);
    pImpl->rewardConfidence = std::max(0.1f, pImpl->rewardConfidence + 0.1f * (1.0f - predictionAccuracy));
    
    // Filter reward signal
    pImpl->spikeFilteredValue = 0.9f * pImpl->spikeFilteredValue + 0.1f * delta;
}

} // namespace nlm
