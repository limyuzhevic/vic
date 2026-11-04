#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    std::vector<float> prediction;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> temporalPattern;
    std::vector<float> predictionWeights;
    std::vector<float> featureExtractor;
    
    // Neural prediction components
    std::vector<float> hiddenLayer1;
    std::vector<float> hiddenLayer2;
    std::vector<float> outputLayer;
    
    // Learning parameters
    float learningRate;
    float temporalDecay;
    float attentionWeight;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), 
             temporalDecay(0.95f), attentionWeight(0.7f) {
        prediction.reserve(100);
        temporalPattern.reserve(50);
        hiddenLayer1.reserve(20);
        hiddenLayer2.reserve(20);
        outputLayer.reserve(100);
        predictionWeights.resize(50, 0.1f);  // Initialize weights for temporal patterns
        featureExtractor.resize(4, 0.0f);   // Basic features: mean, variance, max, min
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// Enhanced prediction using neural networks
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    
    // Extract temporal features from current state
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>(currentData);
    }
    
    // Extract features: mean, variance, max, min
    float sum = 0.0f, sumSq = 0.0f, max = currentData[0], min = currentData[0];
    for (float val : currentData) {
        sum += val;
        sumSq += val * val;
        if (val > max) max = val;
        if (val < min) min = val;
    }
    float mean = sum / currentData.size();
    float variance = sumSq / currentData.size() - mean * mean;
    
    // Update feature extractor with temporal decay
    for (size_t i = 0; i < std::min(pImpl->featureExtractor.size(), currentData.size()); ++i) {
        pImpl->featureExtractor[i] = pImpl->featureExtractor[i] * pImpl->temporalDecay + 
                                   (i < 4 ? (i == 0 ? mean : i == 1 ? variance : i == 2 ? max : min) : 0.0f) * (1.0f - pImpl->temporalDecay);
    }
    
    // Neural network prediction
    pImpl->prediction.clear();
    pImpl->hiddenLayer1.clear();
    pImpl->hiddenLayer2.clear();
    pImpl->outputLayer.clear();
    
    // Input layer: temporal pattern + features
    size_t inputSize = std::min(currentData.size(), pImpl->temporalPattern.size());
    
    // Update temporal pattern with decay
    for (size_t i = 0; i < pImpl->temporalPattern.size(); ++i) {
        pImpl->temporalPattern[i] *= pImpl->temporalDecay;
    }
    
    // Add current state to temporal pattern
    for (size_t i = 0; i < inputSize; ++i) {
        pImpl->temporalPattern[i] += currentData[i] * (1.0f - pImpl->temporalPattern[i]);
    }
    
    // Feed forward through neural layers
    // Layer 1: temporal + feature input to hidden
    size_t hidden1Size = std::min(pImpl->temporalPattern.size(), pImpl->hiddenLayer1.size());
    for (size_t i = 0; i < hidden1Size; ++i) {
        float weightedSum = 0.0f;
        for (size_t j = 0; j < pImpl->temporalPattern.size(); ++j) {
            size_t idx = j % pImpl->predictionWeights.size();
            weightedSum += pImpl->temporalPattern[j] * pImpl->predictionWeights[idx % 25];  // Distribute weights
        }
        // Add feature input
        if (i < pImpl->featureExtractor.size()) {
            weightedSum += pImpl->featureExtractor[i] * 2.0f;
        }
        pImpl->hiddenLayer1[i] = 1.0f / (1.0f + std::exp(-weightedSum * 10.0f));  // Sigmoid
    }
    
    // Layer 2: hidden1 to hidden2
    size_t hidden2Size = std::min(pImpl->hiddenLayer1.size(), pImpl->hiddenLayer2.size());
    for (size_t i = 0; i < hidden2Size; ++i) {
        float weightedSum = 0.0f;
        for (size_t j = 0; j < pImpl->hiddenLayer1.size(); ++j) {
            weightedSum += pImpl->hiddenLayer1[j] * 0.5f;
        }
        pImpl->hiddenLayer2[i] = 1.0f / (1.0f + std::exp(-weightedSum * 10.0f));  // Sigmoid
    }
    
    // Output layer: hidden2 to prediction
    pImpl->outputLayer.resize(currentData.size(), 0.0f);
    for (size_t i = 0; i < currentData.size(); ++i) {
        float weightedSum = 0.0f;
        for (size_t j = 0; j < pImpl->hiddenLayer2.size(); ++j) {
            weightedSum += pImpl->hiddenLayer2[j] * 0.3f;
        }
        // Add temporal component
        if (i < pImpl->temporalPattern.size()) {
            weightedSum += pImpl->temporalPattern[i] * 1.5f;
        }
        // Add feature component
        if (i < 4) {
            weightedSum += pImpl->featureExtractor[i] * 0.8f;
        }
        pImpl->outputLayer[i] = 1.0f / (1.0f + std::exp(-weightedSum * 10.0f));  // Sigmoid
        
        // Scale to similar range as input
        pImpl->prediction.push_back(pImpl->outputLayer[i] * 255.0f);
    }
    
    // Calculate confidence based on prediction variance
    float predVariance = 0.0f;
    float predMean = 0.0f;
    for (float val : pImpl->prediction) {
        predMean += val;
    }
    if (!pImpl->prediction.empty()) {
        predMean /= pImpl->prediction.size();
        for (float val : pImpl->prediction) {
            predVariance += (val - predMean) * (val - predMean);
        }
        predVariance /= pImpl->prediction.size();
        
        // Lower variance = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - predVariance / 50000.0f);
    }
    
    return std::make_unique<SensoryInput>(pImpl->prediction);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error with temporal weighting
    float weightedError = 0.0f;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        
        // Higher weight for more recent inputs
        float weight = std::pow(0.9f, static_cast<float>(i));  // Exponential decay
        weightedError += std::abs(error) * weight;
        totalWeight += weight;
    }
    
    if (totalWeight > 0.0f) {
        pImpl->predictionError = weightedError / totalWeight;
    }
    
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Limit error history size
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction weights based on error (learning)
    if (pImpl->predictionError > 0.1f) {  // Only learn from significant errors
        for (size_t i = 0; i < std::min(pImpl->predictionWeights.size(), predData.size()); ++i) {
            // Hebbian-like learning: strengthen weights for patterns that lead to error
            float learningSignal = pImpl->predictionError * (predData[i] > actualData[i] ? 1.0f : -1.0f);
            pImpl->predictionWeights[i] += learningSignal * pImpl->learningRate * 10.0f;
            
            // Constrain weights
            pImpl->predictionWeights[i] = std::max(0.0f, std::min(1.0f, pImpl->predictionWeights[i]));
        }
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) return;
    
    // Extract features from observation
    float sum = 0.0f, sumSq = 0.0f, max = obsData[0], min = obsData[0];
    for (float val : obsData) {
        sum += val;
        sumSq += val * val;
        if (val > max) max = val;
        if (val < min) min = val;
    }
    
    // Update temporal pattern with new observation
    for (size_t i = 0; i < std::min(obsData.size(), pImpl->temporalPattern.size()); ++i) {
        // Blend new observation with temporal memory
        pImpl->temporalPattern[i] = pImpl->temporalPattern[i] * 0.8f + obsData[i] * 0.2f;
    }
    
    // Update feature extractor with decay
    for (size_t i = 0; i < std::min(pImpl->featureExtractor.size(), obsData.size()); ++i) {
        float feature = (i == 0) ? (sum / obsData.size()) : 
                       (i == 1) ? (sumSq / obsData.size() - (sum / obsData.size()) * (sum / obsData.size())) :
                       (i == 2) ? max : min;
        pImpl->featureExtractor[i] = pImpl->featureExtractor[i] * 0.95f + feature * 0.05f;
    }
    
    // Store observation in temporal pattern for sequence prediction
    if (!pImpl->temporalPattern.empty()) {
        // Shift temporal pattern forward
        for (size_t i = pImpl->temporalPattern.size() - 1; i > 0; --i) {
            pImpl->temporalPattern[i] = pImpl->temporalPattern[i - 1];
        }
        pImpl->temporalPattern[0] = (obsData.size() > 0) ? obsData[0] : 0.0f;
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
    pImpl->temporalPattern.assign(pImpl->temporalPattern.size(), 0.0f);
    pImpl->featureExtractor.assign(pImpl->featureExtractor.size(), 0.0f);
}

} // namespace nlm
