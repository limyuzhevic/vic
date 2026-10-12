#include "MotorSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Neural activity integration
    std::unordered_map<ActionType, float> actionNeuralWeights;
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Motor state
    float arousalLevel;
    float neuromodulation;
    float fatigueFactor;
    
    // Learning and adaptation
    float learningRate;
    float explorationStrength;
    float costSensitivity;
    
    // Statistics
    size_t totalActionsExecuted;
    size_t successfulActions;
    size_t sequenceCount;
    
    // Advanced selection
    float temperature;  // For softmax selection
    float entropy;     // For entropy-based exploration
    
    Impl() {
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
        
        // Initialize neural weights
        for (auto action : availableActions) {
            actionNeuralWeights[action] = 1.0f;
            actionPreferences[action] = 1.0f;
        }
        
        arousalLevel = 0.5f;
        neuromodulation = 0.0f;
        fatigueFactor = 0.0f;
        learningRate = 0.01f;
        explorationStrength = 0.1f;
        costSensitivity = 1.0f;
        
        totalActionsExecuted = 0;
        successfulActions = 0;
        sequenceCount = 0;
        
        temperature = 1.0f;
        entropy = 0.0f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Get all neurons in the motor region
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    return selectActionWithNeuralDynamics(motorRegion, neurons,
                                        pImpl->arousalLevel, pImpl->neuromodulation);
}

std::unique_ptr<Action> MotorSystem::selectActionWithNeuralDynamics(
    const NeuralRegion& motorRegion,
    const std::vector<Neuron*>& neurons,
    float arousalLevel,
    float neuromodulation) {
    
    // Calculate neural activity for each potential action
    std::vector<float> neuralActivations(availableActions.size(), 0.0f);
    std::vector<ActionType> candidateActions;
    
    for (size_t i = 0; i < availableActions.size(); ++i) {
        ActionType action = availableActions[i];
        
        // Calculate neural activation based on motor neuron group activity
        float activation = 0.0f;
        for (Neuron* neuron : neurons) {
            // Only consider neurons matching this action type
            // This would be refined based on motor neuron organization
            activation += std::abs(neuron->getState().membranePotential - 
                                  neuron->getState().restingPotential) * 
                         pImpl->actionNeuralWeights[action];
        }
        
        // Average over number of neurons (simplified)
        if (!neurons.empty()) {
            activation /= neurons.size();
        }
        
        // Apply arousal modulation
        activation *= (1.0f + arousalLevel * 0.5f);
        
        // Apply neuromodulation
        activation *= (1.0f + neuromodulation * 0.3f);
        
        // Apply preference weights
        activation *= pImpl->actionPreferences[action];
        
        if (activation > 0.01f) {  // Threshold for viable actions
            neuralActivations[i] = activation;
            candidateActions.push_back(action);
        }
    }
    
    // If no actions above threshold, default to wait
    if (candidateActions.empty()) {
        auto action = std::make_unique<Action>(ActionType::Wait);
        action->getDynamics().neuralDrive = 0.0f;
        action->incrementExecutionCount();
        pImpl->totalActionsExecuted++;
        pImpl->actionHistory.push_back(action.get());
        return action;
    }
    
    // Apply action selection algorithm
    std::unique_ptr<Action> selectedAction;
    
    // Choose selection strategy based on arousal and exploration
    float explorationChance = explorationStrength * arousalLevel;
    float r = std::rand() / (RAND_MAX + 1.0f);
    
    if (r < explorationChance) {
        // Epsilon-greedy exploration
        selectedAction = epsilonGreedySelection(
            std::vector<const Action*>(candidateActions.size(), nullptr),
            neuralActivations
        );
    } else {
        // Softmax selection with temperature
        selectedAction = softmaxActionSelection(
            std::vector<const Action*>(candidateActions.size(), nullptr),
            neuralActivations,
            std::vector<float>(candidateActions.size(), 1.0f)
        );
    }
    
    // Create action with proper motor dynamics
    ActionType selectedType = selectedAction->getType();
    MotorDynamics dynamics = computeMotorDynamics(selectedType, motorRegion, neurons);
    
    auto action = std::make_unique<Action>(selectedType, dynamics);
    action->updateNeuralDrive(0.5f, arousalLevel, neuromodulation);
    
    // Update statistics
    action->incrementExecutionCount();
    pImpl->totalActionsExecuted++;
    pImpl->actionHistory.push_back(action.get());
    
    // Update neural weights for learning
    pImpl->actionNeuralWeights[selectedType] *= (1.0f + learningRate * 0.1f);
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 2.0f);
}

