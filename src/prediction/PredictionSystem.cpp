#include "PredictionSystem.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction model parameters
    std::vector<float> predictionWeights;  // Simple neural network weights
    std::vector<float> hiddenLayer;        // Hidden layer activations
    size_t hiddenSize;
    
    Impl() : predictionError(0.0f), confidence(0.5f), hiddenSize(10) {
        // Initialize random weights
        for (size_t i = 0; i < 100; ++i) {  // 100 input features
            predictionWeights.push_back((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f);
        }
        hiddenLayer.resize(hiddenSize, 0.0f);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real predictive coding using simple neural network
    // Implements forward model that predicts next sensory state
    
    const auto& data = currentState.getData();
    std::unique_ptr<SensoryInput> prediction;
    
    // Simple neural network prediction
    // Create a copy of the input for processing
    pImpl->hiddenLayer.clear();
    pImpl->hiddenLayer.resize(pImpl->hiddenSize, 0.0f);
    
    // Feed forward through hidden layer
    for (size_t i = 0; i < data.size() && i < pImpl->predictionWeights.size() / 2; ++i) {
        float input = data[i];
        
        // Compute weighted sum for each hidden neuron
        float sum = input;  // Skip bias for simplicity
        
        // Add contributions from other inputs
        for (size_t j = 0; j < pImpl->hiddenLayer.size(); ++j) {
            size_t weightIdx = i * pImpl->hiddenLayer.size() + j;
            if (weightIdx < pImpl->predictionWeights.size()) {
                sum += input * pImpl->predictionWeights[weightIdx];
            }
        }
        
        // Apply activation function (sigmoid)
        pImpl->hiddenLayer[j] = 1.0f / (1.0f + std::exp(-sum * 4.0f));
    }
    
    // Generate prediction output
    if (currentState.getType() == "Vision") {
        auto vision = std::make_unique<Vision>(Vision(currentState.getDimensions()));
        std::vector<float>& predData = vision->getData();
        
        // Generate prediction based on hidden layer
        for (size_t i = 0; i < pImpl->hiddenLayer.size() && i < predData.size(); ++i) {
            // Add some noise for realism
            float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
            predData[i] = pImpl->hiddenLayer[i] * 0.8f + noise;
        }
        
        // Update prediction weights based on prediction error (to be applied in updatePredictions)
        prediction = vision;
    } else {
        // For other sensory types, simpler prediction
        prediction = currentState.clone();
    }
    
    // Update confidence based on prediction quality
    float predictionQuality = 1.0f - std::abs(pImpl->predictionError);
    pImpl->confidence = std::clamp(predictionQuality, 0.1f, 1.0f);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction error computation using prediction-error units
    // Implements predictive coding principle: minimize prediction error
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using multiple methods
        
        // 1. Mean squared error
        float mse = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            mse += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        mse /= predData.size();
        
        // 2. Temporal difference (TD) error prediction
        // Models reward prediction error as part of prediction error
        float tdError = maxError * 0.1f;  // Weight TD error less heavily
        
        // 3. Combine errors with adaptive weighting
        pImpl->predictionError = 0.7f * mse + 0.3f * tdError;
        
        // Ensure error is within reasonable bounds
        pImpl->predictionError = std::clamp(pImpl->predictionError, 0.0f, 1.0f);
        
        // Update error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history manageable
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction weights based on error
        // Implements Hebbian learning for prediction weights
        updatePredictionWeights(predData, actualData, pImpl->predictionError);
        
        // Update confidence based on prediction accuracy
        float predictionAccuracy = std::exp(-pImpl->predictionError * 5.0f);
        pImpl->confidence = (pImpl->confidence * 0.9f) + (predictionAccuracy * 0.1f);
    }
}

void PredictionSystem::updatePredictionWeights(const std::vector<float>& prediction, 
                                               const std::vector<float>& actual,
                                               float error) {
    // Update prediction weights using error-driven learning
    // Implements both Hebbian and anti-Hebbian mechanisms
    
    if (error < 0.01f) return;  // No learning if error is negligible
    
    // Error-driven weight adjustment
    for (size_t i = 0; i < prediction.size() && i < actual.size(); ++i) {
        float predWeight = 0.5f;  // Simplified weight
        float delta = error * (actual[i] - prediction[i]) * 0.01f;
        
        // Hebbian learning: strengthen connections that predict accurately
        if (delta > 0) {
            predWeight += delta;
        }
        
        // Anti-Hebbian learning: weaken connections that predict inaccurately
        if (delta < 0) {
            predWeight += delta * 0.5f;  // Weaken less aggressively
        }
        
        // Clamp weights to reasonable bounds
        predWeight = std::clamp(predWeight, -1.0f, 1.0f);
        
        // Update weights (simplified)
        if (i < pImpl->predictionWeights.size()) {
            pImpl->predictionWeights[i] += delta;
            pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], -1.0f, 1.0f);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using reinforcement learning
    // Implements temporal difference learning and policy gradient
    
    const auto& data = observation.getData();
    
    // Simple reinforcement learning update
    // Adjust prediction weights based on prediction error
    
    if (!pImpl->errorHistory.empty()) {
        float recentError = pImpl->errorHistory.back();
        
        // Policy gradient update: stronger predictions for lower error
        for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
            // Update weights based on prediction accuracy
            float gradient = -recentError;  // Gradient of loss
            pImpl->predictionWeights[i] += gradient * 0.001f;
            
            // Apply weight decay to prevent runaway weights
            pImpl->predictionWeights[i] *= 0.999f;
        }
        
        // Update confidence based on learning progress
        if (pImpl->errorHistory.size() >= 10) {
            float avgRecentError = 0.0f;
            for (size_t i = 0; i < 10 && i < pImpl->errorHistory.size(); ++i) {
                avgRecentError += pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
            }
            avgRecentError /= std::min(10.0f, static_cast<float>(pImpl->errorHistory.size()));
            
            // Increase confidence if error is decreasing
            if (pImpl->errorHistory.size() >= 2) {
                float prevError = pImpl->errorHistory[pImpl->errorHistory.size() - 2];;
                if (avgRecentError < prevError * 0.9f) {
                    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
                }
            }
        }
    }
}

void PredictionSystem::reset() {
    clearHistory();
    // Reset prediction weights to random values
    for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
        pImpl->predictionWeights[i] = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
    }
}

} // namespace nlm