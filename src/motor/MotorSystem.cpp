#include "MotorSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences; // Action preference scores
    std::unordered_map<ActionType, float> neuralActionWeights; // Weights based on neural activity
    SimulationStep lastActionStep;
    
    // Policy-based selection parameters
    float explorationRate;      // Rate of random action selection
    float exploitationRate;     // Rate of preference-based selection
    float neuralInfluence;      // How much neural activity influences selection
    
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
        lastActionStep = 0;
        explorationRate = 0.1f;
        exploitationRate = 0.7f;
        neuralInfluence = 0.2f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Implement real action selection based on neural activity
    // Uses policy-based selection with neural influence
    
    auto neurons = motorRegion.getNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate neural influence scores
    std::vector<float> neuralScores;
    for (const auto& action : pImpl->availableActions) {
        float score = calculateNeuralActionScore(action, neurons);
        neuralScores.push_back(score);
    }
    
    // Apply action preferences
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        ActionType action = pImpl->availableActions[i];
        float preference = pImpl->actionPreferences.count(action) ? 
                          pImpl->actionPreferences[action] : 0.0f;
        neuralScores[i] *= (1.0f + preference);
    }
    
    // Policy-based selection
    float explorationBonus = (rand() % 100) / 100.0f; // 0-1 random
    
    float totalScore = 0.0f;
    for (float score : neuralScores) {
        totalScore += score;
    }
    
    // Choose action based on scores
    float threshold = (rand() % 100) / 100.0f * totalScore;
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < neuralScores.size(); ++i) {
        cumulative += neuralScores[i];
        if (cumulative >= threshold) {
            ActionType selectedAction = pImpl->availableActions[i];
            
            // Record action in history
            pImpl->actionHistory.push_back(new Action(selectedAction));
            pImpl->lastActionStep = 1000; // Placeholder for current step
            
            return std::make_unique<Action>(selectedAction);
        }
    }
    
    // Fallback to wait action
    pImpl->actionHistory.push_back(new Action(ActionType::Wait));
    return std::make_unique<Action>(ActionType::Wait);
}

float MotorSystem::calculateNeuralActionScore(ActionType action, 
                                              const std::vector<std::unique_ptr<Neuron>>& neurons) const {
    float score = 0.0f;
    
    for (const auto& neuron : neurons) {
        // Get neuron properties
        float firingRate = neuron->getFiringRate();
        float membranePotential = neuron->getMembranePotential();
        
        // Map action type to neural correlates
        float actionRelevance = getActionNeuralRelevance(action, neuron);
        
        // Calculate contribution to action score
        float neuronContribution = (firingRate * 0.3f + 
                                   (membranePotential + 70.0f) * 0.7f / 140.0f) * 
                                  actionRelevance;
        
        score += neuronContribution;
    }
    
    // Normalize score
    return score / std::max(1.0f, static_cast<float>(neurons.size()));
}

float MotorSystem::getActionNeuralRelevance(ActionType action, 
                                           const std::unique_ptr<Neuron>& neuron) const {
    // Different neurons have different action preferences based on type
    // This is a simplified implementation
    
    // For motor cortex neurons, different actions have different relevance
    float baseRelevance = 0.1f;
    
    switch (action) {
        case ActionType::MoveForward:
            return baseRelevance * 1.5f;
        case ActionType::MoveBackward:
            return baseRelevance * 1.3f;
        case ActionType::MoveLeft:
            return baseRelevance * 1.4f;
        case ActionType::MoveRight:
            return baseRelevance * 1.4f;
        case ActionType::TurnLeft:
            return baseRelevance * 1.2f;
        case ActionType::TurnRight:
            return baseRelevance * 1.2f;
        case ActionType::Look:
            return baseRelevance * 1.1f;
        case ActionType::Interact:
            return baseRelevance * 1.3f;
        case ActionType::Wait:
            return baseRelevance * 0.5f;
        default:
            return baseRelevance;
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set or update preference for a specific action
    pImpl->actionPreferences[action] = std::clamp(preference, -1.0f, 1.0f);
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
