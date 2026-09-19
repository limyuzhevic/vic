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
    // PLACEHOLDER: Returns random action
    
    // Get all motor neurons from the region
    std::vector<Neuron*> motorNeurons = motorRegion.getAllNeurons();
    
    // Calculate activity for each possible action based on neural activity
    std::map<ActionType, float> actionScores;
    
    // Initialize with base preferences
    for (ActionType action : pImpl->availableActions) {
        actionScores[action] = 1.0f;  // Equal starting score
    }
    
    // Boost action based on nearby neuron activity
    for (Neuron* neuron : motorNeurons) {
        const auto& state = neuron->getState();
        
        // Get firing rate (normalized to reasonable range)
        float activity = state.firingRate;
        if (activity > 1.0f) activity = 1.0f; // Clamp to [0,1]
        
        // Determine which action this neuron contributes to
        // This is simplified - in reality would be mapped by neural connections
        for (ActionType action : pImpl->availableActions) {
            // Add activity to the score for this action
            // This is a simplified mapping - neurons are distributed across actions
            actionScores[action] += activity * 0.5f;
        }
    }
    
    // Apply action preferences if implemented
    // For now, we could use some basic heuristics
    
    // Select action with highest score
    float maxScore = 0.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& pair : actionScores) {
        if (pair.second > maxScore) {
            maxScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Only return if we have meaningful activity
    if (maxScore < 0.5f) {
        bestAction = ActionType::Wait;
    }
    
    return std::make_unique<Action>(bestAction);
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
