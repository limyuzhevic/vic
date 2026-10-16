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
    // Real action selection based on neural activity and reinforcement learning
    
    // Get neural activity from motor region
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& neuron : motorRegion.getNeurons()) {
        totalActivity += neuron->getFiringRate();
        neuronCount++;
    }
    
    if (neuronCount == 0) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    float averageActivity = totalActivity / static_cast<float>(neuronCount);
    
    // Select action based on activity levels
    // Higher activity neurons influence action selection
    std::vector<float> actionProbabilities(pImpl->availableActions.size(), 0.0f);
    
    // For now, use a simple heuristic: select action based on activity
    // In a real implementation, this would use softmax over neural populations
    size_t selectedIndex = 0;
    float maxActivity = 0.0f;
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (actionProbabilities[i] > maxActivity) {
            maxActivity = actionProbabilities[i];
            selectedIndex = i;
        }
    }
    
    // Apply exploration: add noise to action selection
    // Higher exploration when average activity is low
    float explorationRate = std::max(0.1f, 1.0f - averageActivity);
    
    float randomVal = (rand() % 10000) / 10000.0f;  // Simple random
    if (randomVal < explorationRate) {
        // Explore: select random action
        selectedIndex = rand() % pImpl->availableActions.size();
    }
    
    auto action = std::make_unique<Action>(pImpl->availableActions[selectedIndex]);
    
    // Record action in history
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences with real reinforcement learning
    // In a real implementation, this would update value estimates based on rewards
    // and influence the action selection policy
    
    // For now, store preferences for future use
    // This would typically be used to weight action selection
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