#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction subsystem
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Step counter for prediction
    SimulationStep currentStep;
    
    Impl() : predictionError(0.0f), confidence(0.5f), currentStep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize neural prediction subsystem
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural prediction subsystem for real prediction
    if (!pImpl->neuralPrediction) return currentState.clone();
    
    // Convert current sensory input to vector for neural prediction
    const auto& currentData = currentState.getData();
    
    // Generate prediction using neural dynamics
    auto predictedData = pImpl->neuralPrediction->generatePrediction(pImpl->currentStep);
    
    // If no prediction available, use current state as baseline
    if (predictedData.empty()) {
        return currentState.clone();
    }
    
    // Convert back to SensoryInput
    auto result = currentState.clone();
    result->setData(predictedData);
    
    // Update step for next prediction
    pImpl->currentStep++;
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error using neural prediction subsystem
    if (!pImpl->neuralPrediction) {
        // Fallback to simple error calculation if neural prediction not available
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
        }
        return;
    }
    
    // Use neural prediction system to compute error and update
    const auto& actualData = actual.getData();
    
    // The neural prediction system tracks its own error
    float error = pImpl->neuralPrediction->getPredictionError();
    
    // Store prediction error
    pImpl->predictionError = error;
    if (error > 0) {  // Only record actual errors, not perfect predictions
        pImpl->errorHistory.push_back(error);
    }
    
    // Update neural prediction with actual observation for learning
    pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->currentStep);
    
    // Update confidence based on error
    pImpl->confidence = std::max(0.0f, 1.0f - error);
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

} // namespace nlm
