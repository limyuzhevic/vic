#include "MotorSystem.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/NeuralPopulation.hpp"
#include "../../brain/Neuron.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action preferences for exploration bias
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Action statistics for learning
    struct ActionStats {
        float successCount = 0.0f;
        float failureCount = 0.0f;
        float totalCount = 0.0f;
        float lastObserved = 0.0f;
    };
    std::unordered_map<ActionType, ActionStats> actionStats;
    
    // Action selection parameters
    float explorationRate = 0.1f;      // Probability of random action
    float exploitationRate = 0.9f;    // Weight for high-activity actions
    float curiosityMultiplier = 1.5f; // How curiosity affects exploration
    float temperature = 1.0f;         // Softmax temperature for action selection
    
    // Historical action tracking
    std::vector<ActionType> recentActions;
    size_t maxRecentActions = 10;
    
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
        for (ActionType action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion,
                                                 float curiosityLevel,
                                                 bool exploreEnabled) {
    // Get all motor neurons from the region
    std::vector<Neuron*> motorNeurons;
    for (auto& population : motorRegion.getPopulations()) {
        for (Neuron* neuron : population->getNeurons()) {
            if (neuron->getType() == NeuronType::Motor) {
                motorNeurons.push_back(neuron);
            }
        }
    }
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate action activity based on neuron firing
    std::unordered_map<ActionType, float> actionActivity;
    for (ActionType action : pImpl->availableActions) {
        actionActivity[action] = 0.0f;
    }
    
    // Map neurons to actions based on population type
    for (size_t i = 0; i < motorNeurons.size() && i < pImpl->availableActions.size(); ++i) {
        Neuron* neuron = motorNeurons[i];
        ActionType action = pImpl->availableActions[i];
        
        // Calculate neural activity from membrane potential
        float activity = std::abs(neuron->getState().membranePotential - 
                                neuron->getState().restingPotential);
        activity = std::clamp(activity, 0.0f, 1.0f);
        
        actionActivity[action] += activity;
    }
    
    // Determine whether to explore or exploit
    float exploreChance = pImpl->explorationRate;
    if (exploreEnabled) {
        exploreChance *= (1.0f + curiosityLevel * pImpl->curiosityMultiplier);
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Explore: choose action based on preferences and statistics
    if (dis(gen) < exploreChance) {
        return selectActionWithExploration(actionActivity);
    }
    
    // Exploit: choose action with highest neural activity
    return selectActionWithActivity(actionActivity);
}

std::unique_ptr<Action> MotorSystem::selectActionWithActivity(
    const std::unordered_map<ActionType, float>& actionActivity) {
    
    // Find action with highest activity
    ActionType bestAction = ActionType::Wait;
    float bestActivity = 0.0f;
    
    for (const auto& pair : actionActivity) {
        ActionType action = pair.first;
        float activity = pair.second * pImpl->exploitationRate;
        
        // Apply preference weight
        float preference = pImpl->actionPreferences[action];
        activity *= preference;
        
        if (activity > bestActivity) {
            bestActivity = activity;
            bestAction = action;
        }
    }
    
    // Create action with default parameters
    return createActionFromType(bestAction);
}

std::unique_ptr<Action> MotorSystem::selectActionWithExploration(
    const std::unordered_map<ActionType, float>& actionActivity) {
    
    // Softmax action selection with temperature
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (const auto& pair : actionActivity) {
        ActionType action = pair.first;
        float activity = pair.second;
        
        // Apply preference weight and temperature
        float preference = pImpl->actionPreferences[action];
        float preferenceLog = std::log(std::max(preference, 0.01f));
        
        // Add statistics-based bias
        const auto& stats = pImpl->actionStats[action];
        float successRate = stats.totalCount > 0 ? 
            (stats.successCount / stats.totalCount) : 0.5f;
        
        float score = activity * 0.6f + 
                     preferenceLog * 0.3f + 
                     successRate * 0.1f;
        
        actionScores.push_back({action, score});
    }
    
    // Add some randomness for exploration
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    float exploreBonus = dis(gen) * 0.5f; // Up to 50% random influence
    
    for (auto& pair : actionScores) {
        pair.second += exploreBonus;
    }
    
    // Apply softmax with temperature
    std::vector<float> expScores;
    float sumExp = 0.0f;
    
    for (const auto& pair : actionScores) {
        float score = pair.second / pImpl->temperature;
        float expScore = std::exp(score);
        expScores.push_back(expScore);
        sumExp += expScore;
    }
    
    // Choose action based on softmax probabilities
    std::uniform_real_distribution<float> dis2(0.0f, sumExp);
    float threshold = dis(gen);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < actionScores.size(); ++i) {
        cumulative += expScores[i];
        if (threshold <= cumulative) {
            return createActionFromType(actionScores[i].first);
        }
    }
    
    // Fallback to first action
    return createActionFromType(actionScores[0].first);
}

std::unique_ptr<Action> MotorSystem::createActionFromType(ActionType type) {
    // Create action with appropriate parameters based on type
    std::vector<float> parameters;
    
    switch (type) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
        case ActionType::MoveLeft:
        case ActionType::MoveRight:
            parameters = {1.0f, 1.0f}; // speed, duration
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            parameters = {45.0f, 0.5f}; // angle, speed
            break;
            
        case ActionType::Look:
        case ActionType::LookUp:
        case ActionType::LookDown:
            parameters = {30.0f, 0.8f}; // gaze angle, smoothness
            break;
            
        case ActionType::Interact:
            parameters = {0.5f, 1.0f}; // strength, duration
            break;
            
        case ActionType::Wait:
            parameters = {0.0f, 1.0f}; // wait duration
            break;
            
        default:
            parameters = {};
            break;
    }
    
    return std::make_unique<Action>(type, parameters);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    preference = std::clamp(preference, 0.0f, 10.0f);
    pImpl->actionPreferences[action] = preference;
}

float MotorSystem::getActionPreference(ActionType action) const {
    auto it = pImpl->actionPreferences.find(action);
    return it != pImpl->actionPreferences.end() ? it->second : 1.0f;
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    pImpl->recentActions.clear();
}

void MotorSystem::recordActionOutcome(const Action& action, float reward, float predictedReward) {
    ActionType actionType = action.getType();
    
    // Record in history
    pImpl->actionHistory.push_back(&action);
    
    // Track in recent actions
    pImpl->recentActions.push_back(actionType);
    if (pImpl->recentActions.size() > pImpl->maxRecentActions) {
        pImpl->recentActions.erase(pImpl->recentActions.begin());
    }
    
    // Update statistics
    auto& stats = pImpl->actionStats[actionType];
    stats.totalCount++;
    
    if (reward > predictedReward) {
        stats.successCount++;
    } else {
        stats.failureCount++;
    }
    
    stats.lastObserved = reward;
    
    // Adjust preferences based on outcome
    if (reward > predictedReward) {
        // Successful action: increase preference
        pImpl->actionPreferences[actionType] *= 1.05f;
    } else if (reward < predictedReward) {
        // Failed action: decrease preference
        pImpl->actionPreferences[actionType] *= 0.95f;
    }
}

void MotorSystem::updateActionPreferences(float explorationRate, float exploitationRate) {
    pImpl->explorationRate = std::clamp(explorationRate, 0.0f, 1.0f);
    pImpl->exploitationRate = std::clamp(exploitationRate, 0.0f, 1.0f);
}

void MotorSystem::reset() {
    clearHistory();
    
    // Reset action statistics
    for (auto& pair : pImpl->actionStats) {
        pair.second = ActionStats();
    }
    
    // Reset preferences to baseline
    for (ActionType action : pImpl->availableActions) {
        pImpl->actionPreferences[action] = 1.0f;
    }
    
    pImpl->explorationRate = 0.1f;
    pImpl->exploitationRate = 0.9f;
    pImpl->curiosityMultiplier = 1.5f;
    pImpl->temperature = 1.0f;
}

float MotorSystem::getActionSuccessRate(ActionType action) const {
    const auto& it = pImpl->actionStats.find(action);
    if (it == pImpl->actionStats.end() || it->second.totalCount == 0) {
        return 0.0f;
    }
    return it->second.successCount / it->second.totalCount;
}

float MotorSystem::getActionFrequency(ActionType action) const {
    int count = 0;
    for (ActionType recentAction : pImpl->recentActions) {
        if (recentAction == action) {
            count++;
        }
    }
    return static_cast<float>(count) / std::max(pImpl->recentActions.size(), 1);
}

} // namespace nlm
