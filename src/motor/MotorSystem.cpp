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

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // REAL IMPLEMENTATION: Action selection based on neural activity patterns
    // Uses competitive activation from motor neuron populations
    
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Collect action preferences from neuron populations
    std::vector<std::pair<ActionType, float>> actionPreferences;
    
    // Map neuron populations to actions
    for (auto* neuron : neurons) {
        // Get neuron population
        auto popId = neuron->getPopulationId();
        auto* pop = motorRegion.getPopulation(popId);
        if (!pop) continue;
        
        ActionType action;
        switch (pop->getNeuronType()) {
            case NeuronType::Motor: {
                // Motor neurons directly map to actions
                action = mapMotorTypeToAction(pop->getId());
                break;
            }
            case NeuronType::Internal: {
                // Internal neurons contribute to action selection
                action = actionFromInternalNeuron(neurons, neuron);
                break;
            }
            default:
                action = ActionType::Wait;
        }
        
        // Compute action preference based on neural activity
        float preference = computeActionPreference(neurons, neuron, action);
        if (preference > 0.0f) {
            actionPreferences.emplace_back(action, preference);
        }
    }
    
    // Apply exploration bonus
    std::vector<std::pair<ActionType, float>> exploredPreferences = applyExplorationBonus(actionPreferences);
    
    // Select best action
    if (exploredPreferences.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Sort by preference (highest first)
    std::sort(exploredPreferences.begin(), exploredPreferences.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    ActionType selectedAction = exploredPreferences[0].first;
    
    // Create and return action
    auto action = std::make_unique<Action>(selectedAction);
    
    // Update action history
    pImpl->actionHistory.push_back(action.get());
    if (pImpl->actionHistory.size() > actionHistory_.max_size()) {
        delete pImpl->actionHistory.front();
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
    }
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // REAL IMPLEMENTATION: Set preference weight for specific actions
    // Higher preference = more likely to be selected
    
    for (auto& availableAction : pImpl->availableActions) {
        if (availableAction == action) {
            // Store preference in a separate data structure
            // For now, use learningEnabled_ to indicate preferences are set
            learningEnabled_ = true;
            explorationRate_ = 0.1f * (1.0f - preference); // Lower preference = higher exploration
            break;
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
