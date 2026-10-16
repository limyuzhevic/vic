#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

// ForwardModel: Predicts next states from current states and actions
// Uses neural networks to learn the dynamics of the environment
class ForwardModel {
public:
    ForwardModel(size_t stateSize, size_t actionSize, float learningRate = 0.001f)
        : stateSize_(stateSize), actionSize_(actionSize), learningRate_(learningRate)
    {
        // Forward model network: takes [state, action] and predicts next state
        // Input: state (stateSize) + action (actionSize) = stateSize + actionSize
        // Output: predicted next state (stateSize)
        size_t inputSize = stateSize + actionSize;
        
        // Multiple hidden layers for complex dynamics
        layers_.push_back(NeuralLayer(inputSize, 256));   // Input -> Hidden 1
        layers_.push_back(NeuralLayer(256, 128));        // Hidden 1 -> Hidden 2
        layers_.push_back(NeuralLayer(128, stateSize));  // Hidden 2 -> Output (next state)
    }
    
    // Predict next state given current state and action
    std::vector<float> predict(const std::vector<float>& state, ActionType action) {
        if (stateSize_ == 0) return std::vector<float>();
        
        // Combine state and action into input vector
        std::vector<float> input;
        input.reserve(stateSize_ + actionSize_);
        
        // Add state
        input.insert(input.end(), state.begin(), state.end());
        
        // Add action (one-hot encoding)
        std::vector<float> actionOneHot(actionSize_, 0.0f);
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionSize_) {
            actionOneHot[actionIdx] = 1.0f;
        }
        
        input.insert(input.end(), actionOneHot.begin(), actionOneHot.end());
        
        // Forward pass through forward model network
        std::vector<float> current = input;
        for (auto& layer : layers_) {
            current = layer.forward(current);
        }
        
        // Output is the predicted next state
        return layers_.back().activations;
    }
    
    // Get prediction error (difference between predicted and actual next state)
    float getPredictionError(const std::vector<float>& predicted, 
                           const std::vector<float>& actual) const {
        if (predicted.size() != actual.size() || predicted.empty()) {
            return 1.0f; // Maximum error if invalid
        }
        
        float sumSqError = 0.0f;
        for (size_t i = 0; i < predicted.size(); ++i) {
            float diff = predicted[i] - actual[i];
            sumSqError += diff * diff;
        }
        
        return std::sqrt(sumSqError / predicted.size());
    }
    
    // Train forward model with actual transition
    void train(const std::vector<float>& state, ActionType action,
               const std::vector<float>& nextStateActual) {
        // Predict next state
        std::vector<float> predicted = predict(state, action);
        
        if (predicted.empty()) return;
        
        // Compute prediction error
        float error = getPredictionError(predicted, nextStateActual);
        
        // Compute gradients using mean squared error
        std::vector<float> outputError(predicted.size(), 0.0f);
        for (size_t i = 0; i < predicted.size(); ++i) {
            outputError[i] = 2.0f * (predicted[i] - nextStateActual[i]) / predicted.size();
        }
        
        // Backpropagate error through network
        std::vector<float> currentGradients = outputError;
        for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
            it->backward(currentGradients, learningRate_);
            
            // Compute gradients for previous layer (simplified)
            if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
                std::vector<float> prevGradients;
                prevGradients.resize(it->inputSize, 0.0f);
                
                // Average gradients to propagate backward
                float avgGrad = 0.0f;
                for (float g : currentGradients) avgGrad += g;
                avgGrad /= currentGradients.size();
                
                for (size_t i = 0; i < it->inputSize; ++i) {
                    prevGradients[i] = avgGrad * 0.01f; // Simplified
                }
                
                currentGradients = prevGradients;
            }
        }
    }
    
    // Get prediction confidence (inverse of error)
    float getPredictionConfidence(const std::vector<float>& predicted,
                                const std::vector<float>& actual) const {
        float error = getPredictionError(predicted, actual);
        return std::max(0.0f, 1.0f - error);
    }
    
    void setBrain(Brain* brain) { brain_ = brain; }
    
