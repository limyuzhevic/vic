#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/Vision.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : brain(nullptr) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->neuralPrediction->initialize(brain);
        NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction");
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Convert current sensory state to vector format for NeuralPrediction
    const std::vector<float>& currentData = currentState.getData();
    
    if (!currentData.empty() && pImpl->neuralPrediction) {
        // Record current state for learning
        pImpl->neuralPrediction->recordSensoryState(currentData, 0);
        
        // Generate prediction for next timestep
        std::vector<float> predictedData = pImpl->neuralPrediction->generatePrediction(0);
        
        if (!predictedData.empty()) {
            // Create Vision input with predicted data (assuming vision for now)
            // In a real implementation, we'd need to determine the sensory type
            auto predictedInput = std::make_unique<Vision>(1, 1, predictedData.size());
            predictedInput->setData(predictedData);
            return predictedInput;
        }
    }
    
    // Fallback: return current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Convert to vector format for NeuralPrediction
    const std::vector<float>& predictedData = predicted.getData();
    const std::vector<float>& actualData = actual.getData();
    
    if (!predictedData.empty() && !actualData.empty() && pImpl->neuralPrediction) {
        // Record actual observation for learning
        pImpl->neuralPrediction->recordSensoryState(actualData, 0);
        
        // Update prediction with actual observation and compute error
        float predictionError = pImpl->neuralPrediction->updateWithObservation(actualData, 0);
        
        // Note: NeuralPrediction doesn't have error history stored in the same way
        // The error is computed on-demand in updateWithObservation
    }
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->neuralPrediction) {
        return pImpl->neuralPrediction->getPredictionError();
    }
    return 0.0f;
}

float PredictionSystem::getConfidence() const {
    if (pImpl->neuralPrediction) {
        return pImpl->neuralPrediction->getPredictionConfidence();
    }
    return 0.0f;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    static const std::vector<float> emptyHistory;
    if (pImpl->neuralPrediction) {
        // NeuralPrediction stores error history in a deque
        // For compatibility with PredictionSystem API, we could extract it
        // but NeuralPrediction's error history is per-call, not persistent
        return emptyHistory;
    }
    return emptyHistory;
}

void PredictionSystem::clearHistory() {
    if (pImpl->neuralPrediction) {
        // NeuralPrediction has clearHistory() method
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Convert observation to vector format
    const std::vector<float>& obsData = observation.getData();
    if (!obsData.empty() && pImpl->neuralPrediction) {
        // Record observation for learning
        pImpl->neuralPrediction->recordSensoryState(obsData, 0);
        NLM_LOG_INFO("PredictionSystem trained with observation");
    }
}

} // namespace nlm
