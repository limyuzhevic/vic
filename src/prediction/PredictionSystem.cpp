#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : brain(nullptr), neuralPrediction(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction component");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction for actual prediction
    if (pImpl->neuralPrediction) {
        const auto& data = currentState.getData();
        
        // Record current state for prediction
        pImpl->neuralPrediction->recordSensoryState(data, 0);
        
        // Generate prediction
        std::vector<float> predictedData = pImpl->neuralPrediction->generatePrediction(0);
        
        // Create a SensoryInput from the predicted data
        auto prediction = std::make_unique<InternalSignals>();
        for (float val : predictedData) {
            prediction->addSignal(val);
        }
        
        return prediction;
    }
    
    // Fallback: return copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (pImpl->neuralPrediction) {
        const auto& actualData = actual.getData();
        
        // Use NeuralPrediction to update predictions and compute error
        float error = pImpl->neuralPrediction->updateWithObservation(actualData, 0);
        
        // Get the computed error from neural prediction
        float predError = pImpl->neuralPrediction->getPredictionError();
        
        // Store error in prediction system for backward compatibility
        pImpl->predictionError = predError;
        pImpl->errorHistory.push_back(predError);
    }
    
    // Fallback: compute simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        float error = sumError / predData.size();
        
        if (!pImpl->errorHistory.empty()) {
            // Update the last error
            pImpl->errorHistory[pImpl->errorHistory.size() - 1] = error;
        } else {
            pImpl->errorHistory.push_back(error);
        }
        pImpl->predictionError = error;
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm
