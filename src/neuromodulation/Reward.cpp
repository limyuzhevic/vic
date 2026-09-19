#include "Reward.hpp"
#include "../world/Observation.hpp"
#include "../environment/Environment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    std::vector<std::pair<float, float>> rewardEvents;  // (reward, confidence)
    
    // Reward computation components
    float explorationBonus;
    float riskSensitivity;
    float temporalDiscount;
    float noveltyBonus;
    
    // Reward prediction components
    float predictedReward;
    float rewardPredictionError;
    float learningRate;
    
    // State tracking for reward computation
    float lastActionValue;
    std::vector<float> actionValues;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), explorationBonus(0.1f),
             riskSensitivity(0.5f), temporalDiscount(0.95f), noveltyBonus(0.0f),
             predictedReward(0.0f), rewardPredictionError(0.0f), learningRate(0.01f),
             lastActionValue(0.0f) {}
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
    
    // Record reward event
    pImpl->rewardEvents.emplace_back(delta, 1.0f);
    
    // Limit history size
    if (pImpl->rewardEvents.size() > 1000) {
        pImpl->rewardEvents.erase(pImpl->rewardEvents.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->rewardEvents.clear();
    pImpl->explorationBonus = 0.1f;
}

float Reward::computeReward(const Observation& observation) const {
    // Implement real reward computation from observation
    // This combines multiple reward sources for a comprehensive value estimate
    
    float baseReward = 0.0f;
    
    // 1. Resource-based rewards
    baseReward += computeResourceRewards(observation);
    
    // 2. Hazard-based penalties
    baseReward += computeHazardPenalties(observation);
    
    // 3. Action outcome rewards
    baseReward += computeActionRewards(observation);
    
    // 4. Novelty bonus (exploration reward)
    baseReward += computeNoveltyBonus(observation);
    
    // 5. Efficiency bonus (consummate resources quickly)
    baseReward += computeEfficiencyBonus(observation);
    
    // 6. Risk-adjusted reward
    baseReward = applyRiskSensitivity(baseReward, observation);
    
    // 7. Temporal discounting
    baseReward *= pImpl->temporalDiscount;
    
    // 8. Exploration bonus based on action diversity
    baseReward += pImpl->explorationBonus;
    
    // Store in history
    pImpl->history.push_back(baseReward);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    NLM_LOG_INFO("Computed reward: " + std::to_string(baseReward));
    
    return baseReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

void Reward::recordReward(float reward, float confidence) {
    // Record reward with confidence for learning
    pImpl->rewardEvents.emplace_back(reward, confidence);
    
    // Update predicted reward with confidence-weighted learning
    float effectiveLearningRate = pImpl->learningRate * confidence;
    pImpl->predictedReward = pImpl->predictedReward * (1.0f - effectiveLearningRate) +
                             reward * effectiveLearningRate;
    
    // Calculate reward prediction error
    pImpl->rewardPredictionError = reward - pImpl->predictedReward;
    
    NLM_LOG_INFO("Recorded reward: " + std::to_string(reward) + ", confidence: " + std::to_string(confidence) +
                 ", prediction error: " + std::to_string(pImpl->rewardPredictionError));
}

float Reward::getPredictedReward() const {
    return pImpl->predictedReward;
}

float Reward::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float reward : pImpl->history) {
        sum += reward;
    }
    return sum / pImpl->history.size();
}

std::vector<float> Reward::getRewardHistory() const {
    return pImpl->history;
}

bool Reward::isHighReward() const {
    return !pImpl->history.empty() && pImpl->history.back() > 10.0f;
}

void Reward::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = bonus;
}

void Reward::setRiskSensitivity(float sensitivity) {
    pImpl->riskSensitivity = sensitivity;
}

void Reward::setTemporalDiscount(float discount) {
    pImpl->temporalDiscount = discount;
}