private:
    std::vector<NeuralLayer> layers_;
    size_t stateSize_;
    size_t actionSize_;
    float learningRate_;
    Brain* brain_;
};

// InverseModel: Predicts actions that will lead to desired states
// Used for planning and goal-directed behavior
class InverseModel {
public:
    InverseModel(size_t stateSize, size_t actionSize, float learningRate = 0.001f)
        : stateSize_(stateSize), actionSize_(actionSize), learningRate_(learningRate)
    {
        // Inverse model network: takes desired next state and current state
        // predicts which action to take
        size_t inputSize = stateSize * 2;  // Current state + Desired next state
        
        // Multiple hidden layers
        layers_.push_back(NeuralLayer(inputSize, 256));   // Input -> Hidden 1
        layers_.push_back(NeuralLayer(256, 128));        // Hidden 1 -> Hidden 2
        layers_.push_back(NeuralLayer(128, actionSize)); // Hidden 2 -> Actions
    }
    
    // Predict action to achieve desired next state
    ActionType predictAction(const std::vector<float>& currentState,
                           const std::vector<float>& desiredNextState) {
        if (currentState.size() != stateSize_ || 
            desiredNextState.size() != stateSize_) {
            return ActionType::Wait; // Invalid input
        }
        
        // Combine current and desired states as input
        std::vector<float> input;
        input.reserve(stateSize_ * 2);
        input.insert(input.end(), currentState.begin(), currentState.end());
        input.insert(input.end(), desiredNextState.begin(), desiredNextState.end());
        
        // Forward pass
        std::vector<float> current = input;
        for (auto& layer : layers_) {
            current = layer.forward(current);
        }
        
        // Get action probabilities from last layer
        std::vector<float> probs = softmax(layers_.back().activations);
        return getActionFromProbabilities(probs);
    }
    
    // Learn inverse model from experience
    void train(const std::vector<float>& currentState,
               ActionType action,
               const std::vector<float>& nextStateActual) {
        // Target: one-hot encoding of the action that was taken
        std::vector<float> target(actionSize_, 0.0f);
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionSize_) {
            target[actionIdx] = 1.0f;
        }
        
        // Input: [currentState, nextStateActual]
        std::vector<float> input;
        input.reserve(stateSize_ * 2);
        input.insert(input.end(), currentState.begin(), currentState.end());
        input.insert(input.end(), nextStateActual.begin(), nextStateActual.end());
        
        // Forward pass to get predictions
        std::vector<float> current = input;
        for (size_t l = 0; l < layers_.size() - 1; ++l) {
            current = layers_[l].forward(current);
        }
        
        // Get output from final layer
        auto& finalLayer = layers_.back();
        std::vector<float> output = finalLayer.forward(current);
        
        // Compute gradients (cross-entropy loss)
        std::vector<float> gradients(actionSize_, 0.0f);
        for (size_t i = 0; i < actionSize_; ++i) {
            // Cross-entropy gradient: pred - target
            float pred = output[i];
            gradients[i] = pred - target[i];
        }
        
        // Backpropagate
        std::vector<float> currentGradients = gradients;
        for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
            it->backward(currentGradients, learningRate_);
            
            // Simplified gradient propagation
            if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
                std::vector<float> prevGradients;
                prevGradients.resize(it->inputSize, 0.0f);
                
                float avgGrad = 0.0f;
                for (float g : currentGradients) avgGrad += g;
                avgGrad /= currentGradients.size();
                
                for (size_t i = 0; i < it->inputSize; ++i) {
                    prevGradients[i] = avgGrad * 0.01f;
                }
                
