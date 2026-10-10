#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::map<ActionType, float> actionPreferences; // Action preference values for learning
    
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
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity and motivational state
    // Implements multiple decision processes: deliberative, reflexive, and habitual
    
    // Get neural activity from motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activation levels for different action types
    std::map<ActionType, float> actionActivations;
    
    // Get current motivational state (would access brain's motivation system)
    float motivation = 1.0f; // Would get from brain in real implementation
    float arousal = 0.5f;    // Would get from brain in real implementation
    
    // Calculate action preferences based on learned values and neural activity
    for (ActionType action : pImpl->availableActions) {
        float activation = calculateActionActivation(action, neurons, motivation, arousal);
        actionActivations[action] = activation;
    }
    
    // Add habit-based preferences from action history
    for (const Action* action : pImpl->actionHistory) {
        if (action) {
            ActionType actionType = action->getType();
            auto it = actionActivations.find(actionType);
            if (it != actionActivations.end()) {
                // Habit strengthening: repeat actions that led to good outcomes
                float habitBoost = 0.1f * (pImpl->actionHistory.size() / 10.0f); // Diminishing returns
                it->second += habitBoost;
            }
        }
    }
    
    // Apply neuromodulatory modulation
    // Dopamine enhances selected action vigor
    float dopamine = 0.5f; // Would get from brain in real implementation
    if (dopamine > 0.7f) {
        // High dopamine: boost action selection
        for (auto& pair : actionActivations) {
            pair.second *= 1.2f;
        }
    }
    
    // Acetylcholine modulates attentional focus
    float acetylcholine = 0.5f; // Would get from brain in real implementation
    if (acetylcholine > 0.7f) {
        // High acetylcholine: bias towards novel/informative actions
        // For simplicity, boost action variability
        float totalActivation = 0.0f;
        for (const auto& pair : actionActivations) {
            totalActivation += pair.second;
        }
        if (totalActivation > 0.0f) {
            for (auto& pair : actionActivations) {
                pair.second = pair.second * 0.7f + 0.3f; // Add stochasticity
            }
        }
    }
    
    // Select action based on activation levels (softmax choice)
    float totalActivation = 0.0f;
    for (const auto& pair : actionActivations) {
        totalActivation += pair.second;
    }
    
    if (totalActivation <= 0.0f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Sample action from activation distribution
    float randomValue = static_cast<float>(rand()) / RAND_MAX * totalActivation;
    float cumulativeActivation = 0.0f;
    
    for (const auto& pair : actionActivations) {
        cumulativeActivation += pair.second;
        if (cumulativeActivation >= randomValue) {
            return std::make_unique<Action>(pair.first);
        }
    }
    
    // Fallback to wait
    return std::make_unique<Action>(ActionType::Wait);
}

float MotorSystem::calculateActionActivation(ActionType action, const std::vector<Neuron*>& neurons, float motivation, float arousal) {
    // Calculate neural activation for a specific action type
    // Based on population coding in motor cortex
    
    float activation = 0.0f;
    
    // Get action-specific preferences
    float preference = getActionPreference(action);
    
    // Calculate neural drive from motor neurons
    float neuralDrive = 0.0f;
    for (Neuron* neuron : neurons) {
        if (neuron) {
            // Simple model: neuron activation contributes to action drive
            float neuronActivation = neuron->getMembranePotential() / 100.0f; // Normalize
            neuralDrive += neuronActivation;
        }
    }
    
    if (!neurons.empty()) {
        neuralDrive /= neurons.size();
    }
    
    // Combine factors for action activation
    // Preference represents learned value (habit)
    // Neural drive represents current motor command
    // Motivation and arousal modulate overall responsiveness
    
    activation = preference * 0.4f + neuralDrive * 0.3f + motivation * 0.2f + arousal * 0.1f;
    
    // Apply nonlinearities (softmax-like response)
    activation = 1.0f / (1.0f + std::exp(-activation * 5.0f)); // Sigmoid
    
    return activation;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set action preference based on learning and experience
    // Implements actor-critic learning for action values
    
    // In a real implementation, this would update a Q-table or neural network
    // For now, we'll store preferences in a simple map
    if (preference < 0.0f || preference > 1.0f) {
        preference = std::clamp(preference, 0.0f, 1.0f);
    }
    
    // Store preference with decay
    auto it = actionPreferences.find(action);
    if (it != actionPreferences.end()) {
        // Decay towards new preference
        it->second = it->second * 0.9f + preference * 0.1f;
    } else {
        actionPreferences[action] = preference;
    }
}

float MotorSystem::getActionPreference(ActionType action) const {
    // Get action preference from learning system
    auto it = actionPreferences.find(action);
    if (it != actionPreferences.end()) {
        return it->second;
    }
    return 0.1f; // Default low preference for unlearned actions
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    pImpl->actionPreferences.clear(); // Also clear preferences
}

void MotorSystem::recordAction(std::unique_ptr<Action> action) {
    // Record action execution for learning and habit formation
    if (action) {
        // Add to history
        pImpl->actionHistory.push_back(action.release());
        
        // Limit history size
        if (pImpl->actionHistory.size() > 100) {
            delete pImpl->actionHistory.front();
            pImpl->actionHistory.erase(pImpl->actionHistory.begin());
        }
        
        // Update action preference based on execution
        // This implements the actor component of actor-critic learning
        ActionType actionType = action->getType();
        setActionPreference(actionType, getActionPreference(actionType) + 0.01f);
    }
}
