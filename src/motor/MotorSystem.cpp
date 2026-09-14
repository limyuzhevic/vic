#include "MotorSystem.hpp"
#include <algorithm>
#include <random>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    float explorationRate;
    
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
        explorationRate = 0.1f;
    }
};

#include "MotorSystem.hpp"
#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    float explorationRate;
    mutable std::mt19937 rng;
    
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
        explorationRate = 0.1f;
        rng.seed(std::random_device()());
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity from motor region
    float totalActivity = 0.0f;
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType actionType : pImpl->availableActions) {
        // Simple heuristic: Get activity from motor region and convert to action preference
        float activity = 0.0f;
        size_t neuronCount = motorRegion.getTotalNeuronCount();
        if (neuronCount > 0) {
            // Calculate activity based on region statistics
            activity = motorRegion.getAverageFiringRate() * 
                      (1.0f + std::sin(static_cast<float>(actionType) * 0.5f)) * 0.5f;
        }
        
        // Apply action preference
        float preference = 1.0f;
        auto prefIt = pImpl->actionPreferences.find(actionType);
        if (prefIt != pImpl->actionPreferences.end()) {
            preference = prefIt->second;
        }
        
        // Combine neural activity and preference
        float score = activity * preference;
        
        // Apply exploration
        if (std::rand() / (RAND_MAX + 1.0) < pImpl->explorationRate) {
            score = 1.0f - score; // Randomize selection
        }
        
        actionScores.push_back({actionType, score});
        totalActivity += activity;
    }
    
    // Select best action based on scores
    float maxScore = -1.0f;
    ActionType selectedAction = ActionType::Wait;
    
    for (const auto& pair : actionScores) {
        if (pair.second > maxScore) {
            maxScore = pair.second;
            selectedAction = pair.first;
        }
    }
    
    // Store in history
    auto action = std::make_unique<Action>(selectedAction);
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
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
