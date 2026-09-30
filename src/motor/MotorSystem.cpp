#include "Action.hpp"
#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Neural activity patterns mapping
    std::unordered_map<ActionType, float> actionStrengths;
    
    // Action preferences for exploration
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Neuromodulation signals
    float neuromodulationLevel;
    float curiosityLevel;
    
    // Neural processing utilities
    static ActionType neuronTypeToActionType(NeuronType type);
    static float normalizeFiringRate(float rate);
    
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
        
        // Initialize all action strengths
        for (ActionType action : availableActions) {
            actionStrengths[action] = 0.0f;
            actionPreferences[action] = 1.0f;  // Equal initial preference
        }
        
        neuromodulationLevel = 0.0f;
        curiosityLevel = 0.0f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

// Convert neuron type to action type (mapping based on motor neuron organization)
ActionType MotorSystem::Impl::neuronTypeToActionType(NeuronType type) {
    switch (type) {
        case NeuronType::Motor:
            // Motor neurons mapped to default actions based on AgentBrain pattern
            return ActionType::MoveForward;
        default:
            return ActionType::Wait;
    }
}

// Normalize firing rate to 0-1 range
float MotorSystem::Impl::normalizeFiringRate(float rate) {
    return std::clamp(rate / 100.0f, 0.0f, 1.0f);  // Assuming 100Hz max firing rate
}

// Process motor neuron populations and convert to action strengths
void MotorSystem::processMotorNeurons(const NeuralRegion& motorRegion) {
    if (!pImpl) return;
    
    // Reset all action strengths
    for (auto& pair : pImpl->actionStrengths) {
        pair.second = 0.0f;
    }
    
    // Process each population in the motor region
    for (size_t i = 0; i < motorRegion.getPopulationCount(); ++i) {
        const NeuralPopulation* population = motorRegion.getPopulation(i);
        NeuronType type = population->getNeuronType();
        
        if (type != NeuronType::Motor) continue;
        
        // Get all neurons in this population
        std::vector<Neuron*> neurons = population->getNeurons();
        
        // Calculate average firing activity for this population
        float populationActivity = 0.0f;
        size_t activeCount = 0;
        
        for (Neuron* neuron : neurons) {
            if (neuron && neuron->isFiring()) {
                float firingRate = neuron->getFiringRate();
                populationActivity += normalizeFiringRate(firingRate);
                activeCount++;
            }
        }
        
        if (activeCount > 0) {
            populationActivity /= activeCount;
        }
        
        // Convert neuron type to action type
        ActionType action = pImpl->neuronTypeToActionType(type);
        
        // Distribute activity to appropriate action strength
        // With winner-take-all: activity flows to its action type
        pImpl->actionStrengths[action] += populationActivity;
        
        // Log for debugging
        if (populationActivity > 0.1f) {
            NLM_LOG_DEBUG("Motor neuron population activity: " + std::to_string(populationActivity) +
                         " for action " + Action::actionTypeToString(action));
        }
    }
}

// Get action strength for a specific action
float MotorSystem::getActionStrength(ActionType action) const {
    if (!pImpl) return 0.0f;
    
    auto it = pImpl->actionStrengths.find(action);
    if (it != pImpl->actionStrengths.end()) {
        return it->second;
    }
    return 0.0f;
}

// Update action strengths after action selection
void MotorSystem::updateActionStrengths(ActionType selectedAction) {
    if (!pImpl) return;
    
    // Boost the strength of the selected action for reinforcement
    if (pImpl->actionStrengths.find(selectedAction) != pImpl->actionStrengths.end()) {
        pImpl->actionStrengths[selectedAction] *= 1.1f;  // 10% boost
        
        // Decay all other actions
        for (auto& pair : pImpl->actionStrengths) {
            if (pair.first != selectedAction) {
                pair.second *= 0.95f;  // 5% decay
            }
        }
    }
}

// Apply neuromodulation effects on action selection
void MotorSystem::applyNeuromodulation(float neuromodulationLevel) {
    if (!pImpl) return;
    
    pImpl->neuromodulationLevel = neuromodulationLevel;
    
    // Neuromodulation affects exploration vs exploitation
    if (neuromodulationLevel > 0.5f) {
        // High neuromodulation = more exploration
        pImpl->curiosityLevel = std::min(1.0f, pImpl->curiosityLevel + 0.1f);
    } else {
        // Low neuromodulation = more exploitation
        pImpl->curiosityLevel = std::max(0.0f, pImpl->curiosityLevel - 0.05f);
    }
}

