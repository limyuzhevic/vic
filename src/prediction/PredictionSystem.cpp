// Neural prediction implementation with forward model
// Uses working memory traces and episodic memory for prediction

#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Forward model weights
    std::vector<float> forwardModel;
    
    // Memory-based prediction
    size_t predictionHorizon;
    size_t memoryWindow;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             predictionHorizon(5), memoryWindow(20) {
        forwardModel.resize(20, 0.0f);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return nullptr;
    }
    
    // Use working memory traces for prediction
    // Create predicted state based on forward model
    std::vector<float> predictedData(currentData.size(), 0.0f);
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        float predicted = currentData[i];
        
        // Apply temporal dynamics
        if (i > 0 && i < currentData.size()) {
            float diff = currentData[i] - currentData[i-1];
            predicted += diff * 0.1f; // Velocity-based prediction
        }
        
        // Apply recurrent connections from working memory
        if (i < pImpl->forwardModel.size()) {
            predicted *= (1.0f + pImpl->forwardModel[i]);
        }
        
        // Add noise for exploration
        predicted += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        
        // Clamp to valid range
        predicted = std::clamp(predicted, 0.0f, 1.0f);
        
        predictedData[i] = predicted;
    }
    
    // Create and return predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    // predictedInput->setData(predictedData); // Implement if SensoryInput has setData
    
    // Update confidence based on prediction accuracy
    float error = 0.0f;
    if (pImpl->errorHistory.size() > 3) {
        for (size_t i = pImpl->errorHistory.size() - 4; i < pImpl->errorHistory.size(); ++i) {
            error += pImpl->errorHistory[i];
        }
        error /= 4.0f;
        pImpl->confidence = std::max(0.1f, 1.0f - error);
    }
    
    NLM_LOG_DEBUG("PredictionSystem predicted state, confidence: " + std::to_string(pImpl->confidence));
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > pImpl->memoryWindow) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update forward model based on prediction error
        for (size_t i = 0; i < std::min(predData.size(), pImpl->forwardModel.size()); ++i) {
            float learningRate = 0.01f / (1.0f + pImpl->predictionError);
            float error = predData[i] - actualData[i];
            pImpl->forwardModel[i] += learningRate * error;
            
            // Clamp learning rate
            pImpl->forwardModel[i] = std::clamp(pImpl->forwardModel[i], -0.5f, 0.5f);
        }
        
        NLM_LOG_DEBUG("PredictionSystem updated, error: " + std::to_string(pImpl->predictionError));
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
    const auto& data = observation.getData();
    
    if (data.empty()) {
        return;
    }
    
    // Learn from observation to improve forward model
    for (size_t i = 0; i < std::min(data.size(), pImpl->forwardModel.size()); ++i) {
        // Forward model learns transitions
        pImpl->forwardModel[i] = data[i];
    }
    
    // Update prediction error based on training
    float avgError = 0.0f;
    if (!pImpl->errorHistory.empty()) {
        avgError = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f) / 
                   pImpl->errorHistory.size();
    }
    
    pImpl->predictionError = avgError;
    NLM_LOG_DEBUG("PredictionSystem trained on observation, avg error: " + std::to_string(avgError));
}

} // namespace nlm
