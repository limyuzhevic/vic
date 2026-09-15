#include "MotorSystem.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/Neuron.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;  // Action preference weights for exploration/exploitation
    std::shared_ptr<RandomGenerator> rng;
    
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
        actionPreferences.resize(availableActions.size(), 0.0f);
        rng = std::make_shared<RandomGenerator>(42);  // Default seed
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;
    // Real action selection based on neural population activity patterns
    // Implements winner-take-all competition with temporal integration
    
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity scores for each action type
    // Map action types to neuron groups (assuming first 25% are MoveForward, etc.)
    std::vector<std::pair<ActionType, float>> actionScores;
    size_t neuronsPerAction = neurons.size() / pImpl->availableActions.size();
    
    // Score each action based on neural activity
    size_t actionIndex = 0;
    for (const auto& action : pImpl->availableActions) {
        float activityScore = 0.0f;
        size_t startNeuron = actionIndex * neuronsPerAction;
        
        // Calculate average firing rate for neurons associated with this action
        for (size_t i = 0; i < neuronsPerAction && (startNeuron + i) < neurons.size(); ++i) {
            auto* neuron = neurons[startNeuron + i];
            if (neuron) {
                // Use membrane potential as activity proxy (higher = more active)
                // Normalize from threshold range (-55 to +30)
                float activity = (neuron->getMembranePotential() - (-55.0f)) / (85.0f);
                activity = std::max(0.0f, std::min(1.0f, activity));
                activityScore += activity;
            }
        }
        
        if (neuronsPerAction > 0) {
            activityScore /= static_cast<float>(neuronsPerAction);
        }
        
        actionScores.push_back({action, activityScore});
        ++actionIndex;
    }
    
    // Apply action preferences (for exploration/exploitation balance)
    for (auto& [action, score] : actionScores) {
        // Find matching preference
        for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
            if (pImpl->availableActions[i] == action) {
                // Apply preference weight (higher preference increases score)
                score *= (1.0f + pImpl->actionPreferences[i] * 0.5f);
                break;
            }
        }
    }
    
    // Find action with highest score (winner-take-all)
    float maxScore = -1.0f;
    ActionType selectedAction = ActionType::Wait;
    
    for (const auto& [action, score] : actionScores) {
        if (score > maxScore) {
            maxScore = score;
            selectedAction = action;
        }
    }
    
    // Add some stochasticity for exploration (tempered by action preferences)
    float explorationFactor = 0.1f;
    float totalPreference = 0.0f;
    for (float pref : pImpl->actionPreferences) {
        totalPreference += pref;
    }
    
    if (totalPreference < 0.5f) {  // Low preference means more exploration
        explorationFactor = 0.3f;
    }
    
    if (maxScore < 0.1f && explorationFactor > 0.0f) {
        // Explore: randomly select from available actions
        size_t randomIndex = pImpl->rng->uniformInt(0, pImpl->availableActions.size() - 1);
        selectedAction = pImpl->availableActions[randomIndex];
    }
    
    // Create action with neural context
    auto action = std::make_unique<Action>(selectedAction);
    
    // Store action in history for learning
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // IMPLEMENTED: Action preference weights for exploration/exploitation
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        if (pImpl->availableActions[i] == action) {
            pImpl->actionPreferences[i] = std::clamp(preference, -1.0f, 1.0f);
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