// Set curiosity level for exploration
void MotorSystem::setCuriosityLevel(float level) {
    if (!pImpl) return;
    
    pImpl->curiosityLevel = std::clamp(level, 0.0f, 1.0f);
}

// Get current curiosity level
float MotorSystem::getCuriosityLevel() const {
    if (!pImpl) return 0.0f;
    return pImpl->curiosityLevel;
}

// Select action with exploration/exploitation balance
std::unique_ptr<Action> MotorSystem::selectActionWithExploration(const NeuralRegion& motorRegion,
                                                               float explorationProbability) {
    // Process neural activity patterns
    processMotorNeurons(motorRegion);
    
    // Apply neuromodulation
    applyNeuromodulation(pImpl->neuromodulationLevel);
    
    // Check for exploration based on curiosity and probability
    float exploreChance = explorationProbability * pImpl->curiosityLevel;
    float r = (rand() % 1000) / 1000.0f;  // Simple random number
    
    if (r < exploreChance) {
        // EXPLORATION: Select random action based on preferences
        NLM_LOG_DEBUG("MotorSystem: Exploring new action (curiosity=" + std::to_string(pImpl->curiosityLevel) + ")");
        
        // Create vector of actions weighted by preferences
        std::vector<std::pair<ActionType, float>> weightedActions;
        for (const auto& pair : pImpl->actionPreferences) {
            weightedActions.push_back(pair);
        }
        
        // Normalize preferences to sum to 1
        float totalWeight = 0.0f;
        for (const auto& pair : weightedActions) {
            totalWeight += pair.second;
        }
        
        if (totalWeight > 0.0f) {
            for (auto& pair : weightedActions) {
                pair.second /= totalWeight;
            }
        }
        
        // Select action based on preferences
        float selection = (rand() % 1000) / 1000.0f;
        float cumulative = 0.0f;
        
        for (const auto& pair : weightedActions) {
            cumulative += pair.second;
            if (selection <= cumulative) {
                std::unique_ptr<Action> action = std::make_unique<Action>(pair.first);
                actionHistory.push_back(action.get());
                return action;
            }
        }
    } else {
        // EXPLOITATION: Select best action based on neural activity
        NLM_LOG_DEBUG("MotorSystem: Exploiting best action (curiosity=" + std::to_string(pImpl->curiosityLevel) + ")");
        
        // Find action with highest strength
        ActionType bestAction = ActionType::Wait;
        float bestStrength = 0.0f;
        
        for (const auto& pair : pImpl->actionStrengths) {
            if (pair.second > bestStrength) {
                bestStrength = pair.second;
                bestAction = pair.first;
            }
        }
        
        // Only select if there's meaningful activity (similar to AgentBrain)
        if (bestStrength > 0.5f) {
            std::unique_ptr<Action> action = std::make_unique<Action>(bestAction);
            actionHistory.push_back(action.get());
            updateActionStrengths(bestAction);
            return action;
        }
    }
    
    // Fallback: Wait action
    std::unique_ptr<Action> action = std::make_unique<Action>(ActionType::Wait);
    actionHistory.push_back(action.get());
    return action;
}

// Implement real action selection based on neural activity
std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Use exploration with default 15% probability when curiosity is moderate
    float explorationProbability = 0.15f + pImpl->curiosityLevel * 0.35f;
    return selectActionWithExploration(motorRegion, explorationProbability);
}

// Set action preferences (for exploration)
void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (!pImpl) return;
    
    auto it = pImpl->actionPreferences.find(action);
    if (it != pImpl->actionPreferences.end()) {
        it->second = std::clamp(preference, 0.0f, 10.0f);  // Preferences from 0 to 10
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    if (!pImpl) return;
    
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

// Get all neural activity patterns
std::unordered_map<ActionType, float> MotorSystem::getActivityPatterns() const {
    if (!pImpl) return {};
    return pImpl->actionStrengths;
}

} // namespace nlm
