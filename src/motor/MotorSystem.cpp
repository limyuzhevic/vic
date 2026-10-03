#include "MotorSystem.hpp"
#include "Action.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <numeric>
#include <functional>

namespace nlm {

struct MotorSystem::Impl {
    // Motor action selection
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Neural integration
    Brain* brain;
    float explorationRate;
    float actionSelectionThreshold;
    float actionTemperature;
    
    // Action preferences for exploration
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Neural activity patterns
    std::vector<NeuronId> motorNeurons;
    std::vector<float> neuralActivityPatterns;
    
    // Random number generator
    std::mt19937 rng;
    
    Impl() : brain(nullptr), explorationRate(0.1f), actionSelectionThreshold(0.5f), 
             actionTemperature(1.0f), rng(std::random_device{}()) {
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
            ActionType::Wait
        };
    }
};

MotorSystem::MotorSystem() 
    : pImpl(std::make_unique<Impl>()) 
    , brain_(nullptr) 
    , explorationRate_(0.1f)
    , actionSelectionThreshold_(0.5f)
    , actionTemperature_(1.0f)
    , rng_(std::random_device{}()) {
}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain) {
    pImpl->brain = brain_
    brain_ = brain;
    
    // Collect motor neuron IDs from all regions
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    for (const auto* neuron : pop->getNeurons()) {
                        pImpl->motorNeurons.push_back(neuron->getId());
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("MotorSystem initialized with " + std::to_string(pImpl->motorNeurons.size()) + " motor neurons");
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (!brain_) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity patterns from motor neurons
    updateNeuralActivityPatterns();
    
    // Compute action scores based on neural activity and preferences
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType action : pImpl->availableActions) {
        float neuralScore = computeNeuralActionScore(action);
        float preferenceScore = computePreferenceScore(action);
        float explorationScore = computeExplorationScore(action);
        
        // Combine scores with temperature scaling
        float combinedScore = (neuralScore * (1.0f - explorationRate_) + 
                              preferenceScore * explorationRate_ + 
                              explorationScore * 0.1f);
        
        // Apply action selection threshold
        if (combinedScore >= actionSelectionThreshold_) {
            actionScores.emplace_back(action, combinedScore);
        }
    }
    
    if (actionScores.empty()) {
        // No actions meet threshold - use temperature-based softmax
        return selectActionWithTemperature();
    }
    
    // Apply softmax with temperature to get probabilities
    std::vector<float> probabilities;
    float totalScore = 0.0f;
    
    for (const auto& pair : actionScores) {
        totalScore += std::exp(pair.second / actionTemperature_);
    }
    
    if (totalScore < 0.0001f) {
        // Very low scores - select randomly
        return selectActionRandom();
    }
    
    // Normalize to probabilities
    std::vector<float> probs;
    for (const auto& pair : actionScores) {
        probs.push_back(std::exp(pair.second / actionTemperature_) / totalScore);
    }
    
    // Select action based on probabilities
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float threshold = dist(pImpl->rng);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < actionScores.size(); ++i) {
        cumulative += probs[i];
        if (threshold <= cumulative) {
            auto action = std::make_unique<Action>(actionScores[i].first);
            addToActionHistory(action.get());
            NLM_LOG_DEBUG("Selected action: " + action->getName() + " (score: " + std::to_string(actionScores[i].second) + ")");
            return action;
        }
    }
    
    // Fallback
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Set preference for action " + std::to_string(static_cast<int>(action)) + " to " + std::to_string(preference));
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    NLM_LOG_DEBUG("Action history cleared");
}

std::vector<float> MotorSystem::getMotorActivityLevels() const {
    std::vector<float> activityLevels;
    if (brain_) {
        for (NeuronId neuronId : pImpl->motorNeurons) {
            float activity = getNeuronActivity(neuronId);
            activityLevels.push_back(activity);
        }
    }
    return activityLevels;
}

void MotorSystem::addToActionHistory(const Action* action) {
    pImpl->actionHistory.push_back(action);
    
    // Keep history size manageable
    if (pImpl->actionHistory.size() > 100) {
        for (size_t i = 0; i < pImpl->actionHistory.size() - 100; ++i) {
            delete pImpl->actionHistory[i];
        }
        pImpl->actionHistory.erase(pImpl->actionHistory.begin(), 
                                   pImpl->actionHistory.begin() + (pImpl->actionHistory.size() - 100));
    }
}

void MotorSystem::updateNeuralActivityPatterns() {
    pImpl->neuralActivityPatterns.clear();
    
    if (!brain_) return;
    
    // Sample neural activity patterns from motor neurons
    std::vector<float> currentPattern;
    
    for (NeuronId neuronId : pImpl->motorNeurons) {
        float activity = getNeuronActivity(neuronId);
        currentPattern.push_back(activity);
    }
    
    if (!currentPattern.empty()) {
        pImpl->neuralActivityPatterns = currentPattern;
    }
}

