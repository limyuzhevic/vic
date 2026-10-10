#include "PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <memory>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> predictionHistory;
    std::unique_ptr<SensoryInput> lastPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Enhanced prediction: Create a predicted state based on current patterns
    // For now, implement a simple but more sophisticated prediction than just copying
    auto predicted = std::make_unique<SensoryInput>();
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Simple temporal smoothing: predict based on recent history
    if (!pImpl->predictionHistory.empty()) {
        // Average recent predictions to create smoother forecast
        size_t recentCount = std::min(size_t(5), pImpl->predictionHistory.size());
        std::vector<float> smoothedData(currentData.size(), 0.0f);
        
        for (size_t i = 0; i < recentCount; ++i) {
            auto& hist = pImpl->predictionHistory[pImpl->predictionHistory.size() - 1 - i];
            const auto& histData = hist->getData();
            if (histData.size() == currentData.size()) {
                for (size_t j = 0; j < currentData.size(); ++j) {
                    smoothedData[j] += histData[j] * (0.4f / recentCount);
                }
            }
        }
        
        // Add current state contribution
        float currentWeight = 0.6f;
        for (size_t j = 0; j < currentData.size(); ++j) {
            smoothedData[j] += currentData[j] * currentWeight;
        }
        
        // Apply simple temporal dynamics
        std::vector<float> finalData(currentData.size());
        for (size_t j = 0; j < currentData.size(); ++j) {
            finalData[j] = smoothedData[j] * 0.9f + currentData[j] * 0.1f;  // Dampen prediction
        }
        
        predicted->setData(finalData);
    } else {
        // First prediction: extrapolate from current
        std::vector<float> finalData = currentData;
        for (float& val : finalData) {
            val = val * 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f - 0.2f;
        }
        predicted->setData(finalData);
    }
    
    pImpl->lastPrediction = std::make_unique<SensoryInput>(*predicted);
    pImpl->predictionHistory.push_back(std::make_unique<SensoryInput>(*predicted));
    
    // Keep history manageable
    if (pImpl->predictionHistory.size() > 100) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Enhanced prediction error calculation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate multiple types of error metrics
        float squaredError = 0.0f;
        float absoluteError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            squaredError += diff * diff;
            absoluteError += std::abs(diff);
            maxError = std::max(maxError, std::abs(diff));
        }
        
        size_t dataSize = predData.size();
        pImpl->predictionError = std::sqrt(squaredError / dataSize);  // RMSE
        float avgAbsoluteError = absoluteError / dataSize;
        
        // Update confidence based on prediction quality
        // Lower error = higher confidence, but with saturation
        float rawConfidence = 1.0f / (1.0f + pImpl->predictionError * 2.0f);
        pImpl->confidence = rawConfidence * 0.8f + 0.1f;  // Don't let confidence be too high
        
        // Add to error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history manageable
        if (pImpl->errorHistory.size() > 50) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Learning signal: prediction error affects neuromodulation
        // This will be used by the brain to update its predictions
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
    pImpl->predictionHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Enhanced training: Store observation for future prediction
    pImpl->predictionHistory.push_back(std::make_unique<SensoryInput>(observation));
    
    // Keep history manageable
    if (pImpl->predictionHistory.size() > 200) {
        // Remove oldest prediction, keep most recent
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Simple learning: Update confidence based on observation diversity
    const auto& data = observation.getData();
    if (!data.empty()) {
        float variance = 0.0f;
        float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= data.size();
        
        // Higher variance means more learning opportunities
        float learningRate = std::min(1.0f, variance * 10.0f);
        pImpl->confidence += learningRate * 0.05f;
        if (pImpl->confidence > 1.0f) pImpl->confidence = 1.0f;
    }
}

} // namespace nlm
