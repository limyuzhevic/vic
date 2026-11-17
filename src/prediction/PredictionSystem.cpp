#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionWeights;  // Learned weights for prediction
    float predictionDecay;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionDecay(0.95f) {
        predictionWeights.reserve(100);  // Pre-allocate for performance
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction accuracy
        updateConfidence(predData, actualData);
    }
}

void PredictionSystem::updateConfidence(const std::vector<float>& predData, const std::vector<float>& actualData) {
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction confidence based on error magnitude
    float totalSquaredError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        totalSquaredError += diff * diff;
    }
    
    float meanSquaredError = totalSquaredError / predData.size();
    
    // Convert to confidence: high error = low confidence, low error = high confidence
    // Use exponential decay for smooth confidence updates
    pImpl->confidence = std::exp(-meanSquaredError * 10.0f);  // Scale factor for reasonable confidence
    pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, just update internal statistics
    const auto& obsData = observation.getData();
    
    // Simple model training: accumulate statistics
    if (!obsData.empty()) {
        // Update prediction weights based on observation
        for (float value : obsData) {
            // Simple learning rule: reinforce patterns that lead to rewards
            pImpl->predictionWeights.push_back(value);
            if (pImpl->predictionWeights.size() > 100) {
                pImpl->predictionWeights.erase(pImpl->predictionWeights.begin());
            }
        }
    }
    
    // Decay old weights
    for (float& weight : pImpl->predictionWeights) {
        weight *= pImpl->predictionDecay;
    }
}

} // namespace nlm
