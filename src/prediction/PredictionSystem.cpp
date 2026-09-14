#include "PredictionSystem.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include "../sensory/InternalSignals.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Model state
    std::vector<float> currentModel;
    std::vector<float> predictionBuffer;
    
    // Learning parameters
    float learningRate;
    float errorDecay;
    float confidenceDecay;
    
    // Statistics
    size_t successfulPredictions;
    size_t totalPredictions;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f)
        , learningRate(0.01f), errorDecay(0.99f), confidenceDecay(0.98f)
        , successfulPredictions(0), totalPredictions(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::update(const SensoryInput& currentSensory, 
                              float neuralState,
                              const std::vector<float>& workingMemory) {
    // Encode sensory input into state representation
    const auto& sensoryData = currentSensory.getData();
    
    // Combine sensory input with neural state and working memory for model state
    std::vector<float> encodedState;
    encodedState.reserve(sensoryData.size() + 1 + workingMemory.size());
    
    // Encode sensory input
    for (float val : sensoryData) {
        encodedState.push_back(val);
    }
    
    // Add neural state as bias
    encodedState.push_back(neuralState);
    
    // Add working memory content
    for (float val : workingMemory) {
        encodedState.push_back(val);
    }
    
    // Update model state (simple temporal integration)
    pImpl->currentModel = encodedState;
    
    // Predict next state based on current model
    std::vector<float> predictedState = predictNextStateInternal(pImpl->currentModel);
    pImpl->predictionBuffer = predictedState;
    
    // Compute prediction error by comparing prediction with actual sensory input
    float error = computePredictionError(predictedState, sensoryData);
    pImpl->predictionError = error;
    
    // Update prediction error history
    if (pImpl->errorHistory.size() < 1000) {
        pImpl->errorHistory.push_back(error);
    } else {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        pImpl->errorHistory.push_back(error);
    }
    
    // Update confidence based on prediction error
    // Lower error = higher confidence
    pImpl->confidence = std::max(0.0f, 1.0f - error);
    
    // Update confidence decay
    pImpl->confidence *= pImpl->confidenceDecay;
    
    // Update internal model based on prediction error (learning)
    updateInternalModel(sensoryData, error);
    
    // Update statistics
    pImpl->totalPredictions++;
    if (error < 0.1f) {  // Consider prediction successful if error is low
        pImpl->successfulPredictions++;
    }
}

void PredictionSystem::updateInternalModel(const std::vector<float>& actualData, 
                                         float error) {
    // Update internal model based on prediction error
    // Simple gradient-based learning: adjust model in direction of actual data
    if (pImpl->currentModel.size() != actualData.size() && !pImpl->currentModel.empty()) {
        return;  // Size mismatch, cannot update
    }
    
    for (size_t i = 0; i < pImpl->currentModel.size() && i < actualData.size(); ++i) {
        // Adjust model based on error - simple delta rule
        float delta = pImpl->learningRate * (actualData[i] - pImpl->currentModel[i]) * error;
        pImpl->currentModel[i] += delta;
    }
}

std::vector<float> PredictionSystem::predictNextStateInternal(const std::vector<float>& state) {
    // Simple prediction: return state with small modifications
    // In a real implementation, this would use neural network or other learned dynamics
    std::vector<float> prediction = state;
    
    // Add small random perturbation (simulating dynamics)
    static std::mt19937 rng(42);  // Use fixed seed for reproducibility
    std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
    
    for (float& val : prediction) {
        float perturbation = dist(rng);
        val += perturbation;
    }
    
    return prediction;
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, 
                                             const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 1.0f;  // Maximum error if sizes don't match or data is empty
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    // Normalize error to [0, 1] range
    float meanSquaredError = sumSquaredError / predicted.size();
    float maxPossibleError = 0.0f;
    for (float val : predicted) {
        float maxVal = std::abs(val);
        if (maxVal > maxPossibleError) maxPossibleError = maxVal;
    }
    maxPossibleError = std::max(maxPossibleError * maxPossibleError, 0.01f);
    
    return std::min(1.0f, std::sqrt(meanSquaredError / maxPossibleError));
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

float PredictionSystem::getAverageConfidence() const {
    if (pImpl->totalPredictions == 0) return 0.0f;
    // Simple average based on error history
    float sum = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += 1.0f - error;  // Convert error to confidence
    }
    return sum / pImpl->errorHistory.size();
}

} // namespace nlm
