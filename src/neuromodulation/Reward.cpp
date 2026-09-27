#include "Reward.hpp"
#include "../world/Observation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Reward::Impl {
    // Reward computation components
    float currentValue;
    float accumulatedReward;
    float expectedValue;           // TD(target) value prediction
    float discountFactor;          // Temporal discount factor
    float predictionError;         // Reward prediction error (RPE)
    float rewardHistorySize;       // Size of reward history buffer
    
    // Neural reward processing
    float rewardSignificance;      // How significant the reward is
    float rewardSalience;          // How salient the reward is
    float rewardValue;             // Normalized reward value
    
    // Temporal processing
    float temporalWindow;          // Time window for reward accumulation
    float decayRate;               // Reward decay rate
    float integrationRate;         // Reward integration rate
    
    // Statistics
    std::vector<float> history;
    std::vector<float> predictionErrorHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    // Environmental factors
    float environmentalBonus;      // Context-dependent reward modulation
    float noveltyBonus;            // Novelty-based reward enhancement
    
    Impl()
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , expectedValue(0.0f)
        , discountFactor(0.95f)
        , predictionError(0.0f)
        , rewardHistorySize(100.0f)
        , rewardSignificance(1.0f)
        , rewardSalience(1.0f)
        , rewardValue(0.0f)
        , temporalWindow(1.0f)
        , decayRate(0.1f)
        , integrationRate(0.5f)
        , environmentalBonus(1.0f)
        , noveltyBonus(1.0f) {
        
        // Seed RNG with chrono for biological variability
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.9f, 1.1f);
        
        // Initialize with some biological variability
        environmentalBonus = dist(rng);
        noveltyBonus = dist(rng);
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > pImpl->rewardHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    // Biological reward integration with saturation
    float rawDelta = delta * pImpl->rewardSignificance * pImpl->environmentalBonus * pImpl->noveltyBonus;
    
    // Neural integration with bounds
    pImpl->accumulatedReward += rawDelta * pImpl->integrationRate;
    pImpl->currentValue = std::min(1.0f, std::max(0.0f, pImpl->currentValue + rawDelta));
    
    // Store history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > pImpl->rewardHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->predictionError = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Real reward computation: neural-based reward prediction
    // Integrates sensory input with reward history and predictions
    
    // Get sensory data from observation
    const std::vector<float>& sensoryData = observation.getData();
    
    if (sensoryData.empty()) {
        NLM_LOG_WARNING("Empty sensory data in reward computation");
        return 0.0f;
    }
    
    // Neural reward computation components
    float reward = 0.0f;
    
    // 1. Primary reward signal based on sensory features
    // Focus on salient features (high variance)
    float featureSalience = 0.0f;
    float featureSum = 0.0f;
    size_t compareLen = std::min(static_cast<size_t>(10), sensoryData.size()); // Use first 10 features for efficiency
    
    for (size_t i = 0; i < compareLen; ++i) {
        featureSum += std::abs(sensoryData[i]);
        // Add penalty for extreme values (avoid outlier sensitivity)
        if (sensoryData[i] > 0.8f || sensoryData[i] < -0.8f) {
            featureSalience += 0.5f;
        }
    }
    
    float avgFeature = featureSum / static_cast<float>(compareLen);
    reward += avgFeature * 0.4f; // Weight for primary sensory reward
    
    // 2. Reward prediction error component
    float predictedValue = pImpl->expectedValue;
    reward += (predictedValue - avgFeature) * 0.3f; // TD error contribution
    
    // 3. Novelty bonus (from novelty system)
    reward += pImpl->noveltyBonus * 0.2f;
    
    // 4. Environmental context modulation
    reward *= pImpl->environmentalBonus * pImpl->rewardSalience;
    
    // Biological constraints and noise
    reward = std::clamp(reward, -0.5f, 2.0f); // Reasonable bounds
    reward += (dist(pImpl->rng) - 1.0f) * 0.05f; // Add small biological noise
    
    // Update prediction error
    pImpl->predictionError = reward - predictedValue;
    pImpl->predictionErrorHistory.push_back(pImpl->predictionError);
    if (pImpl->predictionErrorHistory.size() > pImpl->rewardHistorySize) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update expected value (temporal difference learning)
    pImpl->expectedValue = pImpl->expectedValue * pImpl->discountFactor + reward * (1.0f - pImpl->discountFactor);
    
    // Store in history and return
    pImpl->history.push_back(reward);
    if (pImpl->history.size() > pImpl->rewardHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

float Reward::getExpectedValue() const {
    return pImpl->expectedValue;
}

void Reward::updateExpectedValue(float actualValue) {
    // Temporal difference update for expected value
    float tdError = actualValue - pImpl->expectedValue;
    pImpl->expectedValue += tdError * (1.0f - pImpl->discountFactor);
    pImpl->predictionError = tdError;
    
    pImpl->predictionErrorHistory.push_back(tdError);
    if (pImpl->predictionErrorHistory.size() > pImpl->rewardHistorySize) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionErrorHistory.clear();
}

float Reward::getPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getDiscountFactor() const {
    return pImpl->discountFactor;
}

} // namespace nlm
