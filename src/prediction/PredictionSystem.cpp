#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

// Forward declaration of Brain
class Brain;

struct PredictionSystem::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionPredictor;
    std::unique_ptr<PredictionErrorSignal> errorSignal;
    float lastPredictedValue;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : brain(nullptr), lastPredictedValue(0.0f), predictionError(0.0f), confidence(0.5f) {}
};

void PredictionSystem::train(const SensoryInput& observation) {
    // Update prediction model with new observation
    // For now, just update the last predicted value
    auto data = observation.getData();
    if (!data.empty()) {
        pImpl->lastPredictedValue = data[0];  // Simple first-value prediction
    }
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                         const SensoryInput& actual) {
    // Compute prediction error
    auto visionPred = Vision::convertInput(predicted);
    auto visionActual = Vision::convertInput(actual);
    
    // Update neural prediction
    if (pImpl->neuralPrediction) {
        pImpl->predictionError = pImpl->neuralPrediction->updateWithObservation(
            visionActual, pImpl->lastPredictedValue);
    }
    
    // Update prediction error signal for neuromodulation
    if (pImpl->errorSignal) {
        float error = pImpl->errorSignal->computeError(visionPred, visionActual);
        pImpl->errorSignal->recordError(error, pImpl->lastPredictedValue);
    }
    
    // Record error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    // Simple confidence based on error history consistency
    if (pImpl->errorHistory.size() < 2) return 0.5f;
    
    float variance = 0.0f;
    float mean = 0.0f;
    for (float error : pImpl->errorHistory) {
        mean += error;
    }
    mean /= pImpl->errorHistory.size();
    
    for (float error : pImpl->errorHistory) {
        float diff = error - mean;
        variance += diff * diff;
    }
    variance /= pImpl->errorHistory.size();
    
    return std::max(0.1f, std::min(0.9f, 1.0f - variance));
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Get neural prediction
    if (!pImpl->neuralPrediction || pImpl->lastPredictedValue == 0) {
        return currentState.clone();
    }
    
    // Generate prediction based on neural dynamics
    auto visionData = Vision::convertInput(currentState);
    std::vector<float> predictedPattern;
    
    // Use action predictor if available
    if (pImpl->actionPredictor) {
        predictedPattern = pImpl->actionPredictor->predictConsequence(
            ActionType::Wait, visionData);
    } else {
        // Use neural prediction
        predictedPattern = pImpl->neuralPrediction->generatePrediction(pImpl->lastPredictedValue);
    }
    
    // Convert back to sensory input
    auto predictedVision = Vision::createInput(predictedPattern);
    
    // Update brain state for next prediction
    pImpl->lastPredictedValue = pImpl->lastPredictedValue + 1;
    
    return std::make_unique<SensoryInput>(predictedVision);
}

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize with neural prediction system
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->actionPredictor = std::make_unique<ActionConsequencePredictor>();
    pImpl->errorSignal = std::make_unique<PredictionErrorSignal>();
    
    pImpl->confidence = 0.5f;
}

PredictionSystem::~PredictionSystem() = default;

} // namespace nlm