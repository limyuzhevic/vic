#include "MotorSystem.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    // Available motor actions
    std::vector<ActionType> availableActions;
    
    // Action preferences based on experience
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, float> actionValues;
    
    // Action history for learning
    std::vector<const Action*> actionHistory;
    std::vector<Timestamp> actionTimes;
    
    // Motor planning state
    std::queue<ActionType> actionQueue;
    std::queue<float> actionProbabilities;
    
    // Motor execution state
    float currentActionTime;
    std::unique_ptr<Action> currentAction;
    float actionProgress;
    
    // Biological motor control parameters
    float motorGain;
    float movementSmoothness;
    float muscleFatigue;
    float sensoryFeedbackWeight;
    
    // Reward and cost learning
    float explorationRate;
    float exploitationRate;
    float learningRate;
    
    // Motor dynamics
    std::vector<float> muscleForces;
    std::vector<float> jointAngles;
    std::vector<float> jointVelocities;
    std::vector<float> jointTorques;
    
    Impl() : 
        motorGain(1.0f),
        movementSmoothness(0.8f),
        muscleFatigue(0.0f),
        sensoryFeedbackWeight(0.6f),
        explorationRate(0.2f),
        exploitationRate(0.8f),
        learningRate(0.01f) {
        
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
        
        // Initialize action preferences
        for (ActionType action : availableActions) {
            actionPreferences[action] = 1.0f / availableActions.size();
            actionValues[action] = 0.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity and learned preferences
    return neuralActionSelection(motorRegion);
}

std::unique_ptr<Action> MotorSystem::neuralActionSelection(const NeuralRegion& motorRegion) {
    // Neural action selection using probabilistic sampling
    std::vector<ActionType> candidates;
    std::vector<float> probabilities;
    
    // Collect candidate actions based on neural activity
    collectCandidateActions(motorRegion, candidates, probabilities);
    
    if (candidates.empty()) {
        // Fallback to action with highest preference
        ActionType bestAction = getBestAction();
        return std::make_unique<Action>(bestAction);
    }
    
    // Normalize probabilities
    float totalProb = 0.0f;
    for (float prob : probabilities) {
        totalProb += prob;
    }
    
    if (totalProb > 0.0f) {
        for (float& prob : probabilities) {
            prob /= totalProb;
        }
    } else {
        // Equal probabilities if total is zero
        for (float& prob : probabilities) {
            prob = 1.0f / probabilities.size();
        }
    }
    
    // Probabilistic sampling
    float randomValue = static_cast<float>(rand()) / RAND_MAX;
    float cumulativeProb = 0.0f;
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        cumulativeProb += probabilities[i];
        if (randomValue <= cumulativeProb) {
            ActionType selectedAction = candidates[i];
            float confidence = probabilities[i];
            
            // Create action with learned properties
            return createActionWithLearning(selectedAction, confidence);
        }
    }
    
    // Fallback to first candidate
    return createActionWithLearning(candidates[0], 1.0f / candidates.size());
}

void MotorSystem::collectCandidateActions(const NeuralRegion& motorRegion, 
                                         std::vector<ActionType>& candidates,
                                         std::vector<float>& probabilities) {
    // Collect actions based on motor neuron activity
    auto neurons = motorRegion.getAllNeurons();
    
    // Map neurons to action types
    std::unordered_map<ActionType, float> actionActivations;
    
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        float activation = calculateNeuronActivation(neuron);
        
        // Map neuron type to action
        ActionType action = mapNeuronToAction(neuron, activation);
        
        // Accumulate activations
        actionActivations[action] += activation;
    }
    
    // Filter and normalize actions
    for (const auto& pair : actionActivations) {
        ActionType action = pair.first;
        float activation = pair.second;
        
        if (activation > 0.01f) {
            float preference = actionPreferences[action];
            float value = actionValues[action];
            
            // Combine neural activation with learned preferences and values
            float score = activation * 0.5f + preference * 0.3f + value * 0.2f;
            
            if (score > 0.01f) {
                candidates.push_back(action);
                probabilities.push_back(score);
            }
        }
    }
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl->actionPreferences.count(action)) {
        pImpl::actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
        
        // Renormalize all preferences
        normalizeActionPreferences();
    }
}

void MotorSystem::updateActionValue(ActionType action, float outcome, float reward) {
    // Update action value based on outcome and reward
    if (pImpl->actionValues.count(action)) {
        // Temporal difference learning
        float oldValue = pImpl->actionValues[action];
        float tdError = reward - oldValue;
        
        // Update value
        pImpl->actionValues[action] = oldValue + pImpl->learningRate * tdError;
        
        // Update preference based on value
        float newPreference = std::max(0.0f, pImpl->actionValues[action]);
        setActionPreference(action, newPreference);
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
    pImpl->actionTimes.clear();
}

void MotorSystem::updateActionExecution(TimestepDuration dt) {
    // Update current action execution
    if (pImpl->currentAction) {
        pImpl->currentActionTime += dt;
        
        // Update action progress
        pImpl->actionProgress = std::min(1.0f, pImpl->currentActionTime / pImpl->currentAction->getDuration());
        
        // Check if action is complete
        if (pImpl->actionProgress >= 1.0f) {
            completeAction();
        }
        
        // Update muscle fatigue
        pImpl->muscleFatigue += dt * 0.001f;  // Fatigue accumulates over time
        pImpl->muscleFatigue = std::min(1.0f, pImpl->muscleFatigue);
        
        // Apply fatigue to action execution
        if (pImpl->muscleFatigue > 0.8f) {
            // Fatigue affects action performance
            pImpl->currentAction->setSuccess(false);
        }
    }
}

bool MotorSystem::executeAction(const ActionType& actionType) {
    // Execute motor action with biological constraints
    std::unique_ptr<Action> action = std::make_unique<Action>(actionType);
    
    // Apply motor gains and biological constraints
    float executionSuccess = calculateExecutionSuccess(actionType);
    
    if (executionSuccess > (0.5f + (1.0f - pImpl->muscleFatigue) * 0.3f)) {
        // Action succeeds
        startActionExecution(std::move(action));
        return true;
    } else {
        // Action fails due to fatigue or poor conditions
        return false;
    }
}

std::unique_ptr<Action> createActionWithLearning(ActionType actionType, float confidence) {
    // Create action with learned properties
    std::unique_ptr<Action> action = std::make_unique<Action>(actionType);
    
    // Set action properties based on learning
    action->setConfidence(confidence);
    action->setLearningPriority(calculateLearningPriority(actionType));
    action->setExecutionCost(calculateExecutionCost(actionType));
    
    return action;
}

ActionType MotorSystem::getBestAction() const {
    // Get action with highest preference
    ActionType bestAction = ActionType::Wait;
    float bestPreference = 0.0f;
    
    for (const auto& pair : pImpl->actionPreferences) {
        if (pair.second > bestPreference) {
            bestAction = pair.first;
            bestPreference = pair.second;
        }
    }
    
    return bestAction;
}

void normalizeActionPreferences() {
    // Renormalize action preferences
    float totalPreference = 0.0f;
    for (const auto& pair : pImpl->actionPreferences) {
        totalPreference += pair.second;
    }
    
    if (totalPreference > 0.0f) {
        for (auto& pair : pImpl->actionPreferences) {
            pair.second /= totalPreference;
        }
    } else {
        // Equal preferences if total is zero
        float equalPreference = 1.0f / pImpl->actionPreferences.size();
        for (auto& pair : pImpl->actionPreferences) {
            pair.second = equalPreference;
        }
    }
}

ActionType mapNeuronToAction(Neuron* neuron, float activation) {
    // Map neuron activity to action type
    if (!neuron) return ActionType::Wait;
    
    // Simple mapping based on neuron type
    NeuronType neuronType = neuron->getType();
    
    switch (neuronType) {
        case NeuronType::Motor:
            if (activation > 0.7f) return ActionType::MoveForward;
            else if (activation > 0.5f) return ActionType::MoveLeft;
            else return ActionType::Wait;
            
        case NeuronType::Sensory:
            if (activation > 0.8f) return ActionType::Look;
            else return ActionType::Wait;
            
        case NeuronType::Internal:
            if (activation > 0.6f) return ActionType::Interact;
            else return ActionType::Wait;
            
        default:
            return ActionType::Wait;
    }
}

float calculateNeuronActivation(Neuron* neuron) {
    // Calculate neuron activation based on membrane potential
    if (!neuron) return 0.0f;
    
    float membranePotential = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    // Normalize to activation between 0 and 1
    float activation = std::max(0.0f, (membranePotential - (-70.0f)) / (threshold - (-70.0f)));
    
    return std::min(activation, 1.0f);
}

float calculateExecutionSuccess(ActionType actionType) {
    // Calculate probability of successful execution
    float baseSuccess = 0.9f;
    
    // Different success rates for different actions
    switch (actionType) {
        case ActionType::MoveForward:
            return baseSuccess;
        case ActionType::MoveBackward:
            return baseSuccess - 0.1f;
        case ActionType::MoveLeft:
            return baseSuccess - 0.05f;
        case ActionType::MoveRight:
            return baseSuccess - 0.05f;
        case ActionType::TurnLeft:
            return baseSuccess - 0.1f;
        case ActionType::TurnRight:
            return baseSuccess - 0.1f;
        case ActionType::Look:
            return baseSuccess;
        case ActionType::Interact:
            return baseSuccess - 0.2f;  // More likely to fail
        default:
            return baseSuccess;
    }
}

float calculateLearningPriority(ActionType actionType) {
    // Calculate learning priority based on action type
    // Actions that provide more learning value get higher priority
    switch (actionType) {
        case ActionType::Interact:
            return 1.0f;  // High learning potential
        case ActionType::Look:
            return 0.8f;  // Information gathering
        case ActionType::MoveForward:
            return 0.6f;  // Exploration
        default:
            return 0.3f;  // Routine actions
    }
}

float calculateExecutionCost(ActionType actionType) {
    // Calculate execution cost (energy, time, risk)
    float baseCost = 1.0f;
    
    switch (actionType) {
        case ActionType::MoveForward:
            return baseCost * 0.8f;
        case ActionType::MoveBackward:
            return baseCost * 1.2f;
        case ActionType::MoveLeft:
            return baseCost * 0.9f;
        case ActionType::MoveRight:
            return baseCost * 0.9f;
        case ActionType::TurnLeft:
            return baseCost * 1.1f;
        case ActionType::TurnRight:
            return baseCost * 1.1f;
        case ActionType::Look:
            return baseCost * 0.5f;  // Low energy
        case ActionType::Interact:
            return baseCost * 1.5f;  // High energy
        default:
            return baseCost;
    }
}

void startActionExecution(std::unique_ptr<Action> action) {
    // Start action execution
    MotorSystem::Impl* impl = static_cast<MotorSystem::Impl*>(/* Need to get pImpl here */);
    
    impl->currentAction = std::move(action);
    impl->currentActionTime = 0.0f;
    impl->actionProgress = 0.0f;
    impl->actionQueue.push(impl->currentAction->getType());
    
    // Add to history
    impl->actionHistory.push_back(impl->currentAction.get());
    impl->actionTimes.push_back(getCurrentTimestamp());
}

void completeAction() {
    // Mark action as completed
    MotorSystem::Impl* impl = static_cast<MotorSystem::Impl*>(/* Need to get pImpl here */);
    
    if (impl->currentAction) {
        impl->currentAction->setCompletionTime(impl->currentActionTime);
        impl->currentAction->setSuccess(true);
    }
    
    impl->currentAction.reset();
    impl->actionProgress = 0.0f;
}

float calculateLearningPriority(ActionType actionType) {
    // Calculate learning priority
    return 1.0f;
}

float calculateExecutionCost(ActionType actionType) {
    // Calculate execution cost
    return 1.0f;
}

} // namespace nlm
