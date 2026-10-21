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
    // PLACEHOLDER: Returns random action with better error handling
    
    if (motorRegion.getTotalNeuronCount() == 0) {
        NLM_LOG_WARNING("Motor region has no neurons for action selection");
    }
    
    if (!pImpl->availableActions.empty()) {
        NLM_LOG_DEBUG("Selecting action from available actions: " + 
                     std::to_string(pImpl->availableActions.size()) + " options");
        
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    
    NLM_LOG_WARNING("No available actions, returning wait action");
    return std::make_unique<Action>(ActionType::Wait);
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
