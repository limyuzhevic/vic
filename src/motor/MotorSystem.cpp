#include "MotorSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action selection parameters
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, std::vector<float>> actionValues;
    
    // Neural integration
    std::vector<float> motorNeuronActivity;
    float selectionTemperature;
    float explorationRate;
    
    // Learning and adaptation
    std::unordered_map<ActionType, float> learnedValues;
    std::unordered_map<ActionType, float> predictionErrors;
    float reinforcementAlpha;
    float temporalDifferenceGamma;
    
    // Planning integration
    float planningHorizon;
    std::vector<ActionType> preferredActionSequences;
    
    // Neuromodulation
    float motivationLevel;
    float arousalLevel;
    float attentionLevel;
    
    // State tracking
    size_t lastSelectedAction;
    float lastActionValue;
    float lastActionReward;
    
    Impl() : 
        selectionTemperature(1.0f), explorationRate(0.1f),
        reinforcementAlpha(0.1f), temporalDifferenceGamma(0.95f),
        planningHorizon(3), motivationLevel(1.0f), arousalLevel(1.0f),
        attentionLevel(1.0f), lastSelectedAction(0), lastActionValue(0.0f), lastActionReward(0.0f) {
        
        // Initialize action preferences
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
        
        // Initialize action values
        for (ActionType action : availableActions) {
            actionValues[action] = 0.0f;
            learnedValues[action] = 0.0f;
            actionPreferences[action] = 1.0f / availableActions.size(); // Equal initial preference
            predictionErrors[action] = 0.0f;
        }
        
        lastSelectedAction = static_cast<size_t>(ActionType::Wait);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity and learned preferences
    
    // Get current motor neuron activity
    pImpl->motorNeuronActivity = extractMotorNeuronActivity(motorRegion);
    
    // Update action values based on neural activity (action-value learning)
    updateActionValuesFromNeuralActivity(motorRegion);
    
    // Get planning influence if available
    float planningInfluence = 0.0f;
    
    // Integrate with neuromodulation
    updateNeuromodulationInfluence();
    
    // Calculate action probabilities using softmax
    std::vector<std::pair<ActionType, float>> actionProbabilities;
    
    float totalValue = calculateTotalActionValue();
    
    for (ActionType action : availableActions) {
        float actionValue = getActionValue(action);
        
        // Add exploration bonus
        float explorationBonus = calculateExplorationBonus(action);
        
        // Add planning bonus
        float planningBonus = calculatePlanningBonus(action);
        
        // Add motivation bonus
        float motivationBonus = motivationLevel * 0.1f;
        
        float finalValue = actionValue + explorationBonus + planningBonus + motivationBonus;
        
        // Apply temperature to action selection
        float probability = std::exp(finalValue / pImpl->selectionTemperature) / totalValue;
        
        actionProbabilities.push_back(std::make_pair(action, probability));
    }
    
    // Normalize probabilities
    float probSum = 0.0f;
    for (const auto& pair : actionProbabilities) {
        probSum += pair.second;
    }
    
    if (probSum > 0.0f) {
        for (auto& pair : actionProbabilities) {
            pair.second /= probSum;
        }
    }
    
    // Select action based on probabilities
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    float randVal = dis(gen);
    float cumulativeProb = 0.0f;
    
    ActionType selectedAction = ActionType::Wait;
    
    for (const auto& pair : actionProbabilities) {
        cumulativeProb += pair.second;
        if (randVal <= cumulativeProb) {
            selectedAction = pair.first;
            break;
        }
    }
    
    // Create action
    auto action = std::make_unique<Action>(selectedAction);
    
    // Store selection history
    pImpl->lastSelectedAction = static_cast<size_t>(selectedAction);
    pImpl->lastActionValue = getActionValue(selectedAction);
    
    // Add to history
    actionHistory.push_back(action.get());
    
    // Keep history bounded
    if (actionHistory.size() > 100) {
        delete actionHistory.front();
        actionHistory.erase(actionHistory.begin());
    }
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set preference for a specific action
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return actionHistory;
}

void MotorSystem::clearHistory() {
    // Clear action history
    for (auto* action : actionHistory) {
        delete action;
    }
    actionHistory.clear();
}

void MotorSystem::Impl::updateActionValuesFromNeuralActivity(const NeuralRegion& motorRegion) {
    // Update action values based on neural activity patterns
    
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) return;
    
    // Calculate population activity
    std::vector<float> populationActivity(neurons.size(), 0.0f);
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        const auto& state = neurons[i]->getState();
        float normalizedActivity = (state.membranePotential - state.restingPotential) /
                                  std::max(state.threshold - state.restingPotential, 1.0f);
        populationActivity[i] = std::clamp(normalizedActivity, 0.0f, 1.0f);
    }
    
    // Map neural activity to actions based on population coding
    for (ActionType action : availableActions) {
        float actionValue = calculateActionValueFromPopulation(action, neurons, populationActivity);
        
        // Apply temporal difference learning
        float predictionError = calculatePredictionError(action);
        
        // Update learned value
        learnedValues[action] += reinforcementAlpha * predictionError;
        
        // Update action-specific value
        actionValues[action] = learnedValues[action] * (1.0f - reinforcementAlpha) +
                             actionValue * reinforcementAlpha;
    }
}

