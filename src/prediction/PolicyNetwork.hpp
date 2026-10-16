#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

// Neural Layer: Basic computation unit for neural networks
struct NeuralLayer {
    std::vector<float> weights;
    std::vector<float> biases;
    std::vector<float> activations;
    std::vector<float> gradients;
    
    size_t inputSize;
    size_t outputSize;
    
    NeuralLayer(size_t inSize, size_t outSize) 
        : inputSize(inSize), outputSize(outSize)
    {
        weights.resize(inputSize * outputSize, 0.0f);
        biases.resize(outputSize, 0.0f);
        activations.resize(outputSize, 0.0f);
        gradients.resize(outputSize, 0.0f);
        
        // Xavier initialization
        float scale = std::sqrt(2.0f / (inputSize + outputSize));
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, scale);
        
        for (auto& w : weights) w = dist(gen);
        for (auto& b : biases) b = dist(gen);
    }
    
    std::vector<float> forward(const std::vector<float>& input) {
        activations.clear();
        activations.resize(outputSize, 0.0f);
        
        // Matrix-vector multiplication: output = weights * input + biases
        for (size_t j = 0; j < outputSize; ++j) {
            float sum = biases[j];
            for (size_t i = 0; i < inputSize; ++i) {
                sum += weights[i * outputSize + j] * input[i];
            }
            activations[j] = std::max(0.0f, sum); // ReLU
        }
        
        return activations;
    }
    
    void backward(const std::vector<float>& error, float learningRate) {
        gradients.resize(outputSize, 0.0f);
        
        // Compute gradients
        for (size_t j = 0; j < outputSize; ++j) {
            float grad = error[j];
            if (activations[j] > 0) {
                grad *= 0.01f; // Derivative of ReLU
            }
            gradients[j] = grad;
            
            // Update weights
            for (size_t i = 0; i < inputSize; ++i) {
                float delta = learningRate * grad * activations[i];
                weights[i * outputSize + j] -= delta;
            }
            biases[j] -= learningRate * grad;
        }
    }
};

// Policy Network: Outputs action probabilities for action selection
class PolicyNetwork {
public:
    PolicyNetwork(size_t stateSize, size_t actionSize, float learningRate = 0.001f)
        : stateSize_(stateSize), actionSize_(actionSize), learningRate_(learningRate)
    {
        // Policy network with multiple hidden layers
        layers_.push_back(NeuralLayer(stateSize, 128));  // State -> Hidden 1
        layers_.push_back(NeuralLayer(128, 64));        // Hidden 1 -> Hidden 2
        layers_.push_back(NeuralLayer(64, actionSize)); // Hidden 2 -> Actions
    }
    
    // Get action probabilities for a given state
    std::vector<float> getActionProbabilities(const std::vector<float>& state) {
        std::vector<float> current = state;
        
        // Forward pass through policy network
        for (auto& layer : layers_) {
            current = layer.forward(current);
        }
        
        // Apply softmax to get probabilities
        std::vector<float> probs = softmax(layers_.back().activations);
        return probs;
    }
    
    // Get action with highest probability (deterministic)
    ActionType getBestAction(const std::vector<float>& state) {
        auto probs = getActionProbabilities(state);
        return getActionFromProbabilities(probs);
    }
    
    // Get action with exploration (stochastic)
    ActionType getActionWithExploration(const std::vector<float>& state, float explorationRate) {
        std::vector<float> probs = getActionProbabilities(state);
        
        // Add exploration (epsilon-greedy)
        float threshold = explorationRate;
        float randVal = getRandomFloat(0.0f, 1.0f);
        
        if (randVal < threshold) {
            return static_cast<ActionType>(getRandomInt(0, actionSize_ - 1));
        } else {
            return getActionFromProbabilities(probs);
        }
    }
    
    // Update policy based on state-action-reward
    void update(const std::vector<float>& state, ActionType action, 
                float reward, const std::vector<float>& nextState, bool done) {
        // Get action probabilities
        auto probs = getActionProbabilities(state);
        
        // Convert action to one-hot
        std::vector<float> target(actionSize_, 0.0f);
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionSize_) {
            target[actionIdx] = 1.0f;
        }
        
        // Compute policy gradient (REINFORCE algorithm)
        std::vector<float> error(actionSize_, 0.0f);
        for (size_t i = 0; i < actionSize_; ++i) {
            error[i] = (target[i] - probs[i]) * reward;
        }
        
