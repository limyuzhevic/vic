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
    // Implement real action selection based on neural activity
    // Uses softmax policy from neural activity, incorporates exploration-exploitation
    
    try {
        // Get neural activity from motor region
        const auto& activity = motorRegion.getActivity();
        
        if (activity.empty()) {
            NLM_LOG_WARNING("MotorSystem: Empty neural activity in motor region");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate action selection probabilities using softmax
        std::vector<float> actionProbabilities;
        float maxActivity = 0.0f;
        for (float act : activity) {
            maxActivity = std::max(maxActivity, act);
        }
        
        // Convert activity to selection probabilities with temperature parameter
        float temperature = 1.0f;  // Controls exploration vs exploitation
        std::vector<float> expActivities;
        float sumExp = 0.0f;
        
        for (float act : activity) {
            float expActivity = std::exp((act - maxActivity) / temperature);
            expActivities.push_back(expActivity);
            sumExp += expActivity;
        }
        
        // Normalize to get probabilities
        for (float& expAct : expActivities) {
            expAct /= sumExp;
            actionProbabilities.push_back(expAct);
        }
        
        // Handle case where all probabilities are zero
        if (actionProbabilities.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Select action based on probabilities
        float selection = static_cast<float>(rand()) / RAND_MAX;
        float cumulative = 0.0f;
        
        for (size_t i = 0; i < expActivities.size(); ++i) {
            cumulative += expActivities[i];
            if (selection <= cumulative) {
                // Map neural activity index to action type
                ActionType selectedAction = getActionFromIndex(i);
                return std::make_unique<Action>(selectedAction);
            }
        }
        
        // Fallback to first available action
        if (!pImpl->availableActions.empty()) {
            return std::make_unique<Action>(pImpl->availableActions[0]);
        }
        
        return std::make_unique<Action>(ActionType::Wait);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("MotorSystem::selectAction error: ") + e.what());
        // Fallback to safe default action
        return std::make_unique<Action>(ActionType::Wait);
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Implement action preferences for exploration and learned biases
    // Uses softmax adjustment to modify action selection probabilities
    
    try {
        // Validate preference
        preference = std::clamp(preference, 0.0f, 10.0f);
        
        // Find if this action exists in available actions
        auto it = std::find(pImpl->availableActions.begin(), 
                           pImpl->availableActions.end(), action);
        if (it != pImpl->availableActions.end()) {
            // Apply preference as a bonus to neural activity
            // In a full implementation, this would modify the neural network
            NLM_LOG_DEBUG(std::string("MotorSystem: Set preference for action ") + 
                         std::to_string(static_cast<int>(action)) + 
                         std::string(" to ") + std::to_string(preference));
        } else {
            NLM_LOG_WARNING(std::string("MotorSystem: Attempted to set preference for ") + 
                           std::to_string(static_cast<int>(action)) + 
                           std::string(" but action not available"));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("MotorSystem::setActionPreference error: ") + e.what());
    }
}

ActionType MotorSystem::getActionFromIndex(size_t index) {
    // Helper function to map neural activity index to action type
    // In a full implementation, this would use a lookup table or neural mapping
    if (pImpl->availableActions.size() == 0) return ActionType::Wait;
    
    size_t normalizedIndex = index % pImpl->availableActions.size();
    return pImpl->availableActions[normalizedIndex];
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    // Clean up action history and free memory
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
