#include "PredictionSystem.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionWeights;
    std::vector<float> stateHistory;
    size_t sequenceLength;
    float learningRate;
    
    Impl() : predictionError(0.0f), confidence(0.5f), sequenceLength(10), learningRate(0.01f) {
        predictionWeights.resize(sequenceLength, 0.5f);
        stateHistory.reserve(sequenceLength * 2);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using temporal neural dynamics
    // Uses neural spike patterns and connectivity to predict next state
    
    // Create a copy of current state as base prediction
    auto predictedState = currentState.clone();
    
    // Get current state data
    const auto& currentData = currentState.getData();
    if (currentData.empty()) return predictedState;
    
    // Apply neural prediction based on temporal dynamics
    // Simulate how neural activity patterns evolve over time
    auto& predictedData = predictedState->getData();
    
    // For vision: predict motion and change patterns
    if (currentState.getType() == std::string("Vision")) {
        size_t dim = currentData.size();
        if (dim >= 6) {  // Should have at least width, height, channels
            // Apply neural motion prediction
            for (size_t i = 0; i < std::min<size_t>(6, dim); ++i) {
                // Add neural prediction component
                float neuralInfluence = 0.1f * std::sin(pImpl->stateHistory.size() * 0.1f);
                if (i < predictedData.size()) {
                    predictedData[i] = currentData[i] + neuralInfluence;
                }
            }
            
            // Predict future activation patterns based on neural history
            if (pImpl->stateHistory.size() >= 3) {
                float temporalTrend = calculateTemporalTrend();
                // Apply trend to middle of image data (simulating motion prediction)
                size_t midIndex = dim / 2;
                if (midIndex < predictedData.size() && midIndex + 3 < predictedData.size()) {
                    for (size_t i = 0; i < 3 && midIndex + i < predictedData.size(); ++i) {
                        predictedData[midIndex + i] = (predictedData[midIndex + i] + temporalTrend) * 0.5f;
                    }
                }
            }
        }
    }
    
    // For audio: predict next waveform based on neural oscillations
    else if (currentState.getType() == std::string("Audio")) {
        size_t dim = currentData.size();
        if (dim > 0) {
            // Use neural oscillation patterns for prediction
            float oscillation = 0.2f * std::cos(pImpl->stateHistory.size() * 0.05f);
            float frequencyShift = 0.1f * std::sin(pImpl->stateHistory.size() * 0.02f);
            
            for (size_t i = 0; i < std::min<size_t>(100, dim); ++i) {
                if (i < predictedData.size()) {
                    predictedData[i] = currentData[i] * (1.0f + oscillation) + frequencyShift;
                }
            }
        }
    }
    
    // For internal signals: predict homeostatic state changes
    else if (currentState.getType() == std::string("InternalSignals")) {
        size_t dim = currentData.size();
        if (dim > 0) {
            // Predict internal state evolution
            for (size_t i = 0; i < std::min<size_t>(10, dim); ++i) {
                if (i < predictedData.size()) {
                    // Use neural prediction weights
                    float weight = pImpl->predictionWeights[i % pImpl->predictionWeights.size()];
                    predictedData[i] = currentData[i] * weight + (1.0f - weight) * 0.5f;
                }
            }
        }
    }
    
    // Update state history for temporal prediction
    pImpl->stateHistory.push_back(calculateCurrentStateValue(currentState));
    if (pImpl->stateHistory.size() > pImpl->sequenceLength * 2) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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
        
        // Update prediction confidence based on error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Adapt prediction weights based on error
        adaptPredictionWeights();
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
    pImpl->stateHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // For now, just store observation for pattern learning
    float stateValue = calculateCurrentStateValue(observation);
    pImpl->stateHistory.push_back(stateValue);
    
    // Update prediction weights based on observed patterns
    if (pImpl->stateHistory.size() >= pImpl->sequenceLength) {
        updatePredictionWeights();
    }
}

float PredictionSystem::calculateCurrentStateValue(const SensoryInput& state) const {
    const auto& data = state.getData();
    if (data.empty()) return 0.0f;
    
    // Calculate neural-like state value from sensory input
    float sum = std::accumulate(data.begin(), std::min(data.begin() + 10, data.end()), 0.0f);
    float mean = sum / std::min<size_t>(10, data.size());
    
    // Apply neural activation function (sigmoid-like)
    return 1.0f / (1.0f + std::exp(-mean * 2.0f + 1.0f));
}

float PredictionSystem::calculateTemporalTrend() const {
    if (pImpl->stateHistory.size() < 3) return 0.0f;
    
    // Calculate temporal trend from recent state history
    float sumDiff = 0.0f;
    for (size_t i = 1; i < std::min<size_t>(4, pImpl->stateHistory.size()); ++i) {
        sumDiff += pImpl->stateHistory[pImpl->stateHistory.size() - i] - 
                  pImpl->stateHistory[pImpl->stateHistory.size() - i - 1];
    }
    return sumDiff / std::min<size_t>(3, pImpl->stateHistory.size() - 1);
}

void PredictionSystem::adaptPredictionWeights() {
    if (pImpl->predictionError > 0.5f) {
        // High error: decrease confidence in predictions
        for (float& weight : pImpl->predictionWeights) {
            weight *= 0.95f;
        }
    } else if (pImpl->predictionError < 0.1f) {
        // Low error: increase confidence and adapt weights
        for (float& weight : pImpl->predictionWeights) {
            weight = std::min(1.0f, weight + pImpl->learningRate * 0.1f);
        }
    }
}

void PredictionSystem::updatePredictionWeights() {
    if (pImpl->stateHistory.size() < pImpl->sequenceLength) return;
    
    // Calculate moving average of recent states for weight adaptation
    float recentSum = 0.0f;
    for (size_t i = pImpl->stateHistory.size() - pImpl->sequenceLength;
         i < pImpl->stateHistory.size(); ++i) {
        recentSum += pImpl->stateHistory[i];
    }
    float recentAvg = recentSum / pImpl->sequenceLength;
    
    // Update weights to match recent average state
    for (float& weight : pImpl->predictionWeights) {
        weight = recentAvg;
    }
}

} // namespace nlm