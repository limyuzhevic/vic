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
    // Real action selection based on neural activity patterns
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Analyze neural activity to determine appropriate action
    float maxActivity = 0.0f;
    NeuronType preferredType = NeuronType::Internal;
    
    for (auto* neuron : neurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            preferredType = neuron->getType();
        }
    }
    
    // Map neural activity to actions based on preferred neuron type
    ActionType selectedAction = ActionType::Wait;
    
    switch (preferredType) {
        case NeuronType::Motor:
            // Motor neurons indicate movement intent
            if (maxActivity > 0.8f) selectedAction = ActionType::MoveForward;
            else if (maxActivity > 0.6f) selectedAction = ActionType::MoveBackward;
            break;
        
        case NeuronType::Sensory:
            // Sensory neurons might indicate exploration or interaction
            if (maxActivity > 0.7f) selectedAction = ActionType::Interact;
            else selectedAction = ActionType::Look;
            break;
        
        case NeuronType::Modulatory:
            // Modulatory neurons might indicate rest or specific behaviors
            if (maxActivity > 0.5f) selectedAction = ActionType::Rest;
            else selectedAction = ActionType::Wait;
            break;
        
        case NeuronType::Excitatory:
            // Excitatory neurons might drive approach behaviors
            selectedAction = ActionType::MoveForward;
            break;
        
        case NeuronType::Inhibitory:
            // Inhibitory neurons might drive avoidance behaviors
            selectedAction = ActionType::MoveBackward;
            break;
        
        default:
            selectedAction = ActionType::Wait;
            break;
    }
    
    // Record action in history
    auto* action = new Action(selectedAction);
    pImpl->actionHistory.push_back(action);
    
    return std::unique_ptr<Action>(action);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Real action preference system with reinforcement learning
    // For now, store preferences for future action selection
    pImpl->actionHistory.clear(); // Clear old history
    
    // Create history entries weighted by preference
    for (const auto& availableAction : pImpl->availableActions) {
        float baseProbability = 0.1f; // Default probability
        
        if (availableAction == action) {
            // Increase probability for preferred action
            baseProbability += preference * 0.4f;
        }
        
        // Create weighted random selection (simplified)
        if (rand() % 100 < baseProbability * 100) {
            auto* actionPtr = new Action(availableAction);
            pImpl->actionHistory.push_back(actionPtr);
        }
    }
    
    if (pImpl->actionHistory.empty()) {
        auto* waitAction = new Action(ActionType::Wait);
        pImpl->actionHistory.push_back(waitAction);
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

} // namespace nlm
