#include "MotorSystem.hpp"
#include "Action.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action selection parameters
    float explorationRate;
    float softmaxTemperature;
    float learningRate;
    float rewardPredictionWeight;
    
    // Neural integration
    std::map<ActionType, float> actionValues;  // Q-values
    std::map<ActionType, float> actionPreferences;  // Preference strengths
    std::map<ActionType, float> actionSelectionProbabilities;
    
    // Neuromodulation influence
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    // History and statistics
    std::map<ActionType, std::vector<float>> actionRewards;
    std::map<ActionType, size_t> actionCounts;
    
    // Random generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Prediction integration
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    Impl() : 
        explorationRate(0.1f),
        softmaxTemperature(1.0f),
        learningRate(0.01f),
        rewardPredictionWeight(0.5f),
        dopamineLevel(0.5f),
        curiosityLevel(0.3f),
        noveltyLevel(0.2f) {
        
        // Initialize random generator
        std::random_device rd;
        rng.seed(static_cast<unsigned int>(std::time(nullptr)));
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        
        // Initialize available actions
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::Interact,
            ActionType::Wait
        };
        
        // Initialize Q-values
        for (ActionType action : availableActions) {
            actionValues[action] = 0.0f;
            actionPreferences[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity
    // Implements multiple selection strategies with neural integration
    
    // Get neural activity from motor region
    std::vector<float> neuralActivations;
    for (const Neuron* neuron : motorRegion.getNeurons()) {
        neuralActivations.push_back(neuron->getFiringRate());
    }
    
    if (neuralActivations.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate action selection scores using multiple factors
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType action : pImpl->availableActions) {
        float score = 0.0f;
        
        // 1. Neural preference from motor region
        float neuralPreference = calculateNeuralPreference(action, neuralActivations);
        score += neuralPreference * 0.4f;
        
        // 2. Q-value from learning
        float qValue = pImpl->actionValues[action];
        score += qValue * 0.3f;
        
        // 3. Preference strength
        float preference = pImpl->actionPreferences[action];
        score += preference * 0.1f;
        
        // 4. Neuromodulation influence
        float neuromodulation = calculateNeuromodulationInfluence(action);
        score += neuromodulation * 0.15f;
        
        // 5. Action suitability based on current state
        float suitability = calculateActionSuitability(action, neuralActivations);
        score += suitability * 0.02f;
        
        // 6. Prediction confidence (if prediction system available)
        if (pImpl->predictionSystem) {
            float predictionConfidence = pImpl->predictionSystem->getConfidence();
            score += predictionConfidence * 0.01f;
        }
        
        // Apply exploration bonus
        if (uniformDist(pImpl->rng) < pImpl->explorationRate) {
            score += uniformDist(pImpl->rng) * 0.1f;  // Random exploration bonus
        }
        
        actionScores.push_back({action, score});
    }
    
    // Apply action selection strategy
    ActionType selectedAction = applyActionSelectionStrategy(actionScores);
    
    // Create action with neural context
    auto action = std::make_unique<Action>(selectedAction);
    
    // Store neural context in action (if possible)
    // In real implementation, action would contain neural activation context
    
    // Add to history
    pImpl->actionHistory.push_back(action.get());
    pImpl->actionCounts[selectedAction]++;
    
    NLM_LOG_INFO("Selected action: " + std::to_string(static_cast<int>(selectedAction)) + 
                " with neural preference: " + std::to_string(calculateNeuralPreference(selectedAction, neuralActivations)));
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl->actionPreferences.find(action) != pImpl->actionPreferences.end()) {
        pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 10.0f);
        NLM_LOG_INFO("Set preference for action " + std::to_string(static_cast<int>(action)) + 
                    " to " + std::to_string(preference));
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

float MotorSystem::Impl::calculateNeuralPreference(ActionType action, const std::vector<float>& neuralActivations) const {
    // Calculate preference based on neural activity in motor regions
    // This implements the relationship between neural firing patterns and action tendencies
    
    float preference = 0.0f;
    
    // For each neuron, check if it has a preference for this action
    // In real implementation, neurons would have action-specific firing patterns
    for (size_t i = 0; i < neuralActivations.size(); ++i) {
        float activation = neuralActivations[i];
        
        // Simulate neural tuning (in real system, neurons would have specific tuning curves)
        float tuningStrength = 0.3f;  // Placeholder for neuron-specific tuning
        float actionMatch = calculateActionMatch(action, i);  // Based on neuron index
        
        preference += activation * tuningStrength * actionMatch;
    }
    
    // Normalize and apply non-linear response
    if (!neuralActivations.empty()) {
        preference /= neuralActivations.size();
    }
    
    // Apply sigmoid function for bounded response
    preference = 1.0f / (1.0f + std::exp(-2.0f * (preference - 0.5f)));
    
    return preference;
}

float MotorSystem::Impl::calculateNeuromodulationInfluence(ActionType action) const {
    // Calculate influence of neuromodulators on action selection
    // Dopamine affects reward prediction, curiosity drives exploration, novelty signals surprise
    
    float influence = 0.0f;
    
    // Dopamine influence (reward prediction)
    influence += dopamineLevel * (1.0f - std::abs(dopamineLevel - 0.5f));  // Inverted U-shaped
    
    // Curiosity influence (exploration drive)
    influence += curiosityLevel * 0.5f;
    
    // Novelty influence (unexpected actions)
    influence += noveltyLevel * (1.0f - noveltyLevel);
    
    // Action-specific modulation based on expected reward
    float expectedReward = 0.0f;
    if (actionCounts.find(action) != actionCounts.end() && actionCounts.at(action) > 0) {
        float totalReward = std::accumulate(actionRewards.at(action).begin(), 
                                          actionRewards.at(action).end(), 0.0f);
        expectedReward = totalReward / actionCounts.at(action);
    }
    
    // Dopamine should drive actions with moderate expected reward (exploration-exploitation balance)
    influence += dopamineLevel * expectedReward * 0.5f;
    
    return std::clamp(influence, 0.0f, 1.0f);
}

float MotorSystem::Impl::calculateActionSuitability(ActionType action, const std::vector<float>& neuralActivations) const {
    // Calculate how suitable an action is for the current neural state
    // Implements forward models and motor planning
    
    float suitability = 0.0f;
    
    // Analyze neural population activity patterns
    float averageActivation = std::accumulate(neuralActivations.begin(), neuralActivations.end(), 0.0f) / 
                              neuralActivations.size();
    
    // High average activation suggests urgency or motivation
    suitability += averageActivation * 0.3f;
    
    // Neural variance indicates uncertainty
    float variance = 0.0f;
    for (float act : neuralActivations) {
        variance += (act - averageActivation) * (act - averageActivation);
    }
    variance /= neuralActivations.size();
    
    // High variance may require different action strategies
    suitability += (1.0f - variance) * 0.2f;
    
    // Action-specific motor costs (simplified)
    float motorCost = calculateMotorCost(action);
    suitability += (1.0f - motorCost) * 0.3f;
    
    // Neural state matching (forward model)
    float neuralMatch = calculateNeuralActionMatch(action, neuralActivations);
    suitability += neuralMatch * 0.2f;
    
    return std::clamp(suitability, 0.0f, 1.0f);
}

float MotorSystem::Impl::calculateMotorCost(ActionType action) const {
    // Simplified motor cost model
    // Different actions have different energetic costs
    
    switch (action) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
            return 0.2f;  // Low cost
        case ActionType::MoveLeft:
        case ActionType::MoveRight:
            return 0.3f;  // Moderate cost
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            return 0.4f;  // Higher cost
        case ActionType::Look:
            return 0.1f;  // Low cost
        case ActionType::Interact:
            return 0.5f;  // High cost
        case ActionType::Wait:
            return 0.0f;  // Zero cost
        default:
            return 0.5f;  // Unknown action cost
    }
}

