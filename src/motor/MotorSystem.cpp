#include "MotorSystem.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include <random>
#include <algorithm>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;
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
        
        // Initialize preferences to neutral values
        actionPreferences.resize(availableActions.size(), 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    // More sophisticated implementation based on neural activity
    
    // Get all neurons in the motor region
    auto neurons = motorRegion.getAllNeurons();
    
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate action preferences based on neural activity
    std::vector<float> actionScores;
    actionScores.reserve(pImpl->availableActions.size());
    
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        float score = calculateActionScore(motorRegion, neurons, pImpl->availableActions[i]);
        actionScores.push_back(score);
    }
    
    // Apply learned preferences
    for (size_t i = 0; i < actionScores.size(); ++i) {
        actionScores[i] *= pImpl->actionPreferences[i];
    }
    
    // Select action based on scores (softmax-like selection)
    std::vector<float> probabilities;
    float sum = 0.0f;
    
    for (float score : actionScores) {
        float expScore = std::exp(score);
        probabilities.push_back(expScore);
        sum += expScore;
    }
    
    if (sum > 0.0f) {
        for (float& prob : probabilities) {
            prob /= sum;
        }
    } else {
        // If all scores are very negative, use uniform distribution
        for (float& prob : probabilities) {
            prob = 1.0f / probabilities.size();
        }
    }
    
    // Roulette wheel selection
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float randomValue = dist(pImpl->rng);
    float cumulativeProb = 0.0f;
    
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulativeProb += probabilities[i];n        if (randomValue <= cumulativeProb) {
            // Found selected action
            auto action = std::make_unique<Action>(pImpl->availableActions[i]);
            pImpl->actionHistory.push_back(action.get());
            return action;
        }
    }
    
    // Fallback to first action
    auto action = std::make_unique<Action>(pImpl->availableActions[0]);
    pImpl->actionHistory.push_back(action.get());
    return action;
}

float MotorSystem::calculateActionScore(const NeuralRegion& motorRegion, 
                                       const std::vector<const Neuron*>& neurons, 
                                       ActionType action) const {
    float score = 0.0f;
    
    // Base score on neural activity patterns associated with each action
    for (const Neuron* neuron : neurons) {
        float neuronActivity = neuron->getFiringRate();
        
        // Get action association strength for this neuron
        float actionAssociation = getActionAssociation(neuron, action);
        
        // Add weighted contribution
        score += neuronActivity * actionAssociation;
        
        // Add modulation based on neuron type
        switch (neuron->getType()) {
            case NeuronType::Motor:
                score += neuronActivity * 1.5f;  // Motor neurons strongly influence action selection
                break;
            case NeuronType::Sensory:
                score += neuronActivity * 0.5f;  // Sensory input contributes to action selection
                break;
            case NeuronType::Internal:
                score += neuronActivity * 0.8f;  // Internal processing influences action selection
                break;
            case NeuronType::Planning:
                score += neuronActivity * 1.2f;  // Planning signals strongly influence action selection
                break;
        }
    }
    
    // Add action-specific biases
    if (action == ActionType::MoveForward) {
        score += 0.5f;  // Forward bias
    } else if (action == ActionType::Look) {
        score += 0.3f;  // Look bias
    }
    
    return score;
}

float MotorSystem::getActionAssociation(const Neuron* neuron, ActionType action) const {
    // Return the strength of association between a neuron and an action
    // This would be learned through experience in a real implementation
    
    // For now, return a simple deterministic value based on neuron ID and action
    // This creates consistent but learnable associations
    uint64_t neuronId = neuron->getId().index();
    uint64_t actionIndex = static_cast<uint64_t>(action);
    
    // Simple hash-like function to create association strength
    uint64_t hash = neuronId ^ (actionIndex * 0x9e3779b97f4a7c15);
    float association = (hash % 1000) / 1000.0f;  // Range [0, 1)
    
    return association;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // Set the preference for a specific action
    
    size_t index = 0;
    for (; index < pImpl->availableActions.size(); ++index) {
        if (pImpl->availableActions[index] == action) {
            break;
        }
    }
    
    if (index < pImpl->availableActions.size()) {
        // Clamp preference to reasonable range
        pImpl->actionPreferences[index] = std::max(0.1f, std::min(2.0f, preference));
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
