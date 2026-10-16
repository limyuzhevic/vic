#pragma once

#include "Action.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Action selection algorithm types
enum class ActionSelectionAlgorithm {
    WinnerTakeAll,
    Stochastic,
    Softmax,
    ThompsonSampling
};

// Neural activity-based action selection strategies
class ActionSelectionStrategy {
public:
    virtual ~ActionSelectionStrategy() = default;
    virtual std::vector<float> scoreActions(const std::vector<ActionType>& availableActions,
                                            const NeuralRegion& motorRegion) = 0;
    virtual std::unique_ptr<Action> selectAction(const std::vector<ActionType>& availableActions,
                                                 const std::vector<float>& scores,
                                                 std::shared_ptr<RandomGenerator> rng) = 0;
};

class WinnerTakeAllStrategy : public ActionSelectionStrategy {
public:
    std::vector<float> scoreActions(const std::vector<ActionType>& availableActions,
                                    const NeuralRegion& motorRegion) override {
        std::vector<float> scores;
        
        // Aggregate activity across motor neuron populations
        float maxActivity = 0.0f;
        
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                float activity = pop->getActivityLevel();
                scores.push_back(activity);
                maxActivity = std::max(maxActivity, activity);
            }
        }
        
        // If no motor populations found, try to find action mapping
        if (scores.empty()) {
            // Default to equal weights for all available actions
            for (size_t i = 0; i < availableActions.size(); ++i) {
                scores.push_back(1.0f);
            }
            maxActivity = 1.0f;
        }
        
        // Normalize scores
        if (maxActivity > 0.0f) {
            for (float& score : scores) {
                score /= maxActivity;
            }
        }
        
        return scores;
    }
    
    std::unique_ptr<Action> selectAction(const std::vector<ActionType>& availableActions,
                                         const std::vector<float>& scores,
                                         std::shared_ptr<RandomGenerator> rng) override {
        if (scores.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Find action with highest score (winner-take-all)
        float maxScore = -1.0f;
        size_t maxIndex = 0;
        
        for (size_t i = 0; i < scores.size(); ++i) {
            if (scores[i] > maxScore) {
                maxScore = scores[i];
                maxIndex = i;
            }
        }
        
        // Add exploration noise
        float noise = rng ? rng->normal(0.0f, 0.05f) : 0.0f;
        float explorationScore = scores[maxIndex] + noise;
        
        if (explorationScore < 0.1f && !availableActions.empty()) {
            // If main action is too suppressed, try another one
            size_t alternativeIndex = (maxIndex + 1) % availableActions.size();
            return std::make_unique<Action>(availableActions[alternativeIndex]);
        }
        
        return std::make_unique<Action>(availableActions[maxIndex]);
    }
};

class StochasticStrategy : public ActionSelectionStrategy {
public:
    std::vector<float> scoreActions(const std::vector<ActionType>& availableActions,
                                    const NeuralRegion& motorRegion) override {
        std::vector<float> scores;
        
        // Convert neural activity to scores
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                float activity = pop->getActivityLevel();
                scores.push_back(activity);
            }
        }
        
        // Ensure we have scores for all available actions
        if (scores.size() != availableActions.size()) {
            float avgActivity = 0.0f;
            if (!scores.empty()) {
                float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
                avgActivity = sum / static_cast<float>(scores.size());
            }
            
            while (scores.size() < availableActions.size()) {
                scores.push_back(avgActivity);
            }
        }
        
        return scores;
    }
    
    std::unique_ptr<Action> selectAction(const std::vector<ActionType>& availableActions,
                                         const std::vector<float>& scores,
                                         std::shared_ptr<RandomGenerator> rng) override {
        if (availableActions.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Select action based on neural activity probabilities
        std::vector<float> probabilities;
        float maxScore = *std::max_element(scores.begin(), scores.end());
        
        // Convert scores to probabilities using softmax-like approach
        float sum = 0.0f;
        for (float score : scores) {
            float expScore = std::exp(score);
            probabilities.push_back(expScore);
            sum += expScore;
        }
        
        if (sum > 0.0f) {
            for (float& prob : probabilities) {
                prob /= sum;
            }
        } else {
            // If all scores are zero or negative, use uniform distribution
            float uniformProb = 1.0f / static_cast<float>(availableActions.size());
            for (float& prob : probabilities) {
                prob = uniformProb;
            }
        }
        
        // Select action based on probabilities
        float rand = rng ? rng->uniformReal(0.0f, 1.0f) : 0.5f;
        float cumulative = 0.0f;
        
        for (size_t i = 0; i < availableActions.size(); ++i) {
            cumulative += probabilities[i];
            if (rand <= cumulative) {
                return std::make_unique<Action>(availableActions[i]);
            }
        }
        
        // Fallback to last action
        return std::make_unique<Action>(availableActions.back());
    }
};

