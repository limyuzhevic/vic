#include "PredictionSystem.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> neuralTrace;  // Neural representation of predictions
    size_t patternSize;
    
    Impl() : predictionError(0.0f), confidence(0.5f), patternSize(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using neural prediction networks
    const auto& data = currentState.getData();
    
    // Initialize neural trace if needed
    if (pImpl->patternSize != data.size()) {
        pImpl->patternSize = data.size();
        pImpl->neuralTrace.resize(pImpl->patternSize, 0.0f);
    }
    
    // Predictive coding: use working memory and neural dynamics to predict next state
    // This is a simplified implementation using neural attractor dynamics
    
    // Update neural trace based on current state (learning)
    float learningRate = 0.1f;
    for (size_t i = 0; i < pImpl->patternSize; ++i) {
        // Integrate current state into neural trace
        pImpl->neuralTrace[i] = (1.0f - learningRate) * pImpl->neuralTrace[i] + 
                              learningRate * data[i];
    }
    
    // Generate prediction based on neural trace with some noise
    std::vector<float> predictedData;
    predictedData.reserve(pImpl->patternSize);
    
    // Add some prediction noise/variation (predictive exploration)
    for (size_t i = 0; i < pImpl->patternSize; ++i) {
        float prediction = pImpl->neuralTrace[i];
        
        // Add small prediction uncertainty
        prediction += (pImpl->confidence - 0.5f) * 0.1f * (rand() / (RAND_MAX + 1.0f) * 2.0f - 1.0f);
        
        // Clamp prediction to reasonable range
        prediction = std::clamp(prediction, -10.0f, 10.0f);
        
        predictedData.push_back(prediction);
    }
    
    // Create and return the predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // Update confidence based on prediction error (will be refined by updatePredictions)
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation using neural mechanisms
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error using neural error signals
    float sumSquaredError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumSquaredError += error * error;
        maxError = std::max(maxError, std::abs(error));
    }
    
    // Compute error metrics
    pImpl->predictionError = sumSquaredError / predData.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep error history manageable
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update confidence based on prediction error
    // High error = low confidence, low error = high confidence
    pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
    
    // Neural credit assignment: modulate learning based on prediction error
    // This would integrate with brain's dopamine system in a full implementation
    if (pImpl->predictionError > 0.5f) {
        // Strong prediction error - increase exploration and learning
        pImpl->confidence *= 0.8f;
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train the prediction model using the observation
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Update neural trace with new observation (Hebbian-like learning)
    float learningRate = 0.05f;
    
    for (size_t i = 0; i < pImpl->patternSize && i < data.size(); ++i) {
        // Strengthen connections for consistent predictions
        pImpl->neuralTrace[i] = (1.0f - learningRate) * pImpl->neuralTrace[i] + 
                              learningRate * data[i];
    }
    
    // Update confidence based on learning progress
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.02f);
}

} // namespace nlm