std::unique_ptr<Action> MotorSystem::softmaxActionSelection(
    const std::vector<const Action*>& candidates,
    const std::vector<float>& neuralActivations,
    const std::vector<float>& preferences) {
    
    if (candidates.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Convert to log probabilities for numerical stability
    std::vector<float> logits;
    for (size_t i = 0; i < candidates.size(); ++i) {
        float logit = neuralActivations[i] * preferences[i] / pImpl->temperature;
        logits.push_back(logit);
    }
    
    // Apply softmax
    float maxLogit = *std::max_element(logits.begin(), logits.end());
    std::vector<float> probs;
    float sumExp = 0.0f;
    
    for (float logit : logits) {
        float expLogit = std::exp((logit - maxLogit) / pImpl->temperature);
        probs.push_back(expLogit);
        sumExp += expLogit;
    }
    
    // Normalize
    for (float& prob : probs) {
        prob /= sumExp;
    }
    
    // Calculate entropy
    entropy = 0.0f;
    for (float prob : probs) {
        if (prob > 0.0f) {
            entropy -= prob * std::log(prob);
        }
    }
    
    // Sample from probability distribution
    float r = static_cast<float>(std::rand()) / RAND_MAX;
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (r <= cumulative) {
            // Return a placeholder action - actual action will be created with proper dynamics
            return std::make_unique<Action>(candidates[i]->getType());
        }
    }
    
    // Fallback
    return std::make_unique<Action>(candidates.back()->getType());
}

std::unique_ptr<Action> MotorSystem::epsilonGreedySelection(
    const std::vector<const Action*>& candidates,
    const std::vector<float>& neuralActivations) {
    
    if (candidates.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find best action based on neural activation
    float bestActivation = -1.0f;
    size_t bestIndex = 0;
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (neuralActivations[i] > bestActivation) {
            bestActivation = neuralActivations[i];
            bestIndex = i;
        }
    }
    
    float r = static_cast<float>(std::rand()) / RAND_MAX;
    
    if (r < explorationStrength) {
        // Explore: random selection
        size_t randomIndex = static_cast<size_t>(r * candidates.size());
        return std::make_unique<Action>(candidates[randomIndex]->getType());
    } else {
        // Exploit: select best action
        return std::make_unique<Action>(candidates[bestIndex]->getType());
    }
}

std::unique_ptr<Action> MotorSystem::maxEntropySelection(
    const std::vector<const Action*>& candidates,
    const std::vector<float>& neuralActivations) {
    
    if (candidates.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate entropy-weighted probabilities
    std::vector<float> probs;
    float totalEntropy = 0.0f;
    
    for (float activation : neuralActivations) {
        // Higher activation with higher entropy (more exploration)
        float prob = activation * (1.0f + entropy * 0.5f);
        probs.push_back(prob);
        totalEntropy += entropy;
    }
    
    // Normalize
    float sumProbs = std::accumulate(probs.begin(), probs.end(), 0.0f);
    if (sumProbs > 0.0f) {
        for (float& prob : probs) {
            prob /= sumProbs;
        }
    }
    
    // Sample
    float r = static_cast<float>(std::rand()) / RAND_MAX;
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (r <= cumulative) {
            return std::make_unique<Action>(candidates[i]->getType());
        }
    }
    
    return std::make_unique<Action>(candidates.back()->getType());
}