float MotorSystem::Impl::calculateActionMatch(ActionType action, size_t neuronIndex) const {
    // Simplified action-neuron matching based on neuron index
    // In real system, this would be based on specific neural tuning curves
    
    // Map action types to neuron groups (simplified)
    std::vector<ActionType> actionGroup = {
        ActionType::MoveForward, ActionType::MoveBackward
    };
    
    if (std::find(actionGroup.begin(), actionGroup.end(), action) != actionGroup.end()) {
        // Motor actions (neurons 0-9)
        return (neuronIndex < 10) ? 1.0f : 0.0f;
    } else {
        // Other actions (neurons 10-19)
        return (neuronIndex >= 10 && neuronIndex < 20) ? 1.0f : 0.0f;
    }
}

float MotorSystem::Impl::calculateNeuralActionMatch(ActionType action, const std::vector<float>& neuralActivations) const {
    // Calculate how well the current neural state matches action requirements
    // Implements forward models and motor planning
    
    float match = 0.0f;
    
    // Analyze neural population activity patterns
    // This is a simplified forward model
    for (size_t i = 0; i < neuralActivations.size() && i < 20; ++i) {  // Limit to first 20 neurons
        float activation = neuralActivations[i];
        
        // Determine if this neuron supports the action
        float actionSupport = calculateActionMatch(action, i);
        
        // Combine activation and support
        match += activation * actionSupport;
    }
    
    // Normalize by number of neurons
    if (!neuralActivations.empty()) {
        match /= neuralActivations.size();
    }
    
    return match;
}

ActionType MotorSystem::applyActionSelectionStrategy(const std::vector<std::pair<ActionType, float>>& actionScores) {
    // Apply action selection strategy based on configurable parameters
    // Combines softmax for exploration with threshold-based selection
    
    if (actionScores.empty()) {
        return ActionType::Wait;
    }
    
    // Find maximum score
    float maxScore = -1.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& pair : actionScores) {
        if (pair.second > maxScore) {
            maxScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Apply softmax to get probabilities
    std::vector<float> probabilities;
    float sumExp = 0.0f;
    
    for (const auto& pair : actionScores) {
        float expScore = std::exp(pair.second / pImpl->softmaxTemperature);
        probabilities.push_back(expScore);
        sumExp += expScore;
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sumExp;
    }
    
    // Sample from probability distribution
    float randVal = uniformDist(pImpl->rng);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < actionScores.size(); ++i) {
        cumulative += probabilities[i];n        if (randVal <= cumulative) {
            return actionScores[i].first;
        }
    }
    
    // Fallback to best action
    return bestAction;
}

} // namespace nlm
