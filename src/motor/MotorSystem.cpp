#include "MotorSystem.hpp"
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    
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
    // Implement real action selection based on neural activity
    // Use neural firing rates and synaptic weights to determine action probability
    
    std::vector<std::pair<ActionType, float>> actionProbabilities;
    
    for (ActionType actionType : pImpl->availableActions) {
        float probability = 0.0f;
        
        // Calculate action probability based on neural activity
        // Get total neural activity for this action type
        float neuralActivity = 0.0f;
        
        // Find the action population in the neural region
        for (const auto& pop : motorRegion.getPopulations()) {
            // Check if this population corresponds to the action type
            // This is a simplified mapping - in a real implementation,
            // there would be explicit action population tagging
            if (pop->getNeuronCount() > 0) {
                float firingRate = pop->getAverageFiringRate();
                neuralActivity += firingRate;
            }
        }
        
        // Convert neural activity to probability using softmax
        if (neuralActivity > 0.0f) {
            probability = std::exp(10.0f * neuralActivity) / (1.0f + std::exp(10.0f * neuralActivity));
        }
        
        // Apply action preferences if set
        auto it = pImpl->actionPreferences.find(actionType);
        if (it != pImpl->actionPreferences.end()) {
            probability *= it->second;
        }
        
        if (probability > 0.01f) {
            actionProbabilities.emplace_back(actionType, probability);
        }
    }
    
    // Select action based on probabilities
    if (actionProbabilities.empty()) {
        // Fallback to random action if no activity detected
        if (!pImpl->availableActions.empty()) {
            size_t idx = (std::rand() % pImpl->availableActions.size());
            return std::make_unique<Action>(pImpl->availableActions[idx]);
        }
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Normalize probabilities
    float totalProb = 0.0f;
    for (const auto& pair : actionProbabilities) {
        totalProb += pair.second;
    }
    
    float randomValue = static_cast<float>(std::rand()) / RAND_MAX * totalProb;
    float cumulativeProb = 0.0f;
    
    for (const auto& pair : actionProbabilities) {
        cumulativeProb += pair.second;
        if (randomValue <= cumulativeProb) {
            // Create action with neural activity as modifier
            auto action = std::make_unique<Action>(pair.first);
            // Store neural activity for future reference
            action->setParameters(std::vector<float>{neuralActivity, cumulativeProb});
            return action;
        }
    }
    
    // Should not reach here, but fallback to wait action
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = preference;
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

// Add missing getActionPreferences() method
const std::unordered_map<ActionType, float>& MotorSystem::getActionPreferences() const {
    return pImpl->actionPreferences;
}

} // namespace nlm
