#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    // Maps action to preference value for exploration/exploitation
    std::unordered_map<ActionType, float> actionPreferences;
    // Previous motor region for continuity
    NeuralRegion* previousMotorRegion;
    // Statistics
    size_t totalActionsSelected;
    size_t totalDecisionCycles;
    
    Impl() : availableActions({
        ActionType::MoveForward,
        ActionType::MoveBackward,
        ActionType::MoveLeft,
        ActionType::MoveRight,
        ActionType::TurnLeft,
        ActionType::TurnRight,
        ActionType::Look,
        ActionType::LookUp,
        ActionType::LookDown,
        ActionType::Interact,
        ActionType::Eat,
        ActionType::Drink,
        ActionType::Rest,
        ActionType::Wait
    }), previousMotorRegion(nullptr), totalActionsSelected(0), totalDecisionCycles(0) {
        // Initialize preferences - higher for basic actions
        for (ActionType action : availableActions) {
            if (action == ActionType::Wait || action == ActionType::Rest) {
                actionPreferences[action] = 0.1f;  // Low preference for rest
            } else if (action == ActionType::Interact || action == ActionType::Eat || action == ActionType::Drink) {
                actionPreferences[action] = 0.5f;  // Medium preference
            } else {
                actionPreferences[action] = 1.0f;  // High preference for movement
            }
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    
    // Update preferences based on previous motor region activity
    if (pImpl->previousMotorRegion && pImpl->totalDecisionCycles > 0) {
        // Analyze motor neuron activity patterns in the previous region
        std::vector<const Neuron*> allNeurons = pImpl->previousMotorRegion->getAllNeurons();
        if (!allNeurons.empty()) {
            // Calculate activity-weighted action preferences
            std::unordered_map<ActionType, float> activityWeights;
            
            for (const auto& action : pImpl->availableActions) {
                activityWeights[action] = pImpl->actionPreferences[action];
            }
            
            // Boost preference for actions that correlate with motor neuron firing
            for (const auto* neuron : allNeurons) {
                if (neuron->isFiring() && neuron->getType() == NeuronType::Motor) {
                    // Motor neuron firing increases preference for related actions
                    // This is simplified - real implementation would need action-to-neuron mapping
                    float activity = std::min(1.0f, neuron->getState().membranePotential / 50.0f);
                    
                    // Randomly assign action preference (simplified)
                    if (!pImpl->availableActions.empty()) {
                        ActionType chosenAction = pImpl->availableActions[rand() % pImpl->availableActions.size()];
                        activityWeights[chosenAction] += activity * 0.5f;
                    }
                }
            }
            
            // Update preferences
            pImpl->actionPreferences = activityWeights;
        }
    }
    
    // Update previous region
    pImpl->previousMotorRegion = &motorRegion;
    
    // Select action based on preferences (weighted random selection)
    float totalWeight = 0.0f;
    for (const auto& pair : pImpl->actionPreferences) {
        totalWeight += pair.second;
    }
    
    if (totalWeight <= 0.0f) {
        // All weights zero or invalid, select random action
        ActionType selectedType = pImpl->availableActions[rand() % pImpl->availableActions.size()];
        pImpl->totalActionsSelected++;
        return std::make_unique<Action>(selectedType);
    }
    
    // Weighted random selection
    float randomValue = static_cast<float>(rand()) / RAND_MAX * totalWeight;
    float cumulativeWeight = 0.0f;
    
    ActionType selectedType = ActionType::Wait; // Default
    for (const auto& pair : pImpl->actionPreferences) {
        cumulativeWeight += pair.second;
        if (randomValue <= cumulativeWeight) {
            selectedType = pair.first;
            break;
        }
    }
    
    pImpl->totalActionsSelected++;
    pImpl->totalDecisionCycles++;
    
    return std::make_unique<Action>(selectedType);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // Clamp preference to valid range [0.0, 1.0]
    float clampedPreference = std::clamp(preference, 0.0f, 1.0f);
    
    // Find action in available actions
    auto it = std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action);
    if (it != pImpl->availableActions.end()) {
        pImpl->actionPreferences[action] = clampedPreference;
    }
    
    // Also set preference in the actual vector if it's different
    if (it != pImpl->availableActions.end()) {
        // Ensure we don't duplicate actions
        for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
            if (pImpl->availableActions[i] == action) {
                // This maintains action order while updating preference
                break;
            }
        }
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
