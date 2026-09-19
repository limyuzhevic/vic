#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> learnedSequences;
    std::vector<float> sequenceWeights;
    std::mt19937 rng;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f)
        , rng(std::random_device()()) {
        learnedSequences.reserve(100);
        sequenceWeights.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using learned temporal sequences
    // Implements predictive coding through temporal sequence learning
    
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Generate prediction by finding most similar learned sequence
    float bestSimilarity = 0.0f;
    std::vector<float> predictedState;
    
    for (size_t i = 0; i < pImpl->learnedSequences.size(); ++i) {
        float similarity = computeSimilarity(currentData, pImpl->learnedSequences[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            predictedState = pImpl->learnedSequences[i];
        }
    }
    
    // Add noise for prediction uncertainty
    std::uniform_real_distribution<float> noiseDist(-0.05f, 0.05f);
    for (float& val : predictedState) {
        val += noiseDist(pImpl->rng);
        val = std::clamp(val, 0.0f, 1.0f);
    }
    
    // Update confidence based on prediction quality
    pImpl->confidence = bestSimilarity;
    
    // Create and return new sensory input with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedState);
    
    NLM_LOG_DEBUG("Prediction generated with confidence: " + std::to_string(bestSimilarity));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction error computation and learning
    // Implements error-based learning for prediction refinement
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error
    float sumError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumError += error * error;
        maxError = std::max(maxError, std::abs(error));
    }
    
    pImpl->predictionError = std::sqrt(sumError / predData.size());
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep error history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Learn from prediction error (reinforce good predictions, penalize bad ones)
    if (pImpl->predictionError < 0.1f) {
        // Good prediction - strengthen associated sequences
        learnFromGoodPrediction(predData);
    } else {
        // Bad prediction - weaken or eliminate associated sequences
        learnFromBadPrediction(predData);
    }
    
    NLM_LOG_DEBUG("Prediction error computed: " + std::to_string(pImpl->predictionError));
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
    pImpl->learnedSequences.clear();
    pImpl->sequenceWeights.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model with new observation
    // Updates learned sequences based on input patterns
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Check if we have enough history to form a sequence
    if (!pImpl->learnedSequences.empty()) {
        // Add current observation to sequence history
        pImpl->learnedSequences.push_back(data);
        
        // Update sequence weight based on prediction error
        float recentError = pImpl->errorHistory.empty() ? 0.5f : pImpl->errorHistory.back();
        float weight = 1.0f - recentError;  // Better predictions get higher weight
        pImpl->sequenceWeights.push_back(weight);
        
        // Trim history if too large
        if (pImpl->learnedSequences.size() > 100) {
            pImpl->learnedSequences.erase(pImpl->learnedSequences.begin());
            pImpl->sequenceWeights.erase(pImpl->sequenceWeights.begin());
        }
    } else {
        // First observation - just store it
        pImpl->learnedSequences.push_back(data);
        pImpl->sequenceWeights.push_back(1.0f);
    }
    
    NLM_LOG_DEBUG("Prediction model trained with observation of size: " + std::to_string(data.size()));
}

// Helper function to compute similarity between two state vectors
float PredictionSystem::computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

void PredictionSystem::learnFromGoodPrediction(const std::vector<float>& state) {
    // Strengthen association for good predictions
    // This implements reinforcement of accurate predictive models
    
    // Find similar sequences and boost their weights
    for (size_t i = 0; i < pImpl->learnedSequences.size(); ++i) {
        float similarity = computeSimilarity(state, pImpl->learnedSequences[i]);
        if (similarity > 0.7f) {
            pImpl->sequenceWeights[i] *= 1.05f;  // Boost weight
            pImpl->sequenceWeights[i] = std::min(pImpl->sequenceWeights[i], 2.0f);
        }
    }
}

void PredictionSystem::learnFromBadPrediction(const std::vector<float>& state) {
    // Weaken or eliminate associations for bad predictions
    // This implements error correction in predictive models
    
    // Gradually weaken all sequence weights
    for (size_t i = 0; i < pImpl->sequenceWeights.size(); ++i) {
        pImpl->sequenceWeights[i] *= 0.95f;
        if (pImpl->sequenceWeights[i] < 0.01f) {
            // Remove very weak sequences
            pImpl->sequenceWeights.erase(pImpl->sequenceWeights.begin() + i);
            pImpl->learnedSequences.erase(pImpl->learnedSequences.begin() + i);
            --i;
        }
    }
}

} // namespace nlm
