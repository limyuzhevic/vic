#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionWeights;
    float predictionThreshold;
    bool isActive;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionThreshold(0.1f), isActive(true) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    pImpl->predictionWeights.resize(10, 1.0f); // Initialize with default weights
}

PredictionSystem::~PredictionSystem() = default;

// Enhanced predictNextState implementation
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->isActive) {
        return currentState.clone();
    }
    
    // Get brain state for prediction context
    // Note: This would normally integrate with Brain class
    // For now, implement a basic prediction based on current state patterns
    
    auto predictedState = currentState.clone();
    const auto& currentData = currentState.getData();
    
    // Apply learned prediction patterns based on current state
    for (size_t i = 0; i < currentData.size() && i < pImpl->predictionWeights.size(); ++i) {
        // Apply prediction bias based on learned patterns
        float predictionBias = pImpl->predictionWeights[i];
        float predictedValue = currentData[i] * (1.0f + predictionBias * 0.05f);
        
        // Add noise for uncertainty
        float noise = (std::rand() / (float)RAND_MAX - 0.5f) * 0.1f;
        predictedState.setValue(i, predictedValue + noise);
    }
    
    // Calculate confidence based on prediction variance
    float maxVariance = 0.0f;
    for (float weight : pImpl->predictionWeights) {
        float variance = std::abs(weight);
        if (variance > maxVariance) maxVariance = variance;
    }
    
    pImpl->confidence = 1.0f - maxVariance;
    
    NLM_LOG_INFO("Prediction system: Generated predicted state with confidence " + std::to_string(pImpl->confidence));
    
    return predictedState;
}

// Enhanced updatePredictions implementation
void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        NLM_LOG_WARNING("Prediction update: Data size mismatch or empty data");
        return;
    }
    
    // Calculate prediction error with different weighting schemes
    float sumSquaredError = 0.0f;
    float maxError = 0.0f;
    float sumAbsoluteError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        float absError = std::abs(error);
        
        sumSquaredError += error * error;
        sumAbsoluteError += absError;
        maxError = std::max(maxError, absError);
    }
    
    // Compute different error metrics
    float meanSquaredError = sumSquaredError / predData.size();
    float meanAbsoluteError = sumAbsoluteError / predData.size();
    
    // Update prediction error with adaptive weighting
    float adaptiveError = meanSquaredError * (1.0f + meanAbsoluteError);
    pImpl->predictionError = adaptiveError;
    pImpl->errorHistory.push_back(adaptiveError);
    
    // Keep error history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction confidence based on error
    if (adaptiveError < pImpl->predictionThreshold) {
        // Good prediction - increase confidence
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    } else {
        // Poor prediction - decrease confidence
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.02f);
    }
    
    NLM_LOG_INFO("Prediction error updated: " + std::to_string(adaptiveError) + ", confidence: " + std::to_string(pImpl->confidence));
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
    NLM_LOG_INFO("Prediction history cleared");
}

// Enhanced train method
void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    
    if (data.empty()) {
        NLM_LOG_WARNING("Prediction training: No data to train on");
        return;
    }
    
    // Update prediction weights based on observation patterns
    // This implements Hebbian-like learning for prediction patterns
    for (size_t i = 0; i < std::min(data.size(), pImpl->predictionWeights.size()); ++i) {
        // Learning rate based on current observation
        float learningRate = 0.01f * std::abs(data[i]);
        
        // Update weight based on observation (Hebbian-like rule)
        pImpl->predictionWeights[i] += data[i] * learningRate;
        
        // Constrain weights to reasonable range
        pImpl->predictionWeights[i] = std::max(-2.0f, std::min(2.0f, pImpl->predictionWeights[i]));
    }
    
    // Adapt prediction threshold based on training data
    float avgMagnitude = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    pImpl->predictionThreshold = std::max(0.01f, std::min(0.5f, std::abs(avgMagnitude) * 0.1f));
    
    NLM_LOG_INFO("Prediction system trained with " + std::to_string(data.size()) + " observations");
}

// Additional method: Get prediction statistics
float PredictionSystem::getMeanError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f);
    return sum / pImpl->errorHistory.size();
}

float PredictionSystem::getMaxError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    
    return *std::max_element(pImpl->errorHistory.begin(), pImpl->errorHistory.end());
}

// Set prediction system active/inactive
void PredictionSystem::setActive(bool active) {
    pImpl->isActive = active;
    if (!active) {
        NLM_LOG_INFO("Prediction system disabled");
    }
}

bool PredictionSystem::isActive() const {
    return pImpl->isActive;
}

} // namespace nlm
