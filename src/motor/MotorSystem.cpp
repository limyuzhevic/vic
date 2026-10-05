#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;
    std::mt19937 randomEngine;
    std::chrono::steady_clock::time_point lastSelectionTime;
    float explorationRate;
    float exploitationWeight;
    float temporalDiscount;
    
    Impl() {
        availableActions = {
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
            ActionType::Wait,
            ActionType::Custom
        };
        
        actionPreferences.resize(availableActions.size(), 1.0f);
        randomEngine = std::mt19937(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        lastSelectionTime = std::chrono::steady_clock::now();
        explorationRate = 0.1f; // Start with 10% exploration
        exploitationWeight = 0.9f;
        temporalDiscount = 0.99f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        // Improved random selection with exploration/exploitation
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        float explorationChance = pImpl->explorationRate;
        
        if (dist(pImpl->randomEngine) < explorationChance) {
            // Exploration: choose random action
            std::uniform_int_distribution<size_t> actionDist(0, pImpl->availableActions.size() - 1);
            size_t idx = actionDist(pImpl->randomEngine);
            return std::make_unique<Action>(pImpl->availableActions[idx]);
        } else {
            // Exploitation: choose based on preferences
            // For now, still just pick first available
            return std::make_unique<Action>(pImpl->availableActions[0]);
        }
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Find the action in availableActions
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            pImpl->actionPreferences[i] = std::clamp(preference, 0.0f, 1.0f);
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
    
    // Reset preferences to uniform
    std::fill(pImpl->actionPreferences.begin(), pImpl->actionPreferences.end(), 1.0f);
    pImpl->explorationRate = 0.1f;
    pImpl->exploitationWeight = 0.9f;
}

} // namespace nlm
