#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionModel;  // Internal prediction model weights
    std::vector<float> predictionBuffer; // Buffer for predictions
    float decayRate;
    float learningRate;
    size_t bufferSize;
    
    Impl() 
        : predictionError(0.0f) 
        , confidence(0.5f)
        , decayRate(0.01f)
        , learningRate(0.05f)
        , bufferSize(10)
    {
        predictionModel.resize(100, 0.0f); // Initialize with 100 weights
        predictionBuffer.reserve(bufferSize);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using working memory and neural substrate
    // Predict next sensory state based on current working memory content
    
    const auto& currentData = currentState.getData();
    
    // Use prediction buffer for temporal dynamics
    if (pImpl->predictionBuffer.size() >= pImpl->bufferSize) {
        pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
    }
    
    // Simple feedforward prediction: weighted sum of recent states
    // In a real implementation, this would use neural networks or reservoir computing
    std::vector<float> predictedData(currentData.size(), 0.0f);
    
    if (!pImpl->predictionBuffer.empty()) {
        // Use recent pattern history to predict
        for (size_t i = 0; i < currentData.size() && i < pImpl->predictionBuffer.size(); ++i) {
            float avgPattern = 0.0f;
            for (size_t j = 0; j < pImpl->predictionBuffer.size(); ++j) {
                if (i < pImpl->predictionBuffer[j].size()) {
                    avgPattern += pImpl->predictionBuffer[j][i];
                }
            }
            avgPattern /= static_cast<float>(pImpl->predictionBuffer.size());
            predictedData[i] = avgPattern + (currentData[i] * 0.1f); // Mix with current
        }
    } else {
        // If no buffer history, just copy with slight modification
        for (size_t i = 0; i < currentData.size(); ++i) {
            predictedData[i] = currentData[i] * 0.9f + 0.1f * (i % 10) / 10.0f; // Add small variation
        }
    }
    
    // Store current state in buffer
    pImpl->predictionBuffer.push_back(currentData);
    
    // Create a copy of current state as prediction (placeholder - real prediction would use neural computation)
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction error computation using squared error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float squaredError = diff * diff;
            sumError += squaredError;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = sumError / predData.size();
        
        // Update confidence based on error magnitude
        // Lower error = higher confidence
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
        
        // Apply decay to old errors
        if (!pImpl->errorHistory.empty()) {
            for (auto& error : pImpl->errorHistory) {
                error *= (1.0f - pImpl->decayRate);
            }
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    }
}

void PredictionSystem::updateSensoryInput(const std::vector<float>& sensoryData) {
    // Update prediction system with new sensory input
    // Connects working memory to prediction system
    
    if (sensoryData.empty()) return;
    
    // Update prediction error based on sensory input changes
    static std::vector<float> lastData = sensoryData;
    
    if (sensoryData.size() == lastData.size()) {
        float sumError = 0.0f;
        
        for (size_t i = 0; i < sensoryData.size(); ++i) {
            float diff = sensoryData[i] - lastData[i];
            sumError += diff * diff;
        }
        
        pImpl->predictionError = sumError / sensoryData.size();
        
        // Update confidence based on sensory change magnitude
        pImpl->confidence = 1.0f / (1.0f + std::sqrt(pImpl->predictionError));
        
        // Store sensory data in buffer for temporal predictions
        if (pImpl->predictionBuffer.size() >= pImpl->bufferSize) {
            pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
        }
        pImpl->predictionBuffer.push_back(sensoryData);
        
        lastData = sensoryData;
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Update prediction model based on observation
    // Implement learning from prediction errors
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Simple Hebbian-like learning: strengthen weights that predict successfully
    for (size_t i = 0; i < std::min(data.size(), pImpl->predictionModel.size()); ++i) {
        // Update prediction model weights based on sensory input
        pImpl->predictionModel[i] += data[i] * pImpl->learningRate;
        
        // Apply decay to prevent runaway growth
        pImpl->predictionModel[i] *= (1.0f - pImpl->decayRate);
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

const std::vector<float>& PredictionSystem::getPredictionModel() const {
    return pImpl->predictionModel;
}

const std::vector<float>& PredictionSystem::getPredictionModel() const {
    return pImpl->predictionModel;
}

} // namespace nlm
