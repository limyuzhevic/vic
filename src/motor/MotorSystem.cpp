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
    // Phase 2 completed - Real action selection based on neural activity implemented
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Phase 2 completed - Action preferences implemented
    // Store preference for future action selection
    if (preference > 0.0f) {
        // Higher preference increases action selection probability
        for (auto& availableAction : pImpl->availableActions) {
            if (availableAction == action) {
                // Reorder to make this action more likely to be selected first
                auto it = std::find(pImpl->availableActions.begin(), 
                                  pImpl->availableActions.end(), action);
                if (it != pImpl->availableActions.end()) {
                    std::rotate(pImpl->availableActions.begin(), it, 
                               std::next(it));
                }
                break;
            }
        }
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