class SoftmaxStrategy : public ActionSelectionStrategy {
private:
    float temperature_;
    
public:
    explicit SoftmaxStrategy(float temperature = 1.0f) : temperature_(temperature) {}
    
    std::vector<float> scoreActions(const std::vector<ActionType>& availableActions,
                                    const NeuralRegion& motorRegion) override {
        std::vector<float> scores;
        
        // Get neural activity for motor populations
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                float activity = pop->getActivityLevel();
                scores.push_back(activity);
            }
        }
        
        // Pad if needed
        if (scores.size() < availableActions.size()) {
            float maxScore = scores.empty() ? 0.0f : *std::max_element(scores.begin(), scores.end());
            while (scores.size() < availableActions.size()) {
                scores.push_back(maxScore);
            }
        }
        
        return scores;
    }
    
    std::unique_ptr<Action> selectAction(const std::vector<ActionType>& availableActions,
                                         const std::vector<float>& scores,
                                         std::shared_ptr<RandomGenerator> rng) override {
        if (availableActions.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Apply temperature-controlled softmax
        std::vector<float> probabilities;
        float sum = 0.0f;
        
        for (float score : scores) {
            float scaledScore = score / temperature_;
            float expScore = std::exp(scaledScore);
            probabilities.push_back(expScore);
            sum += expScore;
        }
        
        if (sum > 0.0f) {
            for (float& prob : probabilities) {
                prob /= sum;
            }
        } else {
            float uniformProb = 1.0f / static_cast<float>(availableActions.size());
            for (float& prob : probabilities) {
                prob = uniformProb;
            }
        }
        
        // Select action based on probabilities with exploration
        float rand = rng ? rng->uniformReal(0.0f, 1.0f) : 0.5f;
        float cumulative = 0.0f;
        
        for (size_t i = 0; i < availableActions.size(); ++i) {
            cumulative += probabilities[i];
            if (rand <= cumulative) {
                return std::make_unique<Action>(availableActions[i]);
            }
        }
        
        return std::make_unique<Action>(availableActions.back());
    }
};

class ThompsonSamplingStrategy : public ActionSelectionStrategy {
private:
    std::unordered_map<ActionType, float> successCount_;
    std::unordered_map<ActionType, float> totalCount_;
    float learningRate_;
    
public:
    explicit ThompsonSamplingStrategy(float learningRate = 0.1f) 
        : learningRate_(learningRate) {}
    
    std::vector<float> scoreActions(const std::vector<ActionType>& availableActions,
                                    const NeuralRegion& motorRegion) override {
        std::vector<float> scores;
        
        // Update success counts based on neural activity
        for (ActionType action : availableActions) {
            float neuralActivity = getNeuralActivityForAction(motorRegion, action);
            scores.push_back(learnActionPreference(action, neuralActivity));
        }
        
        return scores;
    }
    
