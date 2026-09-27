#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    // Integrated prediction subsystems
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal;
    
    // Combined metrics
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        // Initialize all prediction subsystems
        neuralPrediction = std::make_unique<NeuralPrediction>();
        actionConsequencePredictor = std::make_unique<ActionConsequencePredictor>();
        predictionErrorSignal = std::make_unique<PredictionErrorSignal>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction to generate prediction based on learned temporal sequences
    const auto& currentData = currentState.getData();
    
    if (!pImpl->neuralPrediction || currentData.empty()) {
        // Fallback to simple copy if prediction system not initialized or no data
        return currentState.clone();
    }
    
    // Generate prediction using neural prediction system
    std::vector<float> predictedData = pImpl->neuralPrediction->generatePrediction(0);
    
    if (predictedData.empty()) {
        // If prediction failed, return copy of current state
        return currentState.clone();
    }
    
    // Convert back to SensoryInput
    auto predictedSensory = std::make_unique<SensoryInput>(predictedData);
    
    // Update confidence based on prediction quality
    float similarity = pImpl->neuralPrediction->getPredictionConfidence();
    pImpl->confidence = similarity;
    
    NLM_LOG_INFO("PredictionSystem: Generated prediction with confidence " + std::to_string(similarity));
    
    return predictedSensory;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Use PredictionErrorSignal to compute prediction error
    if (!pImpl->predictionErrorSignal) {
        NLM_LOG_ERROR("PredictionSystem: Prediction error signal not initialized");
        return;
    }
    
    // Convert to vector format for error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Compute prediction error using PredictionErrorSignal
    float error = pImpl->predictionErrorSignal->computeError(predData, actualData);
    
    // Update prediction error metrics
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Update PredictionErrorSignal history
    static SimulationStep lastStep = 0;
    pImpl->predictionErrorSignal->recordError(error, lastStep);
    
    // Update confidence based on error (lower error = higher confidence)
    pImpl->confidence = std::max(0.0f, 1.0f - error);
    
    NLM_LOG_INFO("PredictionSystem: Updated predictions - Error: " + std::to_string(error) + 
                 ", Confidence: " + std::to_string(pImpl->confidence));
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
    if (!pImpl->neuralPrediction || !pImpl->actionConsequencePredictor) {
        NLM_LOG_ERROR("PredictionSystem: Cannot train - subsystems not initialized");
        return;
    }
    
    // Convert observation to vector format for training
    const auto& observationData = observation.getData();
    
    if (!observationData.empty()) {
        // Train NeuralPrediction system with current observation
        pImpl->neuralPrediction->recordSensoryState(observationData, 0);
        
        // Train ActionConsequencePredictor (though we don't have action context here)
        // This would typically be called from ActionConsequencePredictor when
        // action experiences are recorded
        
        NLM_LOG_INFO("PredictionSystem: Trained with observation of " + 
                     std::to_string(observationData.size()) + " data points");
    } else {
        NLM_LOG_WARN("PredictionSystem: Cannot train - empty observation data");
    }
}

} // namespace nlm
