#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Forward model for sensory prediction
    float predictionError;
    float confidence;
    float predictionStrength;
    
    // Prediction history
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    
    // Neural prediction parameters
    float learningRate;
    float predictionDecay;
    float stabilityThreshold;
    
    Impl() 
        : predictionError(0.0f)
        , confidence(0.5f)
        , predictionStrength(0.5f)
        , learningRate(0.01f)
        , predictionDecay(0.95f)
        , stabilityThreshold(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real predictive coding implementation
    // Uses current state to predict next state through forward model
    
    auto predicted = currentState.clone();
    const auto& currentData = currentState.getData();
    
    // Apply prediction based on learned forward model
    // In a real system, this would use neural predictions
    // For now, we apply a simple predictive update
    std::vector<float>& predictedData = predicted->getData();
    
    if (!currentData.empty() && predictedData.size() == currentData.size()) {
        // Smooth prediction towards current state
        for (size_t i = 0; i < predictedData.size(); ++i) {
            predictedData[i] = currentData[i] * pImpl->predictionStrength + 
                             predictedData[i] * (1.0f - pImpl->predictionStrength);
        }
        
        // Add prediction noise
        float noiseLevel = 0.1f * (1.0f - pImpl->predictionStrength);
        for (size_t i = 0; i < predictedData.size(); ++i) {
            predictedData[i] += (pImpl->rng->uniformFloat(-noiseLevel, noiseLevel));
        }
        
        pImpl->predictionHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->predictionHistory.size() > 100) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update predictions based on actual observation
    // Calculate prediction error and update forward model
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        
        // Update confidence based on error
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError * 10.0f);
        
        // Update prediction strength (learning rate)
        pImpl->predictionStrength += (pImpl->predictionError - pImpl->predictionStrength) * pImpl->learningRate;
        pImpl->predictionStrength = std::clamp(pImpl->predictionStrength, 0.1f, 1.0f);
        
        // Store error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Decay history
        pImpl->confidence *= pImpl->predictionDecay;
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
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
    pImpl->predictionHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->predictionStrength = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model with new observation
    // Update forward model parameters based on prediction error
    
    // For now, we just track that we received an observation
    // In a real implementation, this would update neural forward models
    pImpl->predictionStrength *= 0.99f;  // Slight decay in prediction accuracy
}

} // namespace nlm
