#include "ActionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct ActionSystem::Impl {
    Brain* brain = nullptr;
    
    // Configuration
    float motorActivationThreshold = 0.5f;
    float actionCoolDown = 0.1f;           // Time between actions
    float actionStallTime = 2.0f;          // Max time without action before stall
    float noveltyActionBias = 0.2f;        // Weight for novelty-driven actions
    float explorationRate = 0.1f;          // Rate of exploratory actions
    
    // State
    float lastActionTime = 0.0f;
    float currentStallTime = 0.0f;
    bool actionStalled = false;
    
    // Statistics
    size_t totalActionsProduced = 0;
    std::vector<size_t> actionsByType;
    float actionSuccessRate = 0.8f;
    float lastActionQuality = 0.0f;
    
    // History
    std::vector<std::unique_ptr<Action>> actionHistory;
    
    // Motor neuron statistics
    size_t totalMotorNeurons = 0;
    float totalMotorActivation = 0.0f;
    float motorActivationVariance = 0.0f;
    
    // Action parameters
    std::vector<float> actionWeights;
    std::vector<float> actionUrgencies;
    std::vector<bool> actionEnabled;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> actionDist;
    std::normal_distribution<float> activationDist;
};

ActionSystem::ActionSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->actionsByType.resize(static_cast<size_t>(ActionType::Count), 0);
    pImpl->actionWeights.resize(static_cast<size_t>(ActionType::Count), 1.0f);
    pImpl->actionUrgencies.resize(static_cast<size_t>(ActionType::Count), 1.0f);
    pImpl->actionEnabled.resize(static_cast<size_t>(ActionType::Count), true);
    
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
}

ActionSystem::~ActionSystem() = default;

void ActionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Count total motor neurons if available
    if (pImpl->brain) {
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    pImpl->totalMotorNeurons += pop->getNeurons().size();
                }
            }
        }
    }
    
    NLM_LOG_INFO("ActionSystem initialized");
}

void ActionSystem::update(float timestep) {
    // Update stall time
    if (pImpl->actionStalled) {
        pImpl->currentStallTime += timestep;
    } else {
        pImpl->currentStallTime = 0.0f;
    }
    
    // Check if action is stalled (no action for too long)
    if (pImpl->currentStallTime > pImpl->actionStallTime) {
        pImpl->actionStalled = true;
        NLM_LOG_WARN("Action production stalled - no actions for " + 
                    std::to_string(pImpl->currentStallTime) + " seconds");
    }
    
    NLM_LOG_DEBUG("ActionSystem updated");
}

std::unique_ptr<Action> ActionSystem::produceAction() {
    if (!pImpl->brain) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Collect motor neuron activity
    std::vector<float> motorActivations;
    size_t activeMotorCount = 0;
    float totalActivation = 0.0f;
    
    for (auto* neuron : pImpl->brain->getMotorNeurons()) {
        bool isActive = neuron->isFiring() || 
                       std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > pImpl->motorActivationThreshold;
        
        if (isActive) {
            ++activeMotorCount;
            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            motorActivations.push_back(activation);
            totalActivation += activation;
        }
    }
    
    pImpl->totalMotorActivation = totalActivation;
    
    if (activeMotorCount == 0) {
        // No active motor neurons - produce wait action
        std::unique_ptr<Action> action = std::make_unique<Action>(ActionType::Wait);
        action->quality = 0.0f;
        recordAction(action.get());
        return action;
    }
    
    // Calculate average motor activation
    float avgActivation = totalActivation / static_cast<float>(activeMotorCount);
    pImpl->motorActivationVariance = 0.0f;
    
    if (activeMotorCount > 1) {
        float sumSquares = 0.0f;
        for (float activation : motorActivations) {
            float diff = activation - avgActivation;
            sumSquares += diff * diff;
        }
        pImpl->motorActivationVariance = sumSquares / static_cast<float>(activeMotorCount - 1);
    }
    
    // Determine action type based on motor neuron patterns
    ActionType selectedAction = selectActionType(avgActivation, activeMotorCount);
    
    // Create action
    std::unique_ptr<Action> action = std::make_unique<Action>(selectedAction);
    
    // Set action quality based on activation consistency
    action->quality = calculateActionQuality(motorActivations, avgActivation);
    
    // Record action
    recordAction(action.get());
    
    pImpl->lastActionTime = pImpl->brain->getCurrentTime();
    pImpl->actionStalled = false;
    
    NLM_LOG_DEBUG("ActionSystem produced action: " + std::to_string(static_cast<int>(selectedAction)) +
                 " (quality: " + std::to_string(action->quality) + ")");
    
    return action;
}

ActionType ActionSystem::selectActionType(float avgActivation, size_t activeMotorCount) {
    // Simple action selection logic based on activation level and count
    
    if (pImpl->actionStalled) {
        // When stalled, force exploration
        float exploreProb = pImpl->explorationRate * pImpl->noveltyActionBias;
        pImpl->actionDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        
        float randVal = pImpl->actionDist(pImpl->rng);
        
        // Randomly select an action (excluding Wait)
        std::vector<ActionType> possibleActions;
        for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
            if (i != static_cast<size_t>(ActionType::Wait) && pImpl->actionEnabled[i]) {
                possibleActions.push_back(static_cast<ActionType>(i));
            }
        }
        
        if (!possibleActions.empty() && randVal < exploreProb) {
            size_t idx = static_cast<size_t>(pImpl->actionDist(pImpl->rng) * possibleActions.size());
            return possibleActions[idx];
        }
    }
    
    // Normal action selection based on activation
    if (avgActivation > pImpl->motorActivationThreshold * 1.5f) {
        if (activeMotorCount >= 3) {
            return ActionType::MoveForward;
        } else {
            return ActionType::Rotate;
        }
    } else if (avgActivation > pImpl->motorActivationThreshold * 0.5f) {
        return ActionType::MoveForward;
    } else if (activeMotorCount > 0) {
        return ActionType::Wait;
    }
    
    // Default to wait
    return ActionType::Wait;
}

float ActionSystem::calculateActionQuality(const std::vector<float>& activations, float avgActivation) const {
    if (activations.size() <= 1) {
        return 0.8f; // Default quality for single or no activations
    }
    
    // Calculate coefficient of variation as quality indicator
    float mean = avgActivation;
    float variance = pImpl->motorActivationVariance;
    float stddev = std::sqrt(variance);
    
    float coefficientOfVariation = (mean > 0.0f) ? (stddev / mean) : 0.0f;
    
    // Lower coefficient of variation indicates more consistent activation = higher quality
    float quality = std::max(0.0f, 1.0f - coefficientOfVariation);
    
    return quality;
}

void ActionSystem::recordAction(const Action* action) {
    if (!action) return;
    
    pImpl->totalActionsProduced++;
    
    size_t actionIdx = static_cast<size_t>(action->type);
    if (actionIdx < pImpl->actionsByType.size()) {
        pImpl->actionsByType[actionIdx]++;
    }
    
    // Update success rate (simplified - assume 80% success for non-wait actions)
    if (action->type != ActionType::Wait) {
        pImpl->actionSuccessRate = (pImpl->actionSuccessRate * 0.9f) + (0.8f * 0.1f);
    } else {
        pImpl->actionSuccessRate = (pImpl->actionSuccessRate * 0.9f) + (0.5f * 0.1f);
    }
    
    pImpl->lastActionQuality = action->quality;
    
    // Add to history (keep last 100 actions)
    pImpl->actionHistory.push_back(std::make_unique<Action>(*action));
    if (pImpl->actionHistory.size() > 100) {
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
    }
}

void ActionSystem::configure(float motorActivationThreshold, float actionCoolDown,
                             float actionStallTime, float noveltyActionBias) {
    pImpl->motorActivationThreshold = motorActivationThreshold;
    pImpl->actionCoolDown = actionCoolDown;
    pImpl->actionStallTime = actionStallTime;
    pImpl->noveltyActionBias = noveltyActionBias;
    
    NLM_LOG_INFO("ActionSystem configured");
}

void ActionSystem::setActionParameters(ActionType actionType, float weight, float urgency) {
    size_t actionIdx = static_cast<size_t>(actionType);
    if (actionIdx < pImpl->actionWeights.size()) {
        pImpl->actionWeights[actionIdx] = weight;
        pImpl->actionUrgencies[actionIdx] = urgency;
    }
}

void ActionSystem::enableActionType(ActionType actionType, bool enabled) {
    size_t actionIdx = static_cast<size_t>(actionType);
    if (actionIdx < pImpl->actionEnabled.size()) {
        pImpl->actionEnabled[actionIdx] = enabled;
    }
}

bool ActionSystem::isActionTypeEnabled(ActionType actionType) const {
    size_t actionIdx = static_cast<size_t>(actionType);
    if (actionIdx < pImpl->actionEnabled.size()) {
        return pImpl->actionEnabled[actionIdx];
    }
    return false;
}

size_t ActionSystem::getTotalActionsProduced() const {
    return pImpl->totalActionsProduced;
}

size_t ActionSystem::getActionsByType(ActionType type) const {
    size_t idx = static_cast<size_t>(type);
    if (idx < pImpl->actionsByType.size()) {
        return pImpl->actionsByType[idx];
    }
    return 0;
}

float ActionSystem::getActionSuccessRate() const {
    return pImpl->actionSuccessRate;
}

void ActionSystem::reset() {
    pImpl->lastActionTime = 0.0f;
    pImpl->currentStallTime = 0.0f;
    pImpl->actionStalled = false;
    
    pImpl->totalActionsProduced = 0;
    std::fill(pImpl->actionsByType.begin(), pImpl->actionsByType.end(), 0);
    pImpl->actionSuccessRate = 0.8f;
    pImpl->lastActionQuality = 0.0f;
    
    pImpl->actionHistory.clear();
    pImpl->totalMotorNeurons = 0;
    pImpl->totalMotorActivation = 0.0f;
    pImpl->motorActivationVariance = 0.0f;
    
    NLM_LOG_INFO("ActionSystem reset");
}

float ActionSystem::getAverageMotorActivation() const {
    return pImpl->totalMotorActivation / static_cast<float>(std::max(pImpl->totalMotorNeurons, size_t(1)));
}

size_t ActionSystem::getActiveMotorNeurons() const {
    // This is a simplified version - in reality we would need to track active neurons
    return static_cast<size_t>(getAverageMotorActivation() * 10.0f); // Rough estimate
}

float ActionSystem::getMotorActivationVariance() const {
    return pImpl->motorActivationVariance;
}

bool ActionSystem::isActionStalled() const {
    return pImpl->actionStalled;
}

std::unique_ptr<Action> ActionSystem::forceAction(ActionType actionType) {
    std::unique_ptr<Action> action = std::make_unique<Action>(actionType);
    action->quality = 1.0f; // Force actions have high quality
    
    recordAction(action.get());
    
    pImpl->lastActionTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0f;
    pImpl->actionStalled = false;
    
    NLM_LOG_INFO("ActionSystem forced action: " + std::to_string(static_cast<int>(actionType)));
    
    return action;
}

const std::vector<std::unique_ptr<Action>>& ActionSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

} // namespace nlm
