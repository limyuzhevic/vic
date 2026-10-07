#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct MotorSystem::Impl {
    // Action selection parameters
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Neural integration
    std::vector<float> neuralActivity;
    std::vector<float> actionValues;
    std::vector<float> actionPreferences;
    std::vector<float> actionBiases;
    
    // Selection mechanism parameters
    float selectionNoise;
    float explorationRate;
    float softmaxTemperature;
    float incentiveSalience;
    
    // Reinforcement learning components
    std::vector<float> actionRewards;
    std::vector<float> actionQValues;
    float learningRate;
    float discountFactor;
    
    // Neuromodulatory influences
    float dopamineLevel;
    float acetylcholineLevel;
    
    // Action history and statistics
    std::vector<float> actionHistoryCounts;
    std::vector<float> actionRecentRewards;
    float explorationHistory;
    float exploitationHistory;
    
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
        
        // Initialize neural-related vectors
        neuralActivity.resize(availableActions.size(), 0.0f);
        actionValues.resize(availableActions.size(), 0.5f);
        actionPreferences.resize(availableActions.size(), 1.0f);
        actionBiases.resize(availableActions.size(), 0.0f);
        actionRewards.resize(availableActions.size(), 0.0f);
        actionQValues.resize(availableActions.size(), 0.5f);
        actionHistoryCounts.resize(availableActions.size(), 0.0f);
        actionRecentRewards.resize(availableActions.size(), 0.0f);
        
        // Selection parameters
        selectionNoise = 0.1f;
        explorationRate = 0.1f;
        softmaxTemperature = 1.0f;
        incentiveSalience = 1.0f;
        learningRate = 0.1f;
        discountFactor = 0.95f;
        
        // Neuromodulatory levels
        dopamineLevel = 0.2f;
        acetylcholineLevel = 0.3f;
        
        explorationHistory = 0.0f;
        exploitationHistory = 0.0f;
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Compute neural activity to action mapping
    updateNeuralActivity(motorRegion);
    
    // Compute action values based on neural activity and preferences
    computeActionValues();
    
    // Apply exploration-exploitation tradeoff
    std::vector<float> selectionProbabilities = computeSelectionProbabilities();
    
    // Select action based on probabilities
    float randomValue = static_cast<float>(rand()) / RAND_MAX;
    float cumulativeProb = 0.0f;
    
    for (size_t i = 0; i < availableActions.size(); ++i) {
        cumulativeProb += selectionProbabilities[i];
        if (randomValue <= cumulativeProb) {
            ActionType selectedType = availableActions[i];
            auto action = std::make_unique<Action>(selectedType);
            action->setValue(actionValues[i]);
            action->setNeuralActivity(neuralActivity[i]);
            
            // Record action in history
            pImpl->actionHistory.push_back(action.get());
            pImpl->actionHistoryCounts[i] += 1.0f;
            
            return action;
        }
    }
    
    // Fallback to random action if selection fails
    if (!pImpl->availableActions.empty()) {
        ActionType selectedType = pImpl->availableActions[0];
        auto action = std::make_unique<Action>(selectedType);
        action->setValue(actionValues[0]);
        action->setNeuralActivity(neuralActivity[0]);
        pImpl->actionHistory.push_back(action.get());
        return action;
    }
    
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Find the action index
    auto it = std::find(pImpl->availableActions.begin(), 
                       pImpl->availableActions.end(), action);
    
    if (it != pImpl->availableActions.end()) {
        size_t index = std::distance(pImpl->availableActions.begin(), it);
        pImpl->actionPreferences[index] = std::clamp(preference, 0.0f, 2.0f);
        
        // Update action biases based on preferences
        pImpl->actionBiases[index] = preference * 0.5f;
        
        // Apply immediate Q-value update for learned preferences
        pImpl->actionQValues[index] = 0.7f * pImpl->actionQValues[index] + 0.3f * preference;
        
        NLM_LOG_DEBUG("Action preference updated: action=" + std::to_string(static_cast<int>(action)) +
                      " preference=" + std::to_string(preference) +
                      " bias=" + std::to_string(pImpl->actionBiases[index]));
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
    
    // Reset action statistics
    for (auto& reward : pImpl->actionRewards) {
        reward = 0.0f;
    }
    for (auto& qValue : pImpl->actionQValues) {
        qValue = 0.5f;
    }
    
    NLM_LOG_INFO("Action history cleared");
}

// Update action reward
void MotorSystem::updateActionReward(ActionType action, float reward) {
    auto it = std::find(pImpl->availableActions.begin(), 
                       pImpl->availableActions.end(), action);
    
    if (it != pImpl->availableActions.end()) {
        size_t index = std::distance(pImpl->availableActions.begin(), it);
        
        // Update reward history
        pImpl->actionRewards[index] = 0.9f * pImpl->actionRewards[index] + 0.1f * reward;
        
        // Update recent reward average
        pImpl->actionRecentRewards[index] = 0.95f * pImpl->actionRecentRewards[index] + 0.05f * reward;
        
        // Update Q-value using temporal difference learning
        float oldQ = pImpl->actionQValues[index];
        float maxQ = *std::max_element(pImpl->actionQValues.begin(), pImpl->actionQValues.end());
        pImpl->actionQValues[index] = oldQ + pImpl->learningRate * (reward + pImpl->discountFactor * maxQ - oldQ);
        
        NLM_LOG_DEBUG("Action reward updated: action=" + std::to_string(static_cast<int>(action)) +
                      " reward=" + std::to_string(reward) +
                      " qvalue=" + std::to_string(pImpl->actionQValues[index]) +
                      " average=" + std::to_string(pImpl->actionRecentRewards[index]));
    }
}

