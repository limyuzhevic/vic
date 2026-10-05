#include "MotorSystem.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action selection parameters
    float explorationRate;  // Probability of random action
    float temperature;      // Softmax temperature for action selection
    float preferenceDecay;  // How quickly preferences decay
    
    // Action preferences for reinforcement learning
    std::map<ActionType, float> actionPreferences;
    
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
        
        // Initialize preferences
        for (auto action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
        
        explorationRate = 0.1f;
        temperature = 0.5f;
        preferenceDecay = 0.95f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity and preferences
    auto neurons = motorRegion.getAllNeurons();
    
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Decay preferences over time (for exploration)
    for (auto& pair : pImpl->actionPreferences) {
        pair.second *= pImpl->preferenceDecay;
    }
    
    // Calculate action values from neural activity
    std::map<ActionType, float> actionValues;
    
    // For each neuron, map its activity to action preferences
    for (auto* neuron : neurons) {
        float firingRate = neuron->getFiringRate();
        if (firingRate > 0.01f) {
            // Normalize firing rate to 0-1
            float normalizedFiring = std::min(1.0f, firingRate / 20.0f);
            
            // Map neuron to action based on region organization
            // Different neurons in motor region control different actions
            ActionType action = mapNeuronToAction(neuron->getId(), motorRegion);
            
            // Apply excitation/inhibition based on neuron type
            float weight = normalizedFiring;
            if (neuron->getType() == NeuronType::Inhibitory) {
                weight *= -0.5f;  // Inhibitory neurons suppress action values
            }
            
            actionValues[action] += weight;
        }
    }
    
    // Blend with preference-based selection for reinforcement learning
    for (auto& pair : pImpl->actionPreferences) {
        float preference = pair.second * 0.3f;  // Preference weight
        actionValues[pair.first] += preference;
    }
    
    // Add exploration: occasionally select random action
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    if (dis(gen) < pImpl->explorationRate) {
        // Choose random action
        if (!pImpl->availableActions.empty()) {
            std::uniform_int_distribution<size_t> actionDist(0, pImpl->availableActions.size() - 1);
            ActionType randomAction = pImpl->availableActions[actionDist(gen)];
            
            // Add small random value to ensure it's chosen
            actionValues[randomAction] += 2.0f;
        }
    }
    
    // Select action using softmax (for probability distribution)
    std::vector<ActionType> actions;
    std::vector<float> scores;
    
    for (auto& pair : actionValues) {
        actions.push_back(pair.first);
        scores.push_back(pair.second);
    }
    
    // Apply softmax to get probabilities
    float maxScore = *std::max_element(scores.begin(), scores.end());
    std::vector<float> probabilities;
    
    for (float score : scores) {
        float expScore = std::exp((score - maxScore) / pImpl->temperature);
        probabilities.push_back(expScore);
    }
    
    float totalProb = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
    for (float& prob : probabilities) {
        prob /= totalProb;
    }
    
    // Select action based on probabilities
    float randVal = dis(gen);
    float cumulative = 0.0f;
    ActionType selectedAction = actions.empty() ? ActionType::Wait : ActionType::Wait;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        cumulative += probabilities[i];
        if (randVal <= cumulative) {
            selectedAction = actions[i];
            break;
        }
    }
    
    // Update action history
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    
    pImpl->actionHistory.push_back(new Action(selectedAction));
    
    return std::make_unique<Action>(selectedAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl->actionPreferences.find(action) != pImpl->actionPreferences.end()) {
        pImpl->actionPreferences[action] = preference;
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

// Helper function to map neuron ID to action
ActionType MotorSystem::Impl::mapNeuronToAction(NeuronId neuronId, const NeuralRegion& region) {
    // Simple mapping: different neuron ranges control different actions
    // This could be more sophisticated in a real implementation
    
    uint64_t id = neuronId;
    
    // Calculate which action group this neuron belongs to
    size_t actionCount = pImpl->availableActions.size();
    size_t groupSize = actionCount / 3;  // Divide into 3 groups
    
    if (id < groupSize) {
        return ActionType::MoveForward;
    } else if (id < 2 * groupSize) {
        return ActionType::TurnLeft;
    } else if (id < 3 * groupSize) {
        return ActionType::TurnRight;
    } else if (id < 4 * groupSize) {
        return ActionType::MoveLeft;
    } else if (id < 5 * groupSize) {
        return ActionType::MoveRight;
    } else if (id < 6 * groupSize) {
        return ActionType::MoveBackward;
    } else if (id < 7 * groupSize) {
        return ActionType::Look;
    } else if (id < 8 * groupSize) {
        return ActionType::Interact;
    } else {
        return ActionType::Wait;
    }
}

} // namespace nlm
