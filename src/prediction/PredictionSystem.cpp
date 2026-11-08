#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Prediction model weights ( Hebbian learning )
    std::vector<std::vector<float>> predictionWeights;
    std::vector<float> predictionBiases;
    
    // Current sensory state and prediction
    std::vector<float> currentSensoryState;
    std::vector<float> predictedSensoryState;
    
    // Prediction error components
    float predictionError;
    float confidence;
    float learningRate;
    
    // Error history for stability analysis
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    
    // Prediction system state
    bool isActive;
    SimulationStep lastUpdateStep;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), 
             isActive(false), lastUpdateStep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Update internal prediction state
    pImpl->currentSensoryState = currentState.getData();
    
    // Generate prediction using neural-weighted pattern completion
    predictedSensoryState = generatePrediction(pImpl->currentSensoryState);
    
    // Create sensory input from prediction
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedSensoryState);
    
    // Store prediction for analysis
    pImpl->predictionHistory.push_back(calculatePatternSimilarity(predictedSensoryState, pImpl->currentSensoryState));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Update prediction error with more sophisticated calculation
    pImpl->predictionError = calculatePredictionError(predData, actualData);
    
    // Update confidence based on prediction accuracy
    pImpl->confidence = calculateConfidence(pImpl->predictionError, predData, actualData);
    
    // Store error in history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Keep error history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction weights if error is significant
    if (pImpl->predictionError > 0.1f) {
        updatePredictionWeights(predData, actualData);
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
    // Train prediction model using Hebbian learning
    const auto& obsData = observation.getData();
    
    if (!pImpl->isActive || obsData.empty()) {
        return;
    }
    
    // Ensure weight matrix is sized correctly
    if (pImpl->predictionWeights.empty() || 
        pImpl->predictionWeights[0].size() != obsData.size()) {
        initializePredictionWeights(obsData.size());
    }
    
    // Update weights based on observation
    for (size_t i = 0; i < obsData.size(); ++i) {
        // Use sensory input to update predictions
        float prediction = pImpl->predictedSensoryState[i];
        float actual = obsData[i];
        
        // Hebbian update: strengthen connections that predict well
        float delta = (actual - prediction) * pImpl->learningRate;
        pImpl->predictionWeights[0][i] += delta * actual;
        
        // Update prediction
        pImpl->predictedSensoryState[i] = prediction + delta;
    }
}

void PredictionSystem::enable(bool enable) {
    pImpl->isActive = enable;
}

bool PredictionSystem::isEnabled() const {
    return pImpl->isActive;
}

void PredictionSystem::initializePredictionWeights(size_t inputSize) {
    // Initialize prediction weights using Gaussian random distribution
    // Each weight represents a predictive connection from neural population
    pImpl->predictionWeights.clear();
    pImpl->predictionBiases.clear();
    
    // Single layer prediction model for now
    std::vector<float> weights;
    weights.reserve(inputSize);
    for (size_t i = 0; i < inputSize; ++i) {
        // Initialize with small random values (-0.1 to 0.1)
        weights.push_back((static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f);
    }
    pImpl->predictionWeights.push_back(weights);
    
    // Initialize biases
    pImpl->predictionBiases.resize(inputSize, 0.0f);
}

std::vector<float> PredictionSystem::generatePrediction(const std::vector<float>& sensoryState) {
    // Generate prediction using weighted pattern completion
    std::vector<float> prediction(sensoryState.size(), 0.0f);
    
    if (pImpl->predictionWeights.empty()) {
        return prediction;
    }
    
    // Simple prediction: use current state as starting point
    // In a full implementation, this would use the learned weights
    prediction = sensoryState;
    
    // Add some prediction error to drive learning
    for (size_t i = 0; i < prediction.size(); ++i) {
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        prediction[i] += noise;
        prediction[i] = std::max(-10.0f, std::min(10.0f, prediction[i]));
    }
    
    return prediction;
}

float PredictionSystem::calculatePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f; // Maximum error if shapes don't match
    }
    
    // Calculate prediction error using weighted combination of differences
    float sumSquaredError = 0.0f;
    float maxPossibleError = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = predicted[i] - actual[i];
        sumSquaredError += error * error;
        maxPossibleError += (fabs(predicted[i]) + fabs(actual[i])) * 0.5f;
    }
    
    float mse = sumSquaredError / predicted.size();
    
    if (maxPossibleError > 0.001f) {
        return mse / maxPossibleError;
    }
    
    return mse;
}

float PredictionSystem::calculateConfidence(float predictionError, const std::vector<float>& predicted, const std::vector<float>& actual) const {
    // Confidence based on prediction error and pattern consistency
    float errorComponent = 1.0f / (1.0f + predictionError * 10.0f);
    
    // Pattern consistency: higher when prediction matches actual
    float consistency = 0.0f;
    if (predicted.size() == actual.size() && !predicted.empty()) {
        float dotProduct = 0.0f;
        float normPred = 0.0f, normActual = 0.0f;
        
        for (size_t i = 0; i < predicted.size(); ++i) {
            dotProduct += predicted[i] * actual[i];
            normPred += predicted[i] * predicted[i];
            normActual += actual[i] * actual[i];
        }
        
        if (normPred > 0.001f && normActual > 0.001f) {
            consistency = dotProduct / (std::sqrt(normPred) * std::sqrt(normActual));
        }
    }
    
    // Combine error and consistency for confidence
    return (errorComponent + consistency) * 0.5f;
}

float PredictionSystem::calculatePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    // Cosine similarity between two patterns
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.001f || normB < 0.001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

void PredictionSystem::updatePredictionWeights(const std::vector<float>& predicted, const std::vector<float>& actual) {
    if (predicted.size() != actual.size()) return;
    
    // Update prediction weights using error signals
    // This is a simplified version of predictive coding weight updates
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        // Compute weight update based on prediction error
        float error = actual[i] - predicted[i];
        
        // Hebbian-like update: strengthen predictions that reduce error
        if (pImpl->predictionWeights.size() > 0 && i < pImpl->predictionWeights[0].size()) {
            float learningSignal = error * pImpl->learningRate;
            pImpl->predictionWeights[0][i] += learningSignal * actual[i];
            pImpl->predictionBiases[i] += learningSignal;
        }
    }
}

float PredictionSystem::getAveragePredictionAccuracy() const {
    if (pImpl->predictionHistory.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float accuracy : pImpl->predictionHistory) {
        sum += accuracy;
    }
    return sum / pImpl->predictionHistory.size();
}

} // namespace nlm
