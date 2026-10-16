#include "MotorSystem.hpp"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action preferences for exploration/exploitation
    std::map<ActionType, float> actionPreferences;
    
    // Action selection parameters
    float explorationRate;
    float temperature;  // for softmax action selection
    
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
        
        // Initialize uniform preferences
        for (ActionType action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
        
        explorationRate = 0.1f;
        temperature = 1.0f;
        
        // Seed random number generator
        srand(static_cast<unsigned int>(time(0)));
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const class NeuralRegion& motorRegion) {
    // Real action selection based on neural activity and preferences
    // Implements a sophisticated action selection mechanism with exploration/exploitation balance
    
    // Calculate action values based on neural activity
    std::vector<std::pair<ActionType, float>> actionValues;
    
    for (ActionType action : pImpl->availableActions) {
        float actionValue = calculateActionValue(motorRegion, action);
        actionValues.push_back({action, actionValue});
    }
    
    // Apply exploration-exploitation balance using softmax
    float totalValue = 0.0f;
    for (const auto& pair : actionValues) {
        totalValue += std::exp(pair.second / pImpl->temperature);
    }
    
    if (totalValue < 1e-6f) {
        // Fallback: random action if all values are very small
        if (!pImpl->availableActions.empty()) {
            ActionType randomAction = pImpl->availableActions[rand() % pImpl->availableActions.size()];
            return std::make_unique<Action>(randomAction);
        }
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Sample action using softmax distribution
    float randomValue = static_cast<float>(rand()) / RAND_MAX * totalValue;
    float cumulativeValue = 0.0f;
    ActionType selectedAction;
    
    for (const auto& pair : actionValues) {
        cumulativeValue += std::exp(pair.second / pImpl->temperature);
        if (randomValue <= cumulativeValue) {
            selectedAction = pair.first;
            break;
        }
    }
    
    return std::make_unique<Action>(selectedAction);
}

float MotorSystem::calculateActionValue(const class NeuralRegion& motorRegion, ActionType action) {
    // Calculate action value based on neural activity and action preferences
    
    float actionValue = 0.0f;
    
    // Neural activity contribution
    // Get neural activity for the motor region
    auto activity = motorRegion.getAverageActivity();
    
    // Map neural activity to action suitability
    if (action == ActionType::MoveForward) {
        // Forward movement associated with positive neural activity in certain regions
        actionValue += activity * 0.5f;
    } else if (action == ActionType::MoveBackward) {
        // Backward movement associated with negative neural activity
        actionValue += (1.0f - activity) * 0.5f;
    } else if (action == ActionType::TurnLeft) {
        // Left turns associated with specific neural patterns
        actionValue += activity * 0.3f + (1.0f - activity) * 0.2f;
    } else if (action == ActionType::TurnRight) {
        // Right turns associated with complementary neural patterns
        actionValue += (1.0f - activity) * 0.3f + activity * 0.2f;
    } else {
        // Other actions get baseline value
        actionValue += activity * 0.1f;
    }
    
    // Action preference contribution
    float preference = getActionPreference(action);
    actionValue += preference * 0.5f;
    
    // Neuromodulatory influence (dopamine, norepinephrine, etc.)
    float neuromodulatoryInfluence = 1.0f;
    // Could integrate with Neuromodulator system here
    actionValue *= neuromodulatoryInfluence;
    
    return actionValue;
}

float MotorSystem::getActionPreference(ActionType action) const {
    // Get action preference for exploration
    auto it = pImpl->actionPreferences.find(action);
    if (it != pImpl->actionPreferences.end()) {
        return it->second;
    }
    
    return 1.0f;  // Default preference
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set action preference for exploration/exploitation trade-off
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 2.0f);
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

void MotorSystem::recordAction(const Action& action) {
    // Record action in history for learning and analysis
    pImpl->actionHistory.push_back(&action);
}

} // namespace nlm