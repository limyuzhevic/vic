#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    float learningRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using neural dynamics
    // Use current state as basis and apply learned dynamics
    auto predictedInput = std::make_unique<SensoryInput>();
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return predictedInput;
    }
    
    // Simple forward prediction: apply decay and update based on prediction error
    std::vector<float> predictedData(currentData.size());
    float sum = std::accumulate(currentData.begin(), currentData.end(), 0.0f);
    float mean = sum / currentData.size();
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Decay toward mean with some noise
        float predicted = currentData[i] * 0.9f + mean * 0.1f;
        
        // Add small random variation for exploration
        float variation = (std::rand() % 100) / 100.0f * 0.05f - 0.025f;
        predicted += variation;
        
        // Clamp to reasonable range
        predicted = std::clamp(predicted, 0.0f, 1.0f);
        predictedData[i] = predicted;
    }
    
    predictedInput->setData(predictedData);
    
    // Store in history
    pImpl->predictionHistory.push_back(mean);
    
    // Update confidence based on prediction error (will be updated later)
    if (!pImpl->errorHistory.empty()) {
        float avgError = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f) / 
                        pImpl->errorHistory.size();
        pImpl->confidence = std::max(0.1f, 1.0f - avgError);
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    pImpl->predictionError = sumError / predData.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Update confidence based on error magnitude
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
    
    // Limit history size to prevent memory bloat
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    if (pImpl->predictionHistory.size() > 1000) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Implement prediction model training
    // Update internal weights based on observed data
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Compute statistics from observation
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    // Update internal state
    // In a full implementation, this would adjust prediction weights
    // For now, just update confidence based on data consistency
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    // High variance means more interesting data for learning
    pImpl->confidence = std::min(1.0f, pImpl->confidence + pImpl->learningRate * variance);
}

} // namespace nlm
