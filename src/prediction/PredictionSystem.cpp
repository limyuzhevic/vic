// PredictionSystem.cpp implementation
#include "PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Prediction model parameters
    float learningRate;           // How quickly the model adapts
    float predictionGain;         // How confident the model is
    float errorThreshold;         // Threshold for prediction errors
    
    // State variables
    std::unique_ptr<SensoryInput> lastPrediction;      // Previous prediction
    float currentPredictionError;                       // Current prediction error
    float confidence;                                    // Current confidence level
    
    // History tracking
    std::vector<float> errorHistory;                    // History of prediction errors
    std::vector<float> confidenceHistory;                // History of confidence levels
    
    // Model weights (simplified for basic prediction)
    std::vector<float> modelWeights;
    
    // Random number generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : learningRate(0.1f), predictionGain(0.5f), errorThreshold(0.5f),
             currentPredictionError(0.0f), confidence(0.5f),
             rng(std::random_device{}()), uniformDist(-1.0f, 1.0f) {
        
        // Initialize with some default weights
        modelWeights = std::vector<float>(10, 0.1f);  // 10 features with small weights
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Basic prediction: add small random changes to current state
    // In a real implementation, this would use learned patterns and neural networks
    
    auto prediction = std::make_unique<SensoryInput>();
    
    // Copy current state
    prediction->features = currentState.features;
    prediction->modality = currentState.modality;
    
    // Add prediction noise (exploration)
    float explorationNoise = pImpl->uniformDist(pImpl->rng);
    float noiseLevel = pImpl->predictionGain * explorationNoise;
    
    // Apply noise to each feature
    for (size_t i = 0; i < prediction->features.size(); ++i) {
        prediction->features[i] += noiseLevel * (pImpl->uniformDist(pImpl->rng) - 0.5f);
        
        // Clamp values to reasonable range
        prediction->features[i] = std::clamp(prediction->features[i], -10.0f, 10.0f);
    }
    
    // Store last prediction
    pImpl->lastPrediction = std::make_unique<SensoryInput>(*prediction);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error
    pImpl->currentPredictionError = 0.0f;
    
    if (predicted.features.size() == actual.features.size()) {
        for (size_t i = 0; i < predicted.features.size(); ++i) {
            float error = predicted.features[i] - actual.features[i];
            pImpl->currentPredictionError += error * error;  // Squared error
        }
        pImpl->currentPredictionError /= static_cast<float>(predicted.features.size());
        pImpl->currentPredictionError = std::sqrt(pImpl->currentPredictionError);  // RMSE
    }
    
    // Update confidence based on prediction error
    pImpl->confidence = std::clamp(1.0f - pImpl->currentPredictionError, 0.0f, 1.0f);
    
    // Add to history
    pImpl->errorHistory.push_back(pImpl->currentPredictionError);
    pImpl->confidenceHistory.push_back(pImpl->confidence);
    
    // Keep history bounded
    const size_t maxHistorySize = 1000;
    if (pImpl->errorHistory.size() > maxHistorySize) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
    }
    
    // Adaptive learning based on error
    if (pImpl->currentPredictionError > pImpl->errorThreshold) {
        // High error: increase learning rate
        pImpl->learningRate = std::min(pImpl->learningRate * 1.1f, 0.5f);
    } else {
        // Low error: decrease learning rate
        pImpl->learningRate = std::max(pImpl->learningRate * 0.9f, 0.01f);
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->currentPredictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->confidenceHistory.clear();
    pImpl->currentPredictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Simple model training: adjust weights based on observation
    // This is a placeholder for more sophisticated learning algorithms
    
    for (size_t i = 0; i < modelWeights.size() && i < observation.features.size(); ++i) {
        // Weight update rule: gradient descent approximation
        float gradient = observation.features[i] * (pImpl->predictionGain - pImpl->confidence);
        modelWeights[i] += pImpl->learningRate * gradient;
        
        // Keep weights bounded
        modelWeights[i] = std::clamp(modelWeights[i], -1.0f, 1.0f);
    }
}

} // namespace nlm
