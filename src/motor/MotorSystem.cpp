#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::map<ActionType, float> actionPreferences;
    std::mt19937 rng;
    
    Impl() : rng(std::random_device()()) {
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
        
        // Initialize with equal preferences
        for (auto action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

// Select action based on neural activity and preferences
std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity from motor region
    float maxActivity = 0.0f;
    ActionType selectedAction = ActionType::Wait;
    
    for (const auto& population : motorRegion.getPopulations()) {
        for (const Neuron* neuron : population->getNeurons()) {
            float activity = neuron->getMembranePotential();
            
            // Map neural activity to action categories
            // Group neurons by functional role based on population type
            NeuronType type = population->getNeuronType();
            
            ActionType action = mapNeuralActivityToAction(type, activity, maxActivity);
            
            if (activity > maxActivity) {
                maxActivity = activity;
                selectedAction = action;
            }
        }
    }
    
    // Apply action preferences and curiosity factors
    selectedAction = applyActionPreferences(selectedAction, maxActivity);
    
    // Create action with appropriate parameters
    auto action = std::make_unique<Action>(selectedAction);
    
    // Add neural context parameters
    addNeuralContextParameters(*action, motorRegion, maxActivity);
    
    // Add to history
    pImpl->actionHistory.push_back(action.get());
    
    NLM_LOG_DEBUG("MotorSystem selected action: " + action->getName() + " (activity: " + std::to_string(maxActivity) + ")");
    
    return action;
}

// Map neural activity to specific action type
ActionType MotorSystem::mapNeuralActivityToAction(NeuronType type, float activity, float& maxActivity) {
    // Normalize activity to [0, 1]
    activity = std::min(activity / 50.0f, 1.0f); // Assume max potential around 50
    
    ActionType action = ActionType::Wait;
    
    switch (type) {
        case NeuronType::Motor:
            // Motor neurons directly map to actions
            if (activity > 0.8f) {
                // High activity means specific action
                // In a real system, we'd know which motor neuron maps to which action
                // For now, use activity to select from available actions
                size_t actionIndex = static_cast<size_t>(activity * pImpl->availableActions.size());
                actionIndex = std::min(actionIndex, pImpl->availableActions.size() - 1);
                action = pImpl->availableActions[actionIndex];
            }
            break;
        case NeuronType::Sensory:
            // Sensory neurons can modulate action selection
            if (activity > 0.7f) {
                // Strong sensory input might trigger specific behaviors
                action = ActionType::Interact;
            } else {
                action = ActionType::Look; // Default looking behavior
            }
            break;
        case NeuronType::Excitatory:
            // Excitatory neurons promote action
            if (activity > 0.6f) {
                action = ActionType::MoveForward;
            }
            break;
        case NeuronType::Inhibitory:
            // Inhibitory neurons reduce action tendency
            if (activity > 0.5f) {
                action = ActionType::Wait;
            } else {
                action = ActionType::MoveForward;
            }
            break;
        default:
            action = ActionType::Wait;
            break;
    }
    
    return action;
}

// Apply action preferences and exploration factors
ActionType MotorSystem::applyActionPreferences(ActionType baseAction, float activity) {
    // Create weighted list of actions based on preferences
    std::vector<std::pair<ActionType, float>> weightedActions;
    
    for (const auto& action : pImpl->availableActions) {
        float weight = pImpl->actionPreferences[action];
        
        // Amplify by neural activity
        if (action == baseAction) {
            weight *= (1.0f + activity * 2.0f); // Encourage selected action
        } else {
            weight *= (1.0f - activity * 0.5f); // Suppress other actions
        }
        
        weightedActions.push_back({action, weight});
    }
    
    // Add exploration factor based on recent action history
    float exploration = 0.1f * (1.0f - activity); // More exploration when activity low
    
    // Calculate total weight
    float totalWeight = 0.0f;
    for (const auto& pair : weightedActions) {
        totalWeight += pair.second;
    }
    
    // Normalize weights
    if (totalWeight > 0.0f) {
        for (auto& pair : weightedActions) {
            pair.second /= totalWeight;
        }
    }
    
    // Apply exploration: sometimes choose random action
    float exploreThreshold = exploration;
    float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(pImpl->rng);
    
    if (r < exploreThreshold) {
        // Choose random action
        std::uniform_int_distribution<size_t> dist(0, weightedActions.size() - 1);
        size_t randomIndex = dist(pImpl->rng);
        return weightedActions[randomIndex].first;
    }
    
    // Select action based on weights
    float threshold = std::uniform_real_distribution<float>(0.0f, 1.0f)(pImpl->rng) * totalWeight;
    float cumulative = 0.0f;
    
    for (const auto& pair : weightedActions) {
        cumulative += pair.second;
        if (cumulative >= threshold) {
            return pair.first;
        }
    }
    
    // Fallback
    return baseAction;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl->availableActions.empty()) return;
    
    bool found = false;
    for (auto& availAction : pImpl->availableActions) {
        if (availAction == action) {
            pImpl->actionPreferences[action] = std::max(0.0f, std::min(2.0f, preference));
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->availableActions.push_back(action);
        pImpl->actionPreferences[action] = std::max(0.0f, std::min(2.0f, preference));
    }
    
    NLM_LOG_DEBUG("Set action preference for " + std::to_string(static_cast<int>(action)) + " to " + std::to_string(preference));
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    NLM_LOG_DEBUG("Action history cleared");
}

} // namespace nlm