float MotorSystem::Impl::calculateActionValueFromPopulation(ActionType action, 
                                                          const std::vector<Neuron*>& neurons,
                                                          const std::vector<float>& populationActivity) const {
    // Calculate action value based on population neural activity
    // This implements population vector coding
    
    // Get preference for this action
    float preference = actionPreferences.count(action) ? actionPreferences.at(action) : 1.0f / availableActions.size();
    
    // Find neurons most associated with this action
    std::vector<size_t> actionNeurons = getActionAssociatedNeurons(action, neurons);
    
    if (actionNeurons.empty()) {
        return preference * learnedValues.count(action) ? learnedValues.at(action) : 0.0f;
    }
    
    // Calculate population activity for action-associated neurons
    float totalActivity = 0.0f;
    for (size_t neuronIdx : actionNeurons) {
        if (neuronIdx < populationActivity.size()) {
            totalActivity += populationActivity[neuronIdx];
        }
    }
    
    float avgActivity = totalActivity / actionNeurons.size();
    
    // Combine preference and activity
    float actionValue = preference * avgActivity * motivationLevel;
    
    // Apply arousal modulation
    actionValue *= arousalLevel;
    
    // Apply attention modulation
    actionValue *= attentionLevel;
    
    return actionValue;
}

std::vector<size_t> MotorSystem::Impl::getActionAssociatedNeurons(ActionType action, const std::vector<Neuron*>& neurons) const {
    // Get neurons associated with a specific action
    // In a real implementation, this would use learned associations
    // For now, use a heuristic based on neuron type and ID
    
    std::vector<size_t> associatedNeurons;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        const auto& state = neurons[i]->getState();
        NeuronType neuronType = neurons[i]->getNeuronType(); // Assuming this method exists
        
        // Heuristic: motor neurons of specific types are associated with specific actions
        bool isAssociated = false;
        
        switch (action) {
            case ActionType::MoveForward:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::MoveBackward:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::TurnLeft:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::TurnRight:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::Look:
                isAssociated = (neuronType == NeuronType::Sensory);
                break;
            case ActionType::Interact:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::Wait:
                isAssociated = (neuronType == NeuronType::Internal);
                break;
            case ActionType::MoveLeft:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
            case ActionType::MoveRight:
                isAssociated = (neuronType == NeuronType::Motor);
                break;
        }
        
        if (isAssociated) {
            associatedNeurons.push_back(i);
        }
    }
    
    return associatedNeurons;
}

float MotorSystem::Impl::calculatePredictionError(ActionType action) const {
    // Calculate prediction error for action-value learning
    
    // Simplified TD learning: prediction - actual outcome
    float prediction = learnedValues.count(action) ? learnedValues.at(action) : 0.0f;
    float outcome = lastActionValue; // Simplified: use last action value
    
    return outcome - prediction;
}

float MotorSystem::Impl::calculateTotalActionValue() const {
    // Calculate sum of exponentiated action values for softmax
    
    float total = 0.0f;
    for (ActionType action : availableActions) {
        float actionValue = actionValues.count(action) ? actionValues.at(action) : 0.0f;
        total += std::exp(actionValue / selectionTemperature);
    }
    
    return total;
}

float MotorSystem::Impl::getActionValue(ActionType action) const {
    return actionValues.count(action) ? actionValues.at(action) : 0.0f;
}

float MotorSystem::Impl::calculateExplorationBonus(ActionType action) const {
    // Calculate exploration bonus based on action frequency
    
    // Simple epsilon-greedy exploration
    float explorationBonus = 0.0f;
    
    // Count how often this action was selected recently
    size_t selectionCount = 0;
    for (size_t i = 0; i < actionHistory.size(); ++i) {
        if (actionHistory[i] && actionHistory[i]->getType() == action) {
            selectionCount++;
        }
    }
    
    // If action was rarely selected, give exploration bonus
    if (selectionCount < 5) {
        explorationBonus = explorationRate * (5 - selectionCount) * 0.1f;
    }
    
    return explorationBonus;
}

float MotorSystem::Impl::calculatePlanningBonus(ActionType action) const {
    // Calculate bonus based on planning system preferences
    
    // In a real implementation, this would integrate with NeuralPlanner
    // For now, provide a simple heuristic
    
    float planningBonus = 0.0f;
    
    // Check if action is in preferred sequences
    for (ActionType plannedAction : preferredActionSequences) {
        if (plannedAction == action) {
            planningBonus += 0.2f;
            break;
        }
    }
    
    return planningBonus;
}

void MotorSystem::Impl::updateNeuromodulationInfluence() {
    // Update influence of neuromodulation systems
    
    // Dopamine influence (simplified)
    motivationLevel = std::clamp(motivationLevel * 0.95f + 0.05f, 0.1f, 2.0f);
    
    // Arousal (simplified: increases with activity)
    arousalLevel = std::clamp(arousalLevel * 0.9f + 0.1f, 0.5f, 2.0f);
    
    // Attention (simplified: decreases with fatigue)
    attentionLevel = std::clamp(attentionLevel * 0.98f, 0.3f, 1.5f);
}

} // namespace nlm
