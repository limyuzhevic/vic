#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<std::unique_ptr<Action>> actionHistory;
    
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
    // TODO PHASE 6: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns first available action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 6: Implement action preferences
}

void MotorSystem::addActionToHistory(std::unique_ptr<Action> action) {
    if (action) {
        pImpl->actionHistory.push_back(std::move(action));
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    // Convert unique_ptr<Action> to const Action* for const interface
    // This is a temporary solution - proper design would use a different API
    static std::vector<const Action*> history;
    history.clear();
    
    for (const auto& actionPtr : pImpl->actionHistory) {
        if (actionPtr) {
            history.push_back(actionPtr.get());
        }
    }
    
    return history;
}

void MotorSystem::clearHistory() {
    pImpl->actionHistory.clear();
}

} // namespace nlm
