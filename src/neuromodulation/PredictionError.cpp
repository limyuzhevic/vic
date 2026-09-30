#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float trace;
    float integration;
    float learningRate;
    float predictionConfidence;
    float errorMagnitude;
    float temporalWindow[5];  // Store recent prediction errors for temporal analysis
    int windowIndex;
    float unexpectedness;
    float behavioralImpact;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             trace(0.0f), integration(0.0f), learningRate(0.3f),
             predictionConfidence(0.5f), errorMagnitude(0.0f), unexpectedness(0.0f),
             behavioralImpact(0.0f), windowIndex(0) {
        for (int i = 0; i < 5; ++i) {
            temporalWindow[i] = 0.0f;
        }
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionError system enhanced initialized");
}

PredictionError::~PredictionError() {
    NLM_LOG_INFO("PredictionError system enhanced destroyed");
}

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system enhanced initialized with learning rate %.2f", pImpl->learningRate);
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Compute raw error
    float rawError = actual - predicted;
    pImpl->error = rawError;
    
    // Compute magnitude
    pImpl->errorMagnitude = std::abs(rawError);
    
    // Compute unexpectedness based on prediction confidence
    pImpl->unexpectedness = pImpl->errorMagnitude * (1.0f - pImpl->predictionConfidence);
    
    // Update temporal window
    pImpl->temporalWindow[pImpl->windowIndex] = pImpl->error;
    pImpl->windowIndex = (pImpl->windowIndex + 1) % 5;
    
    // Compute average recent error
    float avgError = 0.0f;
    for (int i = 0; i < 5; ++i) {
        avgError += pImpl->temporalWindow[i];
    }
    avgError /= 5.0f;
    
    // Integration with prediction error trace
    pImpl->trace = std::max(pImpl->trace, pImpl->errorMagnitude * 0.5f);
    pImpl->trace = std::max(0.0f, pImpl->trace - 0.02f);
    
    // Behavioral impact based on error significance
    pImpl->behavioralImpact = std::min(1.0f, pImpl->errorMagnitude * 2.0f * pImpl->unexpectedness);
    
    // Store in history
    pImpl->history.push_back(pImpl->error);
    
    // Cap history size
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    
    // Update prediction confidence based on recent prediction accuracy
    if (!pImpl->history.empty()) {
        float recentAvgError = 0.0f;
        size_t historySize = std::min(static_cast<size_t>(10), pImpl->history.size());
        for (size_t i = 0; i < historySize; ++i) {
            recentAvgError += std::abs(pImpl->history[pImpl->history.size() - 1 - i]);
        }
        recentAvgError /= historySize;
        
        // Increase confidence if predictions have been accurate
        pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence + 0.05f * (1.0f - recentAvgError));
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->trace = 0.0f;
    pImpl->integration = 0.0f;
    pImpl->predictionConfidence = 0.5f;
    pImpl->errorMagnitude = 0.0f;
    pImpl->unexpectedness = 0.0f;
    pImpl->behavioralImpact = 0.0f;
}

float PredictionError::getMagnitude() const {
    return pImpl->errorMagnitude;
}

float PredictionError::getTrace() const {
    return pImpl->trace;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void PredictionError::setPredictionConfidence(float confidence) {
    pImpl->predictionConfidence = std::clamp(confidence, 0.0f, 1.0f);
}

void PredictionError::enhanceLearning(float learningSignal) {
    // Enhanced prediction error specifically for learning acceleration
    pImpl->learningRate = std::min(pImpl->learningRate + learningSignal * 0.1f, 1.0f);
    
    // Update trace with learning signal
    pImpl->trace = std::min(pImpl->trace + learningSignal * 0.5f, 1.0f);
    
    // Increase prediction confidence for successful learning
    pImpl->predictionConfidence = std::min(pImpl->predictionConfidence + learningSignal * 0.2f, 1.0f);
}

void PredictionError::enhanceBehavioralAdaptation(float adaptationStrength) {
    // Enhanced prediction error specifically for behavioral adaptation
    pImpl->behavioralImpact = std::min(pImpl->behavioralImpact + adaptationStrength * 0.3f, 1.0f);
    
    // Stronger prediction errors trigger stronger behavioral adaptation
    pImpl->integration = std::min(pImpl->integration + adaptationStrength * 0.4f, 1.0f);
    
    // Update prediction confidence based on adaptation success
    pImpl->predictionConfidence = std::min(pImpl->predictionConfidence + adaptationStrength * 0.15f, 1.0f);
}

float PredictionError::getLearningEfficiency() const {
    // Compute learning efficiency based on error significance and behavioral impact
    float errorSignificance = pImpl->errorMagnitude * pImpl->unexpectedness;
    float adaptationEfficiency = pImpl->behavioralImpact * pImpl->learningRate;
    
    return std::min(1.0f, (errorSignificance + adaptationEfficiency) / 2.0f);
}

float PredictionError::getTemporalConsistency() const {
    // Compute consistency of prediction errors over time
    if (pImpl->history.size() < 2) return 1.0f;
    
    float stdDev = 0.0f;
    float mean = 0.0f;
    for (float error : pImpl->history) {
        mean += error;
    }
    mean /= pImpl->history.size();
    
    for (float error : pImpl->history) {
        float diff = error - mean;
        stdDev += diff * diff;
    }
    stdDev = std::sqrt(stdDev / pImpl->history.size());
    
    // Lower standard deviation means more consistent predictions
    return std::max(0.0f, 1.0f - stdDev);
}

} // namespace nlm