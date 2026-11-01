#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
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
    // Implement real action selection based on neural activity
    // More sophisticated than just returning the first action
    
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate firing rates and activation levels
    std::vector<float> neuronActivations;
    for (auto* neuron : neurons) {
        float activation = 0.0f;
        
        // Get membrane potential relative to threshold
        float potential = neuron->getMembranePotential();
        float threshold = neuron->getThreshold();
        float resting = neuron->getRestingPotential();
        
        if (potential > resting) {
            // Normalize activation: 0.0 to 1.0 based on threshold
            activation = std::min(1.0f, (potential - resting) / (threshold - resting));
        }
        
        // Apply firing rate if available
        if (neuron->getFiringState() == FiringState::Active) {
            activation = std::max(activation, neuron->getFiringRate());
        }
        
        neuronActivations.push_back(activation);
    }
    
    // Find most active neurons
    float maxActivation = 0.0f;
    size_t bestNeuronIndex = 0;
    
    for (size_t i = 0; i < neuronActivations.size(); ++i) {
        if (neuronActivations[i] > maxActivation) {
            maxActivation = neuronActivations[i];
            bestNeuronIndex = i;
        }
    }
    
    // Apply action preferences
    if (maxActivation < 0.1f) {
        // Low activation: explore or wait
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Map neuron type to action type
    NeuronType neuronType = neurons[bestNeuronIndex]->getType();
    
    ActionType selectedAction;
    switch (neuronType) {
        case NeuronType::Sensory:
            // Sensory neurons might trigger interaction or look actions
            if (std::rand() % 3 == 0) {  // 1/3 chance to look
                selectedAction = ActionType::Look;
            } else {
                selectedAction = ActionType::Interact;
            }
            break;
        case NeuronType::Motor:
            // Motor neurons directly influence motor actions
            selectedAction = neurons[bestNeuronIndex]->getId() % static_cast<uint64_t>(ActionType::Count);
            break;
        case NeuronType::Internal:
            // Internal neurons can influence planning
            if (maxActivation > 0.7f) {
                // High confidence: execute planned action
                selectedAction = neurons[bestNeuronIndex]->getId() % static_cast<uint64_t>(ActionType::Count);
            } else {
                // Low confidence: wait and observe
                selectedAction = ActionType::Wait;
            }
            break;
        default:
            selectedAction = ActionType::Wait;
    }
    
    // Create action and add to history
    auto action = std::make_unique<Action>(selectedAction);
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Implement action preferences for exploration/exploitation balance
    // Store preferences and use them in action selection
    
    // Create a simple preference system
    struct ActionPreference {
        ActionType action;
        float preference;
    };
    
    // For now, store in action history as preference
    // In a more advanced implementation, this would be in a dedicated preference vector
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            // Update preference (placeholder implementation)
            // This would be expanded with a proper preference storage system
            break;
        }
    }
    
    // Log preference setting
    NLM_LOG_INFO("Set preference for action " + std::to_string(static_cast<int>(action)) + 
                 " to " + std::to_string(preference));
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

} // namespace nlm
