#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace nlm {

struct MotorSystem::Impl {
    float actionPreferences[6];  // Action type -> preference value (for 6 basic actions)
    float actionProbabilities[6]; // Action type -> selection probability
    float learningRate;
    size_t totalActionsSelected;
    std::vector<ActionType> actionHistory;
    
    Impl() {
        for (int i = 0; i < 6; ++i) {
            actionPreferences[i] = 0.0f;
            actionProbabilities[i] = 1.0f / 6.0f;  // Uniform initially
        }
        learningRate = 0.01f;
        totalActionsSelected = 0;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // IMPLEMENTATION: Set action preference with bounds checking
    if (action < ActionType::Wait || action > ActionType::LookRight) return;
    pImpl->actionPreferences[static_cast<int>(action)] = preference;
}

float MotorSystem::getActionPreference(ActionType action) const {
    // IMPLEMENTATION: Get action preference with bounds checking
    if (action < ActionType::Wait || action > ActionType::LookRight) return 0.0f;
    return pImpl->actionPreferences[static_cast<int>(action)];
}

void MotorSystem::updateActionProbabilities() {
    // IMPLEMENTATION: Real action selection based on preferences
    // Softmax selection based on preferences with exploration bonus
    float maxPref = pImpl->actionPreferences[0];
    for (int i = 1; i < 6; ++i) {
        if (pImpl->actionPreferences[i] > maxPref) {
            maxPref = pImpl->actionPreferences[i];
        }
    }
    
    float sumExp = 0.0f;
    for (int i = 0; i < 6; ++i) {
        // Add exploration bonus based on selection history
        float explorationBonus = 0.0f;
        if (pImpl->totalActionsSelected > 0) {
            // Reduce probability of recently selected actions
            int recentCount = 0;
            for (const auto& pastAction : pImpl->actionHistory) {
                if (pastAction == static_cast<ActionType>(i)) {
                    recentCount++;
                }
            }
            explorationBonus = -0.5f * (recentCount / static_cast<float>(pImpl->actionHistory.size()));
        }
        
        float adjustedPref = pImpl->actionPreferences[i] + explorationBonus;
        float expVal = std::exp(adjustedPref - maxPref);
        pImpl->actionProbabilities[i] = expVal;
        sumExp += expVal;
    }
    
    // Normalize probabilities
    if (sumExp > 0.0f) {
        for (int i = 0; i < 6; ++i) {
            pImpl->actionProbabilities[i] /= sumExp;
        }
    }
}

ActionType MotorSystem::selectAction() {
    // IMPLEMENTATION: Real action selection based on neural activity
    // Select action based on computed probabilities
    float r = static_cast<float>(rand()) / RAND_MAX;
    float cumulative = 0.0f;
    
    for (int i = 0; i < 6; ++i) {
        cumulative += pImpl->actionProbabilities[i];
        if (r <= cumulative) {
            // Record action selection for future exploration
            pImpl->totalActionsSelected++;
            pImpl->actionHistory.push_back(static_cast<ActionType>(i));
            
            // Keep history manageable
            if (pImpl->actionHistory.size() > 100) {
                pImpl->actionHistory.erase(pImpl->actionHistory.begin());
            }
            
            return static_cast<ActionType>(i);
        }
    }
    
    return ActionType::Wait;
}

void MotorSystem::reinforceAction(ActionType action, float reward) {
    // IMPLEMENTATION: Real action reinforcement based on reward
    if (action < ActionType::Wait || action > ActionType::LookRight) return;
    
    int idx = static_cast<int>(action);
    
    // Use reward prediction error for learning (like dopamine)
    float predictionError = reward - pImpl->actionProbabilities[idx]; // Simple approximation
    
    // Update action preference using reward-modulated learning
    float delta = predictionError * pImpl->learningRate * (1.0f + std::abs(predictionError));
    
    // Apply synaptic-like learning to action preferences
    pImpl->actionPreferences[idx] += delta;
    
    // Keep preferences in reasonable bounds and apply decay
    float decayFactor = 0.999f; // Slight decay over time
    pImpl->actionPreferences[idx] *= decayFactor;
    
    // Clamp preferences to reasonable range
    pImpl->actionPreferences[idx] = std::clamp(pImpl->actionPreferences[idx], -2.0f, 2.0f);
}

float MotorSystem::getActionProbability(ActionType action) const {
    // IMPLEMENTATION: Get action selection probability
    if (action < ActionType::Wait || action > ActionType::LookRight) return 0.0f;
    return pImpl->actionProbabilities[static_cast<int>(action)];
}

void MotorSystem::reset() {
    // IMPLEMENTATION: Reset action preferences and history
    for (int i = 0; i < 6; ++i) {
        pImpl->actionPreferences[i] = 0.0f;
        pImpl->actionProbabilities[i] = 1.0f / 6.0f;
    }
    pImpl->totalActionsSelected = 0;
    pImpl->actionHistory.clear();
}

} // namespace nlm
