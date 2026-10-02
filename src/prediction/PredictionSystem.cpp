#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction integration
    std::unique_ptr<NeuralPrediction> neuralPredictor;
    float neuralPredictionConfidence;
    float neuralPredictionError;
    
    Impl() : predictionError(0.0f), confidence(0.5f), neuralPredictionConfidence(0.0f), neuralPredictionError(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (pImpl->neuralPredictor && pImpl->neuralPredictor->isInitialized()) {
        // Use neural prediction system for real prediction
        const auto& sensoryData = currentState.getData();
        
        // Convert to format NeuralPrediction expects
        std::vector<float> stateVector(sensoryData.begin(), sensoryData.end());
        
        // Record current state for prediction
        pImpl->neuralPredictor->recordSensoryState(stateVector, 0);
        
        // Generate prediction
        std::vector<float> predictedState = pImpl->neuralPredictor->generatePrediction(0);
        
        // Convert back to SensoryInput format
        auto predictedInput = std::make_unique<SensoryInput>(predictedState.size());
        predictedInput->setData(predictedState);
        
        // Update error and confidence based on neural prediction
        pImpl->neuralPredictionError = pImpl->neuralPredictor->getPredictionError();
        pImpl->neuralPredictionConfidence = pImpl->neuralPredictor->getConfidence();
        
        NLM_LOG_INFO("PredictionSystem: Used neural prediction, confidence: " + std::to_string(pImpl->neuralPredictionConfidence));
        
        return predictedInput;
    }
    
    // Fallback to basic prediction
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
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
    
    // Update neural prediction if available
    if (pImpl->neuralPredictor) {
        std::vector<float> actualStateVector(actualData.begin(), actualData.end());
        float neuralError = pImpl->neuralPredictor->updateWithObservation(actualStateVector, 0);
        pImpl->predictionError = neuralError; // Use neural error as primary
    }
}

float PredictionSystem::getPredictionError() const {
    // Combine neural and traditional prediction errors
    float neuralError = pImpl->neuralPredictionError;
    float traditionalError = pImpl->predictionError;
    
    // Weight neural error higher if neural predictor is active
    float weight = pImpl->neuralPredictor && pImpl->neuralPredictor->isInitialized() ? 0.7f : 0.3f;
    
    return (neuralError * weight) + (traditionalError * (1.0f - weight));
}

float PredictionSystem::getConfidence() const {
    // Combine neural and traditional confidence
    float neuralConfidence = pImpl->neuralPredictionConfidence;
    float traditionalConfidence = pImpl->confidence;
    
    float weight = pImpl->neuralPredictor && pImpl->neuralPredictor->isInitialized() ? 0.7f : 0.3f;
    
    return (neuralConfidence * weight) + (traditionalConfidence * (1.0f - weight));
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
    if (pImpl->neuralPredictor) {
        const auto& obsData = observation.getData();
        std::vector<float> obsVector(obsData.begin(), obsData.end());
        
        pImpl->neuralPredictor->recordSensoryState(obsVector, 0);
        
        NLM_LOG_INFO("PredictionSystem: Neural predictor trained with observation");
    }
    
    // Also train traditional predictor
    const auto& obsData = observation.getData();
    if (!obsData.empty()) {
        // Update confidence based on observation
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    }
}

void PredictionSystem::initializeWithBrain(Brain* brain) {
    if (brain) {
        pImpl->neuralPredictor = std::make_unique<NeuralPrediction>();
        pImpl->neuralPredictor->initialize(brain);
        NLM_LOG_INFO("PredictionSystem: Neural predictor initialized with brain");
    }
}

float PredictionSystem::getNeuralPredictionError() const {
    return pImpl->neuralPredictionError;
}

float PredictionSystem::getNeuralPredictionConfidence() const {
    return pImpl->neuralPredictionConfidence;
}

bool PredictionSystem::hasNeuralPredictor() const {
    return pImpl->neuralPredictor && pImpl->neuralPredictor->isInitialized();
}

} // namespace nlm
