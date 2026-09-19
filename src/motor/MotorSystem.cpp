#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <cassert>
#include <random>
#include <unordered_map>
#include <limits>

namespace nlm {

struct MotorSystem::Impl {
    // Motor system configuration
    Config config;
    
    // Available actions
    std::vector<ActionType> availableActions;
    
    // Action value estimates (Q-values) for reinforcement learning
    std::unordered_map<ActionType, float> actionValues;
    
    // Eligibility traces for delayed reward association
    std::unordered_map<ActionType, float> eligibilityTraces;
    
    // Action history for learning and statistics
    std::vector<const Action*> actionHistory;
    
    // Temporal tracking
    ActionType lastSelectedAction;
    float lastReward;
    float cumulativeReward;
    float tdError;  // Temporal difference error
    
    // Action persistence tracking
    std::unordered_map<ActionType, float> actionPersistence;
    
    // Random number generator for noise and exploration
    std::unique_ptr<RandomGenerator> rng;
    
    // Statistics tracking
    size_t totalSteps;
    size_t consecutiveSwitches;
    
    Impl() : lastSelectedAction(ActionType::Wait), lastReward(0.0f), cumulativeReward(0.0f), 
             tdError(0.0f), totalSteps(0), consecutiveSwitches(0) {
        
        // Initialize available actions
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
        
        // Initialize action values
        for (const auto& action : availableActions) {
            actionValues[action] = config.initialActionValue;
            actionPersistence[action] = 0.0f;
            eligibilityTraces[action] = 0.0f;
        }
        
        // Initialize random generator
        rng = std::make_unique<RandomGenerator>(42);
    }
    
    // Validate neural region and extract motor neuron activities
    bool validateMotorRegion(const NeuralRegion& motorRegion) const {
        if (!motorRegion.getTotalNeuronCount()) {
            return false;
        }
        
        // Check for motor neuron populations
        bool hasMotorPopulation = false;
        for (const auto& pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                hasMotorPopulation = true;
                break;
            }
        }
        
        return hasMotorPopulation;
    }
    
    // Extract population vector from motor neuron activities
    std::vector<float> extractPopulationVector(const NeuralRegion& motorRegion) const {
        std::vector<float> populationVector(availableActions.size(), 0.0f);
        
        // Get all motor neurons from the region
        auto neurons = motorRegion.getAllNeurons();
        
        // Map actions to neuron indices for population vector decoding
        // This is a simplified model - in practice, actions would be encoded in
        // specific motor neuron populations with known directional tuning
        
        size_t actionIndex = 0;
        for (const auto& neuron : neurons) {
            if (actionIndex >= availableActions.size()) break;
            
            // Convert firing rate to population vector component
            // Use sigmoid transformation to get values in [0, 1]
            float firingRate = neuron->getFiringRate();
            float normalizedActivity = 1.0f / (1.0f + std::exp(-0.1f * (firingRate - 10.0f)));
            
            populationVector[actionIndex] = normalizedActivity;
            actionIndex++;
        }
        
        return populationVector;
    }
    
    // Apply winner-take-all competitive dynamics
    ActionType applyWinnerTakeAll(const std::vector<float>& populationVector) const {
        if (populationVector.empty()) {
            return ActionType::Wait;
        }
        
        // Find maximum activity
        float maxActivity = *std::max_element(populationVector.begin(), populationVector.end());
        
        // Find all neurons with maximum activity (ties)
        std::vector<size_t> winners;
        for (size_t i = 0; i < populationVector.size(); ++i) {
            if (std::abs(populationVector[i] - maxActivity) < 1e-6f) {
                winners.push_back(i);
            }
        }
        
        // If single winner, use it
        if (winners.size() == 1) {
            return availableActions[winners[0]];
        }
        
        // If multiple winners, apply softmax selection based on action values
        std::vector<float> winnerValues;
        for (size_t winnerIdx : winners) {
            winnerValues.push_back(actionValues.at(availableActions[winnerIdx]));
        }
        
        // Apply Boltzmann selection among winners
        float maxValue = *std::max_element(winnerValues.begin(), winnerValues.end());
        std::vector<float> expValues;
        
        for (float value : winnerValues) {
            expValues.push_back(std::exp((value - maxValue) / config.temperature));
        }
        
        float sumExp = std::accumulate(expValues.begin(), expValues.end(), 0.0f);
        
        if (sumExp > 0.0f) {
            float randomValue = rng->uniformReal(0.0, sumExp);
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < winnerValues.size(); ++i) {
                cumulative += expValues[i];
                if (randomValue <= cumulative) {
                    return availableActions[winners[i]];
                }
            }
        }
        
