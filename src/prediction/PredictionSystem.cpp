#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionModels;  // Model weights for different patterns
    float learningRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f) {
        predictionModels.resize(10, std::vector<float>(10, 0.0f));  // Simple pattern model
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 6: Implement real prediction using NLM's neural substrate
    // For now, implement a basic but functional prediction system
    
    const auto& currentData = currentState.getData();
    std::unique_ptr<SensoryInput> predictedState = std::make_unique<SensoryInput>();
    
    if (currentData.empty()) {
        predictedState->setData(currentData);
        return predictedState;
    }
    
    // Create prediction using simple neural-inspired dynamics
    std::vector<float> predictedData = currentData;
    
    // Apply neural-like prediction dynamics
    // Predict change based on patterns in the data
    size_t dataSize = currentData.size();
    
    // Predict next state using a simple recurrent connection model
    // This mimics how neural predictions work in the brain
    for (size_t i = 0; i < dataSize; ++i) {
        // Get context from surrounding elements
        float context = 0.0f;
        float totalWeight = 0.0f;
        
        // Look at neighboring elements for pattern completion
        for (int j = -3; j <= 3; ++j) {
            if (j == 0) continue;
            int idx = i + j;
            if (idx >= 0 && idx < static_cast<int>(dataSize)) {
                float weight = 1.0f / std::abs(j);
                context += currentData[idx] * weight;
                totalWeight += weight;
            }
        }
        
        if (totalWeight > 0.0f) {
            context /= totalWeight;
            
            // Predict change based on context
            float predictionStrength = pImpl->confidence * 0.1f;
            predictedData[i] += (context - currentData[i]) * predictionStrength;
            
            // Apply biological constraints (values stay within reasonable range)
            predictedData[i] = std::clamp(predictedData[i], -10.0f, 10.0f);
        }
    }
    
    // Add noise to make prediction more realistic
    for (float& value : predictedData) {
        value += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    }
    
    // Update prediction model
    for (size_t i = 0; i < std::min(dataSize, pImpl->predictionModels.size()); ++i) {
        for (size_t j = 0; j < dataSize; ++j) {
            if (i < pImpl->predictionModels.size() && j < pImpl->predictionModels[i].size()) {
                pImpl->predictionModels[i][j] = (pImpl->predictionModels[i][j] * 0.9f) + 
                                              (currentData[j] * 0.1f);
            }
        }
    }
    
    predictedState->setData(predictedData);
    
    NLM_LOG_DEBUG("Prediction system: predicted state from {} elements", currentData.size());
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // PHASE 6: Implement real prediction error computation
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error using biologically plausible method
        float sumSquaredError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
            maxError = std::max(maxError, std::abs(error));
        }
        
        // Store error metrics
        pImpl->predictionError = sumSquaredError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error
        // High error -> low confidence, low error -> high confidence
        float normalizedError = std::min(1.0f, maxError / 10.0f);
        pImpl->confidence = 1.0f - normalizedError;
        
        // Ensure confidence stays in valid range
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
        
        // Learn from the error
        train(predicted);
        
        NLM_LOG_DEBUG("Prediction error: %.6f, confidence: %.3f", 
                     pImpl->predictionError, pImpl->confidence);
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // PHASE 6: Implement real prediction model training
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update prediction models using Hebbian-like learning
    // Strong correlations strengthen the connections
    
    size_t dataSize = data.size();
    size_t numModels = pImpl->predictionModels.size();
    
    for (size_t i = 0; i < numModels && i < dataSize; ++i) {
        for (size_t j = 0; j < dataSize && j < pImpl->predictionModels[i].size(); ++j) {
            // Update model weights based on observation
            // This mimics how neurons strengthen connections during learning
            float oldWeight = pImpl->predictionModels[i][j];
            
            // Hebbian learning: fire together, wire together
            // Stronger observation -> stronger connection
            float newWeight = oldWeight + (data[j] * pImpl->learningRate * 0.1f);
            
            // Constrain weight to reasonable range
            newWeight = std::clamp(newWeight, -1.0f, 1.0f);
            
            pImpl->predictionModels[i][j] = newWeight;
        }
    }
    
    // Occasionally update learning rate based on prediction error
    if (!pImpl->errorHistory.empty()) {
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Adapt learning rate based on prediction accuracy
        pImpl->learningRate = std::clamp(pImpl->learningRate + avgError * 0.001f - 0.0005f, 0.001f, 0.1f);
    }
}

} // namespace nlm
