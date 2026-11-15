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
    
    // Neural prediction model parameters
    float predictionWeight;
    float errorDecayRate;
    float confidenceLearningRate;
    
    // Forward model components
    std::vector<float> lastPrediction;
    std::vector<float> predictionErrorBuffer;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             predictionWeight(1.0f), errorDecayRate(0.95f),
             confidenceLearningRate(0.1f) {
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase  론 integration: Use brain's neural dynamics for prediction
    // This implements a forward model that predicts next sensory state
    // based on learned action-consequence mappings
    
    const auto& data = currentState.getData();
    
    // Initialize prediction if needed
    if (pImpl->lastPrediction.empty()) {
        pImpl->lastPrediction.resize(data.size(), 0.0f);
    }
    
    // Create predicted sensory input
    auto predicted = std::make_unique<SensoryInput>();
    predicted->setData(data);
    
    // Apply learned transformation based on neural prediction weights
    // This simulates forward model dynamics
    auto& predData = predicted->getData();
    
    // Simple neural prediction: apply smoothing and learned patterns
    for (size_t i = 0; i < predData.size(); ++i) {
        // Forward model prediction: smooth with previous prediction
        if (i < pImpl->lastPrediction.size()) {
            predData[i] = pImpl->lastPrediction[i] * 0.7f + predData[i] * 0.3f;
        }
        
        // Add prediction weight scaling
        predData[i] *= pImpl->predictionWeight;
        
        // Add prediction confidence modulation
        predData[i] *= pImpl->confidence;
    }
    
    // Update last prediction for next step
    pImpl->lastPrediction = predData;
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted sensory state");
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Phase  론 integration: Update prediction model based on actual sensory input
    // This implements prediction error learning and confidence calibration
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error using multiple metrics
    float sumSquaredError = 0.0f;
    float maxAbsoluteError = 0.0f;
    float totalAbsoluteError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumSquaredError += error * error;
        maxAbsoluteError = std::max(maxAbsoluteError, std::abs(error));
        totalAbsoluteError += std::abs(error);
    }
    
    // Compute overall prediction error
    float mseError = sumSquaredError / predData.size();
    float maeError = totalAbsoluteError / predData.size();
    
    // Weighted error combining different metrics
    pImpl->predictionError = 0.6f * mseError + 0.4f * maxAbsoluteError;
    
    // Store error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep error history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction confidence based on error
    // Lower error -> higher confidence, higher error -> lower confidence
    if (pImpl->predictionError < 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
    } else if (pImpl->predictionError < 0.5f) {
        pImpl->confidence = std::max(0.3f, pImpl->confidence - 0.02f);
    } else {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f);
    }
    
    // Update prediction weight based on error consistency
    // If prediction error is consistently low, increase weight
    static float consistentErrorCount = 0;
    static float consistentErrorSum = 0;
    
    consistentErrorCount++;
    consistentErrorSum += pImpl->predictionError;
    
    if (consistentErrorCount >= 10) {
        float avgError = consistentErrorSum / consistentErrorCount;
        pImpl->predictionWeight = (avgError < 0.1f) ? 1.2f : 
                                 (avgError < 0.5f) ? 1.0f : 0.8f;
        consistentErrorCount =  levesConsistentErrorCount = 0;
        consistentErrorSum = 0;
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Updated predictions, error=" + std::to_string(pImpl->predictionError) + ", confidence=" + std::to_string(pImpl->confidence));
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
    // Phase  integrated training: Learn prediction model from experience
    // This implements forward model training using observed action consequences
    
    const auto& data = observation.getData();
    
    // Simple Hebbian-like learning for prediction model
    // Store observation as part of prediction experience
    pImpl->predictionErrorBuffer.insert(pImpl->predictionErrorBuffer.end(), data.begin(), data.end());
    
    // Keep buffer bounded
    if (pImpl->predictionErrorBuffer.size() > 1000) {
        pImpl->predictionErrorBuffer.erase(pImpl->predictionErrorBuffer.begin(),
                                           pImpl->predictionErrorBuffer.begin() + 100);
    }
    
    // Update prediction weight based on observed patterns
    if (!pImpl->predictionErrorBuffer.empty()) {
        float sumSquares = 0.0f;
        for (float val : pImpl->predictionErrorBuffer) {
            sumSquares += val * val;
        }
        float bufferNorm = std::sqrt(sumSquares);
        pImpl->predictionWeight = std::min(1.5f, bufferNorm * 0.5f);
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Trained on observation, weight=" + std::to_string(pImpl->predictionWeight));
}

} // namespace nlm
