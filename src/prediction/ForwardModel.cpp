// Update ForwardModel implementation to include proper training
#include "ForwardModel.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void ForwardModel::train(const std::vector<float>& state, ActionType action,
                         const std::vector<float>& nextStateActual,
                         float reward) {
    // Predict next state
    std::vector<float> predicted = predict(state, action);
    
    if (predicted.empty() || nextStateActual.empty()) return;
    
    // Compute prediction error (MSE)
    float error = 0.0f;
    for (size_t i = 0; i < predicted.size() && i < nextStateActual.size(); ++i) {
        float diff = predicted[i] - nextStateActual[i];
        error += diff * diff;
    }
    error /= predicted.size();
    
    // Compute gradients using backpropagation
    std::vector<float> outputError(predicted.size(), 0.0f);
    for (size_t i = 0; i < predicted.size(); ++i) {
        outputError[i] = 2.0f * (predicted[i] - nextStateActual[i]) / predicted.size();
    }
    
    // Backpropagate through the network
    std::vector<float> currentGradients = outputError;
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
        it->backward(currentGradients, learningRate_);
        
        // Compute gradients for previous layer
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
    
    // Store training experience
    trainingExperiences_.push_back({state, action, nextStateActual, predicted, error, reward});
    
    // Limit memory
    if (trainingExperiences_.size() > 10000) {
        trainingExperiences_.erase(trainingExperiences_.begin());
    }
}

} // namespace nlm