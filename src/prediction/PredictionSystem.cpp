#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction model components
    float learningRate;
    float predictionDecay;
    float minConfidence;
    
    // Pattern matching for prediction
    std::vector<float> lastObservation;
    std::vector<float> predictedObservation;
    SimulationStep lastObservationStep;
    
    // For neural-style prediction
    std::mt19937 rng;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.1f), 
             predictionDecay(0.95f), minConfidence(0.1f), lastObservationStep(0) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement neural prediction using pattern matching and learned associations
    auto currentData = currentState.getData();
    
    if (currentData.empty()) {
        NLM_LOG_WARNING("Empty sensory input for prediction");
        return std::make_unique<SensoryInput>();
    }
    
    // Update last observation
    pImpl->lastObservation = currentData;
    pImpl->lastObservationStep++;
    
    // Apply decay to confidence and error history
    pImpl->confidence *= pImpl->predictionDecay;
    
    // Neural prediction: create new prediction based on learned patterns
    // This is a simplified neural prediction model using weighted combinations
    pImpl->predictedObservation.clear();
    pImpl->predictedObservation.reserve(currentData.size());
    
    // Base prediction on current state with some variation
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Start with current value, add predicted variation
        float predictedValue = currentData[i];
        
        // Add noise for exploration
        if (pImpl->confidence > 0.3f) {
            predictedValue += dist(pImpl->rng) * 0.05f;
        }
        
        // Apply nonlinear transformation (sigmoid-like)
        predictedValue = 1.0f / (1.0f + std::exp(-predictedValue));
        
        pImpl->predictedObservation.push_back(predictedValue);
    }
    
    // Calculate prediction error if we have previous prediction
    if (!pImpl->lastObservation.empty()) {
        float error = 0.0f;
        for (size_t i = 0; i < currentData.size(); ++i) {
            error += std::abs(currentData[i] - pImpl->predictedObservation[i]);
        }
        error /= currentData.size();
        pImpl->predictionError = error;
        
        // Update confidence based on error
        pImpl->confidence = std::max(pImpl->minConfidence, 1.0f - error);
        
        if (pImpl->predictionError > 0.5f) {
            NLM_LOG_DEBUG("High prediction error: " + std::to_string(pImpl->predictionError));
        }
    }
    
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Create new sensory input with predicted state
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(pImpl->predictedObservation);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Enhanced prediction update with neural learning
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate more sophisticated prediction error
    float sumSquaredError = 0.0f;
    float maxAbsoluteError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumSquaredError += diff * diff;
        maxAbsoluteError = std::max(maxAbsoluteError, std::abs(diff));
    }
    
    // Update prediction error with both MSE and max error components
    pImpl->predictionError = std::sqrt(sumSquaredError / predData.size()) * 0.7f + 
                            maxAbsoluteError * 0.3f;
    
    // Store in history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update confidence based on error magnitude
    pImpl->confidence = std::max(pImpl->minConfidence, 
                                std::exp(-pImpl->predictionError * 5.0f));
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
    // Neural learning for prediction model
    auto data = observation.getData();
    
    if (data.empty()) return;
    
    // Adapt prediction parameters based on new observations
    pImpl->learningRate = std::min(0.2f, pImpl->learningRate * 1.1f);
    
    // Update pattern weights (simplified neural learning)
    for (size_t i = 0; i < data.size(); ++i) {
        // This is where the prediction model would learn from the observation
        // In a full implementation, this would update synaptic weights
        // for the predictive coding network
        
        // Simple temporal correlation for now
        float patternStrength = data[i] * pImpl->learningRate;
        
        // Apply to predicted observation for next time step
        if (i < pImpl->predictedObservation.size()) {
            pImpl->predictedObservation[i] *= (1.0f + patternStrength);
        }
    }
    
    // Update historical data
    pImpl->lastObservation = data;
    
    NLM_LOG_DEBUG("Prediction model trained with " + std::to_string(data.size()) + " observations");
}

} // namespace nlm