MotorDynamics MotorSystem::computeMotorDynamics(const ActionType& actionType,
                                              const NeuralRegion& motorRegion,
                                              const std::vector<Neuron*>& neurons) {
    MotorDynamics dynamics;
    
    // Set action type
    dynamics.motorCommand = motorCommandFromActionType(actionType);
    
    // Calculate neural drive based on motor neuron activity
    float totalNeuralInput = 0.0f;
    for (Neuron* neuron : neurons) {
        totalNeuralInput += std::abs(neuron->getState().membranePotential - 
                                    neuron->getState().restingPotential);
    }
    
    if (!neurons.empty()) {
        dynamics.neuralDrive = totalNeuralInput / neurons.size();
    }
    
    // Set execution parameters based on action type
    switch (actionType) {
        case ActionType::MoveForward:
            dynamics.executionTime = 100.0f + dynamics.neuralDrive * 50.0f;
            dynamics.energyCost = 0.5f + dynamics.neuralDrive * 0.3f;
            dynamics.accuracy = 0.8f + dynamics.neuralDrive * 0.2f;
            dynamics.stability = 0.9f - dynamics.neuralDrive * 0.1f;
            dynamics.force = 2.0f + dynamics.neuralDrive * 5.0f;
            dynamics.velocity = 1.0f + dynamics.neuralDrive * 0.5f;
            break;
            
        case ActionType::MoveBackward:
            dynamics.executionTime = 120.0f + dynamics.neuralDrive * 60.0f;
            dynamics.energyCost = 0.6f + dynamics.neuralDrive * 0.4f;
            dynamics.accuracy = 0.7f + dynamics.neuralDrive * 0.15f;
            dynamics.stability = 0.8f - dynamics.neuralDrive * 0.15f;
            dynamics.force = 1.5f + dynamics.neuralDrive * 4.0f;
            dynamics.velocity = -0.8f + dynamics.neuralDrive * 0.4f;
            break;
            
        case ActionType::TurnLeft:
            dynamics.executionTime = 80.0f + dynamics.neuralDrive * 40.0f;
            dynamics.energyCost = 0.4f + dynamics.neuralDrive * 0.25f;
            dynamics.accuracy = 0.75f + dynamics.neuralDrive * 0.15f;
            dynamics.stability = 0.85f - dynamics.neuralDrive * 0.1f;
            dynamics.torque = 3.0f + dynamics.neuralDrive * 6.0f;
            dynamics.velocity = 0.0f;
            break;
            
        case ActionType::TurnRight:
            dynamics.executionTime = 80.0f + dynamics.neuralDrive * 40.0f;
            dynamics.energyCost = 0.4f + dynamics.neuralDrive * 0.25f;
            dynamics.accuracy = 0.75f + dynamics.neuralDrive * 0.15f;
            dynamics.stability = 0.85f - dynamics.neuralDrive * 0.1f;
            dynamics.torque = 3.0f + dynamics.neuralDrive * 6.0f;
            dynamics.velocity = 0.0f;
            break;
            
        case ActionType::Look:
            dynamics.executionTime = 60.0f + dynamics.neuralDrive * 30.0f;
            dynamics.energyCost = 0.3f + dynamics.neuralDrive * 0.2f;
            dynamics.accuracy = 0.9f + dynamics.neuralDrive * 0.1f;
            dynamics.stability = 0.95f - dynamics.neuralDrive * 0.05f;
            dynamics.velocity = 0.5f + dynamics.neuralDrive * 0.25f;
            break;
            
        case ActionType::Interact:
            dynamics.executionTime = 150.0f + dynamics.neuralDrive * 75.0f;
            dynamics.energyCost = 0.8f + dynamics.neuralDrive * 0.5f;
            dynamics.accuracy = 0.7f + dynamics.neuralDrive * 0.2f;
            dynamics.stability = 0.8f - dynamics.neuralDrive * 0.15f;
            dynamics.force = 3.0f + dynamics.neuralDrive * 7.0f;
            dynamics.velocity = 0.3f + dynamics.neuralDrive * 0.15f;
            break;
            
        default: // Wait, Rest, etc.
            dynamics.executionTime = 10.0f;
            dynamics.energyCost = 0.1f;
            dynamics.accuracy = 1.0f;
            dynamics.stability = 1.0f;
            dynamics.force = 0.0f;
            dynamics.velocity = 0.0f;
            break;
    }
    
    // Apply fatigue effects
    dynamics.energyCost *= (1.0f + fatigueFactor_ * 0.3f);
    dynamics.accuracy *= (1.0f - fatigueFactor_ * 0.2f);
    dynamics.stability *= (1.0f - fatigueFactor_ * 0.15f);
    
    // Apply adaptation effects
    dynamics.force *= (1.0f + dynamics.adaptationState * 0.2f);
    dynamics.accuracy *= (1.0f + dynamics.adaptationState * 0.3f);
    
    return dynamics;
}

MotorCommand MotorSystem::motorCommandFromActionType(ActionType actionType) {
    switch (actionType) {
        case ActionType::MoveForward:
            return MotorCommand::MoveForward;
        case ActionType::MoveBackward:
            return MotorCommand::MoveBackward;
        case ActionType::MoveLeft:
            return MotorCommand::TurnLeft;
        case ActionType::MoveRight:
            return MotorCommand::TurnRight;
        case ActionType::TurnLeft:
            return MotorCommand::TurnLeft;
        case ActionType::TurnRight:
            return MotorCommand::TurnRight;
        case ActionType::Look:
            return MotorCommand::LookLeft; // Simplified
        case ActionType::LookUp:
            return MotorCommand::LookRight; // Simplified
        case ActionType::LookDown:
            return MotorCommand::Interact; // Simplified
        case ActionType::Interact:
            return MotorCommand::Interact;
        case ActionType::Wait:
            return MotorCommand::Wait;
        default:
            return MotorCommand::Wait;
    }
}

