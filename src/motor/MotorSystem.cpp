#include "MotorSystem.hpp"
#include <cstdlib>
#include <algorithm>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    float explorationRate;  // Probability of exploring rather than exploiting
    float temperature;      // For softmax action selection
    
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
            ActionType::Wait
        };
        explorationRate = 0.1f;
        temperature = 1.0f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // For now: implement probabilistic action selection with exploration
    
    // Get available actions (actions that have neurons with sufficient activity)
    std::vector<ActionType> candidateActions;
    const auto& neurons = motorRegion.getAllNeurons();
    
    for (const auto& neuron : neurons) {
        if (neuron->isFiring() && neuron->getFiringRate() > 0.1f) {
            // Determine action type based on neuron type (for now as placeholder)
            // In a real implementation, this would use neural activity patterns
            ActionType action = ActionType::Wait;  // Placeholder
            
            // Check if we have preferences for this action
            auto prefIt = pImpl->actionPreferences.find(action);
            if (prefIt != pImpl->actionPreferences.end() && prefIt->second > 0.5f) {
                candidateActions.push_back(action);
            }
        }
    }
    
    if (candidateActions.empty()) {
        // No active neurons, return wait action
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Exploration: randomly select from all actions
    float randomVal = static_cast<float>(rand()) / RAND_MAX;
    if (randomVal < pImpl->explorationRate) {
        size_t randomIdx = rand() % candidateActions.size();
        return std::make_unique<Action>(candidateActions[randomIdx]);
    }
    
    // Exploitation: select action based on preferences
    std::vector<std::pair<ActionType, float>> weightedActions;
    for (ActionType action : candidateActions) {
        float weight = 1.0f;  // Default weight
        auto prefIt = pImpl->actionPreferences.find(action);
        if (prefIt != pImpl->actionPreferences.end()) {
            weight = prefIt->second;
        }
        weightedActions.push_back({action, weight});
    }
    
    // Sort by weight (simple approach)
    std::sort(weightedActions.begin(), weightedActions.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select highest weight action
    return std::make_unique<Action>(weightedActions[0].first);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Clip preference to valid range
    float clippedPreference = std::clamp(preference, 0.0f, 1.0f);
    pImpl->actionPreferences[action] = clippedPreference;
    
    // Log preference change for debugging
    // In production, this would use a proper logging system
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