        // Backpropagate through network
        std::vector<float> currentGradients = error;
        for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
            it->backward(currentGradients, learningRate_);
            
            // Compute gradients for previous layer (simplified)
            if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
                currentGradients.clear();
                // In a full implementation, we'd compute actual backprop
                currentGradients.resize(it->inputSize, 0.0f);
            }
        }
    }
    
    void setBrain(Brain* brain) { brain_ = brain; }
    
private:
    std::vector<NeuralLayer> layers_;
    size_t stateSize_;
    size_t actionSize_;
    float learningRate_;
    Brain* brain_;
    
    std::vector<float> softmax(const std::vector<float>& logits) {
        std::vector<float> probs(logits.size());
        float maxLogit = *std::max_element(logits.begin(), logits.end());
        float sum = 0.0f;
        
        // Compute exponentials
        for (size_t i = 0; i < logits.size(); ++i) {
            probs[i] = std::exp(logits[i] - maxLogit);
            sum += probs[i];
        }
        
        // Normalize
        for (auto& p : probs) {
            p /= sum;
        }
        
        return probs;
    }
    
    ActionType getActionFromProbabilities(const std::vector<float>& probs) {
        float cumulative = 0.0f;
        float randVal = getRandomFloat(0.0f, 1.0f);
        
        for (size_t i = 0; i < probs.size(); ++i) {
            cumulative += probs[i];
            if (randVal <= cumulative) {
                return static_cast<ActionType>(i);
            }
        }
        
        // Fallback to first action
        return static_cast<ActionType>(0);
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
    
    int getRandomInt(int min, int max) {
        if (!brain_) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(min, max);
            return dist(gen);
        }
        return brain_->getRandomGenerator()->uniformInt(min, max);
    }
};

// Value Network: Estimates expected future rewards for state evaluation
class ValueNetwork {
public:
    ValueNetwork(size_t stateSize, float learningRate = 0.001f)
        : stateSize_(stateSize), learningRate_(learningRate)
    {
        // Value network with multiple hidden layers
        layers_.push_back(NeuralLayer(stateSize, 128));   // State -> Hidden 1
        layers_.push_back(NeuralLayer(128, 64));         // Hidden 1 -> Hidden 2
        layers_.push_back(NeuralLayer(64, 1));          // Hidden 2 -> Value
    }
    
    // Get value estimate for a given state
    float getValue(const std::vector<float>& state) {
        std::vector<float> current = state;
        
        // Forward pass through value network
        for (auto& layer : layers_) {
            current = layer.forward(current);
        }
        
        return layers_.back().activations[0];
    }
    
    // Get value gradient (for policy gradient)
    float getValueGradient(const std::vector<float>& state) {
        // Simplified: compute gradient of value w.r.t. state
        float gradient = 0.0f;
        
        std::vector<float> current = state;
        std::vector<std::vector<float>> activations;
        
        // Store activations for backpropagation
        for (auto& layer : layers_) {
            current = layer.forward(current);
            activations.push_back(current);
        }
        
        // Backpropagate through value network
        float error = 1.0f;  // Target value (simplified)
        std::vector<float> layerError(1, error - activations.back()[0]);
        
        // Simplified backprop
        float weightGradientSum = 0.0f;
        size_t totalWeights = 0;
        
        for (size_t l = layers_.size() - 1; l > 0; --l) {
            auto& layer = layers_[l];
            auto& prevActs = activations[l - 1];
            
            for (size_t j = 0; j < layer.outputSize; ++j) {
                float grad = layerError[0];
                if (activations[l][j] > 0) grad *= 0.01f; // ReLU derivative
                
                for (size_t i = 0; i < layer.inputSize; ++i) {
                    weightGradientSum += std::abs(grad * prevActs[i]);
                    totalWeights++;
                }
            }
            
            if (l > 1) {
                // Compute error for previous layer
                layerError.resize(layers_[l - 1].outputSize, 0.0f);
                for (size_t j = 0; j < layers_[l - 1].outputSize; ++j) {
                    for (size_t i = 0; i < layer.outputSize; ++i) {
                        layerError[j] += layerError[i] * layer.weights[j * layer.outputSize + i];
                    }
                }
            }
        }
        
        return weightGradientSum / std::max(1.0f, static_cast<float>(totalWeights));
    }
    
    void setBrain(Brain* brain) { brain_ = brain; }
    
private:
    std::vector<NeuralLayer> layers_;
    size_t stateSize_;
    float learningRate_;
    Brain* brain_;
};

} // namespace nlm