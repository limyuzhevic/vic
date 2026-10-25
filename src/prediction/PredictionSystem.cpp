#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionPatterns;
    std::vector<std::vector<float>> actualPatterns;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Simple prediction based on current state with some transformation
    auto predicted = currentState.clone();
    const auto& data = currentState.getData();
    
    if (!data.empty()) {
        std::vector<float> predictedData = data;
        
        // Apply simple transformation to create prediction
        for (size_t i = 0; i < predictedData.size(); ++i) {
            // Add some dynamics: decay, add noise, apply nonlinearity
            predictedData[i] *= 0.8f;  // Decay
            predictedData[i] += 0.1f * static_cast<float>(rand()) / RAND_MAX - 0.05f;  // Noise
            predictedData[i] = std::tanh(predictedData[i]);  // Nonlinearity
        }
        
        predicted->setData(predictedData);
        NLM_LOG_DEBUG("Generated prediction from current sensory state");
    }
    
    return predicted;
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
        
        float newPredictionError = sumError / predData.size();
        pImpl->predictionError = newPredictionError;
        pImpl->errorHistory.push_back(newPredictionError);
        
        // Keep error history manageable
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - newPredictionError);
        
        // Store patterns for learning
        pImpl->predictionPatterns.push_back(predData);
        pImpl->actualPatterns.push_back(actualData);
        
        if (pImpl->predictionPatterns.size() > 1000) {
            pImpl->predictionPatterns.erase(pImpl->predictionPatterns.begin());
            pImpl->actualPatterns.erase(pImpl->actualPatterns.begin());
        }
        
        NLM_LOG_DEBUG(std::string("Prediction error: ") + std::to_string(newPredictionError) +
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
    pImpl->predictionPatterns.clear();
    pImpl->actualPatterns.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    NLM_LOG_INFO("PredictionSystem history cleared");
}

float PredictionSystem::getAverageError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f);
    return sum / pImpl->errorHistory.size();
}

float PredictionSystem::getPredictionAccuracy() const {
    if (pImpl->predictionPatterns.empty()) return 0.0f;
    
    float totalSimilarity = 0.0f;
    for (size_t i = 0; i < pImpl->predictionPatterns.size(); ++i) {
        const auto& pred = pImpl->predictionPatterns[i];
        const auto& actual = pImpl->actualPatterns[i];
        
        if (pred.size() == actual.size() && !pred.empty()) {
            float similarity = computeCosineSimilarity(pred, actual);
            totalSimilarity += similarity;
        }
    }
    
    return totalSimilarity / pImpl->predictionPatterns.size();
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    if (data.empty()) {
        return;
    }
    
    // Store observation for learning
    pImpl->actualPatterns.push_back(data);
    
    // Generate prediction for this observation
    auto predicted = predictNextState(observation);
    
    // Update the system with this prediction-actual pair
    updatePredictions(predicted, observation);
    
    NLM_LOG_DEBUG(std::string("Trained prediction system with observation: ") + 
        std::to_string(data.size()) + " features");
}

float PredictionSystem::computeCosineSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm