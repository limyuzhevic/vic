#include "PredictionSystem.hpp"
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"

// Forward declarations
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // Use neural prediction system if available
    if (pImpl->neuralPrediction && pImpl->brain) {
        // Extract current state as neural activity pattern
        const std::vector<float>& currentData = currentState.getData();
        
        // Generate prediction using neural prediction
        std::vector<float> predictedState = pImpl->neuralPrediction->generatePrediction(pImpl->brain->getCurrentStep());
        
        // Create appropriate SensoryInput type (for now, use InternalSignals)
        auto predictedInput = std::make_unique<InternalSignals>();
        for (float val : predictedState) {
            predictedInput->addSignal(val);
        }
        
        return predictedInput;
    }
    
    // Fallback: Just return a copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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
    
    // Also update neural prediction if available
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->brain->getCurrentStep());
    }
}

float PredictionSystem::getPredictionError() const {
    float baseError = pImpl->predictionError;
    float neuralError = pImpl->neuralPrediction ? pImpl->neuralPrediction->getPredictionError() : 0.0f;
    
    // Combine errors for overall prediction error
    return (baseError + neuralError) * 0.5f;
}

float PredictionSystem::getConfidence() const {
    // Simple confidence calculation based on error
    float error = getPredictionError();
    return std::max(0.0f, 1.0f - error);
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
    // TODO PHASE 2: Train prediction model
    // Extract observation data
    const std::vector<float>& obsData = observation.getData();
    
    // Record for learning
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(obsData, pImpl->brain->getCurrentStep());
    }
}

NeuralPrediction* PredictionSystem::getNeuralPrediction() {
    return pImpl->neuralPrediction.get();
}

} // namespace nlm