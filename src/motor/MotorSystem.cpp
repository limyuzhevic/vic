#include "MotorSystem.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;  // Preference scores for exploration
    std::vector<float> actionRewards;      // Reward history for learning
    float explorationRate;                 // Rate of random exploration vs exploitation
    
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
        actionRewards.resize(availableActions.size(), 0.0f);
        explorationRate = 0.1f; // 10% exploration
    }
};

MotorSystem::MotorSystem() : pImpl(std::make_unique<Impl>()) {}

MotorSystem::~MotorSystem() {
    clearHistory();
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity from motor region
    const std::vector<float>& neuralActivity = motorRegion.getActivity();
    
    if (neuralActivity.empty()) {
        // If no neural activity, use preference-based selection
        return selectActionByPreference();
    }
    
    // Convert neural activity to action selection weights
    std::vector<float> actionWeights;
    actionWeights.reserve(pImpl->availableActions.size());
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        float neuralWeight = 1.0f;
        if (i < neuralActivity.size()) {
            neuralWeight = std::max(0.0f, neuralActivity[i]);
        }
        
        // Combine neural weight with preference and exploration
        float preferenceWeight = pImpl->actionPreferences[i];
        float explorationFactor = 1.0f + pImpl->explorationRate;
        
        float totalWeight = neuralWeight * preferenceWeight * explorationFactor;
        actionWeights.push_back(totalWeight);
    }
    
    // Normalize weights to create a probability distribution
    float totalWeight = 0.0f;
    for (float weight : actionWeights) {
        totalWeight += weight;
    }
    
    if (totalWeight <= 0.0f) {
        // Fallback to equal probability if all weights are zero
        std::uniform_int_distribution<size_t> dist(0, pImpl->availableActions.size() - 1);
        size_t selectedIndex = dist(std::mt19937(std::random_device()()));
        return std::make_unique<Action>(pImpl->availableActions[selectedIndex]);
    }
    
    // Select action based on weighted random choice
    float selectionThreshold = static_cast<float>(rand()) / RAND_MAX;
    float cumulativeWeight = 0.0f;
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        cumulativeWeight += actionWeights[i] / totalWeight;
        if (selectionThreshold <= cumulativeWeight) {
            // Store selected action in history
            auto* selectedAction = new Action(pImpl->availableActions[i]);
            pImpl->actionHistory.push_back(selectedAction);
            return std::unique_ptr<Action>(selectedAction);
        }
    }
    
    // Fallback to first action if selection failed
    auto* fallbackAction = new Action(pImpl->availableActions[0]);
    pImpl->actionHistory.push_back(fallbackAction);
    return std::unique_ptr<Action>(fallbackAction);
}

std::unique_ptr<Action> MotorSystem::selectActionByPreference() {
    // Select action based purely on preference scores
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find action with highest preference
    size_t bestIndex = 0;
    float bestPreference = pImpl->actionPreferences[0];
    
    for (size_t i = 1; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->actionPreferences[i] > bestPreference) {
            bestPreference = pImpl->actionPreferences[i];
            bestIndex = i;
        }
    }
    
    // Add exploration: with small probability, select random action
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(std::mt19937(std::random_device()())) < pImpl->explorationRate) {
        std::uniform_int_distribution<size_t> actionDist(0, pImpl->availableActions.size() - 1);
        size_t randomIndex = actionDist(std::mt19937(std::random_device()()));
        
        auto* selectedAction = new Action(pImpl->availableActions[randomIndex]);
        pImpl->actionHistory.push_back(selectedAction);
        return std::unique_ptr<Action>(selectedAction);
    }
    
    // Select best preference action
    auto* selectedAction = new Action(pImpl->availableActions[bestIndex]);
    pImpl->actionHistory.push_back(selectedAction);
    return std::unique_ptr<Action>(selectedAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Validate preference
    float clampedPreference = std::max(0.0f, std::min(preference, 10.0f));
    
    // Find the action in the list and update its preference
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            pImpl->actionPreferences[i] = clampedPreference;
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

void MotorSystem::recordActionResult(ActionType action, float reward) {
    // Find the action in history and update its reward
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            pImpl->actionRewards[i] = reward;
            break;
        }
    }
}

float MotorSystem::getActionValue(ActionType action) const {
    // Calculate action value based on average reward
    float totalValue = 0.0f;
    int count = 0;
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            totalValue += pImpl->actionRewards[i];
            count++;
            break;
        }
    }
    
    return count > 0 ? totalValue / count : 0.0f;
}

void MotorSystem::updatePreferencesFromHistory() {
    // Update preferences based on reward history (actor-critic style)
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        float currentPreference = pImpl->actionPreferences[i];
        float reward = pImpl->actionRewards[i];
        
        // Apply reinforcement learning update rule
        float learningRate = 0.1f;
        float preferenceChange = learningRate * (reward - currentPreference);
        
        pImpl->actionPreferences[i] += preferenceChange;
        
        // Keep preferences in reasonable bounds
        pImpl->actionPreferences[i] = std::max(0.0f, std::min(pImpl->actionPreferences[i], 10.0f));
    }
}

} // namespace nlm
