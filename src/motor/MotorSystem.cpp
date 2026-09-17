#include "MotorSystem.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action selection parameters
    float explorationRate;           // Probability of exploring random actions (0.0-1.0)
    float exploitationWeight;        // Weight given to high-activity neurons (0.0-1.0)
    float noiseLevel;                 // Random noise for exploration (0.0-1.0)
    std::mt19937 rng;                 // Random number generator
    
    Impl() : explorationRate(0.1f), exploitationWeight(0.7f), noiseLevel(0.05f),
             rng(std::random_device()()) {
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
    // Combines exploitation of high-activity neurons with exploration
    
    // Get all neurons in the motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        // No neurons, return wait action
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Decide whether to explore or exploit
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float decision = dist(pImpl->rng);
    
    if (decision < pImpl->explorationRate) {
        // Exploration: choose random action with noise
        std::uniform_int_distribution<size_t> actionDist(0, pImpl->availableActions.size() - 1);
        size_t randomIndex = actionDist(pImpl->rng);
        
        // Add small noise to selection probability
        std::uniform_real_distribution<float> noiseDist(-pImpl->noiseLevel, pImpl->noiseLevel);
        float noise = noiseDist(pImpl->rng);
        
        // Occasionally prefer certain actions during exploration
        float bias = 0.0f;
        if (randomIndex < 3) {  // Forward, backward, turn
            bias = 0.1f + noise;
        }
        
        float finalProb = std::max(0.0f, std::min(1.0f, bias));
        if (finalProb > 0.5f) {
            return std::make_unique<Action>(pImpl->availableActions[randomIndex]);
        }
    }
    
    // Exploitation: select based on neural activity
    std::vector<std::pair<float, ActionType>> actionScores;
    
    // Map neural activity to action preferences
    for (size_t i = 0; i < neurons.size() && i < pImpl->availableActions.size(); ++i) {
        // Get neuron firing rate
        float firingRate = neurons[i]->getFiringRate();
        
        // Scale firing rate to action preference (0-1 range)
        float normalizedFiring = std::min(1.0f, firingRate / 50.0f);  // Assume max 50 Hz
        
        // Apply exploitation weight
        float score = normalizedFiring * pImpl->exploitationWeight;
        
        // Add noise for stochasticity
        std::uniform_real_distribution<float> noiseDist(-pImpl->noiseLevel, pImpl->noiseLevel);
        score += noiseDist(pImpl->rng);
        
        actionScores.push_back(std::make_pair(score, pImpl->availableActions[i]));
    }
    
    if (actionScores.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find action with highest score
    auto bestAction = std::max_element(actionScores.begin(), actionScores.end(),
                                      [](const std::pair<float, ActionType>& a,
                                         const std::pair<float, ActionType>& b) {
                                          return a.first < b.first;
                                      });
    
    // Check if score is above threshold to ensure meaningful selection
    if (bestAction->first > 0.1f) {
        return std::make_unique<Action>(bestAction->second);
    } else {
        // Low score - default to wait
        return std::make_unique<Action>(ActionType::Wait);
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO: Implement action preference system for learning preferences
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
