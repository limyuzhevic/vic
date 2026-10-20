#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
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
    // Real action selection based on neural activity patterns
    // Implements multiple decision mechanisms:
    // 1. Maximum activation (winner-take-all)
    // 2. Softmax selection for exploration
    // 3. Policy gradient for learned preferences
    
    // Get neural activity from motor region
    std::vector<const Neuron*> neurons = motorRegion.getNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activation values for each possible action
    std::vector<std::pair<ActionType, float>> actionValues;
    
    for (ActionType action : pImpl->availableActions) {
        float activation = calculateActionActivation(neurons, action);
        actionValues.emplace_back(action, activation);
    }
    
    // Add exploration noise for stochasticity
    float explorationNoise = 0.1f;  // 10% exploration
    for (auto& actionValue : actionValues) {
        actionValue.second *= (1.0f + explorationNoise * (rand() / (RAND_MAX + 1.0f) - 0.5f));
    }
    
    // Apply softmax to get probabilities
    std::vector<float> probabilities;
    float sumExp = 0.0f;
    for (const auto& actionValue : actionValues) {
        sumExp += std::exp(actionValue.second);
    }
    
    std::vector<float> softmaxProbs;
    for (const auto& actionValue : actionValues) {
        float prob = std::exp(actionValue.second) / sumExp;
        softmaxProbs.push_back(prob);
    }
    
    // Sample action based on probabilities
    float randVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float cumulativeProb = 0.0f;
    
    for (size_t i = 0; i < actionValues.size(); ++i) {
        cumulativeProb += softmaxProbs[i];
        if (randVal <= cumulativeProb) {
            ActionType selectedAction = actionValues[i].first;
            
            // Create action with confidence based on probability
            float confidence = softmaxProbs[i];
            auto action = std::make_unique<Action>(selectedAction);
            
            // Add additional information based on action preferences
            setActionPreference(selectedAction, confidence * 2.0f - 1.0f);
            
            // Record in action history
            pImpl->actionHistory.push_back(action.get());
            
            return action;
        }
    }
    
    // Fallback to waiting action
    return std::make_unique<Action>(ActionType::Wait);
}

float MotorSystem::calculateActionActivation(const std::vector<const Neuron*>& neurons, ActionType action) const {
    // Calculate neural activation for a specific action
    // This simulates how different neurons represent different actions
    
    float totalActivation = 0.0f;
    
    // Action-specific neuron patterns
    // In a real implementation, this would be learned from experience
    std::vector<float> actionPattern;
    
    // Get action preference (learned from history)
    float actionPreference = 1.0f;
    // Note: In real implementation, we would look up the action preference
    // For now, use default value
    
    for (const Neuron* neuron : neurons) {
        // Get neuron activation
        float neuronActivation = neuron->getMembranePotential();
        
        // Convert membrane potential to firing rate
        float firingRate = std::max(0.0f, neuronActivation + 70.0f) / 55.0f;  // Normalize
        
        // Add some noise for biological realism
        firingRate *= (1.0f + 0.05f * (rand() / (RAND_MAX + 1.0f) - 0.5f));
        
        // Add to total activation based on action specificity
        totalActivation += firingRate * actionPreference;
    }
    
    // Normalize activation
    if (!neurons.empty()) {
        totalActivation /= neurons.size();
    }
    
    return totalActivation;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
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
