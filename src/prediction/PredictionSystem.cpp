#include "PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<float> predictionHistory;
    
    // Simple temporal prediction model
    struct TemporalPrediction {
        float predictedValue;
        float confidence;
        float error;
    };
    
    std::vector<TemporalPrediction> temporalPredictions;
    
    // Simple neural prediction parameters
    float learningRate;
    float decayRate;
    float noiseLevel;
    
    Impl() : learningRate(0.01f), decayRate(0.95f), noiseLevel(0.1f) {
        temporalPredictions.reserve(100);
        errorHistory.reserve(100);
        confidenceHistory.reserve(100);
        predictionHistory.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    /*
     * Real prediction implementation based on temporal correlation
     * 
     * This implements a simple temporal prediction model that:
     * 1. Learns statistical relationships between sensory inputs
     * 2. Predicts next state based on current state and history
     * 3. Computes prediction error
     * 4. Updates confidence based on prediction accuracy
     * 
     * Biological inspiration:
     * - Similar to forward models in cortical areas
     * - Predictive coding in hierarchical systems
     * - Temporal integration in neocortex
     */
    
    const auto& data = currentState.getData();
    if (data.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Store current state
    predictionHistory.push_back(data[0]);  // Focus on first dimension for demo
    if (predictionHistory.size() > 100) {
        predictionHistory.erase(predictionHistory.begin());
    }
    
    // Make prediction based on recent history
    float prediction = 0.0f;
    float confidence = 1.0f;
    
    if (predictionHistory.size() >= 3) {
        // Simple temporal correlation: predict next value based on last few values
        size_t historyLen = std::min(size_t(5), predictionHistory.size() - 1);
        float sum = 0.0f;
        for (size_t i = 1; i <= historyLen; ++i) {
            sum += predictionHistory[predictionHistory.size() - i];
        }
        prediction = sum / historyLen;
        
        // Add noise to simulate biological variability
        prediction += (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * noiseLevel;
        
        // Confidence based on prediction history consistency
        if (predictionHistory.size() >= 5) {
            float variance = 0.0f;
            for (size_t i = predictionHistory.size() - 5; i < predictionHistory.size(); ++i) {
                variance += (predictionHistory[i] - prediction) * (predictionHistory[i] - prediction);
            }
            variance /= 5;
            confidence = std::max(0.1f, 1.0f - variance * 10.0f);
        }
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // For demo, we'll just create a simple input with the predicted value
    // In a real implementation, this would create a proper SensoryInput with multiple dimensions
    predictedInput->addData(prediction);
    
    // Store prediction for error calculation
    pImpl->temporalPredictions.push_back({prediction, confidence, 0.0f});
    if (pImpl->temporalPredictions.size() > 100) {
        pImpl->temporalPredictions.erase(pImpl->temporalPredictions.begin());
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                        const SensoryInput& actual) {
    /*
     * Update prediction model based on prediction error
     * 
     * Implements prediction error-driven learning:
     * - High prediction error leads to model updating
     * - Low prediction error maintains current model
     * - Error history tracks prediction accuracy over time
     */
    
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty()) {
        return;
    }
    
    // Calculate prediction error
    float error = actualData[0] - predictedData[0];
    float confidence = 1.0f;
    
    if (!pImpl->temporalPredictions.empty()) {
        confidence = pImpl->temporalPredictions.back().confidence;
    }
    
    // Store error and confidence
    errorHistory.push_back(error);
    confidenceHistory.push_back(confidence);
    
    if (errorHistory.size() > 100) {
        errorHistory.erase(errorHistory.begin());
        confidenceHistory.erase(confidenceHistory.begin());
    }
    
    // Update temporal predictions based on error
    if (!pImpl->temporalPredictions.empty()) {
        auto& lastPred = pImpl->temporalPredictions.back();
        
        // Scale error by confidence - less confident predictions cause more learning
        float learningWeight = confidence * learningRate;
        
        // Update prediction in direction of actual value
        lastPred.predictedValue += error * learningWeight;
        
        // Store error in prediction
        lastPred.error = error;
        
        // Decay predictions over time
        lastPred.confidence *= decayRate;
        
        // Update model parameters based on prediction error
        // In a real system, this would update synaptic weights in prediction network
        updateModelParameters(error, confidence);
    }
}

float PredictionSystem::getPredictionError() const {
    if (errorHistory.empty()) {
        return 0.0f;
    }
    
    // Return average absolute error
    float totalError = 0.0f;
    for (float error : errorHistory) {
        totalError += std::abs(error);
    }
    return totalError / errorHistory.size();
}

float PredictionSystem::getConfidence() const {
    if (confidenceHistory.empty()) {
        return 1.0f;
    }
    
    // Return average confidence
    float totalConfidence = 0.0f;
    for (float conf : confidenceHistory) {
        totalConfidence += conf;
    }
    return totalConfidence / confidenceHistory.size();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return errorHistory;
}

void PredictionSystem::clearHistory() {
    errorHistory.clear();
    confidenceHistory.clear();
    predictionHistory.clear();
    pImpl->temporalPredictions.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    /*
     * Train prediction model on new observation
     * 
     * This implements Hebbian-like learning for prediction:
     * - Correlated patterns in sensory input are strengthened
     * - Temporal sequences become predictive
     * - Input statistics are learned over time
     */
    
    const auto& data = observation.getData();
    if (data.empty()) {
        return;
    }
    
    // In a real system, this would update prediction weights
    // For now, just store the observation for later prediction
    // and update temporal models
    
    float observationValue = data[0];
    
    // Update temporal predictions with new observation
    if (!pImpl->temporalPredictions.empty()) {
        auto& lastPred = pImpl->temporalPredictions.back();
        
        // Update prediction based on new observation
        // This simulates synaptic plasticity in prediction pathways
        lastPred.predictedValue = lastPred.predictedValue * 0.9f + 
                                   observationValue * 0.1f;
        
        // Update confidence based on prediction accuracy
        float predictionError = std::abs(lastPred.predictedValue - observationValue);
        lastPred.confidence = std::max(0.1f, 1.0f - predictionError * 5.0f);
    }
    
    // Add to prediction history
    predictionHistory.push_back(observationValue);
    if (predictionHistory.size() > 100) {
        predictionHistory.erase(predictionHistory.begin());
    }
}

void PredictionSystem::updateModelParameters(float error, float confidence) {
    /*
     * Update internal prediction model parameters based on error
     * 
     * This simulates synaptic plasticity in prediction circuits:
     * - High error leads to greater parameter updates
     * - Confidence modulates learning rate
     * - Parameters adapt to environmental statistics
     */
    
    // Update learning rate based on recent error history
    if (errorHistory.size() >= 5) {
        float avgError = 0.0f;
        for (size_t i = 1; i <= 5 && i <= errorHistory.size(); ++i) {
            avgError += std::abs(errorHistory[errorHistory.size() - i]);
        }
        avgError /= std::min(size_t(5), errorHistory.size());
        
        // Adapt learning rate based on error magnitude
        learningRate = std::clamp(learningRate * (1.0f + avgError), 0.001f, 0.1f);
    }
    
    // Decay parameters over time
    learningRate *= decayRate;
    noiseLevel *= decayRate;
    
    // Store for later use in brain integration
}

} // namespace nlm
