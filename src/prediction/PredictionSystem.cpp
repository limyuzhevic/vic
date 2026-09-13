#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize neural prediction components
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    actionPredictor_ = std::make_unique<ActionConsequencePredictor>();
    errorSignal_ = std::make_unique<PredictionErrorSignal>();
    
    NLM_LOG_INFO("PredictionSystem initialized with neural prediction components");
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    if (neuralPrediction_) neuralPrediction_->initialize(brain);
    if (actionPredictor_) actionPredictor_->initialize(brain);
    NLM_LOG_INFO("PredictionSystem integrated with brain");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using neural substrate
    // Convert SensoryInput to vector for neural processing
    const auto& sensoryData = currentState.getData();
    
    // Use neural prediction to generate prediction
    if (neuralPrediction_) {
        // Record current state for learning and prediction
        neuralPrediction_->recordSensoryState(sensoryData, 0);
        
        // Generate prediction using learned temporal sequences
        auto predictionVector = neuralPrediction_->generatePrediction(0);
        
        // If prediction is empty, use current state as baseline
        if (predictionVector.empty()) {
            predictionVector = sensoryData;
        }
        
        // Create new sensory input based on prediction
        std::unique_ptr<SensoryInput> predictedInput = currentState.clone();
        
        // Convert back to appropriate sensory type based on data characteristics
        // For simplicity, create InternalSignals with predicted values
        auto* internalSignals = new InternalSignals();
        for (float value : predictionVector) {
            internalSignals->addSignal(value);
        }
        
        NLM_LOG_INFO("PredictionSystem: Generated prediction using neural substrate");
        return std::unique_ptr<SensoryInput>(internalSignals);
    }
    
    // Fallback: return copy of current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Real prediction error computation using neural prediction components
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (!predictedData.empty() && !actualData.empty() && 
        predictedData.size() == actualData.size()) {
        
        // Use neural prediction error signal for accurate error computation
        if (errorSignal_) {
            pImpl->predictionError = errorSignal_->computeError(predictedData, actualData);
            errorSignal_->recordError(pImpl->predictionError, 0);
        } else {
            // Fallback error computation
            float sumError = 0.0f;
            for (size_t i = 0; i < predictedData.size(); ++i) {
                float diff = predictedData[i] - actualData[i];
                sumError += diff * diff;
            }
            pImpl->predictionError = sumError / predictedData.size();
        }
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Train neural prediction with error signal for learning
        if (neuralPrediction_) {
            neuralPrediction_->updateWithObservation(actualData, 0);
        }
        
        NLM_LOG_INFO("PredictionSystem: Updated predictions and computed error");
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
    if (neuralPrediction_) neuralPrediction_->clearHistory();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using neural learning
    const auto& obsData = observation.getData();
    
    if (neuralPrediction_) {
        // Use neural prediction to learn temporal sequences and action consequences
        neuralPrediction_->recordSensoryState(obsData, 0);
        
        // Train action consequence predictor with current observation
        if (actionPredictor_) {
            // This would need to be connected to the actual action that led to this observation
            // For now, we'll just record the observation as a state transition
            actionPredictor_->recordExperience(
                ActionType::Wait, 
                std::vector<float>(), 
                obsData, 
                0.0f
            );
        }
        
        NLM_LOG_INFO("PredictionSystem: Trained neural prediction model");
    }
}

} // namespace nlm
