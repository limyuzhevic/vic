#include "Reward.hpp"
#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <unordered_map>

namespace nlm {

struct Reward::Impl {
    // Reward computation components
    struct {
        float currentValue;           // Current computed reward value
        float targetValue;            // Target reward value based on optimization
        float optimizationWeight;     // Weight for reward optimization
        float explorationReward;     // Reward for exploratory actions
        float exploitationReward;    // Reward for exploitative actions
        float noveltyReward;          // Reward for novel experiences
        float predictionErrorReward;  // Reward for prediction errors
        std::deque<float> rewardHistory; // History of computed rewards
        size_t maxHistorySize;        // Maximum reward history size
    } computation;
    
    // Reward function optimization
    struct {
        std::vector<float> rewardParameters;     // Parameters of the reward function
        float learningRate;                       // Learning rate for reward function optimization
        float explorationBonus;                   // Bonus for exploring new reward functions
        float exploitationBonus;                  // Bonus for exploiting known reward functions
        std::unordered_map<size_t, float> stateValues; // Value estimates for each state
        std::unordered_map<size_t, std::vector<float>> actionValues; // Value estimates for each (state, action) pair
        float valueFunctionError;                // Error between predicted and actual rewards
        bool valueFunctionConverged;             // Whether value function has converged
    } optimization;
    
    // Sparse vs dense reward handling
    struct {
        float sparseRewardThreshold;    // Threshold for considering a reward as sparse
        float denseRewardFactor;       // Factor for dense reward computation
        float intrinsicRewardWeight;   // Weight of intrinsic reward in total reward
        float extrinsicRewardWeight;   // Weight of extrinsic reward in total reward
        std::vector<float> sparseRewards;   // History of sparse rewards
        std::vector<float> denseRewards;   // History of dense rewards
        bool useSparseRewards;          // Whether to use sparse rewards
        bool useDenseRewards;           // Whether to use dense rewards
    } rewardTypes;
    
    // Reward discounting and shaping
    struct {
        float discountFactor;         // Discount factor for future rewards (gamma)
        float lambda;                 // Eligibility trace parameter (lambda)
        float rewardDecayRate;         // Decay rate for reward signals
        float rewardSmoothing;        // Smoothing factor for reward computation
        std::deque<float> discountedRewards; // History of discounted rewards
        std::deque<float> shapedRewards;     // History of shaped rewards
        size_t discountHistorySize;   // Maximum discounted reward history size
    } discounting;
    
    // Implementation tracking
    bool initialized;                // Whether reward system has been initialized
    bool enabled;                     // Whether reward system is enabled
    float lastUpdateTime;             // Last update timestamp
    float totalCumulativeReward;      // Total cumulative reward over time
    float rewardPredictionError;     // Error between predicted and actual rewards
    
