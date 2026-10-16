#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::map<ActionType, float> actionPreferences;
    
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
    if (!pImpl->availableActions.empty()) {
        // Simple action selection based on neural activity
        // Count firing neurons by population type
        std::map<ActionType, float> actionScores;
        
        // Get all neurons from the motor region
        auto neurons = motorRegion.getAllNeurons();
        
        // For each action, compute score based on associated neuron activity
        for (ActionType action : pImpl->availableActions) {
            // Map action types to neuron populations for scoring
            float score = 0.0f;
            
            // Get neurons based on action type
            for (const auto& neuron : neurons) {
                // Simple heuristic: assign different neuron populations to different actions
                // In a real system, this would be more sophisticated with dedicated action selection circuits
                if (action == ActionType::MoveForward && neuron->getPopulationId() == 0) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::MoveBackward && neuron->getPopulationId() == 1) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::MoveLeft && neuron->getPopulationId() == 2) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::MoveRight && neuron->getPopulationId() == 3) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::TurnLeft && neuron->getPopulationId() == 4) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::TurnRight && neuron->getPopulationId() == 5) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::Look && neuron->getPopulationId() == 6) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::Interact && neuron->getPopulationId() == 7) {
                    score += neuron->getFiringRate();
                } else if (action == ActionType::Wait) {
                    // Wait action gets baseline activity
                    score += 0.1f;
                }
            }
            
            // Apply softmax to get probabilities
            actionScores[action] = std::exp(score);
        }
        
        // Normalize to probabilities
        float totalScore = 0.0f;
        for (const auto& pair : actionScores) {
            totalScore += pair.second;
        }
        
        if (totalScore > 0.0f) {
            // Select action with highest score (simplified argmax)
            ActionType selectedAction = pImpl->availableActions[0];
            float maxScore = 0.0f;
            
            for (const auto& pair : actionScores) {
                if (pair.second > maxScore) {
                    maxScore = pair.second;
                    selectedAction = pair.first;
                }
            }
            
            // Create action with optional parameters
            std::vector<float> params;
            if (selectedAction == ActionType::MoveForward || 
                selectedAction == ActionType::MoveBackward) {
                // Add direction parameters
                params.push_back(1.0f);  // speed
                params.push_back(0.0f);  // angle (simplified)
            }
            
            auto action = std::make_unique<Action>(selectedAction, params);
            
            // Add to action history
            pImpl->actionHistory.push_back(action.get());
            
            return action;
        }
    }
    
    // Fallback to wait action
    auto action = std::make_unique<Action>(ActionType::Wait);
    pImpl->actionHistory.push_back(action.get());
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // PLACEHOLDER: Currently not implemented
    // In a real implementation, this would store preferences and influence action selection
    // Preferences could be based on:
    // - Reward history
    // - Neural activity patterns  
    // - Internal drive states
    // - Environmental constraints
    
    // For now, store preferences in a map for future use
    pImpl->actionPreferences[action] = preference;
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
