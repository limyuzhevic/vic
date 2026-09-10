#include "PredictionSystem.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionModel;  // Simplified prediction model
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        // Initialize prediction model with some random weights
        predictionModel.resize(10, 0.1f);  // Simple 10-element model
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(
    const SensoryInput& currentState,
    const std::vector<float>& workingMemoryPattern,
    const std::vector<const EpisodicMemoryItem*>& relevantEpisodes) {
    
    // Use working memory pattern as the primary basis for prediction
    // Blend with patterns from relevant episodes
    const auto& currentData = currentState.getData();
    
    // Create predicted state based on current state and context
    std::vector<float> predictedData = currentData;
    
    // Blend with episode patterns if available
    float episodeBlendFactor = 0.2f;
    for (const auto* episode : relevantEpisodes) {
        if (episode->sensoryState.size() == predictedData.size()) {
            for (size_t i = 0; i < predictedData.size(); ++i) {
                predictedData[i] += episode->sensoryState[i] * episodeBlendFactor;
            }
        }
    }
    
    // Apply prediction model (simple linear combination)
    // In a real implementation, this would use a neural network or similar
    if (!workingMemoryPattern.empty() && workingMemoryPattern.size() == predictedData.size()) {
        for (size_t i = 0; i < predictedData.size(); ++i) {
            predictedData[i] = predictedData[i] * pImpl->predictionModel[i % pImpl->predictionModel.size()];
        }
    }
    
    // Create and return predicted SensoryInput
    return std::make_unique<SensoryInput>(predictedData);
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
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error (lower error = higher confidence)
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Update prediction model weights based on error (simple learning)
        for (size_t i = 0; i < predData.size() && i < pImpl->predictionModel.size(); ++i) {
            float learningRate = 0.01f * pImpl->confidence;
            float weightAdjustment = learningRate * (actualData[i] - predData[i]);
            pImpl->predictionModel[i] += weightAdjustment;
            pImpl->predictionModel[i] = std::clamp(pImpl->predictionModel[i], 0.0f, 1.0f);
        }
    }
}

void PredictionSystem::updatePredictionError(float error, TimestepDuration dt) {
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Update confidence based on current error
    pImpl->confidence = std::max(0.0f, 1.0f - error);
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

void PredictionSystem::trainFromEpisode(const EpisodicMemoryItem& episode) {
    // Use episode pattern to improve prediction model
    if (!episode.sensoryState.empty()) {
        // Blend episode pattern into prediction model
        float learningRate = 0.05f;
        
        for (size_t i = 0; i < episode.sensoryState.size() && i < pImpl->predictionModel.size(); ++i) {
            pImpl->predictionModel[i] = pImpl->predictionModel[i] * (1.0f - learningRate) + 
                                      episode.sensoryState[i] * learningRate;
        }
        
        // Store episode in prediction context for future predictions
        // (In a real implementation, this would be more sophisticated)
    }
}

} // namespace nlm