    Impl() {
        // Initialize reward computation
        computation.currentValue = 0.0f;
        computation.targetValue = 0.0f;
        computation.optimizationWeight = 0.5f;
        computation.explorationReward = 0.1f;
        computation.exploitationReward = 0.8f;
        computation.noveltyReward = 0.2f;
        computation.predictionErrorReward = 0.3f;
        computation.maxHistorySize = 1000;
        
        // Initialize optimization
        optimization.learningRate = 0.01f;
        optimization.explorationBonus = 0.2f;
        optimization.exploitationBonus = 0.9f;
        optimization.valueFunctionError = 0.0f;
        optimization.valueFunctionConverged = false;
        
        // Initialize reward types
        rewardTypes.sparseRewardThreshold = 0.5f;
        rewardTypes.denseRewardFactor = 0.1f;
        rewardTypes.intrinsicRewardWeight = 0.3f;
        rewardTypes.extrinsicRewardWeight = 0.7f;
        rewardTypes.useSparseRewards = true;
        rewardTypes.useDenseRewards = true;
        
        // Initialize discounting
        discounting.discountFactor = 0.95f;
        discounting.lambda = 0.9f;
        discounting.rewardDecayRate = 0.99f;
        discounting.rewardSmoothing = 0.1f;
        discounting.discountHistorySize = 500;
        
        // Implementation tracking
        initialized = false;
        enabled = true;
        lastUpdateTime = 0.0f;
        totalCumulativeReward = 0.0f;
        rewardPredictionError = 0.0f;
    }
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->computation.currentValue;
}

void Reward::setValue(float value) {
    pImpl->computation.currentValue = value;
    pImpl->computation.rewardHistory.push_back(value);
    
    // Maintain history size
    if (pImpl->computation.rewardHistory.size() > pImpl->computation.maxHistorySize) {
        pImpl->computation.rewardHistory.pop_front();
    }
    
    // Update total cumulative reward
    totalCumulativeReward += value;
}

void Reward::add(float delta) {
    setValue(getValue() + delta);
}

void Reward::reset() {
    pImpl->computation.currentValue = 0.0f;
    pImpl->computation.targetValue = 0.0f;
    pImpl->computation.rewardHistory.clear();
    
    NLM_LOG_INFO("Reward system reset");
}

float Reward::computeReward(const Observation& observation) const {
    if (!pImpl->initialized) return 0.0f;
    
    // Compute reward from observation
    float reward = 0.0f;
    
    // Extract features from observation
    std::vector<float> features = extractObservationFeatures(observation);
    
    // Compute component rewards
    float extrinsicReward = computeExtrinsicReward(features);
    float intrinsicReward = computeIntrinsicReward(features);
    float shapedReward = computeShapedReward(features);
    
    // Combine rewards based on weights
    reward = extrinsicReward * pImpl->rewardTypes.extrinsicRewardWeight +
             intrinsicReward * pImpl->rewardTypes.intrinsicRewardWeight +
             shapedReward * (1.0f - pImpl->rewardTypes.extrinsicRewardWeight - 
                           pImpl->rewardTypes.intrinsicRewardWeight);
    
    // Apply reward optimization
    reward = optimizeReward(reward, features);
    
    // Apply discounting
    reward = applyDiscounting(reward);
    
    // Update history
    pImpl->computation.rewardHistory.push_back(reward);
    if (pImpl->computation.rewardHistory.size() > pImpl->computation.maxHistorySize) {
        pImpl->computation.rewardHistory.pop_front();
    }
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->computation.rewardHistory;
}

void Reward::clearHistory() {
    pImpl->computation.rewardHistory.clear();
    NLM_LOG_INFO("Reward history cleared");
}

void Reward::updateRewardOptimization() {
    if (!pImpl->initialized) return;
    
    // Update value function
    updateValueFunction();
    
    // Update reward parameters
    updateRewardParameters();
    
    // Check for convergence
    checkValueFunctionConvergence();
}

void Reward::updateValueFunction() {
    if (!pImpl->brain) return;
    
    // Update state value estimates
    for (const auto& pair : pImpl->optimization.stateValues) {
        size_t state = pair.first;
        float currentValue = pair.second;
        
        // Get current reward
        float reward = getValue();
        
        // TD update for state value
        float tdUpdate = pImpl->optimization.learningRate * 
                        (reward - currentValue);
        
        setStateValue(state, currentValue + tdUpdate);
    }
}

void Reward::updateRewardParameters() {
    // Update reward function parameters based on performance
    if (pImpl->computation.rewardHistory.size() < 10) return;
    
    // Compute average reward
    float sum = std::accumulate(pImpl->computation.rewardHistory.begin(),
                               pImpl->computation.rewardHistory.end(), 0.0f);
    float avgReward = sum / pImpl->computation.rewardHistory.size();
    
    // Update reward parameters based on average reward
    for (float& param : pImpl->optimization.rewardParameters) {
        param += pImpl->optimization.learningRate * (avgReward - param);
    }
}

void Reward::checkValueFunctionConvergence() {
    if (pImpl->computation.rewardHistory.size() < 50) return;
    
    // Compute value function error
    float sum = std::accumulate(pImpl->computation.rewardHistory.begin(),
                               pImpl->computation.rewardHistory.end(), 0.0f);
    float meanReward = sum / pImpl->computation.rewardHistory.size();
    
    // Compute standard deviation
    float variance = 0.0f;
    for (float reward : pImpl->computation.rewardHistory) {
        variance += (reward - meanReward) * (reward - meanReward);
    }
    variance /= pImpl->computation.rewardHistory.size();
    
    pImpl->optimization.valueFunctionError = std::sqrt(variance);
    
    // Check for convergence (small error and stable parameters)
    bool converged = pImpl->optimization.valueFunctionError < 0.01f;
    
    if (converged && !pImpl->optimization.valueFunctionConverged) {
        NLM_LOG_INFO("Reward value function converged");
        pImpl->optimization.valueFunctionConverged = true;
    }
}

float Reward::computeExtrinsicReward(const std::vector<float>& features) const {
    if (!pImpl->brain) return 0.0f;
    
    // Get current state from brain
    size_t currentState = pImpl->optimization.stateValues.empty() ? 0 : 
                         pImpl->optimization.stateValues.begin()->first;
    
    // Compute extrinsic reward based on state value
    float stateValue = getStateValue(currentState);
    
    // Apply reward shaping
    float shapedReward = stateValue * pImpl->rewardTypes.denseRewardFactor;
    
    // Check if reward is sparse or dense
    if (std::abs(shapedReward) < pImpl->rewardTypes.sparseRewardThreshold) {
        // Sparse reward
        if (pImpl->rewardTypes.useSparseRewards) {
            return shapedReward;
        }
    } else {
        // Dense reward
        if (pImpl->rewardTypes.useDenseRewards) {
            return shapedReward;
        }
    }
    
    return 0.0f;
}

float Reward::computeIntrinsicReward(const std::vector<float>& features) const {
    if (!pImpl->brain) return 0.0f;
    
    // Get neuromodulators from brain
    auto* curiosity = pImpl->brain->getCuriosity();
    auto* novelty = pImpl->brain->getNovelty();
    auto* predictionError = pImpl->brain->getPredictionErrorSignal();
    
    // Compute intrinsic reward components
    float curiosityReward = (curiosity ? curiosity->getLevel() * pImpl->computation.explorationReward : 0.0f);
    float noveltyReward = (novelty ? novelty->getLevel() * pImpl->computation.noveltyReward : 0.0f);
    float predictionErrorReward = (predictionError ? 
                                  std::abs(predictionError->getError()) * pImpl->computation.predictionErrorReward : 0.0f);
    
    // Combine intrinsic reward components
    float intrinsicReward = curiosityReward + noveltyReward + predictionErrorReward;
    
    return intrinsicReward;
}

float Reward::computeShapedReward(const std::vector<float>& features) const {
    if (!pImpl->brain) return 0.0f;
    
    // Get current state
    size_t currentState = pImpl->optimization.stateValues.empty() ? 0 : 
                         pImpl->optimization.stateValues.begin()->first;
    
    // Get action from brain if available
    size_t currentAction = 0;
    auto* action = pImpl->brain->produceAction();
    if (action) {
        // In a real implementation, this would extract action ID
        currentAction = 0; // Placeholder
    }
    
    // Compute shaped reward using value function
    float stateValue = getStateValue(currentState);
    float actionValue = getActionValue(currentState, currentAction);
    
    // Apply reward shaping
    float shapedReward = stateValue - actionValue; // Advantage calculation
    
    return shapedReward;
}

float Reward::optimizeReward(float reward, const std::vector<float>& features) {
    if (!pImpl->initialized) return reward;
    
    // Apply reward optimization
    // This would typically involve policy gradient or actor-critic methods
    
    // For now, apply simple smoothing and clipping
    float optimizedReward = reward * pImpl->discounting.rewardSmoothing + 
                           getValue() * (1.0f - pImpl->discounting.rewardSmoothing);
    
    // Clip reward to reasonable range
    optimizedReward = std::clamp(optimizedReward, -10.0f, 10.0f);
    
    return optimizedReward;
}

float Reward::applyDiscounting(float reward) {
    if (!pImpl->initialized) return reward;
    
    // Apply exponential discounting
    float discountedReward = reward * pImpl->discounting.discountFactor;
    
    // Store in discounted rewards history
    pImpl->discounting.discountedRewards.push_back(discountedReward);
    if (pImpl->discounting.discountedRewards.size() > pImpl->discounting.discountHistorySize) {
        pImpl->discounting.discountedRewards.pop_front();
    }
    
    // Apply lambda eligibility traces
    float lambdaDiscounted = discountedReward * pImpl->discounting.lambda;
    
    // Store in shaped rewards history
    pImpl->discounting.shapedRewards.push_back(lambdaDiscounted);
    if (pImpl->discounting.shapedRewards.size() > pImpl->discounting.discountHistorySize) {
        pImpl->discounting.shapedRewards.pop_front();
    }
    
    return lambdaDiscounted;
}

std::vector<float> Reward::extractObservationFeatures(const Observation& observation) const {
    std::vector<float> features;
    
    // Extract features from observation data
    const auto& data = observation.getData();
    features.assign(data.begin(), data.end());
    
    // Add additional features
    features.push_back(observation.getTimestamp());
    features.push_back(observation.getConfidence());
    
    // Normalize features
    normalizeFeatures(features);
    
    return features;
}

void Reward::normalizeFeatures(std::vector<float>& features) const {
    if (features.empty()) return;
    
    // Compute mean and standard deviation
    float sum = std::accumulate(features.begin(), features.end(), 0.0f);
    float mean = sum / features.size();
    
    float variance = 0.0f;
    for (float feature : features) {
        variance += (feature - mean) * (feature - mean);
    }
    variance /= features.size();
    
    float stdDev = std::sqrt(variance);
    
    // Normalize features
    if (stdDev > 0.0f) {
        for (float& feature : features) {
            feature = (feature - mean) / stdDev;
        }
    }
}

float Reward::getStateValue(size_t state) const {
    auto it = pImpl->optimization.stateValues.find(state);
    if (it != pImpl->optimization.stateValues.end()) {
        return it->second;
    }
    return 0.0f;
}

void Reward::setStateValue(size_t state, float value) {
    pImpl->optimization.stateValues[state] = value;
}

float Reward::getActionValue(size_t state, size_t action) const {
    auto stateIt = pImpl->optimization.actionValues.find(state);
    if (stateIt != pImpl->optimization.actionValues.end()) {
        auto actionMap = stateIt->second;
        auto actionIt = actionMap.find(action);
        if (actionIt != actionMap.end()) {
            return actionIt->second;
        }
    }
    return 0.0f;
}

} // namespace nlm