        // Fallback to first winner
        return availableActions[winners[0]];
    }
    
    // Apply exploration vs exploitation balance
    ActionType applyExplorationExploitation(const ActionType& deterministicAction) {
        // Epsilon-greedy selection
        if (rng->bernoulli(config.explorationRate)) {
            // Explore: select random action
            return *rng->pickRandom(availableActions);
        }
        
        // Exploit: select based on action values with softmax
        std::vector<float> actionValuesVec;
        std::vector<ActionType> actionsVec;
        
        for (const auto& action : availableActions) {
            actionValuesVec.push_back(actionValues[action]);
            actionsVec.push_back(action);
        }
        
        float maxValue = *std::max_element(actionValuesVec.begin(), actionValuesVec.end());
        std::vector<float> expValues;
        
        for (float value : actionValuesVec) {
            expValues.push_back(std::exp((value - maxValue) / config.temperature));
        }
        
        float sumExp = std::accumulate(expValues.begin(), expValuesVec.end(), 0.0f);
        
        if (sumExp > 0.0f) {
            float randomValue = rng->uniformReal(0.0, sumExp);
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < actionsVec.size(); ++i) {
                cumulative += expValues[i];
                if (randomValue <= cumulative) {
                    return actionsVec[i];
                }
            }
        }
        
        return deterministicAction;
    }
    
    // Add biological noise to action selection
    ActionType addInternalNoise(const ActionType& action) {
        if (config.internalNoise <= 0.0f) {
            return action;
        }
        
        // With probability equal to noise level, select a different random action
        if (rng->bernoulli(config.internalNoise)) {
            // Select random action different from current
            std::vector<ActionType> otherActions;
            for (const auto& act : availableActions) {
                if (act != action) {
                    otherActions.push_back(act);
                }
            }
            
            if (!otherActions.empty()) {
                return *rng->pickRandom(otherActions);
            }
        }
        
        return action;
    }
    
    // Update action persistence
    void updateActionPersistence() {
        // Decay persistence for all actions
        for (auto& [action, persistence] : actionPersistence) {
            persistence *= std::exp(-1.0f / config.actionPersistence);
        }
        
        // Increase persistence for last selected action
        if (totalSteps > 0) {
            actionPersistence[lastSelectedAction] = 1.0f;
        }
    }
    
    // Record selected action
    void recordSelectedAction(const ActionType& action) {
        // Create a new Action object (caller takes ownership)
        auto* newAction = new Action(action);
        actionHistory.push_back(newAction);
        
        // Clean up old history if it gets too large
        if (actionHistory.size() > 1000) {
            delete actionHistory.front();
            actionHistory.erase(actionHistory.begin());
        }
        
        // Update last selected action
        lastSelectedAction = action;
    }
    
    // Apply temporal difference learning
    void applyTemporalDifferenceLearning(float reward) {
        lastReward = reward;
        cumulativeReward += reward;
        
        // Calculate temporal difference error
        if (totalSteps > 0) {
            // Simple TD(0) update: δ = r + γ * max_a Q(s',a) - Q(s,a)
            float maxNextValue = 0.0f;
            for (const auto& action : availableActions) {
                maxNextValue = std::max(maxNextValue, actionValues[action]);
            }
            
            tdError = reward + config.discountFactor * maxNextValue - actionValues[lastSelectedAction];
            
            // Update action value with eligibility trace
            eligibilityTraces[lastSelectedAction] += 1.0f;
            
            // Apply update
            actionValues[lastSelectedAction] += config.learningRate * tdError * eligibilityTraces[lastSelectedAction];
            
            // Decay eligibility traces
            for (auto& [action, trace] : eligibilityTraces) {
                trace *= config.eligibilityDecay;
            }
        }
    }
    
    // Apply policy gradient for stochastic policies
    void applyPolicyGradient() {
        // Policy gradient update for exploration/exploitation balance
        // ∇J ≈ (r - b) * ∇log π(a|s)
        if (totalSteps > 0 && std::abs(tdError) > 1e-6f) {
            // Update action preferences based on reward prediction error
            float baseline = cumulativeReward / static_cast<float>(totalSteps);
            float advantage = lastReward - baseline;
            
            // Adjust action preferences proportionally to advantage
            for (auto& [action, value] : actionValues) {
                if (action == lastSelectedAction) {
                    value += config.learningRate * advantage;
                }
            }
        }
    }
};  // struct Impl

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Validate input
    if (!pImpl->validateMotorRegion(motorRegion)) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Extract population vector from motor neuron activities
    std::vector<float> populationVector = pImpl->extractPopulationVector(motorRegion);
    
    // Apply winner-take-all competitive dynamics
    ActionType selectedAction = pImpl->applyWinnerTakeAll(populationVector);
    
    // Apply exploration vs exploitation balance
    selectedAction = pImpl->applyExplorationExploitation(selectedAction);
    
    // Add biological noise
    selectedAction = pImpl->addInternalNoise(selectedAction);
    
    // Update action persistence
    pImpl->updateActionPersistence();
    
    // Record selected action
    pImpl->recordSelectedAction(selectedAction);
    
    // Create and return action
    return std::make_unique<Action>(selectedAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Validate input
    if (std::isnan(preference) || std::isinf(preference)) {
        return;
    }
    
    if (std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action) == pImpl->availableActions.end()) {
        return;
    }
    
    // Apply reinforcement learning update
    float oldValue = pImpl->actionValues[action];
    pImpl->actionValues[action] = preference;
    
    // Adjust other action values to maintain sum of values (if needed)
    // For now, we allow absolute preference values
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    
    // Reset learning state
    pImpl->cumulativeReward = 0.0f;
    pImpl->totalSteps = 0;
    pImpl->consecutiveSwitches = 0;
}

const MotorSystem::Config& MotorSystem::getConfig() const {
    return pImpl->config;
}

bool MotorSystem::updateConfig(const Config& newConfig) {
    // Validate new configuration
    if (newConfig.explorationRate < newConfig.minExplorationRate || 
        newConfig.explorationRate > newConfig.maxExplorationRate) {
        return false;
    }
    
    if (newConfig.temperature < newConfig.minTemperature || 
        newConfig.temperature > newConfig.maxTemperature) {
        return false;
    }
    
    if (newConfig.learningRate < newConfig.minLearningRate || 
        newConfig.learningRate > newConfig.maxLearningRate) {
        return false;
    }
    
    if (newConfig.actionPersistence <= 0.0f) {
        return false;
    }
    
    if (newConfig.internalNoise < 0.0f || newConfig.internalNoise > 1.0f) {
        return false;
    }
    
    if (newConfig.eligibilityDecay <= 0.0f || newConfig.eligibilityDecay >= 1.0f) {
        return false;
    }
    
    if (newConfig.discountFactor <= 0.0f || newConfig.discountFactor >= 1.0f) {
        return false;
    }
    
    // All validations passed, update configuration
    pImpl->config = newConfig;
    return true;
}

std::unordered_map<ActionType, float> MotorSystem::getActionValues() const {
    return pImpl->actionValues;
}

void MotorSystem::recordReward(float reward) {
    if (std::isnan(reward) || std::isinf(reward)) {
        return;
    }
    
    pImpl->lastReward = reward;
    pImpl->applyTemporalDifferenceLearning(reward);
    pImpl->applyPolicyGradient();
}

void MotorSystem::advanceStep(float dt) {
    pImpl->totalSteps++;
    
    // Update action persistence
    pImpl->updateActionPersistence();
}

} // namespace nlm
