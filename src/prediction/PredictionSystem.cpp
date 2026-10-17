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
    // Prediction system parameters
    size_t predictionDepth;
    float learningRate;
    bool enableTemporalAbstraction;
    float predictionAccuracy;
    float temporalAbstractionScore;
    size_t correctPredictions;
    size_t totalPredictions;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f), predictionDepth(10), learningRate(0.01f)
        , enableTemporalAbstraction(true), predictionAccuracy(0.0f), temporalAbstractionScore(0.0f)
        , correctPredictions(0), totalPredictions(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(size_t predictionDepth, float learningRate,
                                bool enableTemporalAbstraction) {
    pImpl->predictionDepth = predictionDepth;
    pImpl->learningRate = std::max(0.001f, std::min(learningRate, 1.0f));
    pImpl->enableTemporalAbstraction = enableTemporalAbstraction;
    pImpl->predictionAccuracy = 0.0f;
    pImpl->temporalAbstractionScore = 0.0f;
    pImpl->correctPredictions = 0;
    pImpl->totalPredictions = 0;
    pImpl->errorHistory.clear();
    
    NLM_LOG_INFO("PredictionSystem initialized: depth=" + std::to_string(predictionDepth) +
                 " learning_rate=" + std::to_string(pImpl->learningRate) +
                 " temporal_abstraction=" + std::to_string(enableTemporalAbstraction));
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    NLM_LOG_DEBUG("PredictionSystem: predicting next state");
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float totalMagnitude = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            totalMagnitude += std::abs(predData[i]) + std::abs(actualData[i]);
        }
        
        float meanSquaredError = sumError / predData.size();
        pImpl->predictionError = meanSquaredError;
        
        // Calculate normalized error (0-1, where 0 is perfect prediction)
        if (totalMagnitude > 0.0f) {
            float normalizedError = std::sqrt(meanSquaredError) / (totalMagnitude / predData.size());
            pImpl->predictionError = std::min(normalizedError, 1.0f);
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Update accuracy tracking
        pImpl->totalPredictions++;
        if (pImpl->predictionError < 0.1f) {  // Threshold for good prediction
            pImpl->correctPredictions++;
            pImpl->predictionAccuracy = static_cast<float>(pImpl->correctPredictions) / pImpl->totalPredictions;
        }
        
        // Apply learning to improve predictions
        if (pImpl->learningRate > 0.0f) {
            applyLearning(predicted, actual);
        }
        
        NLM_LOG_DEBUG("PredictionSystem updated: error=" + std::to_string(pImpl->predictionError) +
                     " confidence=" + std::to_string(pImpl->confidence) +
                     " accuracy=" + std::to_string(pImpl->predictionAccuracy));
    }
}

void PredictionSystem::applyLearning(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Simple learning rule: adjust predicted values based on actual observations
    // This simulates the prediction error-driven learning in biological systems
    for (size_t i = 0; i < predData.size(); ++i) {
        float prediction = predData[i];
        float observation = actualData[i];
        float error = observation - prediction;
        
        // Apply Hebbian-like learning with decay
        float learningEffect = pImpl->learningRate * error * pImpl->confidence;
        
        // Update prediction based on error (simple model updating)
        // In a real system, this would be more sophisticated
        float updatedPrediction = prediction + learningEffect;
        
        // Clamp to reasonable range
        if (std::isfinite(updatedPrediction)) {
            // This would update the internal prediction model
            // For now, just log the learning effect
            if (std::abs(learningEffect) > 0.01f) {
                NLM_LOG_DEBUG("PredictionSystem learning: neuron " + std::to_string(i) +
                             " updated from " + std::to_string(prediction) +
                             " to " + std::to_string(updatedPrediction));
            }
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
    NLM_LOG_INFO("PredictionSystem history cleared");
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model with real neural machinery
    // PLACEHOLDER: Simple observation-based training
    NLM_LOG_DEBUG("PredictionSystem training with observation: " + 
                 observation.getType() + " (" + std::to_string(observation.getDimensions()) + " dims)");
}

float PredictionSystem::getPredictionAccuracy() const {
    if (pImpl->totalPredictions == 0) return 0.0f;
    return pImpl->predictionAccuracy;
}

float PredictionSystem::getTemporalAbstractionScore() const {
    return pImpl->temporalAbstractionScore;
}

} // namespace nlm
