#include "MotorSystem.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences; // Preference weights for each action
    float explorationRate;
    float softmaxTemperature;
    mutable std::mt19937 rng;
    
    Impl() : explorationRate(0.1f), softmaxTemperature(1.0f), rng(std::random_device{}()) {
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
            ActionType::Wait,
            ActionType::Custom
        };
        actionPreferences.resize(availableActions.size(), 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity
    // Uses softmax policy with neural activity as preferences
    // Implements exploration/exploitation tradeoff
    
    // Get neural activity from motor region
    float regionActivity = motorRegion.getTotalActivity();
    
    // Calculate action preferences based on neural activity
    std::vector<float> actionScores(availableActions.size(), 0.0f);
    
    // Get neurons in the motor region
    const auto& neurons = motorRegion.getNeurons();
    
    if (!neurons.empty()) {
        // Calculate action preferences based on neural activity patterns
        for (size_t i = 0; i < availableActions.size() && i < neurons.size(); ++i) {
            const Neuron* neuron = neurons[i];
            
            // Base preference from neuron activity
            float neuronActivity = neuron->getActivity();
            float basePreference = neuronActivity * pImpl->actionPreferences[i];
            
            // Add modulation based on other neurons for context
            for (size_t j = 0; j < neurons.size(); ++j) {
                if (i != j) {
                    const Neuron* otherNeuron = neurons[j];
                    float otherActivity = otherNeuron->getActivity();
                    
                    // Context modulation: similar activities increase preference
                    float similarity = std::exp(-std::abs(neuronActivity - otherActivity));
                    basePreference += similarity * otherActivity * 0.1f;
                }
            }
            
            // Add neuromodulation effects
            float neuromodulation = 1.0f;
            // In real implementation, would integrate with Dopamine system
            // For now, use a simple scaling
            neuromodulation = 0.5f + 0.5f * std::tanh(regionActivity);
            
            actionScores[i] = basePreference * neuromodulation;
        }
    } else {
        // Fallback to preference-based selection
        for (size_t i = 0; i < availableActions.size(); ++i) {
            actionScores[i] = pImpl->actionPreferences[i];
        }
    }
    
    // Apply exploration using softmax policy
    std::vector<float> probabilities;
    float sum = 0.0f;
    
    for (float score : actionScores) {
        // Add exploration noise
        std::uniform_real_distribution<float> noiseDist(-1.0f, 1.0f);
        float noisyScore = score + pImpl->explorationRate * noiseDist(pImpl->rng);
        
        float expScore = std::exp(noisyScore / pImpl->softmaxTemperature);
        probabilities.push_back(expScore);
        sum += expScore;
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sum;
    }
    
    // Select action based on probabilities
    float selection = std::uniform_real_distribution<float>(0.0f, 1.0f)(pImpl->rng);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < availableActions.size(); ++i) {
        cumulative += probabilities[i];
        if (selection <= cumulative) {
            auto action = std::make_unique<Action>(availableActions[i]);
            
            // Add neural context as parameters
            std::vector<float> params;
            params.push_back(regionActivity);
            params.push_back(static_cast<float>(actionScores[i]));
            
            // Set parameters if action supports them
            if (availableActions[i] == ActionType::MoveForward || 
                availableActions[i] == ActionType::MoveBackward ||
                availableActions[i] == ActionType::MoveLeft || 
                availableActions[i] == ActionType::MoveRight) {
                params.push_back(regionActivity * 0.1f); // Movement speed
            } else if (availableActions[i] == ActionType::TurnLeft || 
                      availableActions[i] == ActionType::TurnRight) {
                params.push_back(regionActivity * 0.5f); // Turn angle
            }
            
            action->setParameters(params);
            
            // Add to history
            pImpl->actionHistory.push_back(action.get());
            
            return action;
        }
    }
    
    // Fallback action
    auto action = std::make_unique<Action>(ActionType::Wait);
    pImpl->actionHistory.push_back(action.get());
    return action;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Implement action preferences using reinforcement learning principles
    
    // Find action type in available actions
    for (size_t i = 0; i < availableActions.size(); ++i) {
        if (availableActions[i] == action) {
            // Update preference with learning rate
            float learningRate = 0.05f;
            pImpl->actionPreferences[i] = pImpl->actionPreferences[i] * (1.0f - learningRate) + 
                                         preference * learningRate;
            pImpl->actionPreferences[i] = std::clamp(pImpl->actionPreferences[i], 0.0f, 2.0f);
            break;
        }
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