// Get current action statistics
MotorStats MotorSystem::getStats() const {
    MotorStats stats;
    
    // Calculate exploration vs exploitation
    stats.explorationRate = pImpl->explorationRate;
    stats.exploitationRate = pImpl->exploitationRate;
    
    // Calculate average neural activity
    if (!pImpl->neuralActivity.empty()) {
        float sum = std::accumulate(pImpl->neuralActivity.begin(), 
                                   pImpl->neuralActivity.end(), 0.0f);
        stats.averageNeuralActivity = sum / pImpl->neuralActivity.size();
    }
    
    // Calculate action diversity
    float maxCount = 0.0f;
    for (float count : pImpl->actionHistoryCounts) {
        if (count > maxCount) maxCount = count;
    }
    if (maxCount > 0.0f) {
        float sumCounts = std::accumulate(pImpl->actionHistoryCounts.begin(), 
                                         pImpl->actionHistoryCounts.end(), 0.0f);
        stats.actionDiversity = (sumCounts / pImpl->actionHistoryCounts.size()) / maxCount;
    }
    
    // Calculate average reward
    if (!pImpl->actionRewards.empty()) {
        float sumRewards = std::accumulate(pImpl->actionRewards.begin(), 
                                          pImpl->actionRewards.end(), 0.0f);
        stats.averageReward = sumRewards / pImpl->actionRewards.size();
    }
    
    return stats;
}

// Update neural activity from motor region
void MotorSystem::updateNeuralActivity(const NeuralRegion& motorRegion) {
    // Get neural activity from motor region
    std::vector<float> regionActivity = motorRegion.getActivity();
    
    // Map region activity to actions (simplified mapping)
    for (size_t i = 0; i < std::min(regionActivity.size(), pImpl->availableActions.size()); ++i) {
        // Apply neuromodulatory influences
        float dopamineModulation = 1.0f + (pImpl->dopamineLevel * 0.5f);
        float achModulation = 1.0f + (pImpl->acetylcholineLevel * 0.3f);
        
        // Compute neural activity with modulation
        float activity = regionActivity[i] * dopamineModulation * achModulation;
        
        // Apply exploration bias
        float explorationBias = 1.0f + (rand() / (RAND_MAX + 1.0f) - 0.5f) * pImpl->selectionNoise;
        
        pImpl->neuralActivity[i] = activity * explorationBias;
        
        // Clamp to reasonable range
        pImpl->neuralActivity[i] = std::clamp(pImpl->neuralActivity[i], 0.0f, 2.0f);
    }
}

// Compute action values using Q-learning style update
void MotorSystem::computeActionValues() {
    for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
        // Base value from neural activity
        float neuralValue = pImpl->neuralActivity[i];
        
        // Add preference bonus
        float preferenceBonus = pImpl->actionPreferences[i];
        
        // Add bias
        float bias = pImpl->actionBiases[i];
        
        // Apply incentive salience
        float incentive = neuralValue * pImpl->incentiveSalience;
        
        // Combine all factors
        float newValue = neuralValue * 0.4f + preferenceBonus * 0.3f + 
                        bias * 0.2f + incentive * 0.1f;
        
        // Apply softmax transformation for action selection
        float expValue = std::exp(newValue / pImpl->softmaxTemperature);
        pImpl->actionValues[i] = expValue / (1.0f + expValue);
        
        // Update Q-values using simple learning rule
        float reward = pImpl->actionRewards[i];
        float oldQ = pImpl->actionQValues[i];
        float maxQ = *std::max_element(pImpl->actionQValues.begin(), pImpl->actionQValues.end());
        
        // Q-learning update
        pImpl->actionQValues[i] = oldQ + pImpl->learningRate * 
                                 (reward + pImpl->discountFactor * maxQ - oldQ);
    }
}

// Compute action selection probabilities
std::vector<float> MotorSystem::computeSelectionProbabilities() {
    std::vector<float> probabilities;
    
    // If no action has meaningful value, use uniform distribution for exploration
    bool hasMeaningfulValue = false;
    for (float value : pImpl->actionValues) {
        if (std::abs(value - 0.5f) > 0.1f) {
            hasMeaningfulValue = true;
            break;
        }
    }
    
    if (!hasMeaningfulValue) {
        // Pure exploration
        float uniformProb = 1.0f / pImpl->availableActions.size();
        probabilities.assign(pImpl->availableActions.size(), uniformProb);
        explorationHistory = 1.0f;
        exploitationHistory = 0.0f;
        return probabilities;
    }
    
    // Weighted selection based on action values
    float totalWeight = std::accumulate(pImpl->actionValues.begin(), 
                                       pImpl->actionValues.end(), 0.0f);
    
    if (totalWeight > 0.0f) {
        for (float value : pImpl->actionValues) {
            probabilities.push_back(value / totalWeight);
        }
    } else {
        // Fallback to uniform distribution
        float uniformProb = 1.0f / pImpl->availableActions.size();
        probabilities.assign(pImpl->availableActions.size(), uniformProb);
    }
    
    // Apply exploration bonus
    for (size_t i = 0; i < probabilities.size(); ++i) {
        // Add exploration bonus based on history
        float explorationBonus = explorationRate * 
                                 std::exp(-pImpl->actionHistoryCounts[i] / 10.0f);
        
        probabilities[i] *= (1.0f + explorationBonus);
    }
    
    // Normalize probabilities
    float probSum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
    if (probSum > 0.0f) {
        for (float& prob : probabilities) {
            prob /= probSum;
        }
    }
    
    explorationHistory = explorationRate;
    exploitationHistory = 1.0f - explorationRate;
    
    return probabilities;
}

} // namespace nlm