#include "MotorSystem.hpp"
#include <cstdlib>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<std::unique_ptr<Action>> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, ActionStats> actionStats;
    mutable std::mutex mutex;  // Add mutex for thread safety
    
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
    // Calculate neural activity from motor region
    // Use average firing rate of motor neurons as proxy for action preference
    float totalActivity = motorRegion.getAverageFiringRate();
    size_t totalNeurons = motorRegion.getTotalNeuronCount();
    
    if (totalNeurons == 0 || totalActivity <= 0.0f) {
        // No activity, return wait action
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate weighted probabilities based on neural activity and preferences
    std::vector<float> weights;
    std::vector<ActionType> eligibleActions;
    
    float totalWeight = 0.0f;
    for (ActionType action : pImpl->availableActions) {
        // Get neural activity influence (use motor region average firing rate)
        float neuralInfluence = totalActivity / 100.0f; // Normalize
        
        // Get preference weight (default to 1.0 if not set)
        float preference = pImpl->actionPreferences.count(action) ? 
            pImpl->actionPreferences[action] : 1.0f;
        
        // Get statistics influence
        const ActionStats& stats = pImpl->actionStats[action];
        float statsInfluence = std::max(0.0f, stats.averageValue);
        
        // Combine influences
        float weight = (neuralInfluence * 0.5f) + (preference * 0.3f) + (statsInfluence * 0.2f);
        
        if (weight > 0.0f) {
            weights.push_back(weight);
            eligibleActions.push_back(action);
            totalWeight += weight;
        }
    }
    
    if (eligibleActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Normalize weights
    std::vector<float> normalizedWeights;
    for (float weight : weights) {
        normalizedWeights.push_back(weight / totalWeight);
    }
    
    // Roulette wheel selection
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < weights.size(); ++i) {
        cumulative += normalizedWeights[i];
        if (random <= cumulative) {
            ActionType selectedAction = eligibleActions[i];
            
            // Create and record the action
            auto action = std::make_unique<Action>(selectedAction);
            pImpl->actionHistory.push_back(std::move(action));
            
            // Update statistics
            ActionStats& stats = pImpl->actionStats[selectedAction];
            stats.executionCount++;
            
            return std::make_unique<Action>(selectedAction);
        }
    }
    
    // Fallback
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<std::unique_ptr<Action>>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    pImpl->actionHistory.clear();
    // Reset statistics
    for (auto& pair : pImpl->actionStats) {
        ActionStats& stats = pair.second;
        stats.executionCount = 0;
        stats.totalReward = 0.0f;
        stats.averageValue = 0.0f;
    }
}

} // namespace nlm