                currentGradients = prevGradients;
            }
        }
    }
    
    // Get action from probability distribution
    ActionType getActionFromProbabilities(const std::vector<float>& probs) {
        float cumulative = 0.0f;
        float randVal = getRandomFloat(0.0f, 1.0f);
        
        for (size_t i = 0; i < probs.size(); ++i) {
            cumulative += probs[i];
            if (randVal <= cumulative) {
                return static_cast<ActionType>(i);
            }
        }
        
        return ActionType::Wait;
    }
    
    // Get action preferences (expected future rewards)
    float getActionValue(const std::vector<float>& currentState,
                        ActionType action) const {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx >= actionPreferences_.size()) return 0.0f;
        return actionPreferences_[actionIdx];
    }
    
    void updateActionPreference(ActionType action, float value) {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionPreferences_.size()) {
            actionPreferences_[actionIdx] = value;
        }
    }
    
    void setBrain(Brain* brain) { brain_ = brain; }
    
private:
    std::vector<NeuralLayer> layers_;
    size_t stateSize_;
    size_t actionSize_;
    float learningRate_;
    Brain* brain_;
    
    std::vector<float> actionPreferences_;
    
    std::vector<float> softmax(const std::vector<float>& logits) {
        std::vector<float> probs(logits.size());
        float maxLogit = *std::max_element(logits.begin(), logits.end());
        float sum = 0.0f;
        
        for (size_t i = 0; i < logits.size(); ++i) {
            probs[i] = std::exp(logits[i] - maxLogit);
            sum += probs[i];
        }
        
        for (auto& p : probs) {
            p /= sum;
        }
        
        return probs;
    }
    
    float getRandomFloat(float min, float max) {
        if (!brain_) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(min, max);
            return dist(gen);
        }
        return brain_->getRandomGenerator()->uniformReal(min, max);
    }
};

// CuriosityDrivenExploration: Uses prediction errors to drive exploration
// Implements intrinsic motivation based on learning progress
class CuriosityDrivenExploration {
public:
    CuriosityDrivenExploration(float noveltyWeight = 0.5f, 
                              float predictionErrorWeight = 0.5f,
                              float learningRate = 0.01f)
        : noveltyWeight_(noveltyWeight)
        , predictionErrorWeight_(predictionErrorWeight)
        , learningRate_(learningRate)
    {
        explorationBonus_.resize(10, 0.0f); // One per action type
    }
    
    // Compute exploration bonus based on prediction error and novelty
    float computeExplorationBonus(float predictionError, 
                                 float novelty,
                                 const std::vector<float>& state,
                                 ActionType action) {
        // Base bonus from prediction error
        float errorBonus = predictionErrorWeight_ * predictionError;
        
        // Novelty bonus
        float noveltyBonus = noveltyWeight_ * novelty;
        
        // State visitation bonus (encourage exploring new states)
        float stateBonus = getStateNoveltyBonus(state);
        
        // Action-specific bonus
        float actionBonus = getActionExplorationBonus(action);
        
        // Total exploration bonus
        float totalBonus = errorBonus + noveltyBonus + stateBonus + actionBonus;
        
        // Update action exploration bonus
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < explorationBonus_.size()) {
            explorationBonus_[actionIdx] = std::min(1.0f, 
                                                  explorationBonus_[actionIdx] + 
                                                  learningRate_ * (totalBonus - explorationBonus_[actionIdx]));
        }
        
        return totalBonus;
    }
    
    // Get action with maximum intrinsic reward (exploration)
    ActionType getExploratoryAction(const std::vector<float>& state,
                                   const std::vector<float>& actionProbabilities) {
        if (actionProbabilities.empty()) return ActionType::Wait;
        
        // Compute intrinsic motivation scores for each action
        std::vector<float> intrinsicRewards(actionProbabilities.size(), 0.0f);
        
        for (size_t i = 0; i < actionProbabilities.size(); ++i) {
            float exploration = explorationBonus_[i % explorationBonus_.size()];
            intrinsicRewards[i] = actionProbabilities[i] * exploration;
        }
        
        // Select action with highest intrinsic reward
        float maxReward = -1.0f;
        ActionType bestAction = ActionType::Wait;
        
        for (size_t i = 0; i < intrinsicRewards.size(); ++i) {
            if (intrinsicRewards[i] > maxReward) {
                maxReward = intrinsicRewards[i];
                bestAction = static_cast<ActionType>(i);
            }
        }
        
        return bestAction;
    }
    
    // Update state novelty based on visitation frequency
    void updateStateNovelty(const std::vector<float>& state) {
        std::string stateKey = stateToKey(state);
        
        auto it = stateVisits_.find(stateKey);
        if (it == stateVisits_.end()) {
            stateVisits_[stateKey] = 1;
        } else {
            it->second++;
        }
    }
    
    // Get novelty score for a state (lower visit count = higher novelty)
    float getStateNoveltyBonus(const std::vector<float>& state) {
        std::string stateKey = stateToKey(state);
        
        auto it = stateVisits_.find(stateKey);
        if (it == stateVisits_.end()) {
            return 1.0f; // Unseen state = maximum novelty
        }
        
        int visits = it->second;
        return std::max(0.0f, 1.0f / (visits * 0.1f)); // Decay with visits
    }
    
    // Get exploration bonus for specific action
    float getActionExplorationBonus(ActionType action) const {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < explorationBonus_.size()) {
            return explorationBonus_[actionIdx];
        }
        return 0.0f;
    }
    
