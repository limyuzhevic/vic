#include "MotorSystem.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;
    
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
        actionPreferences.resize(availableActions.size(), 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity
    // Uses motor neuron population activity to determine action
    
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get all neurons from the motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        // Fall back to preference-based selection
        return selectActionByPreference();
    }
    
    // Calculate activity of each motor neuron group
    float maxActivity = 0.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        ActionType action = pImpl->availableActions[i];
        
        // Find neurons corresponding to this action (simplified mapping)
        float activity = calculateActionActivity(neurons, action);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            bestAction = action;
        }
    }
    
    // Return action with highest activity
    auto action = std::make_unique<Action>(bestAction);
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

ActionType MotorSystem::selectActionByPreference() const {
    // Select action based on learned preferences
    if (pImpl->availableActions.empty()) {
        return ActionType::Wait;
    }
    
    // Find action with highest preference
    size_t bestIndex = 0;
    float bestPreference = pImpl->actionPreferences[0];
    
    for (size_t i = 1; i < pImpl->actionPreferences.size(); ++i) {
        if (pImpl->actionPreferences[i] > bestPreference) {
            bestPreference = pImpl->actionPreferences[i];
            bestIndex = i;
        }
    }
    
    return pImpl->availableActions[bestIndex];
}

float MotorSystem::calculateActionActivity(const std::vector<Neuron*>& neurons, ActionType action) const {
    // Calculate neural activity for a specific action
    // This would normally map actions to specific motor neuron populations
    
    if (neurons.empty()) return 0.0f;
    
    // Simple mapping based on action type
    // In a real implementation, this would be more sophisticated
    float totalActivity = 0.0f;
    
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        // Get neuron activity based on membrane potential
        float activity = neuron->getMembranePotential();
        
        // Apply action-specific weights
        if (action == ActionType::MoveForward || action == ActionType::MoveBackward) {
            activity *= 1.2f; // Motor actions have higher weight
        } else if (action == ActionType::TurnLeft || action == ActionType::TurnRight) {
            activity *= 1.1f; // Turning actions
        }
        
        totalActivity += activity;
    }
    
    // Normalize by number of neurons
    return totalActivity / neurons.size();
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set preference for a specific action
    auto it = std::find(pImpl->availableActions.begin(), 
                       pImpl->availableActions.end(), action);
    if (it != pImpl->availableActions.end()) {
        size_t index = std::distance(pImpl->availableActions.begin(), it);
        pImpl->actionPreferences[index] = std::clamp(preference, 0.0f, 10.0f);
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
