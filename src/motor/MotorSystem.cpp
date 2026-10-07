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

// TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        // Simple neural-based action selection based on average firing rate
        float avgFiringRate = motorRegion.getAverageFiringRate();
        
        // Normalize firing rate to influence action selection
        // Higher firing rate increases probability of certain actions
        if (avgFiringRate > 2.0f) {
            // High firing rate promotes exploration - random action
            int idx = rand() % pImpl->availableActions.size();
            return std::make_unique<Action>(pImpl->availableActions[idx]);
        } else if (avgFiringRate > 0.5f) {
            // Medium firing rate promotes moderate exploration
            int idx = std::min(2, (int)pImpl->availableActions.size() - 1);
            return std::make_unique<Action>(pImpl->availableActions[idx]);
        } else {
            // Low firing rate promotes default action
            return std::make_unique<Action>(ActionType::Wait);
        }
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // PLACEHOLDER: Action preferences not yet implemented
    // In a full implementation, this would store action preferences
    // and use them in selectAction() to influence action selection
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
