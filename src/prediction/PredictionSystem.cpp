#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    std::unique_ptr<NeuralPrediction> neuralPredictor;
    std::unique_ptr<ActionConsequencePredictor> actionPredictor;
    std::unique_ptr<PredictionErrorSignal> errorSignal;
    
    // Current prediction state
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Configuration
    bool enableTemporalPrediction;
    bool enableActionConsequences;
    float temporalPredictionWeight;
    float actionPredictionWeight;
    
    Impl() :
        predictionError(0.0f),
        confidence(0.5f),
        enableTemporalPrediction(true),
        enableActionConsequences(true),
        temporalPredictionWeight(0.7f),
        actionPredictionWeight(0.3f) {
        
        neuralPredictor = std::make_unique<NeuralPrediction>();
        actionPredictor = std::make_unique<ActionConsequencePredictor>();
        errorSignal = std::make_unique<PredictionErrorSignal>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using NLM's neural substrate
    // Combines temporal prediction, action consequences, and neural dynamics
    
    if (!pImpl->enableTemporalPrediction && !pImpl->enableActionConsequences) {
        return currentState.clone();
    }
    
    std::vector<float> predictedFeatures;
    float combinedConfidence = 0.0f;
    
    // 1. Temporal prediction (what will come next)
    if (pImpl->enableTemporalPrediction && pImpl->neuralPredictor) {
        // Convert current state to feature vector
        const auto& currentData = currentState.getData();
        
        // Generate neural prediction
        std::vector<float> neuralPrediction = pImpl->neuralPredictor->generatePrediction(0);
        
        if (!neuralPrediction.empty()) {
            predictedFeatures = neuralPrediction;
            combinedConfidence += pImpl->neuralPredictor->getPredictionConfidence() * 
                                pImpl->temporalPredictionWeight;
            
            NLM_LOG_INFO("Temporal prediction successful, confidence: " + 
                        std::to_string(pImpl->neuralPredictor->getPredictionConfidence()));
        }
    }
    
    // 2. Action consequence prediction (what happens after actions)
    if (pImpl->enableActionConsequences && pImpl->actionPredictor) {
        // For now, use current state as baseline and improve with action predictions
        if (predictedFeatures.empty()) {
            predictedFeatures = currentData;
        }
        
        // Get action prediction confidence
        float actionConfidence = pImpl->actionPredictor->getConsequenceConfidence(
            ActionType::MoveForward, currentData);
        
        combinedConfidence += actionConfidence * pImpl->actionPredictionWeight;
    }
    
    // If we have no predictions, return current state
    if (predictedFeatures.empty()) {
        return currentState.clone();
    }
    
    // Update confidence
    pImpl->confidence = combinedConfidence;
    
    // Create new sensory input with predicted features
    // For now, create a simple Vision with predicted features
    // In real implementation, would have proper type inference
    auto predictedInput = std::make_unique<Vision>();
    predictedInput->setData(predictedFeatures);
    
    NLM_LOG_INFO("Prediction completed with confidence: " + std::to_string(combinedConfidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update predictions based on actual observations
    // This involves learning from prediction errors and adjusting neural connections
    
    // 1. Update neural prediction system
    if (pImpl->neuralPredictor) {
        // Convert actual state to feature vector
        const auto& actualData = actual.getData();
        
        // Generate prediction for comparison
        auto predictedClone = predicted.clone();
        const auto& predictedData = predictedClone->getData();
        
        // Compute prediction error
        float error = pImpl->errorSignal->computeError(predictedData, actualData);
        pImpl->predictionError = error;
        pImpl->errorSignal->recordError(error, 0);
        
        // Update neural prediction with actual observation
        pImpl->neuralPredictor->updateWithObservation(actualData, 0);
    }
    
    // 2. Update action consequence prediction
    if (pImpl->actionPredictor) {
        // For now, we don't have action history in this interface
        // In real implementation, this would record the prediction-actual pair
    }
    
    // Update error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_INFO("Predictions updated, error: " + std::to_string(pImpl->predictionError));
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
    if (pImpl->actionPredictor) {
        pImpl->actionPredictor->clear();
    }
    if (pImpl->errorSignal) {
        // No clear method in error signal
    }
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using the observed sensory input
    // This involves learning temporal sequences and action consequences
    
    if (!pImpl->neuralPredictor) return;
    
    // Convert observation to feature vector
    const auto& obsData = observation.getData();
    
    // Record this state for temporal sequence learning
    pImpl->neuralPredictor->recordSensoryState(obsData, 0);
    
    NLM_LOG_INFO("Prediction model trained with observation: " + 
                std::to_string(obsData.size()) + " features");
}

} // namespace nlm
