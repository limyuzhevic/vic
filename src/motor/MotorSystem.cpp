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

// Phase 2 enhanced functionality

// Enhanced action selection based on neural activity
std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Get action with highest neural activation
std::unique_ptr<Action> MotorSystem::selectActionBasedOnActivation(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement neural activation-based action selection
    // PLACEHOLDER: Returns first available action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Get action based on preference weights
std::unique_ptr<Action> MotorSystem::selectActionWithPreferences(const std::vector<float>& preferences) {
    // TODO PHASE 2: Implement preference-based action selection
    // PLACEHOLDER: Use first available action
    if (!pImpl->availableActions.empty() && !preferences.empty()) {
        // Simple implementation: pick action with highest preference
        size_t bestAction = 0;
        float bestPreference = preferences[0];
        for (size_t i = 1; i < preferences.size() && i < pImpl->availableActions.size(); ++i) {
            if (preferences[i] > bestPreference) {
                bestPreference = preferences[i];
                bestAction = i;
            }
        }
        return std::make_unique<Action>(pImpl->availableActions[bestAction]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Get action based on neural population activity
std::unique_ptr<Action> MotorSystem::selectActionBasedOnPopulation(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement population-based action selection
    // PLACEHOLDER: Returns first available action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Get action with most recent neural activation
std::unique_ptr<Action> MotorSystem::selectActionBasedOnRecency(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement recency-based action selection
    // PLACEHOLDER: Returns first available action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Get current available actions
const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

// Set action preference
void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // PLACEHOLDER: Simple implementation
    // This would be enhanced with neural weighting
}

// Get action history
const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

// Clear action history
void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