float MotorSystem::computeNeuralActionScore(ActionType action) const {
    // Map action type to neural pattern expectations
    std::vector<float> actionPattern = getActionNeuralPattern(action);
    
    if (pImpl->neuralActivityPatterns.empty() || actionPattern.empty()) {
        return 0.5f; // Neutral score
    }
    
    // Compute similarity between neural activity and action pattern
    return computePatternSimilarity(pImpl->neuralActivityPatterns, actionPattern);
}

float MotorSystem::computePreferenceScore(ActionType action) const {
    auto it = pImpl->actionPreferences.find(action);
    if (it != pImpl->actionPreferences.end()) {
        return it->second;
    }
    
    // Default preference
    return 0.5f;
}

float MotorSystem::computeExplorationScore(ActionType action) const {
    // Use neural diversity for exploration score
    if (pImpl->neuralActivityPatterns.empty()) {
        return 0.5f;
    }
    
    // Compute diversity of neural activity
    float meanActivity = std::accumulate(pImpl->neuralActivityPatterns.begin(), 
                                        pImpl->neuralActivityPatterns.end(), 0.0f) / 
                       pImpl->neuralActivityPatterns.size();
    
    float variance = 0.0f;
    for (float activity : pImpl->neuralActivityPatterns) {
        variance += (activity - meanActivity) * (activity - meanActivity);
    }
    variance /= pImpl->neuralActivityPatterns.size();
    
    // Higher variance -> more exploration opportunity
    return std::min(1.0f, variance * 2.0f);
}

bool MotorSystem::isActionPreferred(ActionType action) const {
    auto it = pImpl->actionPreferences.find(action);
    return it != pImpl->actionPreferences.end() && it->second > 0.7f;
}

float MotorSystem::getActionPreferenceScore(ActionType action) const {
    auto it = pImpl->actionPreferences.find(action);
    return it != pImpl->actionPreferences.end() ? it->second : 0.5f;
}

std::unique_ptr<Action> MotorSystem::selectActionWithTemperature() {
    // Temperature-based action selection when neural activity is low
    std::vector<float> probabilities;
    
    // Compute softmax over all actions
    float totalExp = 0.0f;
    for (ActionType action : pImpl->availableActions) {
        float score = computePreferenceScore(action);
        totalExp += std::exp(score / actionTemperature_);
    }
    
    if (totalExp < 0.0001f) {
        return selectActionRandom();
    }
    
    // Sample from distribution
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float threshold = dist(pImpl->rng);
    float cumulative = 0.0f;
    
    for (ActionType action : pImpl->availableActions) {
        float score = computePreferenceScore(action);
        float prob = std::exp(score / actionTemperature_) / totalExp;
        cumulative += prob;
        
        if (threshold <= cumulative) {
            auto selectedAction = std::make_unique<Action>(action);
            addToActionHistory(selectedAction.get());
            return selectedAction;
        }
    }
    
    return selectActionRandom();
}

std::unique_ptr<Action> MotorSystem::selectActionRandom() {
    std::uniform_int_distribution<size_t> dist(0, pImpl->availableActions.size() - 1);
    size_t index = dist(pImpl->rng);
    
    auto action = std::make_unique<Action>(pImpl->availableActions[index]);
    addToActionHistory(action.get());
    
    NLM_LOG_DEBUG("Randomly selected action: " + action->getName());
    return action;
}

float MotorSystem::getNeuronActivity(NeuronId neuronId) const {
    // Find neuron in brain and get its activity
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron->getState().membranePotential;
                }
            }
        }
    }
    return 0.0f;
}

std::vector<float> MotorSystem::getActionNeuralPattern(ActionType action) const {
    // Return expected neural pattern for each action type
    // This is a simplified implementation - in a real system, this would be learned
    
    switch (action) {
        case ActionType::MoveForward:
            return {0.8f, 0.3f, 0.1f, 0.2f, 0.0f, 0.1f}; // Higher motor neuron activity
        case ActionType::MoveBackward:
            return {0.2f, 0.7f, 0.3f, 0.1f, 0.0f, 0.2f};
        case ActionType::MoveLeft:
            return {0.3f, 0.2f, 0.8f, 0.1f, 0.0f, 0.3f};
        case ActionType::MoveRight:
            return {0.2f, 0.3f, 0.1f, 0.8f, 0.0f, 0.2f};
        case ActionType::TurnLeft:
            return {0.1f, 0.2f, 0.3f, 0.4f, 0.9f, 0.1f}; // Higher head/neck neuron activity
        case ActionType::TurnRight:
            return {0.2f, 0.3f, 0.2f, 0.3f, 0.1f, 0.9f};
        case ActionType::Look:
            return {0.3f, 0.4f, 0.2f, 0.2f, 0.1f, 0.8f};
        case ActionType::Interact:
            return {0.4f, 0.2f, 0.3f, 0.3f, 0.7f, 0.2f}; // Higher precision neuron activity
        default:
            return {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f}; // Default neutral pattern
    }
}

float MotorSystem::computePatternSimilarity(const std::vector<float>& pattern1, 
                                          const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.5f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) {
        return 0.5f;
    }
    
    return dotProduct / (norm1 * norm2);
}

} // namespace nlm