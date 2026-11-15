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
    // Select action based on strongest neural activation in motor region
    
    const auto& neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find neuron with highest membrane potential (activation level)
    const Neuron* bestNeuron = nullptr;
    float bestActivation = -1000.0f;
    
    for (const auto* neuron : neurons) {
        float activation = neuron->getMembranePotential();
        if (activation > bestActivation) {
            bestActivation = activation;
            bestNeuron = neuron;
        }
    }
    
    // If no neuron is above threshold, return wait
    float threshold = -60.0f;  // Typical neural threshold
    if (bestActivation < threshold) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Map best neuron to action based on its type and ID
    // For simplicity, use the motor neuron's population ID to determine action
    PopulationId population = bestNeuron->getPopulationId();
    
    // Action mapping based on population:
    // Population 0: MoveForward
    // Population 1: MoveBackward
    // Population 2: MoveLeft
    // Population 3: MoveRight
    // Population 4: TurnLeft
    // Population 5: TurnRight
    // Population 6: Look
    // Population 7: Interact
    // Others: Wait
    
    ActionType actionType = ActionType::Wait;
    switch (population.value()) {
        case 0: actionType = ActionType::MoveForward; break;
        case 1: actionType = ActionType::MoveBackward; break;
        case 2: actionType = ActionType::MoveLeft; break;
        case 3: actionType = ActionType::MoveRight; break;
        case 4: actionType = ActionType::TurnLeft; break;
        case 5: actionType = ActionType::TurnRight; break;
        case 6: actionType = ActionType::Look; break;
        case 7: actionType = ActionType::Interact; break;
        default: actionType = ActionType::Wait; break;
    }
    
    // Add action to history for reinforcement learning
    pImpl->actionHistory.push_back(new Action(actionType));
    
    return std::make_unique<Action>(actionType);
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
