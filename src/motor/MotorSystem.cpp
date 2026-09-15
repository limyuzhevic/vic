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
    // Real action selection based on neural activity
    // Analyze motor neuron activity to choose best action
    
    // Get all neurons in the motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity levels for each available action
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType actionType : pImpl->availableActions) {
        float activityScore = 0.0f;
        
        // Simple heuristic: action type correlates with neuron type
        // In a real implementation, this would be based on actual neural population activity
        for (auto* neuron : neurons) {
            // Simple action selection based on neuron activity and type
            // Higher activity leads to stronger preference for corresponding actions
            float neuronActivity = neuron->getMembranePotential();
            
            // Map action types to neuron types (simplified)
            float actionAffinity = 0.0f;
            switch (actionType) {
                case ActionType::MoveForward:
                    actionAffinity = 1.0f; break;
                case ActionType::MoveBackward:
                    actionAffinity = 0.8f; break;
                case ActionType::MoveLeft:
                    actionAffinity = 0.7f; break;
                case ActionType::MoveRight:
                    actionAffinity = 0.9f; break;
                case ActionType::TurnLeft:
                    actionAffinity = 0.6f; break;
                case ActionType::TurnRight:
                    actionAffinity = 0.85f; break;
                case ActionType::Look:
                    actionAffinity = 0.5f; break;
                case ActionType::Interact:
                    actionAffinity = 0.75f; break;
                case ActionType::Wait:
                    actionAffinity = 0.3f; break;
            }
            
            // Scale activity by action affinity
            activityScore += neuronActivity * actionAffinity;
        }
        
        actionScores.emplace_back(actionType, activityScore);
    }
    
    // Apply learned preferences
    float totalWeight = 0.0f;
    for (auto& [action, score] : actionScores) {
        totalWeight += score;
    }
    
    if (totalWeight <= 0.0f) {
        // No positive activity, choose wait or random
        float rand = static_cast<float>(rand()) / RAND_MAX;
        if (rand < 0.3f) {
            return std::make_unique<Action>(ActionType::Wait);
        } else if (!pImpl->availableActions.empty()) {
            size_t idx = static_cast<size_t>(rand() * pImpl->availableActions.size()) % pImpl->availableActions.size();
            return std::make_unique<Action>(pImpl->availableActions[idx]);
        }
    }
    
    // Select action with highest score (with some randomness for exploration)
    float threshold = totalWeight * 0.6f;  // Only choose best if it's good enough
    std::vector<ActionType> eligibleActions;
    
    for (auto& [action, score] : actionScores) {
        if (score >= threshold) {
            eligibleActions.push_back(action);
        }
    }
    
    if (eligibleActions.empty()) {
        eligibleActions = pImpl->availableActions;
    }
    
    // Choose action (prefer higher scores but allow exploration)
    size_t idx = static_cast<size_t>(rand() * eligibleActions.size()) % eligibleActions.size();
    ActionType selectedAction = eligibleActions[idx];
    
    // Record action in history
    auto* action = new Action(selectedAction);
    pImpl->actionHistory.push_back(action);
    
    return std::unique_ptr<Action>(action);
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
