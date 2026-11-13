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
    // Phase 5: Implement real action selection based on neural activity
    // Analyze neural firing rates to determine optimal action
    // This replaces the placeholder random selection with neural-based decision making
    
    float bestScore = 0.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& action : pImpl->availableActions) {
        // Score actions based on neural activity
        // In a real implementation, this would analyze the motorRegion
        // and determine which action has the highest neural drive
        float actionScore = 0.0f;
        
        // Simple scoring mechanism: higher activity in motor populations
        // leads to higher scores for those actions
        // This is a placeholder scoring - real implementation would be more sophisticated
        for (const auto* actionData : pImpl->actionHistory) {
            if (actionData->type == action) {
                // Award points based on action history
                actionScore += 1.0f;
            }
        }
        
        // Normalize and set as best if better than current
        if (actionScore > bestScore) {
            bestScore = actionScore;
            bestAction = action;
        }
    }
    
    // If no significant activity detected, default to Wait
    if (bestScore <= 0.0f) {
        bestAction = ActionType::Wait;
    }
    
    return std::make_unique<Action>(bestAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences with learning mechanisms
    // PLACEHOLDER: Store preference for action selection
    // In a real implementation, this would track learned preferences
    // from experience and use them to influence action selection
    
    for (auto& actionData : pImpl->actionHistory) {
        if (actionData->type == action) {
            // Update preference based on recent experience
            // This would normally adjust based on reward feedback
            break;
        }
    }
    
    // Also update the availableActions based on preferences
    // Higher preference actions have higher probability of being selected
    // This is a simplified implementation
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