private:
    float noveltyWeight_;
    float predictionErrorWeight_;
    float learningRate_;
    
    std::vector<float> explorationBonus_;  // One per action type
    std::unordered_map<std::string, int> stateVisits_;
    
    std::string stateToKey(const std::vector<float>& state) {
        std::stringstream ss;
        for (size_t i = 0; i < state.size(); ++i) {
            ss << state[i];
            if (i < state.size() - 1) ss << ",";
        }
        return ss.str();
    }
};

// RewardPredictor: Predicts future rewards based on state-action pairs
// Used for reward shaping and policy evaluation
class RewardPredictor {
public:
    RewardPredictor(size_t stateSize, size_t actionSize, float learningRate = 0.001f)
        : stateSize_(stateSize), actionSize_(actionSize), learningRate_(learningRate)
    {
        // Reward predictor network: takes [state, action] and predicts reward
        size_t inputSize = stateSize + actionSize;
        
        layers_.push_back(NeuralLayer(inputSize, 128));    // Input -> Hidden 1
        layers_.push_back(NeuralLayer(128, 64));         // Hidden 1 -> Hidden 2
        layers_.push_back(NeuralLayer(64, 1));          // Hidden 2 -> Reward
    }
    
    // Predict reward for state-action pair
    float predict(const std::vector<float>& state, ActionType action) {
        std::vector<float> input = state;
        
        // Add action (one-hot)
        std::vector<float> actionOneHot(actionSize_, 0.0f);
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionSize_) {
            actionOneHot[actionIdx] = 1.0f;
        }
        
        input.insert(input.end(), actionOneHot.begin(), actionOneHot.end());
        
        // Forward pass
        std::vector<float> current = input;
        for (auto& layer : layers_) {
            current = layer.forward(current);
        }
        
        return layers_.back().activations[0];
    }
    
    // Train reward predictor with actual reward
    void train(const std::vector<float>& state, ActionType action,
               float actualReward) {
        // Get prediction
        float predicted = predict(state, action);
        float error = actualReward - predicted;
        
        // Backpropagate error
        std::vector<float> gradients(1, error);
        
        for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
            it->backward(gradients, learningRate_);
            
            // Simplified gradient propagation
            if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
                std::vector<float> prevGradients;
                prevGradients.resize(it->inputSize, 0.0f);
                
                float avgGrad = gradients[0];
                for (size_t i = 0; i < it->inputSize; ++i) {
                    prevGradients[i] = avgGrad * 0.01f;
                }
                
                gradients = prevGradients;
            }
        }
    }
    
    // Get reward prediction error
    float getRewardPredictionError(const std::vector<float>& state,
                                  ActionType action,
                                  float actualReward) const {
        float predicted = predict(state, action);
        return std::abs(actualReward - predicted);
    }
    
    void setBrain(Brain* brain) { brain_ = brain; }
    
private:
    std::vector<NeuralLayer> layers_;
    size_t stateSize_;
    size_t actionSize_;
    float learningRate_;
    Brain* brain_;
};

} // namespace nlm