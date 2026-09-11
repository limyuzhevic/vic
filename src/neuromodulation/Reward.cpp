#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Reward::Impl {
    // Current reward state
    float currentValue;
    float accumulatedReward;
    float predictionError;
    
    // Reward history for learning
    std::vector<float> history;
    size_t maxHistorySize;
    
    // Reward computation parameters
    float baselineValue;
    float rewardScale;
    float predictionErrorScale;
    
    // Temporal components
    float immediateReward;
    float expectedFutureReward;
    float discountedFutureReward;
    
    // Neural integration
    float dopamineInfluence;
    float neuromodulationLevel;
    
    // Exploration/exploitation balance
    float explorationBonus;
    float exploitationValue;
    
    // Time tracking
    SimulationStep lastUpdateTime;
    SimulationStep currentTime;
    
    // Random generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> rewardDist;
    std::uniform_real_distribution<float> explorationDist;
    
    Impl()
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , predictionError(0.0f)
        , maxHistorySize(1000)
        , baselineValue(0.0f)
        , rewardScale(1.0f)
        , predictionErrorScale(0.5f)
        , immediateReward(0.0f)
        , expectedFutureReward(0.0f)
        , discountedFutureReward(0.0f)
        , dopamineInfluence(1.0f)
        , neuromodulationLevel(1.0f)
        , explorationBonus(0.0f)
        , exploitationValue(0.0f)
        , lastUpdateTime(0)
        , currentTime(0)
        , rng(std::random_device{}())
        , rewardDist(-0.5f, 2.0f)
        , explorationDist(0.0f, 1.0f)
    {
        history.reserve(maxHistorySize);
    }
};

Reward::Reward() : pImpl(new Impl()) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
    pImpl->history.push_back(value);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::add(float delta) {
    // Compute prediction error based on delta
    pImpl->predictionError = delta;
    
    // Apply neuromodulation influence
    float modulatedDelta = delta * pImpl->dopamineInfluence * pImpl->neuromodulationLevel;
    
    // Update current and accumulated rewards
    pImpl->currentValue += modulatedDelta;
    pImpl->accumulatedReward += modulatedDelta;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentValue);
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Log reward with prediction error
    NLM_LOG_INFO("Reward: delta=" + std::to_string(delta) +
                 " error=" + std::to_string(pImpl->predictionError) +
                 " current=" + std::to_string(pImpl->currentValue) +
                 " accumulated=" + std::to_string(pImpl->accumulatedReward));
}

void Reward::reset() {
    pImpl->currentValue = pImpl->baselineValue;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Extract relevant features from observation for reward computation
    float totalReward = 0.0f;
    
    // Check for resource access
    const auto& objects = observation.getObjects();
    for (const auto& obj : objects) {
        if (obj.type == WorldObjectType::Resource) {
            // Positive reward for resources
            totalReward += obj.value * 2.0f;  // Double value for finding resources
        } else if (obj.type == WorldObjectType::Hazard) {
            // Negative reward for hazards
            totalReward -= std::abs(obj.value) * 1.5f;  // More penalty for hazards
        }
    }
    
    // Bonus for exploration (novelty)
    if (!objects.empty()) {
        totalReward += pImpl->explorationBonus * 0.5f;
    }
    
    // Calculate prediction error contribution
    float predictionErrorContribution = std::abs(pImpl->predictionError) * pImpl->predictionErrorScale;
    totalReward += predictionErrorContribution;
    
    // Add expected future reward component
    totalReward += pImpl->expectedFutureReward * 0.3f;
    
    // Apply exploration-exploitation balance
    if (pImpl->explorationDist(pImpl->rng) < 0.3f) {
        // Exploration: add randomness for discovery
        totalReward += rewardDist(pImpl->rng) * 0.2f;
    }
    
    // Scale and clamp reward
    totalReward *= pImpl->rewardScale;
    if (totalReward < -1.0f) totalReward = -1.0f;
    if (totalReward > 2.0f) totalReward = 2.0f;
    
    return totalReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm