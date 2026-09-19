#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include <random>
#include <algorithm>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::mt19937 rng;
    std::uniform_real_distribution<float> actionDist;
    std::vector<float> actionPreferences;
    
    Impl() 
        : rng(std::random_device()()), 
          actionDist(0.0f, 1.0f),
          actionPreferences(static_cast<size_t>(ActionType::Custom) + 1, 1.0f) {
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
        actionPreferences.resize(availableActions.size(), 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity patterns and preferences
    // Implements softmax action selection with exploration
    
    // Get motor neuron activity levels
    float maxActivity = 0.0f;
    for (const auto* neuron : motorRegion.getNeurons()) {
        float activity = neuron->getMembranePotential() / neuron->getThreshold();
        if (activity > maxActivity) maxActivity = activity;
    }
    
    // If no significant activity, return wait action
    if (maxActivity < 0.01f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Collect action preferences from motor neuron populations
    std::vector<std::pair<float, ActionType>> actionScores;
    
    // Analyze motor neuron populations for action mapping
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        float preference = pImpl->actionPreferences[i];
        
        // Get activity from neurons in the corresponding population
        for (const auto* pop : motorRegion.getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                float populationActivity = 0.0f;
                size_t activeCount = 0;
                for (const auto* neuron : pop->getNeurons()) {
                    float activity = neuron->getMembranePotential() / neuron->getThreshold();
                    populationActivity += activity;
                    activeCount++;
                }
                if (activeCount > 0) {
                    populationActivity /= activeCount;
                }
                
                // Map population activity to action preferences
                // Different motor neuron types control different actions
                if (i < 4) { // Move actions
                    preference *= (populationActivity * 2.0f + 0.5f);
                } else if (i >= 4 && i < 7) { // Turn/Look actions
                    preference *= (populationActivity * 1.5f + 0.5f);
                }
            }
        }
        
        // Add exploration noise
        float noise = pImpl->actionDist(pImpl->rng);
        preference *= (1.0f + noise * 0.1f); // 10% exploration noise
        
        actionScores.push_back({preference, pImpl->availableActions[i]});
    }
    
    // Apply softmax selection
    float maxScore = 0.0f;
    for (const auto& score : actionScores) {
        if (score.first > maxScore) maxScore = score.first;
    }
    
    // Compute selection probabilities
    std::vector<float> probabilities;
    float sumExp = 0.0f;
    for (const auto& score : actionScores) {
        float expScore = std::exp((score.first - maxScore) / 0.5f); // Temperature = 0.5
        probabilities.push_back(expScore);
        sumExp += expScore;
    }
    
    // Normalize probabilities
    if (sumExp > 0.0f) {
        for (float& prob : probabilities) {
            prob /= sumExp;
        }
    } else {
        // Fallback to uniform distribution
        for (size_t i = 0; i < probabilities.size(); ++i) {
            probabilities[i] = 1.0f / probabilities.size();
        }
    }
    
    // Select action based on probabilities
    float selection = pImpl->actionDist(pImpl->rng);
    float cumulative = 0.0f;
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulative += probabilities[i];
        if (selection <= cumulative) {
            // Record the selected action
            auto action = std::make_unique<Action>(actionScores[i].second);
            pImpl->actionHistory.push_back(action.get());
            return action;
        }
    }
    
    // Fallback
    auto action = std::make_unique<Action>(ActionType::Wait);
    pImpl->actionHistory.push_back(action.get());
    return action;
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
