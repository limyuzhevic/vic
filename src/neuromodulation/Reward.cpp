#include "Reward.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float explorationBonus;
    float noveltyBonus;
    RewardFunctionType rewardFunction;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             explorationBonus(1.0f), noveltyBonus(0.0f),
             rewardFunction(RewardFunctionType::Curiosity) {}
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
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) {
    // PHASE 2 IMPROVEMENT: Comprehensive reward computation with multiple modalities
    // Supports different reward functions and bonus mechanisms
    
    pImpl->currentValue = 0.0f;
    
    switch (pImpl->rewardFunction) {
        case RewardFunctionType::Sparse:
            pImpl->currentValue = computeSparseReward(observation);
            break;
        case RewardFunctionType::Dense:
            pImpl->currentValue = computeDenseReward(observation);
            break;
        case RewardFunctionType::Shaped:
            pImpl->currentValue = computeShapedReward(observation);
            break;
        case RewardFunctionType::Curiosity:
            pImpl->currentValue = computeCuriosityBasedReward(observation);
            break;
        default:
            pImpl->currentValue = computeSparseReward(observation);
    }
    
    pImpl->history.push_back(pImpl->currentValue);
    return pImpl->currentValue;
}

float Reward::computeSparseReward(const Observation& observation) const {
    // Simple reward for specific goal achievement
    float reward = 0.0f;
    
    // Check for goal states
    if (observation.getFeatureByName("target_reached")) {
        reward = 10.0f;
    }
    
    // Small penalty for time to discourage lingering
    if (observation.getFeatureByName("episode_length")) {
        float episodeLength = observation.getFeatureByName("episode_length");
        reward -= episodeLength * 0.01f; // 0.01 per step
    }
    
    return reward;
}

float Reward::computeDenseReward(const Observation& observation) const {
    // Continuous reward based on progress toward goals
    float reward = 0.0f;
    
    // Distance-based reward
    if (observation.getFeatureByName("target_distance")) {
        float distance = observation.getFeatureByName("target_distance");
        reward = std::max(-1.0f, -distance * 0.1f); // Negative until target reached
    }
    
    // Reward for forward progress
    if (observation.getFeatureByName("progress")) {
        float progress = observation.getFeatureByName("progress");
        reward += progress * 0.5f; // Positive reinforcement for progress
    }
    
    // Reward for avoiding penalties
    if (observation.getFeatureByName("penalty")) {
        float penalty = observation.getFeatureByName("penalty");
        reward -= penalty * 2.0f; // Strong penalty for bad actions
    }
    
    return reward;
}

float Reward::computeShapedReward(const Observation& observation) const {
    // Shaped reward with intrinsic motivation components
    float extrinsicReward = 0.0f;
    float intrinsicReward = 0.0f;
    
    // Extrinsic component: goal achievement
    extrinsicReward = computeSparseReward(observation);
    
    // Intrinsic components: curiosity, exploration, learning
    intrinsicReward = pImpl->explorationBonus + pImpl->noveltyBonus;
    
    // Combine with weighting
    float totalReward = extrinsicReward * 0.7f + intrinsicReward * 0.3f;
    
    // Normalize to reasonable range
    return std::clamp(totalReward, -5.0f, 5.0f);
}

float Reward::computeCuriosityBasedReward(const Observation& observation) const {
    // Curiosity-driven reward with exploration bonuses
    float reward = 0.0f;
    
    // Base reward from environment (sparse + dense components)
    reward = (computeSparseReward(observation) + computeDenseReward(observation)) * 0.5f;
    
    // Curiosity bonus for novel experiences
    if (observation.getFeatureByName("novelty_level")) {
        float novelty = observation.getFeatureByName("novelty_level");
        float curiosityBonus = novelty * pImpl->explorationBonus * 2.0f;
        reward += curiosityBonus;
    }
    
    // Exploration bonus for trying new actions
    if (observation.getFeatureByName("exploration_score")) {
        float exploration = observation.getFeatureByName("exploration_score");
        float explorationBonus = exploration * pImpl->explorationBonus;
        reward += explorationBonus;
    }
    
    // Learning bonus for information gain
    if (observation.getFeatureByName("learning_signal")) {
        float learningSignal = observation.getFeatureByName("learning_signal");
        reward += learningSignal * 0.5f;
    }
    
    return reward;
}

float Reward::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

void Reward::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = bonus;
}

float Reward::getNoveltyBonus() const {
    return pImpl->noveltyBonus;
}

void Reward::setNoveltyBonus(float bonus) {
    pImpl->noveltyBonus = bonus;
}

void Reward::setRewardFunction(RewardFunctionType function) {
    pImpl->rewardFunction = function;
}

RewardFunctionType Reward::getRewardFunction() const {
    return pImpl->rewardFunction;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
