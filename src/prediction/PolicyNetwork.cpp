#include "PolicyNetwork.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// PolicyNetwork implementation
void PolicyNetwork::update(const std::vector<float>& state, ActionType action, 
                          float reward, const std::vector<float>& nextState, bool done) {
    // Get action probabilities for the current state
    auto probs = getActionProbabilities(state);
    
    // Convert action to one-hot encoding
    std::vector<float> target(actionSize_, 0.0f);
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx < actionSize_) {
        target[actionIdx] = 1.0f;
    }
    
    // Compute policy gradient using REINFORCE algorithm
    // Policy gradient = (target - probs) * reward
    std::vector<float> gradients(actionSize_, 0.0f);
    for (size_t i = 0; i < actionSize_; ++i) {
        gradients[i] = (target[i] - probs[i]) * reward;
    }
    
    // Backpropagate through the network layers
    std::vector<float> currentGradients = gradients;
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
        it->backward(currentGradients, learningRate_);
        
        // Compute gradients for previous layer (simplified)
        // In a full implementation, we'd properly chain gradients through all layers
        if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
            // For now, use simplified gradient propagation
            std::vector<float> prevGradients;
            prevGradients.resize(it->inputSize, 0.0f);
            
            // Average gradients to propagate backward
            float avgGrad = 0.0f;
            for (float g : currentGradients) avgGrad += g;
            avgGrad /= currentGradients.size();
            
            for (size_t i = 0; i < it->inputSize; ++i) {
                prevGradients[i] = avgGrad * 0.1f; // Simplified
            }
            
            currentGradients = prevGradients;
        }
    }
}

// ValueNetwork implementation
void ValueNetwork::update(const std::vector<float>& state, float targetValue) {
    std::vector<float> current = state;
    
    // Forward pass to compute current value
    for (auto& layer : layers_) {
        current = layer.forward(current);
    }
    
    float currentValue = layers_.back().activations[0];
    float error = targetValue - currentValue;
    
    // Backpropagate error through the value network
    std::vector<float> gradients(1, error);
    
    // Backpropagate through layers
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
        it->backward(gradients, learningRate_);
        
        // Compute gradients for previous layer
        if (std::distance(it, layers_.rbegin()) < layers_.size() - 1) {
            std::vector<float> prevGradients;
            prevGradients.resize(it->inputSize, 0.0f);
            
            // Simplified gradient propagation
            for (size_t j = 0; j < it->outputSize; ++j) {
                float grad = gradients[0];
                if (j < prevGradients.size()) {
                    prevGradients[j] = grad * 0.1f;
                }
            }
            
            gradients = prevGradients;
        }
    }
}

} // namespace nlm