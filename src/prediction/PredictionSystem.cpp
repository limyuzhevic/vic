#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<float> predictionWeights;
    std::vector<float> predictionBias;
    float learningRate;
    float errorDecayRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), errorDecayRate(0.05f) {
        // Initialize with small random weights
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.1f, 0.1f);
        
        for (int i = 0; i < 10; ++i) {
            predictionWeights.push_back(dis(gen));
            predictionBias.push_back(dis(gen) * 0.1f);
        }
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, implement a simple forward model that predicts next state
    // based on current state and learned weights
    
    const auto& currentData = currentState.getData();
    std::unique_ptr<SensoryInput> predictedInput = std::make_unique<SensoryInput>();
    
    // Get dimensions
    size_t stateDim = currentData.size();
    if (stateDim == 0) {
        return predictedInput;
    }
    
    // Extend state if needed
    std::vector<float> predictedData = currentData;
    predictedData.resize(std::max(stateDim, pImpl->predictionWeights.size()), 0.0f);
    
    // Apply prediction weights (forward model)
    for (size_t i = 0; i < predictedData.size() && i < pImpl->predictionWeights.size(); ++i) {
        // Simple prediction: add weighted effect based on current state
        float weight = pImpl->predictionWeights[i];
        float bias = pImpl->predictionBias[i];
        
        // Compute prediction: weighted sum of current state plus bias
        float predictedValue = bias;
        for (size_t j = 0; j < std::min(stateDim, static_cast<size_t>(10)); ++j) {
            // Use first 10 dimensions of state for prediction
            predictedValue += currentData[j] * weight * 0.1f;
        }
        
        // Apply nonlinear activation
        predictedValue = 1.0f / (1.0f + std::exp(-predictedValue)); // Sigmoid
        
        // Update prediction
        predictedData[i] = predictedValue;
    }
    
    // Add action effect if currentState represents action context
    // In a real implementation, this would use action-consequence learning
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Simple dynamics: small decay plus noise
        predictedData[i] *= 0.95f; // Natural decay
        
        // Add some stochasticity
        static std::mt19937 rng(std::random_device{}());
        std::normal_distribution<float> noise(0.0f, 0.01f);
        predictedData[i] += noise(rng);
        
        // Clamp to valid range
        predictedData[i] = std::max(0.0f, std::min(1.0f, predictedData[i]));
    }
    
    predictedInput->setData(predictedData);
    predictedInput->setTimestamp(currentState.getTimestamp());
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted next state (error=" + 
                 std::to_string(pImpl->predictionError) + ")");
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // Update prediction model based on prediction error
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update prediction weights based on error (simple Hebbian learning)
        for (size_t i = 0; i < std::min(predData.size(), pImpl->predictionWeights.size()); ++i) {
            // Weight update proportional to error
            float gradient = -2.0f * (predData[i] - actualData[i]);
            pImpl->predictionWeights[i] += pImpl->learningRate * gradient * predData[i];
            
            // Limit weight magnitude
            pImpl->predictionWeights[i] = std::max(-1.0f, std::min(1.0f, pImpl->predictionWeights[i]));
        }
        
        // Decay confidence based on error
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        NLM_LOG_DEBUG("PredictionSystem: Updated predictions, error=" + 
                     std::to_string(pImpl->predictionError) + ", confidence=" + 
                     std::to_string(pImpl->confidence));
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
    // TODO PHASE 2: Train prediction model
    // For now, update prediction bias with observation
    const auto& data = observation.getData();
    if (!data.empty()) {
        for (size_t i = 0; i < std::min(data.size(), pImpl->predictionBias.size()); ++i) {
            // Update bias towards observation
            pImpl->predictionBias[i] += pImpl->learningRate * (data[i] - pImpl->predictionBias[i]);
        }
    }
}

} // namespace nlm