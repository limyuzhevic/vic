#include "MotorSystem.hpp"
#include "Action.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action selection statistics
    std::unordered_map<ActionType, int> selectionCounts;
    std::unordered_map<ActionType, int> successCounts;
    int totalSelections = 0;
    float explorationRate = 0.1f;
    float exploitationRate = 0.9f;
    float temperature = 1.0f; // For softmax selection
    float noiseLevel = 0.05f; // For exploration noise
    
    // Action preferences and weights
    std::unordered_map<ActionType, float> actionPreferences;
    float maxPreference = 2.0f;
    
    // Input preprocessing filters
    float motorThreshold = 0.1f; // Minimum neural activity for action
    float actionGain = 1.0f; // Scaling factor for neural drive
    
    // History and decay
    static constexpr int MAX_HISTORY = 100;
    
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
        
        // Initialize preferences (balanced initial state)
        for (ActionType action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    return selectActionWithPreprocessing(motorRegion);
}

std::unique_ptr<Action> MotorSystem::selectActionWithPreprocessing(const NeuralRegion& motorRegion) {
    // Enhanced action selection with input preprocessing
    
    // 1. Input preprocessing: extract neural activity patterns
    std::vector<std::pair<NeuronId, float>> neuronActivations;
    
    for (auto* neuron : motorRegion.getNeurons()) {
        float firingRate = neuron->getFiringRate();
        float membranePotential = neuron->getMembranePotential();
        
        // Combine firing rate and membrane potential for better signal
        float activation = firingRate * 0.7f + 
                          std::clamp((membranePotential + 70.0f) * 0.03f, 0.0f, 1.0f) * 0.3f;
        
        if (activation > pImpl->motorThreshold) {
            neuronActivations.emplace_back(neuron->getId(), activation);
        }
    }
    
    if (neuronActivations.empty()) {
        // No sufficient neural activity, default to Wait
        auto action = std::make_unique<Action>(ActionType::Wait);
        recordActionOutcome(ActionType::Wait, false); // Default action considered failure
        return action;
    }
    
    // 2. Map neural activations to action preferences
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (ActionType action : pImpl->availableActions) {
        // Base score from preference
        float score = pImpl->actionPreferences[action];
        
        // Neural modulation: different actions activate different neuron populations
        // This is a simplified model - in reality, you'd have action-specific neuron groups
        float neuralModulation = 1.0f;
        
        // Add neural activity contribution (with noise for exploration)
        float neuralContribution = 0.0f;
        for (auto& activation : neuronActivations) {
            // Add random component for exploration
            float noise = (std::rand() / (RAND_MAX + 1.0f) - 0.5f) * 2.0f * pImpl->noiseLevel;
            neuralContribution += activation.second * (1.0f + noise);
        }
        
        // Normalize and apply
        float avgNeuralActivity = neuralContribution / static_cast<float>(neuronActivations.size());
        neuralModulation = 1.0f + avgNeuralActivity * pImpl->actionGain * 0.1f;
        
        // Temperature-controlled softmax selection
        if (pImpl->temperature > 0.0f) {
            float preferenceScore = std::log(std::max(pImpl->actionPreferences[action], 0.01f)) * 
                                  (pImpl->exploitationRate + pImpl->explorationRate);
            score = preferenceScore * neuralModulation;
        } else {
            // Pure neural modulation (high exploration)
            score = avgNeuralActivity * pImpl->actionGain;
        }
        
        // Apply noise for exploration
        if (avgNeuralActivity < 0.05f) {
            // Low activity areas get more exploration
            score += (std::rand() / (RAND_MAX + 1.0f) - 0.5f) * 2.0f;
        }
        
        actionScores.emplace_back(action, score);
    }
    
    // 3. Action selection with softmax probability distribution
    // Find maximum score for deterministic fallback
    float maxScore = -1e6f;
    ActionType bestAction = ActionType::Wait;
    for (auto& pair : actionScores) {
        if (pair.second > maxScore) {
            maxScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Apply softmax with temperature
    if (pImpl->temperature > 0.1f) {
        // Softmax selection
        std::vector<float> probabilities;
        float sum = 0.0f;
        
        for (auto& pair : actionScores) {
            float expScore = std::exp(pair.second / pImpl->temperature);
            probabilities.push_back(expScore);
            sum += expScore;
        }
        
        // Normalize to get probabilities
        for (float& prob : probabilities) {
            prob /= sum;
        }
        
        // Select action based on probabilities
        float selection = (std::rand() / (RAND_MAX + 1.0f)) * sum;
        float cumulative = 0.0f;
        std::unique_ptr<Action> selectedAction;
        
        for (size_t i = 0; i < actionScores.size(); ++i) {
            cumulative += probabilities[i];
            if (selection <= cumulative) {
                selectedAction = std::make_unique<Action>(actionScores[i].first);
                break;
            }
        }
        
        if (!selectedAction) {
            selectedAction = std::make_unique<Action>(bestAction);
        }
        
        // Record selection
        pImpl->selectionCounts[selectedAction->getType()]++;
        pImpl->totalSelections++;
        pImpl->actionHistory.push_back(selectedAction.get());
        
        // Maintain history size
        if (pImpl->actionHistory.size() > pImpl->MAX_HISTORY) {
            delete pImpl->actionHistory.front();
            pImpl->actionHistory.erase(pImpl->actionHistory.begin());
        }
        
        return selectedAction;
    } else {
        // Deterministic selection (high exploitation)
        auto action = std::make_unique<Action>(bestAction);
        pImpl->selectionCounts[bestAction]++;
        pImpl->totalSelections++;
        pImpl->actionHistory.push_back(action.get());
        
        if (pImpl->actionHistory.size() > pImpl->MAX_HISTORY) {
            delete pImpl->actionHistory.front();
            pImpl->actionHistory.erase(pImpl->actionHistory.begin());
        }
        
        return action;
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set action preference with bounds and smoothing
    preference = std::clamp(preference, 0.0f, pImpl->maxPreference);
    
    // Exponential moving average to prevent abrupt preference changes
    float currentPref = pImpl->actionPreferences[action];
    pImpl->actionPreferences[action] = currentPref * 0.8f + preference * 0.2f;
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    
    // Reset statistics
    pImpl->selectionCounts.clear();
    pImpl->successCounts.clear();
    pImpl->totalSelections = 0;
}

MotorSystem::SelectionStats MotorSystem::getSelectionStats() const {
    SelectionStats stats;
    stats.selectionCounts = pImpl->selectionCounts;
    stats.totalSelections = pImpl->totalSelections;
    stats.explorationRate = pImpl->explorationRate;
    stats.exploitationRate = pImpl->exploitationRate;
    stats.temperature = pImpl->temperature;
    return stats;
}

void MotorSystem::resetStats() {
    pImpl->selectionCounts.clear();
    pImpl->successCounts.clear();
    pImpl->totalSelections = 0;
    pImpl->explorationRate = 0.1f;
    pImpl->exploitationRate = 0.9f;
    pImpl->temperature = 1.0f;
}

float MotorSystem::getActionSuccessRate(ActionType action) const {
    int selections = pImpl->selectionCounts[action];
    int successes = pImpl->successCounts[action];
    
    if (selections == 0) return 0.0f;
    return static_cast<float>(successes) / static_cast<float>(selections);
}

void MotorSystem::recordActionOutcome(ActionType action, bool success) {
    pImpl->successCounts[action] += success ? 1 : 0;
}

} // namespace nlm
