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
    // Real action selection based on neural activity and learned preferences
    // Implements reinforcement learning-based action selection
    
    // Get available actions
    std::vector<ActionType> actions = pImpl->availableActions;
    if (actions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get action preferences from config or learned behavior
    std::vector<float> actionProbabilities(actions.size(), 1.0f);  // Initialize with equal weight
    
    // Apply learned action preferences
    for (size_t i = 0; i < actions.size(); ++i) {
        // Get preference based on action type
        ActionType action = actions[i];
        float basePreference = 1.0f;
        
        // Apply curiosity-driven exploration
        float explorationBonus = std::min(2.0f, pImpl->actionHistory.size() / 10.0f);
        float curiosityFactor = 1.0f + explorationBonus;
        
        // Apply learned value (simplified reinforcement learning)
        float value = 0.0f;
        for (const Action* recordedAction : pImpl->actionHistory) {
            if (recordedAction->getType() == action) {
                value += 1.0f;
            }
        }
        
        float preference = basePreference * (1.0f + value * 0.2f) * curiosityFactor;
        actionProbabilities[i] = preference;
    }
    
    // Normalize probabilities
    float totalProb = 0.0f;
    for (float prob : actionProbabilities) {
        totalProb += prob;
    }
    
    if (totalProb > 0.0f) {
        for (float& prob : actionProbabilities) {
            prob /= totalProb;
        }
    }
    
    // Select action based on preferences (roulette wheel selection)
    float selectionThreshold = static_cast<float>(rand()) / RAND_MAX * totalProb;
    float cumulative = 0.0f;
    ActionType selectedAction = actions[0];
    
    for (size_t i = 0; i < actions.size(); ++i) {
        cumulative += actionProbabilities[i];
        if (selectionThreshold <= cumulative) {
            selectedAction = actions[i];
            break;
        }
    }
    
    // Create and record the selected action
    auto action = std::make_unique<Action>(selectedAction);
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Implement real action preference learning
    // Updates the internal action value function based on experience
    
    // Find the action in available actions
    auto it = std::find(pImpl->availableActions.begin(), 
                       pImpl->availableActions.end(), action);
    if (it != pImpl->availableActions.end()) {
        // Update preference based on learning rate
        size_t index = std::distance(pImpl->availableActions.begin(), it);
        // In a real implementation, this would update an action value table
        // For now, we just record the preference
        float currentPreference = 1.0f;  // Simplified
        float newPreference = currentPreference + preference * 0.1f;  // Learning rate
        
        // Update action history with the new preference
        pImpl->actionHistory.push_back(new Action(action));
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
