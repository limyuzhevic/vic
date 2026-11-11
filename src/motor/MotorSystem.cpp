#include "MotorSystem.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionStatistics;
    
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
        actionStatistics.resize(static_cast<size_t>(ActionType::Count), 0.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(std::make_unique<Impl>()) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // REAL IMPLEMENTATION: Select action based on neural activity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Get neural firing statistics from motor region
    float firingStrength = motorRegion.getFiringNeuronCount();
    float maxFiring = motorRegion.getMaxFiring();
    
    // Select action based on neural activity
    if (!pImpl->availableActions.empty() && firingStrength > 0.0f) {
        // Use neural firing to determine which action to select
        size_t actionIndex = std::min<size_t>(firingStrength % pImpl->availableActions.size(), pImpl->availableActions.size() - 1);
        ActionType selectedAction = pImpl->availableActions[actionIndex];
        
        // Create action with neural-based confidence
        float confidence = maxFiring > 0.0f ? (firingStrength / maxFiring) : 0.1f;
        auto action = std::make_unique<Action>(selectedAction);
        action->setConfidence(confidence);
        
        // Add to history
        pImpl->actionHistory.push_back(action.release());
        
        // Update statistics
        size_t actionIndex = static_cast<size_t>(selectedAction);
        if (actionIndex < pImpl->actionStatistics.size()) {
            pImpl->actionStatistics[actionIndex] += confidence;
        }
        
        std::cout << "[Motor] Selected action " << static_cast<int>(selectedAction) 
                  << " with confidence " << confidence << " based on neural activity " << firingStrength << std::endl;
        
        return action;
    }
    
    // Fallback: return random action if no neural activity
    if (!pImpl->availableActions.empty()) {
        std::uniform_int_distribution<size_t> dist(0, pImpl->availableActions.size() - 1);
        size_t actionIndex = dist(gen);
        ActionType selectedAction = pImpl->availableActions[actionIndex];
        
        auto action = std::make_unique<Action>(selectedAction);
        action->setConfidence(0.1f);  // Low confidence for random action
        
        pImpl->actionHistory.push_back(action.release());
        pImpl->actionStatistics[actionIndex] += 0.1f;
        
        std::cout << "[Motor] Selected random action " << static_cast<int>(selectedAction) 
                  << " due to no neural activity" << std::endl;
        
        return action;
    }
    
    // Ultimate fallback
    auto action = std::make_unique<Action>(ActionType::Wait);
    action->setConfidence(0.0f);
    pImpl->actionHistory.push_back(action.release());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // REAL IMPLEMENTATION: Implement action preferences
    if (static_cast<size_t>(action) < pImpl->actionStatistics.size()) {
        pImpl->actionStatistics[static_cast<size_t>(action)] = preference;
        std::cout << "[Motor] Set preference for action " << static_cast<int>(action) 
                  << " to " << preference << std::endl;
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

const std::vector<float>& MotorSystem::getActionStatistics() const {
    return pImpl->actionStatistics;
}

} // namespace nlm
