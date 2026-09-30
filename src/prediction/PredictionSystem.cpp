#include "PredictionSystem.hpp"
#include "../brain/NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    NeuralPrediction* neuralPrediction;  // Reference to brain's neural prediction system
    RandomGenerator* rng;
    
    // Prediction state
    float predictionError;
    float confidence;
    float predictionStability;
    float predictionAccuracy;
    
    // Error tracking
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<float> accuracyHistory;
    
    // Recent prediction data
    std::vector<float> lastPredicted;
    std::vector<float> lastActual;
    double lastPredictionTime;
    
    // Neural prediction parameters
    float predictionSmoothing;      // How much to trust predictions vs new data
    float errorDecayRate;           // How fast prediction error decays
    float confidenceSmoothing;     // How much to smooth confidence changes
    float minConfidence;            // Minimum confidence threshold
    float maxConfidence;            // Maximum confidence threshold
    
    // Training statistics
    size_t totalTrainingSamples;
    float totalPredictionError;
    float cumulativeReward;
    
    Impl() : neuralPrediction(nullptr), rng(nullptr),
             predictionError(0.0f), confidence(0.5f),
             predictionStability(1.0f), predictionAccuracy(0.5f),
             lastPredictionTime(0.0), predictionSmoothing(0.3f),
             errorDecayRate(0.01f), confidenceSmoothing(0.2f),
             minConfidence(0.1f), maxConfidence(1.0f),
             totalTrainingSamples(0), totalPredictionError(0.0f),
             cumulativeReward(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(NeuralPrediction* neuralPrediction, RandomGenerator* rng) {
    pImpl->neuralPrediction = neuralPrediction;
    pImpl->rng = rng;
    
    NLM_LOG_INFO("PredictionSystem: Initialized with neural prediction system");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Update prediction stability based on recent performance
    updatePredictionMetrics();
    
    if (!pImpl->neuralPrediction) {
        NLM_LOG_WARNING("PredictionSystem: No neural prediction system available, returning current state");
        return currentState.clone();
    }
    
    // Use neural prediction system to generate future state
    std::unique_ptr<SensoryInput> predictedState = pImpl->neuralPrediction->predictNextState(
        currentState, pImpl->predictionSmoothing);
    
    if (predictedState) {
        // Store for error calculation
        pImpl->lastPredicted = predictedState->getData();
        pImpl->lastActual = currentState.getData();
        pImpl->lastPredictionTime = predictedState->getTimestamp();
        
        // Calculate prediction confidence based on neural activity patterns
        pImpl->confidence = calculateConfidence(predictedState->getData());
        pImpl->confidence = std::clamp(pImpl->confidence, pImpl->minConfidence, pImpl->maxConfidence);
        
        // Decay error over time
        pImpl->predictionError *= (1.0f - pImpl->errorDecayRate);
        
        NLM_LOG_DEBUG("PredictionSystem: Generated prediction with confidence " + 
                     std::to_string(pImpl->confidence));
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (pImpl->lastPredicted.empty() || pImpl->lastActual.empty()) {
        return;
    }
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error (squared error)
    float sumSquaredError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumSquaredError += error * error;
        maxError = std::max(maxError, std::abs(error));
    }
    
    float meanSquaredError = sumSquaredError / predData.size();
    
    // Update prediction error with temporal weighting
    float temporalWeight = 1.0f;  // Could be time since last update
    pImpl->predictionError = (pImpl->predictionError * 0.9f) + (meanSquaredError * 0.1f * temporalWeight);
    
    // Add to history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    pImpl->confidenceHistory.push_back(pImpl->confidence);
    pImpl->accuracyHistory.push_back(1.0f / (1.0f + meanSquaredError));
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
        pImpl->accuracyHistory.erase(pImpl->accuracyHistory.begin());
    }
    
    // Update confidence based on error
    if (pImpl->predictionError > 0.0f) {
        pImpl->confidence *= std::pow(0.95f, pImpl->predictionError);
        pImpl->confidence = std::clamp(pImpl->confidence, pImpl->minConfidence, pImpl->maxConfidence);
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Updated prediction error to " + std::to_string(pImpl->predictionError) +
                 " with confidence " + std::to_string(pImpl->confidence));
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
    pImpl->confidenceHistory.clear();
    pImpl->accuracyHistory.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("PredictionSystem: History cleared");
}

void PredictionSystem::train(const SensoryInput& observation, float reward) {
    if (!pImpl->neuralPrediction) {
        NLM_LOG_WARNING("PredictionSystem: No neural prediction system for training");
        return;
    }
    
    // Update training statistics
    pImpl->totalTrainingSamples++;
    pImpl->cumulativeReward += reward;
    
    // Use neural prediction for training (plasticity-based learning)
    pImpl->neuralPrediction->trainPrediction(observation, reward);
    
    // Update prediction accuracy
    updatePredictionMetrics();
    
    // Record prediction performance
    pImpl->totalPredictionError += pImpl->predictionError;
    
    if (pImpl->totalTrainingSamples % 100 == 0) {
        float avgError = pImpl->totalPredictionError / pImpl->totalTrainingSamples;
        float avgReward = pImpl->cumulativeReward / pImpl->totalTrainingSamples;
        
        NLM_LOG_INFO("PredictionSystem: Training stats - Samples: " + std::to_string(pImpl->totalTrainingSamples) +
                     ", Avg Error: " + std::to_string(avgError) +
                     ", Avg Reward: " + std::to_string(avgReward) +
                     ", Current Confidence: " + std::to_string(pImpl->confidence));
    }
}

void PredictionSystem::reset() {
    clearHistory();
    pImpl->lastPredicted.clear();
    pImpl->lastActual.clear();
    pImpl->lastPredictionTime = 0.0;
    pImpl->predictionStability = 1.0f;
    pImpl->predictionAccuracy = 0.5f;
    pImpl->totalTrainingSamples = 0;
    pImpl->totalPredictionError = 0.0f;
    pImpl->cumulativeReward = 0.0f;
    
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->reset();
    }
    
    NLM_LOG_INFO("PredictionSystem: Reset complete");
}

float PredictionSystem::getPredictionStability() const {
    return pImpl->predictionStability;
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->predictionAccuracy;
}

void PredictionSystem::updatePredictionMetrics() {
    // Calculate prediction stability (consistency over time)
    if (pImpl->errorHistory.size() >= 10) {
        // Look at recent error consistency
        size_t recentSize = std::min<size_t>(10, pImpl->errorHistory.size());
        float sumRecentErrors = std::accumulate(
            pImpl->errorHistory.end() - recentSize, pImpl->errorHistory.end(), 0.0f);
        float avgRecentError = sumRecentErrors / recentSize;
        
        // Stability: lower average error and less variance = higher stability
        float errorVariance = 0.0f;
        for (size_t i = pImpl->errorHistory.size() - recentSize; i < pImpl->errorHistory.size(); ++i) {
            float diff = pImpl->errorHistory[i] - avgRecentError;
            errorVariance += diff * diff;
        }
        errorVariance /= recentSize;
        
        pImpl->predictionStability = 1.0f / (1.0f + avgRecentError + errorVariance);
    } else {
        pImpl->predictionStability = 1.0f - (pImpl->predictionError * 0.5f);
    }
    
    pImpl->predictionStability = std::clamp(pImpl->predictionStability, 0.0f, 1.0f);
    
    // Calculate prediction accuracy based on confidence vs actual performance
    if (!pImpl->confidenceHistory.empty() && !pImpl->errorHistory.empty()) {
        // Use confidence as predictor of accuracy
        float avgConfidence = std::accumulate(
            pImpl->confidenceHistory.begin(), pImpl->confidenceHistory.end(), 0.0f) /
            pImpl->confidenceHistory.size();
        
        float avgError = std::accumulate(
            pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f) /
            pImpl->errorHistory.size();
        
        // Accuracy is inverse relationship with error, moderated by confidence
        pImpl->predictionAccuracy = (1.0f / (1.0f + avgError)) * avgConfidence;
    }
}

float PredictionSystem::calculateConfidence(const std::vector<float>& predictionData) const {
    if (predictionData.empty()) {
        return 0.5f;
    }
    
    // Calculate confidence based on prediction variance and error patterns
    float mean = std::accumulate(predictionData.begin(), predictionData.end(), 0.0f) / predictionData.size();
    
    float variance = 0.0f;
    for (float value : predictionData) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= predictionData.size();
    
    // Lower variance = higher confidence (in stable predictions)
    float varianceConfidence = 1.0f / (1.0f + variance * 10.0f);
    
    // Blend with historical confidence smoothing
    float smoothedConfidence = pImpl->confidence * (1.0f - pImpl->confidenceSmoothing) + 
                               varianceConfidence * pImpl->confidenceSmoothing;
    
    return smoothedConfidence;
}

} // namespace nlm
