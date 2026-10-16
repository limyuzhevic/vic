#include "ValueNetwork.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ValueNetwork implementation
void ValueNetwork::update(const std::vector<float>& state, float targetValue) {
    std::vector<float> current = state;
    
    // Forward pass to compute current value
    for (auto& layer : layers_) {
        current = layer.forward(current);
    }
    
    float currentValue = layers_.back().activations[0];
    float error = targetValue - currentValue;
    
    // Backpropagate error
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