float Reward::computeResourceRewards(const Observation& observation) const {
    // Reward for consuming resources (food, water, etc.)
    float resourceReward = 0.0f;
    
    if (observation.resourceValue > 0.0f) {
        // Resource value scaled by resource size
        resourceReward = observation.resourceValue * 10.0f;
        
        // Bonus for picking up multiple resources
        if (observation.resourcesCollected > 1) {
            resourceReward *= 1.5f;
        }
    }
    
    return resourceReward;
}

float Reward::computeHazardPenalties(const Observation& observation) const {
    // Penalty for hazards and dangerous situations
    float hazardPenalty = 0.0f;
    
    if (observation.hazardValue < 0.0f) {
        // Hazard value is negative, convert to penalty
        hazardPenalty = observation.hazardValue * 15.0f; // Scale penalty
    }
    
    // Additional penalty for close proximity to hazards
    if (observation.distanceToHazard < 2.0f) {
        hazardPenalty += (2.0f - observation.distanceToHazard) * 5.0f;
    }
    
    return hazardPenalty;
}

float Reward::computeActionRewards(const Observation& observation) const {
    // Reward based on action outcomes
    float actionReward = 0.0f;
    
    // Reward for successful actions
    if (observation.actionSuccess) {
        actionReward += 1.0f;
        
        // Bonus for efficiency (fewer steps to success)
        if (observation.stepsToSuccess < 10) {
            actionReward += (10 - observation.stepsToSuccess) * 0.5f;
        }
    }
    
    // Penalty for failed actions
    if (!observation.actionSuccess && observation.actionAttempted) {
        actionReward -= 2.0f;
    }
    
    // Reward for reaching goals
    if (observation.goalReached) {
        actionReward += 5.0f;
    }
    
    return actionReward;
}

float Reward::computeNoveltyBonus(const Observation& observation) const {
    // Bonus for exploring new areas or discovering new resources
    float noveltyBonus = 0.0f;
    
    // Bonus for new resource types
    if (observation.newResourceType) {
        noveltyBonus += 2.0f;
    }
    
    // Bonus for first-time hazard discovery
    if (observation.firstTimeHazard) {
        noveltyBonus += 1.0f;
    }
    
    // Bonus for novel navigation patterns
    if (observation.novelPathTaken) {
        noveltyBonus += 0.5f;
    }
    
    // Scale by exploration level
    noveltyBonus *= (1.0f + pImpl->explorationBonus);
    
    return noveltyBonus;
}

float Reward::computeEfficiencyBonus(const Observation& observation) const {
    // Bonus for efficient behavior (low energy cost for high reward)
    float efficiencyBonus = 0.0f;
    
    if (observation.energyCost > 0.0f && observation.rewardReceived > 0.0f) {
        // Calculate efficiency (reward per energy unit)
        float efficiency = observation.rewardReceived / observation.energyCost;
        
        // Bonus for high efficiency
        efficiencyBonus = efficiency * 2.0f;
        
        // Extra bonus for excellent efficiency
        if (efficiency > 2.0f) {
            efficiencyBonus += (efficiency - 2.0f) * 3.0f;
        }
    }
    
    return efficiencyBonus;
}

float Reward::applyRiskSensitivity(float baseReward, const Observation& observation) const {
    // Apply risk sensitivity to reward (risk-seeking vs risk-averse)
    
    // Calculate risk measure (variance of outcomes)
    float riskMeasure = observation.actionRisk;
    
    // Apply sensitivity
    if (pImpl->riskSensitivity > 0.5f) {
        // Risk-seeking: increase reward for risky actions
        baseReward *= (1.0f + (riskMeasure * 2.0f));
    } else {
        // Risk-averse: decrease reward for risky actions
        baseReward *= (1.0f - (riskMeasure * 2.0f));
    }
    
    return std::clamp(baseReward, -10.0f, 10.0f);
}

float Reward::getLearningRate() const {
    return pImpl->learningRate;
}

void Reward::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

} // namespace nlm