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
    
    // Enhanced action selection with neural integration:
    
    // Get neural activity from motor region
    float maxActivity = 0.0f;
    ActionType selectedAction = ActionType::Wait;
    
    for (size_t i = 0; i < motorRegion.neuronCount; ++i) {
        const auto& neuron = motorRegion.neurons[i];
        float activity = neuron.getActivityLevel();
        
        if (activity > maxActivity) {
            maxActivity = activity;
            
            // Map neuron index to action type
            ActionType action = mapNeuronToAction(i, motorRegion.neuronCount);
            selectedAction = action;
        }
    }
    
    // Apply action preferences based on history
    selectedAction = applyActionPreferences(selectedAction, motorRegion);
    
    // Apply neuromodulation influence
    selectedAction = applyNeuromodulationInfluence(selectedAction, motorRegion);
    
    // Create and return action
    auto action = std::make_unique<Action>(selectedAction);
    
    // Apply action costs and benefits
    applyActionCostsBenefits(action.get(), motorRegion);
    
    // Store in history
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    
    // Enhanced action preferences with multiple factors:
    
    // Store preference
    pImpl->actionHistory.push_back(std::make_unique<Action>(action).release());
    
    // Apply temporal discounting
    applyTemporalDiscounting(action, preference);
    
    // Apply habit formation
    applyHabitFormation(action, preference);
    
    // Apply value-based learning
    applyValueBasedLearning(action, preference);
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
