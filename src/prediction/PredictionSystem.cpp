#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionBuffer;
    std::mt19937 rng;
    
    Impl() : predictionError(0.0f), confidence(0.5f), rng(std::random_device{}()) {
        predictionBuffer.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        auto emptyInput = std::make_unique<SensoryInput>();
        return emptyInput;
    }
    
    // Create predicted state with noise and temporal patterns
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // Add temporal continuity: current state with small modifications
    std::vector<float> predictedData = currentData;
    
    // Add temporal dynamics: gradual changes based on brain state
    float dt = 0.001f;  // Default timestep
    
    // Apply prediction using neural dynamics
    // For now, use simple random walk with noise
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < predictedData.size(); ++i) {
        // Add noise for prediction uncertainty
        float noise = dist(pImpl->rng);
        predictedData[i] += noise;
        
        // Apply neuromodulation influence (dopamine affects exploration)
        // In a full implementation, this would access brain state
        float neuromodulation = 0.0f;  // Would get from brain
        predictedData[i] += neuromodulation * 0.05f;
    }
    
    // Clamp to valid range [0, 1] for sensory inputs
    for (float& val : predictedData) {
        val = std::max(0.0f, std::min(1.0f, val));
    }
    
    predictedInput->setData(predictedData);
    pImpl->predictionBuffer.push_back(confidence);
    
    if (pImpl->predictionBuffer.size() > 100) {
        pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                         const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        pImpl->predictionError = 0.0f;
        return;
    }
    
    // Compute prediction error using various error metrics
    float sumSquaredError = 0.0f;
    float sumAbsoluteError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumSquaredError += diff * diff;
        sumAbsoluteError += std::abs(diff);
        maxError = std::max(maxError, std::abs(diff));
    }
    
    size_t n = predData.size();
    float mse = sumSquaredError / n;           // Mean Squared Error
    float mae = sumAbsoluteError / n;          // Mean Absolute Error
    
    // Combine error metrics (weighted)
    pImpl->predictionError = 0.7f * mse + 0.3f * mae;
    
    // Update confidence based on error
    // Lower error = higher confidence
    pImpl->confidence = std::max(0.0f, 1.0f - std::sqrt(pImpl->predictionError) * 5.0f);
    pImpl->confidence = std::min(1.0f, pImpl->confidence);
    
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Limit history size
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
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
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Simple online learning: update internal model
    // In a full implementation, this would train neural networks
    // or update synaptic weights for prediction
    
    // Store observation for learning
    pImpl->predictionBuffer.push_back(data[0]);  // Use first value as sample
    
    if (pImpl->predictionBuffer.size() > 100) {
        pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
    }
    
    // Gradually improve confidence based on accumulated data
    if (!pImpl->predictionBuffer.empty()) {
        float avgValue = 0.0f;
        for (float val : pImpl->predictionBuffer) {
            avgValue += val;
        }
        avgValue /= pImpl->predictionBuffer.size();
        
        // Improve confidence if predictions are reasonable
        if (avgValue > 0.1f && avgValue < 0.9f) {
            pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
        }
    }
}

} // namespace nlm
