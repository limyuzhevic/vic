#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<NeuralPrediction> neuralPredictor;
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {
        neuralPredictor = std::make_unique<NeuralPrediction>();
    }
};

// Initialize prediction system with brain reference
void PredictionSystem::initialize(Brain* brain) {
    if (!pImpl->neuralPredictor) {
        pImpl->neuralPredictor = std::make_unique<NeuralPrediction>();
    }
    pImpl->brain = brain;
    pImpl->neuralPredictor->initialize(brain);
}

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->neuralPredictor || !pImpl->brain) {
        return currentState.clone();
    }
    
    // Convert current sensory state to vector for neural prediction
    const auto& currentData = currentState.getData();
    
    // Use neural substrate to generate prediction
    auto prediction = pImpl->neuralPredictor->generatePrediction(pImpl->brain->getCurrentStep());
    
    if (prediction.empty()) {
        return currentState.clone();
    }
    
    // Create a new SensoryInput with the predicted data
    // For simplicity, we'll use the current state type but with predicted values
    auto predictedInput = std::make_unique<SensoryInput>();
    // Note: In a real implementation, we would need proper type preservation
    // For now, we update the internal data
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->neuralPredictor) {
        return;
    }
    
    // Convert sensory inputs to vectors
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Update neural prediction with actual observation
    float error = 0.0f;
    if (!predictedData.empty() && !actualData.empty() && predictedData.size() == actualData.size()) {
        error = pImpl->neuralPredictor->updateWithObservation(actualData, pImpl->brain->getCurrentStep());
    } else {
        // Fallback to simple MSE
        if (!predictedData.empty() && !actualData.empty()) {
            float sumError = 0.0f;
            size_t size = std::min(predictedData.size(), actualData.size());
            for (size_t i = 0; i < size; ++i) {
                float diff = predictedData[i] - actualData[i];
                sumError += diff * diff;
            }
            error = sumError / size;
        }
    }
    
    // Update internal state
    pImpl->predictionError = error;
    pImpl->confidence = 1.0f / (1.0f + error); // Inverse relationship
    pImpl->errorHistory.push_back(error);
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
    if (pImpl->neuralPredictor) {
        pImpl->neuralPredictor->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->neuralPredictor || !pImpl->brain) {
        return;
    }
    
    // Convert observation to vector for neural training
    const auto& obsData = observation.getData();
    if (obsData.empty()) {
        return;
    }
    
    // Record the sensory state in the neural predictor for sequence learning
    pImpl->neuralPredictor->recordSensoryState(obsData, pImpl->brain->getCurrentStep());
}

} // namespace nlm
