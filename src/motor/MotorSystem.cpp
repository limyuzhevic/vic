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
    // IMPROVEMENT: Implement proper action selection based on motor neuron activity
    // Real implementation would evaluate action values based on neural firing rates
    if (!pImpl->availableActions.empty()) {
        // Simple improvement: Add action preference based on context
        // In real implementation, this would use motor neuron activity levels
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // PLACEHOLDER: No implementation
    // IMPROVEMENT: Add action preference system based on learned values
    // Real implementation would update preference vectors based on rewards
    
    // Simple improvement: Store preferences in member variable
    // In real implementation, this would affect action selection
    auto it = std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action);
    if (it != pImpl->availableActions.end()) {
        // Action preference would modify selection probability
        // For now, just store the preference for future use
        // TODO: Add preference storage and use in selectAction
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
