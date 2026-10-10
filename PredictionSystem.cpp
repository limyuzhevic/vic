#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Prediction state
    float predictionError;
    float confidence;
    float predictionLevel;
    float predictionAccuracy;
    std::vector<float> mostRecentPrediction;
    
    // History tracking
    std::deque<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionLevel(0.0f), predictionAccuracy(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::vector<float> PredictionSystem::generatePrediction(SimulationStep currentStep) {
    if (pImpl->brain == nullptr) {
        return std::vector<float>();
    }
    
    // TODO: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Generate a simple prediction
    if (pImpl->mostRecentPrediction.empty()) {
        return std::vector<float>(10, 0.5f);
    }
    
    NLM_LOG_DEBUG("Prediction generated for step %d", currentStep);
    return pImpl->mostRecentPrediction;
}

float PredictionSystem::updateWithObservation(const std::vector<float>& actualState, 
                                           SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    if (predicted.empty()) {
        pImpl->predictionError = 0.0f;
        return pImpl->predictionError;
    }
    
    // Compute prediction error
    pImpl->predictionError = computePredictionError(predicted, actualState);
    
    // Add to history
    addToPredictionHistory(pImpl->predictionError);
    
    // Update prediction accuracy based on error
    pImpl->predictionAccuracy = std::max(0.0f, 1.0f - pImpl->predictionError);
    
    // Update confidence based on accuracy and history
    if (!pImpl->errorHistory.empty()) {
        float avgError = std::accumulate(pImpl->errorHistory.begin(), 
                                        pImpl->errorHistory.end(), 0.0f) / 
                         pImpl->errorHistory.size();
        pImpl->confidence = std::max(0.1f, 1.0f - avgError);
    }
    
    // Update prediction level
    pImpl->predictionLevel = pImpl->predictionAccuracy * pImpl->confidence;
    
    NLM_LOG_DEBUG("Prediction updated at step %d: error=%.3f, accuracy=%.3f, confidence=%.3f, level=%.3f", 
                  currentStep, pImpl->predictionError, pImpl->predictionAccuracy, 
                  pImpl->confidence, pImpl->predictionLevel);
    
    return pImpl->predictionError;
}

void PredictionSystem::addToPredictionHistory(float predictionError) {
    pImpl->errorHistory.push_back(predictionError);
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted,
                                            const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumError += diff * diff;
    }
    
    return std::sqrt(sumError / predicted.size());
}

void PredictionSystem::updatePredictionWeights(float error, float reward) {
    // TODO: Implement weight updates based on error and reward
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getPredictionConfidence() const {
    return pImpl->confidence;
}

const std::deque<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->mostRecentPrediction.clear();
}

float PredictionSystem::getPredictionLevel() const {
    return pImpl->predictionLevel;
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->predictionAccuracy;
}

const std::vector<float>& PredictionSystem::getMostRecentPrediction() const {
    return pImpl->mostRecentPrediction;
}

} // namespace nlm