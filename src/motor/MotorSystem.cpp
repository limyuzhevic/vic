#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<std::pair<ActionType, float>> actionPreferences;
    std::mt19937 rng;
    
    Impl() : rng(std::random_device{}()) {
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
        
        // Initialize preferences
        actionPreferences.reserve(availableActions.size());
        for (const auto& action : availableActions) {
            actionPreferences.emplace_back(action, 1.0f);
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Simple action selection based on neural activity
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity scores for each action type
    std::vector<std::pair<ActionType, float>> actionScores;
    
    // Group neurons by activity type (sensory/motor patterns)
    std::vector<Neuron*> motorNeurons = neurons;
    
    // Calculate average membrane potential deviation as activity measure
    float totalActivity = 0.0f;
    for (const auto& neuron : neurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        totalActivity += activity;
    }
    
    float avgActivity = totalActivity / neurons.size();
    
    // Strategy 1: High activity -> exploratory action
    if (avgActivity > 5.0f) {
        // Select action based on preferences with some randomness
        std::vector<ActionType> possibleActions;
        float totalPref = 0.0f;
        
        for (const auto& pair : pImpl->actionPreferences) {
            if (pair.second > 0.1f) {  // Only consider actions with reasonable preference
                possibleActions.push_back(pair.first);
                totalPref += pair.second;
            }
        }
        
        if (!possibleActions.empty() && totalPref > 0.0f) {
            // Weighted random selection
            std::uniform_real_distribution<float> dist(0.0f, totalPref);
            float choice = dist(pImpl->rng);
            float sum = 0.0f;
            for (const auto& action : possibleActions) {
                for (const auto& pref : pImpl->actionPreferences) {
                    if (pref.first == action) {
                        sum += pref.second;
                        if (sum >= choice) {
                            return std::make_unique<Action>(action);
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Strategy 2: Normal activity -> based on active neuron patterns
    // Count firing neurons in different groups
    struct ActionGroup {
        ActionType action;
        float score;
    };
    
    std::vector<ActionGroup> actionScores;
    
    // Default to waiting if low activity
    if (avgActivity < 1.0f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // For now, return a default action based on activity level
    // Phase 2 will implement more sophisticated strategies
    if (avgActivity > 2.0f) {
        // Some movement
        std::uniform_int_distribution<int> dist(0, 6);
        int choice = dist(pImpl->rng);
        switch (choice) {
            case 0: return std::make_unique<Action>(ActionType::MoveForward);
            case 1: return std::make_unique<Action>(ActionType::MoveBackward);
            case 2: return std::make_unique<Action>(ActionType::TurnLeft);
            case 3: return std::make_unique<Action>(ActionType::TurnRight);
            case 4: return std::make_unique<Action>(ActionType::Interact);
            default: return std::make_unique<Action>(ActionType::Wait);
        }
    }
    
    // Intermediate activity
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Find and update preference
    for (auto& pair : pImpl->actionPreferences) {
        if (pair.first == action) {
            pair.second = std::max(0.0f, std::min(2.0f, preference));  // Clamp to [0, 2]
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

float MotorSystem::calculateActionScore(const ActionType& action, const NeuralRegion& motorRegion) const {
    // Simple scoring based on brain state
    float score = 1.0f;
    
    // Apply preference lookup
    for (const auto& pref : pImpl->actionPreferences) {
        if (pref.first == action) {
            score *= pref.second;
            break;
        }
    }
    
    return score;
}

} // namespace nlm
