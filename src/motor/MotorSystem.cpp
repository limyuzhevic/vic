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
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Enhanced neural integration for Phase 6
    
    // Get motor neurons from the region
    const auto& neurons = motorRegion.getNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity levels for each motor action
    // Motor neurons are organized by type (MoveForward, MoveBackward, etc.)
    float activity[ActionType::Count];
    for (int i = 0; i < ActionType::Count; ++i) {
        activity[i] = 0.0f;
    }
    
    // Map motor neurons to actions based on their type
    for (const auto* neuron : neurons) {
        // Determine which action this neuron contributes to
        // This is a simplified mapping - in real implementation would use
        // neuron properties, connections, etc.
        NeuronType type = neuron->getType();
        float neuronActivity = neuron->getFiringRate();
        
        // Map neural type to action type
        ActionType action;
        switch (type) {
            case NeuronType::Motor: action = ActionType::MoveForward; break;
            case NeuronType::Inhibitory: action = ActionType::Wait; break;
            default: action = ActionType::Wait; break;
        }
        
        // Add to action activity (weighted by firing rate)
        int actionIdx = static_cast<int>(action);
        if (actionIdx >= 0 && actionIdx < ActionType::Count) {
            activity[actionIdx] += neuronActivity;
        }
    }
    
    // Find action with highest activity
    ActionType selectedAction = ActionType::Wait;
    float maxActivity = 0.0f;
    
    for (int i = 0; i < ActionType::Count; ++i) {
        if (activity[i] > maxActivity) {
            maxActivity = activity[i];
            selectedAction = static_cast<ActionType>(i);
        }
    }
    
    // Apply action preferences (exploration bias)
    // Higher activity gets additional preference
    if (maxActivity > 0.0f) {
        // Add some exploration bias based on action preferences
        float explorationNoise = 0.1f;  // 10% exploration
        if ((rand() % 100) / 100.0f < explorationNoise) {
            // Randomly select different action with small probability
            int randomAction = rand() % ActionType::Count;
            if (randomAction != static_cast<int>(selectedAction)) {
                selectedAction = static_cast<ActionType>(randomAction);
            }
        }
    }
    
    // Add to action history
    pImpl->actionHistory.push_back(new Action(selectedAction));
    
    return std::make_unique<Action>(selectedAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
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
