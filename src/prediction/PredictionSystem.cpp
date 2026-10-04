#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : neuralPrediction(std::make_unique<NeuralPrediction>()) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction to generate sophisticated prediction
    if (!pImpl->neuralPrediction) {
        NLM_LOG_ERROR("NeuralPrediction not initialized in PredictionSystem");
        return currentState.clone();
    }
    
    // Convert SensoryInput to vector for NeuralPrediction
    const auto& currentData = currentState.getData();
    std::vector<float> sensoryState(currentData.begin(), currentData.end());
    
    // Use NeuralPrediction to generate prediction
    std::vector<float> predictedState = pImpl->neuralPrediction->generatePrediction(0);
    
    if (predictedState.empty()) {
        // If prediction failed, return a modified version of current state
        predictedState = sensoryState;
        // Add some realistic noise
        for (auto& val : predictedState) {
            val += (pImpl->neuralPrediction->getPredictionError() * 0.1f);
        }
    }
    
    // Create new SensoryInput with predicted state
    auto result = std::make_unique<SensoryInput>();
    result->setData(predictedState);
    
    // Store prediction for error computation
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(predictedState, 0);
    }
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Use NeuralPrediction to compute prediction error
    if (!pImpl->neuralPrediction) return;
    
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    std::vector<float> predictedState(predictedData.begin(), predictedData.end());
    std::vector<float> actualState(actualData.begin(), actualData.end());
    
    // Compute and store prediction error
    float error = pImpl->neuralPrediction->updateWithObservation(actualState, 0);
    
    // Store error in error history
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    if (!pImpl->neuralPrediction) return 0.5f;
    return pImpl->neuralPrediction->getPredictionConfidence();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train the neural prediction system
    if (pImpl->neuralPrediction) {
        const auto& data = observation.getData();
        std::vector<float> sensoryState(data.begin(), data.end());
        pImpl->neuralPrediction->recordSensoryState(sensoryState, 0);
    }
}

} // namespace nlm
