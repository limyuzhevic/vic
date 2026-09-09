#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Store recent predictions for comparison with actual observations
    std::vector<std::vector<float>> predictionBuffer;
    float temporalDecay;  // For forgetting old predictions
    
    Impl() : predictionError(0.0f), confidence(0.5f), temporalDecay(0.95f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    // For now, we store the prediction in the buffer for later error computation
    const auto& currentData = currentState.getData();
    
    // Create a prediction based on simple patterns
    // In a real implementation, this would use neural dynamics to predict
    auto predictedInput = std::make_unique<SensoryInput>();
    std::vector<float> predictedData = currentData;
    
    // Simple prediction: slight variations based on patterns
    if (!currentData.empty()) {
        // Apply a small forward shift pattern to simulate prediction
        for (size_t i = 0; i < currentData.size(); ++i) {
            if (i < currentData.size() - 1) {
                predictedData[i] = currentData[i] * 0.8f + currentData[i + 1] * 0.2f;
            } else {
                predictedData[i] = currentData[i] * 1.1f;  // Slight forward drift
            }
        }
        
        // Add some noise to simulate uncertainty
        if (!pImpl->predictionBuffer.empty()) {
            float noiseFactor = 1.0f - pImpl->confidence;
            for (size_t i = 0; i < predictedData.size(); ++i) {
                predictedData[i] += (pImpl->rng ? pImpl->rng->uniformFloat(-0.1f, 0.1f) : 0.0f) * noiseFactor;
            }
        }
        
        // Store prediction for error computation
        pImpl->predictionBuffer.push_back(predictedData);
        
        // Apply temporal decay to older predictions
        if (pImpl->predictionBuffer.size() > 10) {
            std::rotate(pImpl->predictionBuffer.begin(), 
                       pImpl->predictionBuffer.begin() + 1, 
                       pImpl->predictionBuffer.end());
            pImpl->predictionBuffer.pop_back();
        }
    }
    
    predictedInput->setData(predictedData);
    
    NLM_LOG_DEBUG("Prediction made: current sum=" + 
                std::to_string(std::accumulate(currentData.begin(), currentData.end(), 0.0f)) +
                ", predicted sum=" + 
                std::to_string(std::accumulate(predictedData.begin(), predictedData.end(), 0.0f)));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                         const SensoryInput& actual) {
    // Update predictions based on actual observation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - std::sqrt(pImpl->predictionError) * 2.0f);
        
        // Apply temporal decay
        for (auto& error : pImpl->errorHistory) {
            error *= pImpl->temporalDecay;
        }
        
        // Limit history size
        if (pImpl->errorHistory.size() > 1000) {
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
    pImpl->predictionBuffer.clear();
}

} // namespace nlm
