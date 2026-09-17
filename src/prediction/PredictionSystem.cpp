#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> predictionBuffer;
    size_t predictionWindow;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionWindow(10) {
        predictionBuffer.reserve(predictionWindow);
        errorHistory.reserve(predictionWindow);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // PHASE 2: Implement real prediction using NLM's neural substrate
    // Simple predictive implementation based on current sensory patterns
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>(currentState);
    }
    
    // Create prediction based on temporal patterns
    auto prediction = std::make_unique<SensoryInput>(currentState);
    auto& predData = prediction->getData();
    
    // Smooth prediction with temporal continuity
    for (size_t i = 0; i < currentData.size(); ++i) {
        float predictedValue = currentData[i];
        
        // Apply simple temporal smoothing
        if (pImpl->predictionBuffer.size() > 1) {
            const auto& prevState = pImpl->predictionBuffer.back()->getData();
            if (i < prevState.size()) {
                // Temporal smoothing factor
                float alpha = 0.7f;
                predictedValue = alpha * currentData[i] + (1.0f - alpha) * prevState[i];
            }
        }
        
        // Add small noise for exploration
        float noise = pImpl->confidence > 0.5f ? 0.1f * (1.0f - pImpl->confidence) : 0.0f;
        if (noise > 0.0f) {
            float r = rand() / (RAND_MAX + 1.0f);
            predictedValue += (r * 2.0f - 1.0f) * noise;
            predictedValue = std::clamp(predictedValue, -1.0f, 1.0f);
        }
        
        predData[i] = predictedValue;
    }
    
    // Store in prediction buffer
    pImpl->predictionBuffer.push_back(std::make_unique<SensoryInput>(*prediction));
    if (pImpl->predictionBuffer.size() > pImpl->predictionWindow) {
        pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation with neural validation
    // Current placeholder: Calculate simple error with enhanced metrics
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error with different metrics
        float sumSquaredError = 0.0f;
        float sumAbsoluteError = 0.0f;
        float maxAbsoluteError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float absDiff = std::abs(diff);
            
            sumSquaredError += diff * diff;
            sumAbsoluteError += absDiff;
            maxAbsoluteError = std::max(maxAbsoluteError, absDiff);
        }
        
        // Combine errors into a single metric
        pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
        
        // Update confidence based on error magnitude
        float errorMagnitude = sumAbsoluteError / predData.size();
        pImpl->confidence = std::max(0.0f, 1.0f - errorMagnitude * 2.0f);
        
        // Store error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > pImpl->predictionWindow) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Log prediction performance
        NLM_LOG_DEBUG("Prediction error: " + std::to_string(pImpl->predictionError) + 
                     ", confidence: " + std::to_string(pImpl->confidence));
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
    pImpl->predictionBuffer.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model with neural substrate
    // Current placeholder: Store observation for future training
    
    // Simple training: update prediction model with observed patterns
    NLM_LOG_DEBUG("Training prediction model with observation of size: " + 
                 std::to_string(observation.getData().size()));
    
    // In a real implementation, this would:
    // 1. Extract temporal features from observation
    // 2. Update internal prediction weights
    // 3. Learn temporal patterns
    // 4. Adjust confidence based on prediction stability
    
    // For now, just acknowledge training request
    NLM_LOG_INFO("Prediction model training initiated (placeholder)");
}

} // namespace nlm
