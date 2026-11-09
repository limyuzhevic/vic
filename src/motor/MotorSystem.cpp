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
    
    // Get motor neuron activity from the motor region
    const auto& populations = motorRegion.getPopulations();
    std::vector<float> motorActivity;
    
    // Calculate activity level for each motor neuron
    for (const auto& pop : populations) {
        for (const auto* neuron : pop->getNeurons()) {
            motorActivity.push_back(neuron->getActivityLevel());
        }
    }
    
    // Determine action based on motor activity patterns
    if (motorActivity.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find the most active motor neuron type
    float maxActivity = 0.0f;
    ActionType selectedAction = ActionType::Wait;
    
    for (ActionType action : pImpl->availableActions) {
        // Get neurons associated with this action type (simplified)
        float actionActivity = calculateActionActivity(motorRegion, action);
        
        if (actionActivity > maxActivity) {
            maxActivity = actionActivity;
            selectedAction = action;
        }
    }
    
    // Return action with preference based on learned traits
    return std::make_unique<Action>(selectedAction);
}

float MotorSystem::calculateActionActivity(const NeuralRegion& motorRegion, ActionType action) const {
    // Calculate activity level for a specific action type based on neural activity
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    const auto& populations = motorRegion.getPopulations();
    for (const auto& pop : populations) {
        for (const auto* neuron : pop->getNeurons()) {
            // Get neuron activity (simplified)
            float activity = neuron->getActivityLevel();
            
            // Weight by action type (simplified - in real implementation would be
            // more sophisticated based on neural encoding)
            float actionWeight = getActionWeight(action);
            
            totalActivity += activity * actionWeight;
            neuronCount++;
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0f;
}

float MotorSystem::getActionWeight(ActionType action) const {
    // Return action weight based on learned preferences or action type
    // This is a placeholder implementation - real system would use
    // learned action preferences from experience
    
    // Default weights based on action type
    switch (action) {
        case ActionType::MoveForward: return 1.0f;
        case ActionType::MoveBackward: return 1.0f;
        case ActionType::MoveLeft: return 1.0f;
        case ActionType::MoveRight: return 1.0f;
        case ActionType::TurnLeft: return 0.8f;
        case ActionType::TurnRight: return 0.8f;
        case ActionType::Look: return 0.6f;
        case ActionType::Interact: return 0.9f;
        case ActionType::Eat: return 0.7f;
        case ActionType::Drink: return 0.7f;
        case ActionType::Rest: return 0.3f;
        case ActionType::Wait: return 0.0f;
        case ActionType::Custom: return 0.5f;
        default: return 0.0f;
    }
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
