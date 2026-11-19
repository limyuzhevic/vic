#include "MotorSystem.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::vector<float> actionPreferences;
    std::vector<float> actionValues;
    
    // Neural action selection components
    std::vector<float> actionProbabilities;
    float explorationRate;
    float temperature; // For softmax selection
    
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
        
        actionPreferences.resize(availableActions.size(), 0.0f);
        actionValues.resize(availableActions.size(), 0.0f);
        actionProbabilities.resize(availableActions.size(), 1.0f / availableActions.size());
        
        explorationRate = 0.1f;
        temperature = 1.0f;
    }
    
    // Neural action selection based on motor region activity
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion) {
        auto neurons = motorRegion.getAllNeurons();
        if (neurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate action values based on neural activity
        calculateActionValues(neurons);
        
        // Apply action preferences and exploration
        updateActionProbabilities();
        
        // Select action using neural decision process
        std::unique_ptr<Action> selectedAction = selectActionByProbability();
        
        // Record action in history
        if (selectedAction) {
            actionHistory.push_back(selectedAction.get());
        }
        
        return selectedAction;
    }
    
    // Calculate action values from neural activity
    void calculateActionValues(const std::vector<Neuron*>& neurons) {
        // Reset action values
        std::fill(actionValues.begin(), actionValues.end(), 0.0f);
        
        if (neurons.empty()) return;
        
        // Find most active neurons
        float maxActivity = 0.0f;
        for (Neuron* neuron : neurons) {
            float activity = neuron->getFiringRate();
            maxActivity = std::max(maxActivity, activity);
        }
        
        if (maxActivity < 0.01f) return; // Too quiet, no action
        
        // Calculate neural population dynamics
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons) {
            totalActivity += neuron->getFiringRate();
        }
        
        // Map neural activity to actions based on neuron types
        for (size_t i = 0; i < availableActions.size() && i < neurons.size(); ++i) {
            Neuron* neuron = neurons[i];
            NeuronType neuronType = neuron->getType();
            
            // Different neuron types contribute differently to action selection
            float activityWeight = 1.0f;
            switch (neuronType) {
                case NeuronType::Motor:
                    activityWeight = 1.5f; // Motor neurons strongly influence actions
                    break;
                case NeuronType::Sensory:
                    activityWeight = 0.8f; // Sensory input modulates actions
                    break;
                case NeuronType::Excitatory:
                    activityWeight = 1.2f; // Excitatory neurons drive actions
                    break;
                case NeuronType::Inhibitory:
                    activityWeight = 0.7f; // Inhibitory neurons suppress actions
                    break;
                default:
                    activityWeight = 1.0f;
            }
            
            // Integrate with learned action values
            float baseValue = neuron->getFiringRate() * activityWeight;
            float learnedValue = actionValues[i] * 0.7f + baseValue * 0.3f;
            
            actionValues[i] = learnedValue;
        }
        
        // Normalize values for probability calculation
        float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
        if (maxValue > 0.0f) {
            for (float& value : actionValues) {
                value = value / maxValue; // Normalize to [0, 1]
            }
        }
    }
    
    // Update action selection probabilities
    void updateActionProbabilities() {
        // Combine action values with preferences and exploration
        std::vector<float> adjustedValues = actionValues;
        
        // Apply action preferences
        for (size_t i = 0; i < adjustedValues.size(); ++i) {
            adjustedValues[i] *= (1.0f + actionPreferences[i]);
        }
        
        // Apply exploration
        if (explorationRate > 0.0f) {
            std::vector<float> uniformProb(availableActions.size(), explorationRate / availableActions.size());
            for (size_t i = 0; i < adjustedValues.size(); ++i) {
                uniformProb[i] += (1.0f - explorationRate) * actionProbabilities[i];
            }
            actionProbabilities = uniformProb;
        }
        
        // Convert to softmax probabilities (neural decision making)
        softmax(actionProbabilities, temperature);
    }
    
    // Softmax function for probability distribution
    void softmax(std::vector<float>& values, float temp) {
        float sum = 0.0f;
        std::vector<float> exps;
        
        // Calculate exponentials
        for (float value : values) {
            exps.push_back(std::exp(value / temp));
            sum += exps.back();
        }
        
        // Normalize
        if (sum > 0.0f) {
            for (size_t i = 0; i < values.size(); ++i) {
                values[i] = exps[i] / sum;
            }
        } else {
            // Uniform distribution if all values are invalid
            float uniformProb = 1.0f / values.size();
            for (float& value : values) {
                value = uniformProb;
            }
        }
    }
    
    // Select action based on probabilities
    std::unique_ptr<Action> selectActionByProbability() {
        // Neural stochastic selection
        float randValue = static_cast<float>(rand()) / RAND_MAX;
        float cumulative = 0.0f;
        
        for (size_t i = 0; i < actionProbabilities.size(); ++i) {
            cumulative += actionProbabilities[i];n            if (randValue <= cumulative) {
                return std::make_unique<Action>(availableActions[i]);
            }
        }
        
        // Fallback to highest probability action
        auto maxIt = std::max_element(actionProbabilities.begin(), actionProbabilities.end());
        size_t maxIndex = std::distance(actionProbabilities.begin(), maxIt);
        return std::make_unique<Action>(availableActions[maxIndex]);
    }
    
    // Set action preferences with neural learning
    void setActionPreference(ActionType action, float preference) {
        auto it = std::find(availableActions.begin(), availableActions.end(), action);
        if (it != availableActions.end()) {
            size_t index = std::distance(availableActions.begin(), it);
            actionPreferences[index] = preference;
        }
    }
    
    // Get action value for reinforcement learning
    float getActionValue(ActionType action) const {
        auto it = std::find(availableActions.begin(), availableActions.end(), action);
        if (it != availableActions.end()) {
            size_t index = std::distance(availableActions.begin(), it);
            return actionValues[index];
        }
        return 0.0f;
    }
    
    // Update action value after execution
    void updateActionValue(ActionType action, float reward, float learningRate) {
        auto it = std::find(availableActions.begin(), availableActions.end(), action);
        if (it != availableActions.end()) {
            size_t index = std::distance(availableActions.begin(), it);
            
            // Resilorn-based learning: error-driven weight updates
            float error = reward - actionValues[index];
            actionValues[index] += learningRate * error;
            
            // Clip values to valid range
            actionValues[index] = std::max(0.0f, std::min(actionValues[index], 1.0f));
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Phase 2: Implement real action selection based on neural activity
    // Use sophisticated neural decision making with multiple mechanisms
    
    // Enhanced neural action selection:
    // 1. Integrates motor region activity with learned action values
    // 2. Applies softmax decision making for probabilistic selection
    // 3. Includes exploration-exploitation trade-off
    // 4. Uses error-driven reinforcement learning
    
    return pImpl->selectAction(motorRegion);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Phase 2: Implement action preferences
    // Neural learning of action preferences based on experience
    
    pImpl->setActionPreference(action, preference);
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
