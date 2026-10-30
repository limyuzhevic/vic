// PredictionSystem Implementation
// Wraps NeuralPrediction to provide the PredictionSystem interface

#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

PredictionSystem::PredictionSystem() {}

PredictionSystem::~PredictionSystem() {}

void PredictionSystem::initialize(Brain* brain) {
    neuralPrediction_.initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction");
}

std::vector<float> PredictionSystem::predictNextState(const std::vector<float>& currentState) {
    return neuralPrediction_.generatePrediction(0); // currentStep would be passed from Brain
}

void PredictionSystem::updatePredictions(const std::vector<float>& predicted, const std::vector<float>& actual) {
    // NeuralPrediction has its own update mechanism via updateWithObservation
    // For now, just delegate to train method
    neuralPrediction_.train(actual);
}

float PredictionSystem::getPredictionError() const {
    return neuralPrediction_.getPredictionError();
}

float PredictionSystem::getConfidence() const {
    return neuralPrediction_.getPredictionConfidence();
}

const std::deque<float>& PredictionSystem::getErrorHistory() const {
    return neuralPrediction_.getErrorHistory();
}

void PredictionSystem::clearHistory() {
    neuralPrediction_.clearHistory();
}

void PredictionSystem::train(const std::vector<float>& observation) {
    // NeuralPrediction records sensory state internally during updateWithObservation
    // So we just delegate to train method
    neuralPrediction_.train(observation);
}

} // namespace nlm