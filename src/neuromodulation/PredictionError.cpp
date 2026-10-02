#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float learningRate;
    float predictionError;
    float confidence;
    TimestepDuration lastUpdateTime;
    float surpriseThreshold;
    float neuromodulatoryStrength;
    float errorHistorySize;
    
    // Error components
    float intensityError;
    float spatialError;
    float temporalError;
    
    // For adaptive thresholding
    float runningMaxError;
    float runningMinError;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.15f), 
             learningRate(0.08f), predictionError(0.0f), confidence(0.5f),
             lastUpdateTime(0), surpriseThreshold(0.3f), neuromodulatoryStrength(1.0f),
             errorHistorySize(0.0f), runningMaxError(0.0f), runningMinError(0.0f) {
        intensityError = 0.0f;
        spatialError = 0.0f;
        temporalError = 0.0f;
    }
};

PredictionError::PredictionError() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Prediction error signal system initialized");
}

PredictionError::~PredictionError::PredictionError() = default;

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error is a powerful learning signal
    // Higher error increases plasticity, but with diminishing returns
    float error = pImpl->predictionError;
    float confidence = pImpl->confidence;
    
    // Non-linear relationship: moderate error maximizes learning
    float plasticity = error * (1.0f - error) * confidence;
    
    // Scale with neuromodulatory strength
    return plasticity * pImpl->neuromodulatoryStrength;
}

void PredictionError::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Decay towards baseline
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Adaptive baseline based on error history
    if (pImpl->predictionError > pImpl->runningMaxError) {
        pImpl->runningMaxError = pImpl->predictionError;
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + pImpl->predictionError * 0.01f);
    }
}

float PredictionError::computeError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        pImpl->predictionError = 0.0f;
        return 0.0f;
    }
    
    // Compute different error components
    pImpl->predictionError = computeTotalError(predicted, actual);
    pImpl->intensityError = computeIntensityError(predicted, actual);
    pImpl->spatialError = computeSpatialError(predicted, actual);
    pImpl->temporalError = computeTemporalError(predicted, actual);
    
    // Update error history stats
    if (pImpl->predictionError > pImpl->runningMaxError) {
        pImpl->runningMaxError = pImpl->predictionError;
    }
    if (pImpl->predictionError < pImpl->runningMinError) {
        pImpl->runningMinError = pImpl->predictionError;
    }
    
    // Update level based on error
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->predictionError * pImpl->learningRate * 2.0f);
    
    // Update confidence based on error consistency
    pImpl->confidence = computeConfidenceFromHistory();
    
    return pImpl->predictionError;
}

float PredictionError::computeTotalError(const std::vector<float>& predicted,
                                       const std::vector<float>& actual) const {
    float totalDiff = 0.0f;
    float intensityDiff = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalDiff += diff;
        intensityDiff += diff * diff;
    }
    
    float meanError = totalDiff / predicted.size();
    float meanSqError = intensityDiff / predicted.size();
    
    // Combine components
    float spatialVar = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    float spatialError = std::sqrt(spatialVar / predicted.size());
    
    return std::sqrt(meanSqError + spatialError * spatialError);
}

float PredictionError::computeIntensityError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual) const {
    float sum = 0.0f;
    float sqSum = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        sum += diff;
        sqSum += diff * diff;
    }
    
    float mean = sum / predicted.size();
    return std::sqrt(sqSum / predicted.size());
}

float PredictionError::computeSpatialError(const std::vector<float>& predicted,
                                          const std::vector<float>& actual) const {
    float meanDiff = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        meanDiff += (predicted[i] - actual[i]);
    }
    meanDiff /= predicted.size();
    
    float variance = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = (predicted[i] - actual[i]) - meanDiff;
        variance += diff * diff;
    }
    
    return std::sqrt(variance / predicted.size());
}

float PredictionError::computeTemporalError(const std::vector<float>& predicted,
                                           const std::vector<float>& actual) const {
    // Temporal error based on timing of changes
    // For now, return a placeholder value
    // In a real implementation, this would analyze timing dynamics
    return 0.0f;
}

float PredictionError::computeConfidenceFromHistory() const {
    // Compute confidence based on consistency of recent errors
    if (pImpl->errorHistory.empty()) return 0.5f;
    
    float totalVariance = 0.0f;
    float meanError = 0.0f;
    
    for (float error : pImpl->errorHistory) {
        meanError += error;
    }
    meanError /= pImpl->errorHistory.size();
    
    for (float error : pImpl->errorHistory) {
        totalVariance += (error - meanError) * (error - meanError);
    }
    
    float variance = totalVariance / pImpl->errorHistory.size();
    
    // Lower variance = higher confidence
    float confidence = std::max(0.1f, 1.0f - variance);
    
    return confidence;
}

float PredictionError::getModulationSignal() const {
    // Convert prediction error to neuromodulatory signal strength
    // Higher error = stronger modulation for learning
    return std::min(1.0f, pImpl->predictionError * 2.0f * pImpl->neuromodulatoryStrength);
}

void PredictionError::recordError(float error, SimulationStep step) {
    pImpl->errorHistory.push_back(error);
    pImpl->errorHistorySize = pImpl->errorHistory.size();
    
    // Keep history manageable
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.pop_front();
    }
    
    // Update running statistics
    if (error > pImpl->runningMaxError) {
        pImpl->runningMaxError = error;
    }
    if (error < pImpl->runningMinError || pImpl->errorHistorySize == 0) {
        pImpl->runningMinError = error;
    }
}

bool PredictionError::isSurprising() const {
    return pImpl->predictionError > pImpl->surpriseThreshold;
}

void PredictionError::setSurpriseThreshold(float t) {
    pImpl->surpriseThreshold = std::clamp(t, 0.0f, 1.0f);
}

float PredictionError::getSurpriseThreshold() const {
    return pImpl->surpriseThreshold;
}

void PredictionError::setNeuromodulatoryStrength(float strength) {
    pImpl->neuromodulatoryStrength = std::clamp(strength, 0.1f, 3.0f);
}

float PredictionError::getNeuromodulatoryStrength() const {
    return pImpl->neuromulatoryStrength;
}

const std::deque<float>& PredictionError::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionError::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
    pImpl->runningMaxError = 0.0f;
    pImpl->runningMinError = 0.0f;
}

} // namespace nlm