    std::unique_ptr<Action> selectAction(const std::vector<ActionType>& availableActions,
                                         const std::vector<float>& scores,
                                         std::shared_ptr<RandomGenerator> rng) override {
        if (availableActions.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Thompson sampling: sample from beta distributions
        std::vector<float> samples;
        
        for (size_t i = 0; i < availableActions.size(); ++i) {
            ActionType action = availableActions[i];
            float alpha = successCount_.count(action) ? successCount_[action] + 1.0f : 1.0f;
            float beta = (totalCount_.count(action) ? totalCount_[action] - successCount_[action] : 0.0f) + 1.0f;
            
            float sample = sampleBetaDistribution(alpha, beta, rng.get());
            samples.push_back(sample);
        }
        
        // Find action with highest sample
        size_t maxIndex = 0;
        float maxSample = samples[0];
        
        for (size_t i = 1; i < samples.size(); ++i) {
            if (samples[i] > maxSample) {
                maxSample = samples[i];
                maxIndex = i;
            }
        }
        
        return std::make_unique<Action>(availableActions[maxIndex]);
    }
    
    void updateActionOutcome(ActionType action, bool success) {
        totalCount_[action] += 1.0f;
        if (success) {
            successCount_[action] += 1.0f;
        }
    }
    
    float getNeuralActivityForAction(const NeuralRegion& motorRegion, ActionType action) const {
        // For now, return average activity - could be enhanced with action-specific mappings
        float totalActivity = 0.0f;
        int motorPopCount = 0;
        
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                totalActivity += pop->getActivityLevel();
                motorPopCount++;
            }
        }
        
        return motorPopCount > 0 ? totalActivity / static_cast<float>(motorPopCount) : 0.0f;
    }
    
    float learnActionPreference(ActionType action, float outcome) {
        // Simple temporal difference learning for action preferences
        if (!successCount_.count(action)) {
            successCount_[action] = outcome;
            totalCount_[action] = 1.0f;
            return outcome;
        }
        
        float currentPref = successCount_[action] / totalCount_[action];
        float tdError = outcome - currentPref;
        successCount_[action] += learningRate_ * tdError;
        totalCount_[action] += 1.0f;
        
        return successCount_[action] / totalCount_[action];
    }
    
    float sampleBetaDistribution(float alpha, float beta, RandomGenerator* rng) {
        if (!rng) return 0.5f;
        
        // Simple beta sampling using inverse transform method
        // In practice, could use more efficient methods
        for (int iter = 0; iter < 100; ++iter) {
            float u1 = rng->uniformReal(0.0f, 1.0f);
            float u2 = rng->uniformReal(0.0f, 1.0f);
            
            float sample = std::sqrt(u1) / (std::sqrt(u1) + std::sqrt(-std::log(u2) * (alpha + beta - 2) / beta + 1.0f));
            return sample;
        }
        return alpha / (alpha + beta);  // Fallback
    }
};

class MotorSystem::Impl {
public:
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    ActionSelectionAlgorithm algorithm;
    float explorationNoise;
    float temperature;
    
    // Learning and state tracking
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, std::vector<float>> actionNeuralCorrelations;
    std::unordered_map<ActionType, int> actionSuccessCount;
    std::unordered_map<ActionType, int> actionTotalCount;
    
    std::shared_ptr<RandomGenerator> rng;
    
    // Strategy instances
    std::unique_ptr<ActionSelectionStrategy> winnerTakeAllStrategy;
    std::unique_ptr<ActionSelectionStrategy> stochasticStrategy;
    std::unique_ptr<ActionSelectionStrategy> softmaxStrategy;
    std::unique_ptr<ActionSelectionStrategy> thompsonSamplingStrategy;
    
    Impl() {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::LookUp,
            ActionType::LookDown,
            ActionType::Interact,
            ActionType::Eat,
            ActionType::Drink,
            ActionType::Rest,
            ActionType::Wait,
            ActionType::Custom
        };
        
        algorithm = ActionSelectionAlgorithm::Softmax;
        explorationNoise = 0.1f;
        temperature = 1.0f;
        
        // Initialize strategies
        winnerTakeAllStrategy = std::make_unique<WinnerTakeAllStrategy>();
        stochasticStrategy = std::make_unique<StochasticStrategy>();
        softmaxStrategy = std::make_unique<SoftmaxStrategy>(temperature);
        thompsonSamplingStrategy = std::make_unique<ThompsonSamplingStrategy>();
        
        // Initialize random generator
        rng = std::make_shared<RandomGenerator>();
        
