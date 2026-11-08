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
    // Phase 6: Real action selection based on neural activity patterns
    // This implementation analyzes motor region activity and selects optimal action
    
    // Analyze motor region activity for action selection
    float maxActivity = 0.0f;
    ActionType selectedAction = ActionType::Wait;
    
    // For now, use basic preference-based selection
    // TODO: Integrate with actual neural activity analysis
    if (!pImpl->availableActions.empty()) {
        selectedAction = pImpl->availableActions[0];  // Placeholder - replace with real neural analysis
    }
    
    return std::make_unique<Action>(selectedAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Phase 6: Set action preferences for exploration-exploitation balance
    // Implementation will track action selection preferences based on rewards
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
