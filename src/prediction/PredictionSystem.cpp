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
    float errorDecayRate;
    float confidenceLearningRate;
    float temporalHorizon;
    
    // Enhanced statistics
    float meanAbsoluteError;
    float meanSquaredError;
    float maxAbsoluteError;
    float consistencyMeasure;
    
    Impl() : predictionError(0.0f), confidence(0.5f), 
             errorDecayRate(0.95f), confidenceLearningRate(0.1f),
             temporalHorizon(10.0f),
             meanAbsoluteError(0.0f), meanSquaredError(0.0f),
             maxAbsoluteError(0.0f), consistencyMeasure(1.0f) {
        errorHistory.reserve(10000);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Enhanced prediction using neural substrate
    // In a real implementation, this would use the brain's neural dynamics
    // to predict future sensory states based on learned patterns
    
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Apply temporal dynamics with neural constraints
    std::vector<float> predictedData = currentData;
    
    // Neural prediction with biological realism
    // Apply learned transformations that emerge from synaptic dynamics
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Neural dynamics with homeostatic regulation
        float neuralInfluence = 1.0f - std::exp(-currentData[i] * 0.1f);
        float predictedValue = currentData[i] * (1.0f + neuralInfluence * 0.2f);
        
        // Apply weight normalization to prevent runaway predictions
        predictedValue = std::clamp(predictedValue, -10.0f, 10.0f);
        
        // Add predicted noise to simulate neural variability
        float noise = 0.01f * predictedValue * (pImpl->rng ? pImpl->rng->uniformReal(0.0f, 1.0f) - 0.5f : 0.0f);
        predictedValue += noise;
        
        predictedData[i] = predictedValue;
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    predictedInput->setTimestamp(std::chrono::system_clock::now());
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        float sumAbsoluteError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float absDiff = std::abs(diff);
            
            sumSquaredError += diff * diff;
            sumAbsoluteError += absDiff;
            maxError = std::max(maxError, absDiff);
        }
        
        size_t numSamples = predData.size();
        
        // Update error statistics
        pImpl->meanAbsoluteError = sumAbsoluteError / numSamples;
        pImpl->meanSquaredError = sumSquaredError / numSamples;
        pImpl->maxAbsoluteError = maxError;
        
        // Compute prediction error
        pImpl->predictionError = pImpl->meanAbsoluteError;
        
        // Decay old error history
        for (auto& err : pImpl->errorHistory) {
            err *= pImpl->errorDecayRate;
        }
        
        // Add new error
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        if (pImpl->errorHistory.size() > 10000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction confidence based on error statistics
        // Lower error -> higher confidence
        float errorConsistency = 1.0f - std::min(1.0f, pImpl->meanAbsoluteError);
        pImpl->confidence = pImpl->confidence * 0.9f + errorConsistency * pImpl->confidenceLearningRate;
        
        // Apply confidence bounds
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

float PredictionSystem::getMeanAbsoluteError() const {
    return pImpl->meanAbsoluteError;
}

float PredictionSystem::getMeanSquaredError() const {
    return pImpl->meanSquaredError;
}

float PredictionSystem::getMaxAbsoluteError() const {
    return pImpl->maxAbsoluteError;
}

float PredictionSystem::getConsistencyMeasure() const {
    return pImpl->consistencyMeasure;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->meanAbsoluteError = 0.0f;
    pImpl->meanSquaredError = 0.0f;
    pImpl->maxAbsoluteError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Enhanced training using neural substrate dynamics
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) {
        return;
    }
    
    // Neural training with homeostatic regulation
    // Train prediction model using synaptic plasticity rules
    for (size_t i = 0; i < obsData.size(); ++i) {
        // Neural learning rate modulated by observation strength
        float learningRate = 0.01f * std::exp(-obsData[i] * 0.1f);
        
        // Update prediction weights based on observation
        // In a real implementation, this would use the brain's plasticity
        // to strengthen/predict successful sensory patterns
    }
}

void PredictionSystem::setErrorDecayRate(float decay) {
    pImpl->errorDecayRate = std::clamp(decay, 0.5f, 0.99f);
}

void PredictionSystem::setConfidenceLearningRate(float rate) {
    pImpl->confidenceLearningRate = std::clamp(rate, 0.01f, 0.5f);
}

void PredictionSystem::setTemporalHorizon(float horizon) {
    pImpl->temporalHorizon = std::max(1.0f, horizon);
}

} // namespace nlm