        // Initialize action preferences
        for (ActionType action : availableActions) {
            actionPreferences[action] = 1.0f;  // Equal initial preference
            actionSuccessCount[action] = 0;
            actionTotalCount[action] = 0;
        }
    }
    
    // Get the appropriate strategy based on algorithm
    ActionSelectionStrategy* getStrategy() {
        switch (algorithm) {
            case ActionSelectionAlgorithm::WinnerTakeAll:
                return winnerTakeAllStrategy.get();
            case ActionSelectionAlgorithm::Stochastic:
                return stochasticStrategy.get();
            case ActionSelectionAlgorithm::Softmax:
                return softmaxStrategy.get();
            case ActionSelectionAlgorithm::ThompsonSampling:
                return thompsonSamplingStrategy.get();
            default:
                return softmaxStrategy.get();
        }
    }
    
    // Update action preferences based on outcome
    void updateActionPreference(ActionType action, bool success, float neuralActivity) {
        // Temporal difference learning
        float currentPref = actionPreferences[action];
        float tdError = static_cast<float>(success) - currentPref;
        float learningRate = 0.1f; // Could be made adaptive
        
        actionPreferences[action] += learningRate * tdError;
        
        // Record success/failure for Thompson sampling
        actionTotalCount[action]++;
        if (success) {
            actionSuccessCount[action]++;
        }
        
        // Store neural correlation
        actionNeuralCorrelations[action].push_back(neuralActivity);
        if (actionNeuralCorrelations[action].size() > 100) {
            actionNeuralCorrelations[action].erase(
                actionNeuralCorrelations[action].begin());
        }
    }
    
    // Calculate enhanced score combining neural activity and preferences
    float calculateEnhancedScore(float neuralActivity, ActionType action) const {
        float baseScore = neuralActivity;
        float prefScore = actionPreferences[action];
        
        // Combine neural activity and learned preferences
        float score = baseScore * 0.7f + prefScore * 0.3f;
        
        // Add history-based bonus
        int totalCount = actionTotalCount.at(action);
        if (totalCount > 0) {
            float successRate = static_cast<float>(actionSuccessCount.at(action)) / totalCount;
            score += successRate * 0.2f;
        }
        
        // Add exploration bonus for less-used actions
        float usagePenalty = std::log(static_cast<float>(totalCount + 1)) * 0.05f;
        score -= usagePenalty;
        
        return std::max(0.0f, score);  // Ensure non-negative
    }
    
    // Get average neural activity from motor region
    float getAverageMotorActivity(const NeuralRegion& motorRegion) const {
        float totalActivity = 0.0f;
        int motorPopCount = 0;
        
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                totalActivity += pop->getActivityLevel();
                motorPopCount++;
            }
        }
        
        return motorPopCount > 0 ? totalActivity / static_cast<float>(motorPopCount) : 0.0f;
    }
    
    // Get neural activity for specific action (placeholder implementation)
    float getActionSpecificActivity(const NeuralRegion& motorRegion, ActionType action) const {
        // In a real implementation, this would map specific motor neuron patterns
        // to specific actions based on connectivity and tuning
        float baseActivity = getAverageMotorActivity(motorRegion);
        
        // Add action-specific modulation based on preferences
        float actionMod = actionPreferences.at(action);
        
        // Add noise for exploration
        float noise = rng ? rng->normal(0.0f, explorationNoise) : 0.0f;
        
        return baseActivity * actionMod + noise;
    }
};

} // namespace nlm

// MotorSystem.cpp implementation
#include "MotorSystem.hpp"

namespace nlm {

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Neural activity-based action selection implementation
    std::vector<ActionType>& availableActions = pImpl->availableActions;
    
    if (availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get strategy instance
    ActionSelectionStrategy* strategy = pImpl->getStrategy();
    if (!strategy) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate neural activity-based scores for each action
    std::vector<float> neuralScores;
    float avgMotorActivity = pImpl->getAverageMotorActivity(motorRegion);
    
    for (ActionType action : availableActions) {
        float actionActivity = pImpl->getActionSpecificActivity(motorRegion, action);
        float enhancedScore = pImpl->calculateEnhancedScore(actionActivity, action);
        neuralScores.push_back(enhancedScore);
    }
    
    // Ensure at least one action is selected even with low scores
    bool allLowScores = true;
    float minScoreThreshold = 0.01f;
    for (float score : neuralScores) {
        if (score > minScoreThreshold) {
            allLowScores = false;
            break;
        }
    }
    
    if (allLowScores && !neuralScores.empty()) {
        // Boost all scores to ensure action selection
        float minScore = *std::min_element(neuralScores.begin(), neuralScores.end());
        float boost = (minScoreThreshold - minScore) * 2.0f;
        for (float& score : neuralScores) {
            score += boost;
        }
    }
    
    // Use the strategy to select action
    std::unique_ptr<Action> selectedAction = strategy->selectAction(
        availableActions, neuralScores, pImpl->rng);
    
    // Record action in history
    if (selectedAction) {
        pImpl->actionHistory.push_back(selectedAction.get());
    }
    
    return selectedAction;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl && pImpl->actionPreferences.count(action)) {
        pImpl->actionPreferences[action] = std::max(0.0f, std::min(1.0f, preference));
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

// Additional methods for enhanced motor control
void MotorSystem::setActionSelectionAlgorithm(ActionSelectionAlgorithm algorithm) {
    if (pImpl) {
        pImpl->algorithm = algorithm;
    }
}

void MotorSystem::setExplorationNoise(float noise) {
    if (pImpl) {
        pImpl->explorationNoise = std::max(0.0f, std::min(1.0f, noise));
    }
}

void MotorSystem::setTemperature(float temp) {
    if (pImpl) {
        pImpl->temperature = std::max(0.1f, temp);
        // Update softmax strategy temperature
        if (pImpl->softmaxStrategy) {
            // Note: In a real implementation, we'd pass this to the strategy
            // For now, the temperature is set in the constructor
        }
    }
}

void MotorSystem::recordActionOutcome(ActionType action, bool success) {
    if (pImpl) {
        // Calculate current neural activity (placeholder - could be enhanced)
        float neuralActivity = 0.5f; // Placeholder
        
        pImpl->updateActionPreference(action, success, neuralActivity);
    }
}

float MotorSystem::getActionPreference(ActionType action) const {
    if (pImpl && pImpl->actionPreferences.count(action)) {
        return pImpl->actionPreferences[action];
    }
    return 0.0f;
}

int MotorSystem::getActionSuccessCount(ActionType action) const {
    if (pImpl && pImpl->actionSuccessCount.count(action)) {
        return pImpl->actionSuccessCount[action];
    }
    return 0;
}

int MotorSystem::getActionTotalCount(ActionType action) const {
    if (pImpl && pImpl->actionTotalCount.count(action)) {
        return pImpl->actionTotalCount[action];
    }
    return 0;
}

std::vector<float> MotorSystem::getActionNeuralCorrelations(ActionType action) const {
    if (pImpl && pImpl->actionNeuralCorrelations.count(action)) {
        return pImpl->actionNeuralCorrelations[action];
    }
    return std::vector<float>();
}

ActionSelectionAlgorithm MotorSystem::getActionSelectionAlgorithm() const {
    return pImpl ? pImpl->algorithm : ActionSelectionAlgorithm::Softmax;
}

float MotorSystem::getExplorationNoise() const {
    return pImpl ? pImpl->explorationNoise : 0.1f;
}

float MotorSystem::getTemperature() const {
    return pImpl ? pImpl->temperature : 1.0f;
}

// Update action selection based on current neural state
void MotorSystem::updateActionSelection(const NeuralRegion& motorRegion) {
    if (!pImpl) return;
    
    // Optionally update strategies based on current neural state
    // This could be used for dynamic strategy switching or parameter adjustment
    
    float currentActivity = pImpl->getAverageMotorActivity(motorRegion);
    
    // Adapt exploration based on activity level
    if (currentActivity < 0.1f) {
        // Low activity - increase exploration
        pImpl->explorationNoise = std::min(0.3f, pImpl->explorationNoise * 1.1f);
    } else if (currentActivity > 0.8f) {
        // High activity - decrease exploration
        pImpl->explorationNoise = std::max(0.01f, pImpl->explorationNoise * 0.9f);
    }
}

// Estimate reward prediction error for reinforcement learning
float MotorSystem::calculateRewardPredictionError(ActionType action, bool outcome) const {
    float predicted = pImpl ? pImpl->actionPreferences[action] : 0.0f;
    float actual = static_cast<float>(outcome);
    return actual - predicted;
}

} // namespace nlm