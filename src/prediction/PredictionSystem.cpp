#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

PredictionSystem::PredictionSystem() 
    : neuralPrediction_(nullptr)
    , actionConsequencePredictor_(nullptr)
    , predictionErrorSignal_(nullptr)
    , brain_(nullptr) {
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain) {
    brain_ = brain;
    
    // Initialize neural prediction system
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain_);
    
    // Initialize action consequence predictor
    actionConsequencePredictor_ = std::make_unique<ActionConsequencePredictor>();
    actionConsequencePredictor_->initialize(brain_);
    
    // Initialize prediction error signal
    predictionErrorSignal_ = std::make_unique<PredictionErrorSignal>();
    
    NLM_LOG_INFO("Prediction system initialized with neural substrate");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralPrediction_ || !brain_) {
        NLM_LOG_WARNING("Prediction system not initialized");
        return currentState.clone();
    }
    
    // Convert SensoryInput to vector for neural prediction
    const auto& stateData = currentState.getData();
    
    // Use neural prediction to generate next state prediction
    std::vector<float> predictedState = neuralPrediction_->generatePrediction(brain_->getCurrentStep());
    
    // If no prediction available, return current state
    if (predictedState.empty()) {
        return currentState.clone();
    }
    
    // Create a Vision input with the predicted state (assuming vision as example)
    // In a real implementation, we would preserve the input type
    auto* visionInput = new Vision();
    visionInput->setData(predictedState);
    
    return std::unique_ptr<SensoryInput>(visionInput);
}

float PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!predictionErrorSignal_ || !neuralPrediction_) {
        return 0.0f;
    }
    
    // Convert SensoryInputs to vectors for prediction error computation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Use NeuralPrediction to update with observation
        float error = neuralPrediction_->updateWithObservation(actualData, brain_->getCurrentStep());
        
        // Compute prediction error using PredictionErrorSignal
        float totalError = predictionErrorSignal_->computeError(predData, actualData);
        predictionErrorSignal_->recordError(totalError, brain_->getCurrentStep());
        
        // Use neural prediction to predict action consequences if needed
        neuralPrediction_->enableTemporalPrediction(true);
        neuralPrediction_->enableActionConsequencePrediction(true);
        
        return totalError;
    }
    
    return 0.0f;
}

float PredictionSystem::getPredictionError() const {
    if (predictionErrorSignal_) {
        return predictionErrorSignal_->computeError(std::vector<float>(), std::vector<float>()); // Simplified
    }
    return 0.0f;
}

float PredictionSystem::getConfidence() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionConfidence();
    }
    return 0.5f;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    if (predictionErrorSignal_) {
        return predictionErrorSignal_->getErrorHistory();
    }
    static const std::vector<float> empty;
    return empty;
}

void PredictionSystem::clearHistory() {
    if (neuralPrediction_) neuralPrediction_->clearHistory();
    if (predictionErrorSignal_) predictionErrorSignal_->getErrorHistory().clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!neuralPrediction_) return;
    
    // Record the observation in neural prediction system
    const auto& data = observation.getData();
    neuralPrediction_->recordSensoryState(data, brain_->getCurrentStep());
    
    // Also update action consequence predictor if action is available
    // (This would require action information to be passed)
}

float PredictionSystem::getModulationSignal() const {
    if (predictionErrorSignal_) {
        return predictionErrorSignal_->getModulationSignal();
    }
    return 0.0f;
}

bool PredictionSystem::isSurprising() const {
    if (predictionErrorSignal_) {
        return predictionErrorSignal_->isSurprising();
    }
    return false;
}

std::vector<std::vector<float>> PredictionSystem::getTemporalPredictions(size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    if (!neuralPrediction_) return predictions;
    
    predictions = neuralPrediction_->predictMultipleSteps(brain_->getCurrentStep(), numSteps);
    return predictions;
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action, 
                                                             const SensoryInput& currentState) {
    if (!actionConsequencePredictor_) return std::vector<float>();
    
    const auto& stateData = currentState.getData();
    return actionConsequencePredictor_->predictConsequence(action, stateData);
}

} // namespace nlm