void MotorSystem::updateMotorAdaptation(const NeuralRegion& motorRegion, float adaptationDelta) {
    // Update motor neuron synaptic weights based on adaptation
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    for (Neuron* neuron : neurons) {
        // Modify neuron excitability based on adaptation
        float currentPotential = neuron->getState().membranePotential;
        float restingPotential = neuron->getState().restingPotential;
        
        // Adapt threshold based on experience
        float newThreshold = neuron->getState().threshold + adaptationDelta * 0.01f;
        neuron->setThreshold(newThreshold);
        
        // Modulate excitability
        if (adaptationDelta > 0.0f) {
            // Positive adaptation increases excitability
            neuron->addToMembranePotential(adaptationDelta * 0.5f);
        }
    }
}

void MotorSystem::adjustMotorPatterns(const Action& successfulAction, const Action& alternativeAction) {
    // Learn from comparing successful vs alternative actions
    float successValue = successfulAction.computeNetValue();
    float alternativeValue = alternativeAction.computeNetValue();
    
    if (alternativeValue > successValue) {
        // Update preferences based on better alternative
        ActionType successfulType = successfulAction.getType();
        ActionType alternativeType = alternativeAction.getType();
        
        pImpl->actionPreferences[alternativeType] *= (1.0f + learningRate_);
        pImpl->actionNeuralWeights[alternativeType] *= (1.0f + learningRate_);
    }
}

float MotorSystem::computeActionCost(const Action& action) const {
    return action.computeCost();
}

float MotorSystem::computeActionBenefit(const Action& action, const NeuralRegion& motorRegion) const {
    return action.computeBenefit();
}

float MotorSystem::computeActionNetValue(const Action& action, const NeuralRegion& motorRegion) const {
    return action.computeNetValue();
}

std::unique_ptr<Action> MotorSystem::planActionSequence(const std::vector<ActionType>& sequenceTypes) {
    // Create a sequence of actions
    std::vector<std::unique_ptr<Action>> sequence;
    
    for (ActionType actionType : sequenceTypes) {
        auto action = std::make_unique<Action>(actionType);
        
        // Set sequence context
        size_t position = sequence.size();
        action->setSequenceContext(position, sequenceTypes.size());
        
        // Apply sequence-specific modifications
        if (position == 0) {
            // First action in sequence gets planning boost
            action->getDynamics().expectedValue *= 1.2f;
        }
        
        if (position == sequenceTypes.size() - 1) {
            // Last action gets completion focus
            action->getDynamics().accuracy *= 1.1f;
        }
        
        sequence.push_back(std::move(action));
    }
    
    // Return the first action as current action
    if (!sequence.empty()) {
        return std::move(sequence[0]);
    }
    
    return std::make_unique<Action>(ActionType::Wait);
}

std::unique_ptr<Action> MotorSystem::adaptActionForContext(const Action& baseAction, 
                                                         const NeuralRegion& contextRegion) {
    // Adapt action based on current neural context
    auto adaptedAction = baseAction.clone();
    
    // Get context neural activity
    std::vector<Neuron*> neurons = contextRegion.getAllNeurons();
    float contextActivation = 0.0f;
    
    for (Neuron* neuron : neurons) {
        contextActivation += std::abs(neuron->getState().membranePotential - 
                                     neuron->getState().restingPotential);
    }
    
    if (!neurons.empty()) {
        contextActivation /= neurons.size();
    }
    
    // Adapt action parameters based on context
    adaptedAction->getDynamics().neuralDrive *= (1.0f + contextActivation * 0.3f);
    
    // Modify execution time based on context
    float adaptationFactor = 1.0f + (contextActivation - 0.5f) * 0.5f;
    adaptedAction->getDynamics().executionTime *= adaptationFactor;
    
    return adaptedAction;
}

void MotorSystem::updateNeuromodulation(float dopamine, float novelty) {
    pImpl->neuromodulation = dopamine * 0.7f + novelty * 0.3f;
    pImpl->neuromodulation = std::clamp(pImpl->neuromodulation, -1.0f, 1.0f);
}

