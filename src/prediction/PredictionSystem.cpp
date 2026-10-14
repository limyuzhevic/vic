#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    Brain* brainReference;
    SimulationStep lastPredictionStep;
    
    // Prediction model weights
    std::vector<float> predictionWeights;
    std::vector<float> predictionBiases;
    
    // Error correction components
    std::vector<float> errorCorrection;
    float learningRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f),
             brainReference(nullptr), lastPredictionStep(0) {
        predictionWeights.reserve(100);
        predictionBiases.reserve(10);
        errorCorrection.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brainReference) {
        NLM_LOG_ERROR("PredictionSystem: Brain reference not set");
        return currentState.clone();
    }
    
    const auto& currentData = currentState.getData();
    size_t stateSize = currentData.size();
    
    // Initialize prediction model if not already done
    if (pImpl->predictionWeights.empty()) {
        // Initialize weights for prediction model
        pImpl->predictionWeights.resize(stateSize, 0.5f);
        pImpl->predictionBiases.resize(10, 0.0f); // 10 hidden units
        pImpl->errorCorrection.resize(stateSize, 0.0f);
        
        // Initialize with random weights for diversity
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> weightDist(-0.5f, 0.5f);
        
        for (size_t i = 0; i < stateSize; ++i) {
            pImpl->predictionWeights[i] = weightDist(rng);
        }
    }
    
    // Create prediction using neural-like computation
    std::unique_ptr<SensoryInput> predictedInput = std::make_unique<SensoryInput>();
    
    // Simulate predictive coding through brain regions
    // Use motor and internal regions for prediction
    for (size_t i = 0; i < std::min(stateSize, size_t(100)); ++i) {
        float prediction = 0.0f;
        
        // Simple neural-like prediction computation
        // In a real system, this would use actual neural connections
        prediction = currentData[i] * pImpl->predictionWeights[i % pImpl->predictionWeights.size()];
        
        // Add bias from hidden units
        for (size_t b = 0; b < pImpl->predictionBiases.size(); ++b) {
            prediction += pImpl->predictionBiases[b] * std::sin(i * 0.1f + b * 0.5f);
        }
        
        // Apply prediction error correction
        if (i < pImpl->errorCorrection.size()) {
            prediction += pImpl->errorCorrection[i];
        }
        
        // Add noise for exploration (stochastic prediction)
        std::mt19937 rng(static_cast<unsigned int>(pImpl->lastPredictionStep + i));
        std::normal_distribution<float> noise(0.0f, 0.1f);
        prediction += noise(rng);
        
        // Clamp prediction to reasonable range
        prediction = std::max(-10.0f, std::min(10.0f, prediction));
        
        // Add to predicted input (simplified - using first dimension as output)
        if (predictedInput->getData().size() <= i) {
            std::vector<float> newData = predictedInput->getData();
            newData.push_back(prediction);
            predictedInput->setData(newData);
        }
    }
    
    // Update prediction history
    pImpl->lastPredictionStep++;
    
    NLM_LOG_INFO("PredictionSystem: Predicted next state with " + 
                 std::to_string(predictedInput->getData().size()) + " dimensions");
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.empty() || actualData.empty()) {
        return;
    }
    
    // Ensure we have matching sizes
    size_t minSize = std::min(predData.size(), actualData.size());
    if (minSize == 0) {
        return;
    }
    
    // Calculate prediction error using multiple metrics
    float mse = 0.0f;        // Mean squared error
    float mae = 0.0f;        // Mean absolute error
    float maxError = 0.0f;   // Maximum error
    float correlation = 0.0f; // Correlation coefficient
    
    float predSum = 0.0f, actualSum = 0.0f, predSqSum = 0.0f, actualSqSum = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        float error = predData[i] - actualData[i];
        mse += error * error;
        mae += std::abs(error);
        maxError = std::max(maxError, std::abs(error));
        
        predSum += predData[i];
        actualSum += actualData[i];
        predSqSum += predData[i] * predData[i];
        actualSqSum += actualData[i] * actualData[i];
        
        // Update error correction for future predictions
        if (i < pImpl->errorCorrection.size()) {
            // Adaptive error correction based on prediction error
            float adaptiveCorrection = error * 0.1f;  // Learning rate factor
            pImpl->errorCorrection[i] += adaptiveCorrection;
            
            // Limit error correction magnitude
            pImpl->errorCorrection[i] = std::max(-1.0f, std::min(1.0f, pImpl->errorCorrection[i]));
        }
    }
    
    // Normalize errors
    mse /= minSize;
    mae /= minSize;
    
    // Calculate correlation
    float predMean = predSum / minSize;
    float actualMean = actualSum / minSize;
    float predVariance = predSqSum / minSize - predMean * predMean;
    float actualVariance = actualSqSum / minSize - actualMean * actualMean;
    float covariance = 0.0f;
    
    if (predVariance > 0.0f && actualVariance > 0.0f) {
        for (size_t i = 0; i < minSize; ++i) {
            covariance += (predData[i] - predMean) * (actualData[i] - actualMean);
        }
        covariance /= minSize;
        correlation = covariance / std::sqrt(predVariance * actualVariance);
    }
    
    // Composite prediction error metric
    pImpl->predictionError = std::sqrt(mse) * 0.6f + 
                             (1.0f - std::abs(correlation)) * 0.3f + 
                             maxError * 0.1f;
    
    // Update confidence based on prediction accuracy
    // Higher error or lower correlation reduces confidence
    pImpl->confidence = std::max(0.0f, std::min(1.0f, 1.0f - pImpl->predictionError));
    if (std::abs(correlation) > 0.8f) {
        // Good prediction increases confidence
        pImpl->confidence = std::min(1.0f, pImpl->confidence * 1.2f);
    }
    
    // Store error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Limit history size
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_INFO("PredictionSystem: Error = " + std::to_string(pImpl->predictionError) + 
                 ", Confidence = " + std::to_string(pImpl->confidence));
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
    if (!pImpl->brainReference) {
        return;
    }
    
    const auto& data = observation.getData();
    if (data.empty()) {
        return;
    }
    
    // Real training using brain's neural mechanisms
    // This simulates how prediction error drives learning in the brain
    
    // Get prediction error
    float currentError = getPredictionError();
    
    // Use error to modulate brain plasticity
    if (pImpl->brainReference) {
        // If we have access to dopamine system, use prediction error to modulate it
        Dopamine* dopamine = pImpl->brainReference->getDopamine();
        if (dopamine) {
            // Prediction error modulates dopamine signaling
            // Negative error (unexpected outcome) increases dopamine
            // This drives learning and adaptation
            dopamine->signalRewardPredictionError(currentError);
        }
        
        // Update prediction model weights based on error
        size_t stateSize = std::min(data.size(), pImpl->predictionWeights.size());
        if (stateSize > 0) {
            float weightAdjustment = currentError * pImpl->learningRate;
            
            // Hebbian-like learning: adjust weights based on prediction error
            for (size_t i = 0; i < stateSize; ++i) {
                // Weight update based on error and current value
                float weightChange = weightAdjustment * data[i] * 0.1f;
                pImpl->predictionWeights[i] += weightChange;
                
                // Clip weights to reasonable range
                pImpl->predictionWeights[i] = std::max(-2.0f, std::min(2.0f, pImpl->predictionWeights[i]));
            }
            
            // Update prediction biases
            for (size_t i = 0; i < pImpl->predictionBiases.size(); ++i) {
                float biasChange = currentError * pImpl->learningRate * 0.01f;
                pImpl->predictionBiases[i] += biasChange;
                pImpl->predictionBiases[i] = std::max(-1.0f, std::min(1.0f, pImpl->predictionBiases[i]));
            }
        }
    }
    
    NLM_LOG_INFO("PredictionSystem: Trained prediction model with error " + 
                 std::to_string(currentError));
}

void PredictionSystem::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

} // namespace nlm