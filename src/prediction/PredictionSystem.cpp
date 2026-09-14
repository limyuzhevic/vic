#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Random/Random.hpp"
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction state
    float predictionLearningRate;
    float predictionDecayRate;
    std::vector<float> predictionPattern;
    std::vector<float> predictionErrorSignal;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             predictionLearningRate(0.01f), predictionDecayRate(0.99f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using temporal pattern learning
    // This simulates forward models in the brain that predict next sensory states
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Predict next state based on temporal dynamics
    std::unique_ptr<SensoryInput> predictedInput = currentState.clone();
    auto& predictedData = predictedInput->getData();
    
    // For now, implement simple temporal prediction: next state is current state with noise
    // In real implementation, this would use learned transition patterns
    float noiseLevel = 0.1f;
    
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Add temporal dynamics: smooth change plus small noise
        if (i > 0) {
            // Temporal smoothing: predict gradual change
            float change = (predictedData[i] - currentData[i]) * 0.5f;
            predictedData[i] = currentData[i] + change + 
                            (pImpl->predictionSystem ? pImpl->predictionSystem->getRandomGenerator()->uniformReal(-noiseLevel, noiseLevel) : 0.0f);
        }
        
        // Ensure within valid range (assuming normalized input)
        predictedData[i] = std::clamp(predictedData[i], 0.0f, 1.0f);
    }
    
    // Store prediction for next update
    pImpl->predictionPattern = currentData;
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error and update prediction model
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = actualData[i] - predData[i];
        sumSquaredError += error * error;
    }
    
    pImpl->predictionError = sumSquaredError / predData.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep error history manageable
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction confidence based on error
    // Lower error = higher confidence
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError * 10.0f);
    
    // Decay confidence over time
    pImpl->confidence *= pImpl->predictionDecayRate;
    
    // Store error signal for neuromodulation
    pImpl->predictionErrorSignal.resize(predData.size());
    for (size_t i = 0; i < predData.size(); ++i) {
        pImpl->predictionErrorSignal[i] = actualData[i] - predData[i];
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
    // Train prediction model using observed data
    // This would update internal transition probabilities and temporal dynamics
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update prediction pattern
    if (pImpl->predictionPattern.size() == data.size()) {
        // Learn transition patterns
        for (size_t i = 0; i < data.size(); ++i) {
            // Simple Hebbian-like learning for prediction patterns
            float currentPattern = pImpl->predictionPattern[i];
            float currentInput = data[i];
            
            // Update prediction pattern towards observed input
            float learningEffect = pImpl->predictionLearningRate * (currentInput - currentPattern);
            pImpl->predictionPattern[i] += learningEffect;
            
            // Clamp pattern to valid range
            pImpl->predictionPattern[i] = std::clamp(pImpl->predictionPattern[i], 0.0f, 1.0f);
        }
    } else {
        pImpl->predictionPattern = data;
    }
    
    // Decay prediction pattern over time (forgetting)
    for (auto& pattern : pImpl->predictionPattern) {
        pattern *= pImpl->predictionDecayRate;
    }
}

} // namespace nlm

} // namespace nlm
