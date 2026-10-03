#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural substrate for prediction
    NeuralPrediction neuralPrediction;
    
    // Cached brain reference for neural substrate access
    Brain* brain;
    
    // Error tracking
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // State tracking
    SimulationStep currentStep;
    std::vector<float> lastObservedState;
    std::vector<float> lastPredictedState;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr), currentStep(0) {};
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize neural prediction subcomponent
    pImpl->neuralPrediction.initialize(nullptr); // brain reference will be set by PredictionSystem::initialize()
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    // Initialize prediction system
    pImpl->brain = brain;
    pImpl->neuralPrediction.initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::setCurrentStep(SimulationStep step) {
    pImpl->currentStep = step;
}

void PredictionSystem::initialize(Brain* brain) {
    // Initialize prediction system
    pImpl->brain = brain;
    pImpl->neuralPrediction.initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Convert sensory input to neural representation (vector<float>)
    const auto& data = currentState.getData();
    std::vector<float> stateVector = data;
    
    // Record current state for future predictions
    if (brain_) {
        pImpl->neuralPrediction.recordSensoryState(stateVector, pImpl->currentStep);
    }
    
    // Generate prediction using neural substrate
    std::vector<float> predictedData = pImpl->neuralPrediction.generatePrediction(pImpl->currentStep);
    
    // Update cached predicted state
    pImpl->lastPredictedState = predictedData;
    
    // Create new sensory input with predicted data
    // Use InternalSignals as base since it's the most generic
    auto predictedInput = std::make_unique<InternalSignals>();
    for (float val : predictedData) {
        predictedInput->addSignal(val);
    }
    
    // Update confidence based on prediction consistency
    pImpl->confidence = pImpl->neuralPrediction.getPredictionConfidence();
    
    // Log prediction
    NLM_LOG_INFO("PredictionSystem: Generated prediction at step " + 
                 std::to_string(pImpl->currentStep) + " with confidence " +
                 std::to_string(pImpl->confidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Convert sensory inputs to neural representations
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    std::vector<float> predVector(predictedData.begin(), predictedData.end());
    std::vector<float> actualVector(actualData.begin(), actualData.end());
    
    // Update cached actual state
    pImpl->lastObservedState = actualVector;
    
    // Update neural prediction with actual observation
    if (brain_) {
        pImpl->neuralPrediction.recordSensoryState(actualVector, pImpl->currentStep);
    }
    
    // Compute real prediction error using NeuralPrediction
    float error = 0.0f;
    if (!predVector.empty() && !actualVector.empty()) {
        // Compute error based on similarity between predicted and actual
        // Lower similarity = higher error
        float similarity = computeVectorSimilarity(predVector, actualVector);
        error = 1.0f - std::max(0.0f, similarity); // Ensure error >= 0
    }
    
    // Update prediction error
    pImpl->predictionError = error;
    
    // Update neural prediction with observation
    if (brain_) {
        pImpl->neuralPrediction.updateWithObservation(actualVector, pImpl->currentStep);
    }
    
    // Update confidence based on error (inverse relationship)
    pImpl->confidence = std::max(0.0f, 1.0f - error);
    
    // Record error in history
    pImpl->errorHistory.push_back(error);
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Utility function for computing similarity between two vectors
float computeVectorSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
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
    // Convert observation to neural representation
    const auto& data = observation.getData();
    std::vector<float> observationVector(data.begin(), data.end());
    
    // Update neural prediction with new observation for learning
    if (brain_) {
        pImpl->neuralPrediction.recordSensoryState(observationVector, pImpl->currentStep);
        
        // Increment step for temporal learning
        pImpl->currentStep++;
        
        // Train neural prediction with this new experience
        auto predicted = pImpl->neuralPrediction.generatePrediction(pImpl->currentStep);
        float error = pImpl->neuralPrediction.updateWithObservation(observationVector, pImpl->currentStep);
        
        // Log training
        NLM_LOG_INFO("PredictionSystem: Training completed at step " + 
                     std::to_string(pImpl->currentStep) + 
                     ", error: " + std::to_string(error));
    }
}

} // namespace nlm