void MotorSystem::updateArousal(float baseline, float sensoryInput) {
    pImpl->arousalLevel = baseline + sensoryInput * 0.1f;
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel, 0.0f, 1.0f);
}

void MotorSystem::updateMotorFatigue(float fatigueRate, double dt) {
    pImpl->fatigueFactor += fatigueRate * dt;
    pImpl->fatigueFactor = std::clamp(pImpl->fatigueFactor, 0.0f, 1.0f);
    
    // Fatigue recovery over time
    pImpl->fatigueFactor *= (1.0f - 0.01f * dt);
}

void MotorSystem::learnFromActionOutcome(const Action& action, float reward, bool success) {
    pImpl->totalActionsExecuted++;
    if (success) {
        pImpl->successfulActions++;
    }
    
    // Update learning based on outcome
    float predictionError = reward - action.getDynamics().expectedValue;
    
    // Update action preferences based on outcome
    ActionType actionType = action.getType();
    if (success) {
        pImpl->actionPreferences[actionType] *= (1.0f + learningRate_);
    } else {
        pImpl->actionPreferences[actionType] *= (1.0f - learningRate_);
    }
    
    // Update neural weights
    pImpl->actionNeuralWeights[actionType] *= (1.0f + learningRate_ * std::abs(predictionError));
    
    // Update action's internal learning
    action.learnFromOutcome(reward, predictionError);
}

void MotorSystem::updateActionPreferences(const std::vector<const Action*>& actions, 
                                         const std::vector<float>& outcomes) {
    if (actions.size() != outcomes.size()) return;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType actionType = actions[i]->getType();
        float outcome = outcomes[i];
        
        if (outcome > 0.0f) {
            pImpl->actionPreferences[actionType] *= (1.0f + learningRate_);
        } else {
            pImpl->actionPreferences[actionType] *= (1.0f - learningRate_);
        }
    }
}

void MotorSystem::adjustExplorationBasedOnUncertainty(const std::vector<const Action*>& actions) {
    // Calculate uncertainty (entropy) across action values
    std::vector<float> actionValues;
    for (const Action* action : actions) {
        actionValues.push_back(action->computeNetValue());
    }
    
    float meanValue = std::accumulate(actionValues.begin(), actionValues.end(), 0.0f) / actionValues.size();
    float variance = 0.0f;
    for (float value : actionValues) {
        variance += (value - meanValue) * (value - meanValue);
    }
    variance /= actionValues.size();
    
    float uncertainty = std::sqrt(variance);
    
    // Adjust exploration based on uncertainty
    explorationStrength_ = explorationStrength * (1.0f + uncertainty * 2.0f);
}

void MotorSystem::updateSequenceProgress(size_t sequenceLength, size_t actionPosition) {
    pImpl->sequenceCount = sequenceLength;
    // Store sequence progress for next action selection
}

void MotorSystem::updateActionExpectedValues(const std::vector<const Action*>& actions, 
                                            const std::vector<float>& outcomes) {
    for (size_t i = 0; i < actions.size(); ++i) {
        float expectedValue = std::accumulate(outcomes.begin(), outcomes.end(), 0.0f) / outcomes.size();
        
        if (actions[i] && actions[i]->getDynamics().expectedValue != expectedValue) {
            actions[i]->getDynamics().expectedValue = expectedValue;
        }
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    // Clean up dynamically allocated actions in history
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

void MotorSystem::reset() {
    // Reset motor state
    pImpl->arousalLevel = 0.5f;
    pImpl->neuromodulation = 0.0f;
    pImpl->fatigueFactor = 0.0f;
    
    // Reset preferences
    for (auto& pair : pImpl->actionPreferences) {
        pair.second = 1.0f;
    }
    
    // Clear history
    clearHistory();
    
    // Reset statistics
    pImpl->totalActionsExecuted = 0;
    pImpl->successfulActions = 0;
    pImpl->sequenceCount = 0;
}

void MotorSystem::emergencyStop() {
    // Rapid fatigue to stop all motor output
    pImpl->fatigueFactor = 1.0f;
    pImpl->arousalLevel = 0.0f;
    
    // Reset all motor commands
    for (auto& action : pImpl->actionHistory) {
        if (action) {
            action->getDynamics().neuralDrive = 0.0f;
            action->getDynamics().motorCommand = MotorCommand::Wait;
        }
    }
}

} // namespace nlm
