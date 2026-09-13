#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionModels;
    std::vector<float> predictionConfidence;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        predictionModels.resize(10);  // 10 different prediction models
        predictionConfidence.resize(10, 0.0f);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using neural substrate
    const auto& data = currentState.getData();
    
    if (data.empty()) {
        return std::make_unique<SensoryInput>(std::vector<float>(10, 0.0f));
    }
    
    // Create a realistic prediction by modifying input with learned patterns
    std::vector<float> prediction(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        // Apply different prediction effects based on model confidence
        float modelContribution = 0.0f;
        float totalConfidence = 0.0f;
        
        for (size_t model = 0; model < pImpl->predictionModels.size(); ++model) {
            if (i < pImpl->predictionModels[model].size() && pImpl->predictionConfidence[model] > 0.1f) {
                // Blend predictions from all models
                float modelPred = pImpl->predictionModels[model][i];
                modelContribution += modelPred * pImpl->predictionConfidence[model];
                totalConfidence += pImpl->predictionConfidence[model];
            }
        }
        
        if (totalConfidence > 0.0f) {
            // Use simple controlled noise for prediction
            float noise = 0.0f;
            if (i < pImpl->predictionModels.size() && i < pImpl->predictionConfidence.size()) {
                if (pImpl->predictionConfidence[i] > 0.5f) {
                    noise = (i % 3 - 1) * 0.05f;  // Simple pattern-based noise
                }
            }
            
            prediction[i] = (data[i] * 0.5f + modelContribution / totalConfidence * 0.5f) * (1.0f + noise);
        } else {
            prediction[i] = data[i];
        }
        
        // Clamp to reasonable range
        prediction[i] = std::clamp(prediction[i], 0.0f, 1.0f);
    }
    
    return std::make_unique<SensoryInput>(prediction);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = std::sqrt(sumError / predData.size());
        pImpl->predictionError = std::min(pImpl->predictionError, 1.0f);
        
        // Update confidence based on prediction accuracy
        if (pImpl->predictionError < 0.1f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
        } else if (pImpl->predictionError > 0.5f) {
            pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.1f);
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // If error is high, update prediction models to improve future predictions
        if (pImpl->predictionError > 0.3f) {
            // Update prediction models with actual data (learning from error)
            for (size_t i = 0; i < predData.size() && i < pImpl->predictionModels.size(); ++i) {
                pImpl->predictionModels[i][i % pImpl->predictionModels[i].size()] = 
                    actualData[i] * 0.3f + pImpl->predictionModels[i][i % pImpl->predictionModels[i].size()] * 0.7f;
            }
        }
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model with new observation
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Update each prediction model with weighted contributions
    for (size_t i = 0; i < pImpl->predictionModels.size(); ++i) {
        float weight = 0.5f / (i + 1);  // Different weights for different models
        
        if (i >= pImpl->predictionModels.size()) {
            pImpl->predictionModels[i].resize(data.size());
        }
        
        for (size_t j = 0; j < data.size() && j < pImpl->predictionModels[i].size(); ++j) {
            pImpl->predictionModels[i][j] = data[j] * weight + pImpl->predictionModels[i][j] * (1.0f - weight);
        }
        
        // Update model confidence based on prediction accuracy
        pImpl->predictionConfidence[i] = std::min(1.0f, pImpl->predictionConfidence[i] + 0.01f);
    }
}

} // namespace nlm
