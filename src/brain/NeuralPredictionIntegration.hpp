// Neural Prediction integration into Brain
// Completes the prediction system integration

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Forward declarations
class NeuralPrediction;

struct Brain::Impl {
    // Prediction system integration
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Prediction-related state
    std::vector<float> currentSensoryState;
    float predictionError;
    float predictionConfidence;
    SimulationStep predictionStep;
    
    // Action-consequence learning
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions;
    
    // Prediction history for analysis
    std::deque<float> predictionErrorHistory;
    
    Impl() : predictionError(0.0f), predictionConfidence(0.5f), predictionStep(0) {}
};

// Integrate NeuralPrediction with Brain
void Brain::integrateNeuralPrediction() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain not initialized");
        return;
    }
    
    try {
        pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
        pImpl->neuralPrediction->initialize(this);
        
        // Configure prediction
        pImpl->neuralPrediction->enableTemporalPrediction(true);
        pImpl->neuralPrediction->enableActionConsequencePrediction(true);
        pImpl->neuralPrediction->setSequenceMemorySize(100);
        pImpl->neuralPrediction->setPredictionHorizon(5);
        
        NLM_LOG_INFO("NeuralPrediction integrated with Brain");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to integrate NeuralPrediction: ") + e.what());
    }
}

// Update prediction based on sensory input
void Brain::updatePrediction(const std::vector<float>& sensoryInput) {
    if (!pImpl || !pImpl->neuralPrediction) {
        return;
    }
    
    try {
        // Record current sensory state
        pImpl->neuralPrediction->recordSensoryState(sensoryInput, pImpl->currentStep);
        
        // Generate prediction for next timestep
        std::vector<float> predicted = pImpl->neuralPrediction->generatePrediction(pImpl->currentStep);
        
        if (!predicted.empty()) {
            // Update prediction confidence
            pImpl->predictionConfidence = pImpl->neuralPrediction->getPredictionConfidence();
            pImpl->predictionError = pImpl->neuralPrediction->getPredictionError();
            
            // Store error in history
            pImpl->predictionErrorHistory.push_back(pImpl->predictionError);
            if (pImpl->predictionErrorHistory.size() > 1000) {
                pImpl->predictionErrorHistory.pop_front();
            }
            
            // Apply prediction-based neuromodulation
            applyPredictionBasedNeuromodulation();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating prediction: ") + e.what());
    }
}

// Record action for action-consequence learning
void Brain::recordActionForPrediction(ActionType action) {
    if (!pImpl || !pImpl->neuralPrediction) {
        return;
    }
    
    try {
        pImpl->recentActions.push_back({action, currentSensoryState_});
        
        // Let NeuralPrediction record the action
        pImpl->neuralPrediction->recordAction(action, pImpl->currentStep);
        
        // Limit history size
        if (pImpl->recentActions.size() > 100) {
            pImpl->recentActions.pop_front();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error recording action for prediction: ") + e.what());
    }
}

// Apply neuromodulation based on prediction error
void Brain::applyPredictionBasedNeuromodulation() {
    if (!pImpl) return;
    
    try {
        // Use prediction error to modulate neuromodulators
        float predictionBasedModulation = pImpl->predictionError;
        
        if (pImpl->dopamine) {
            // Dopamine increases with prediction error (prediction error signal)
            float currentLevel = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(currentLevel + predictionBasedModulation * 0.1f);
        }
        
        if (pImpl->curiosity) {
            // Curiosity increases when prediction is unreliable
            float curiosityLevel = pImpl->curiosity->getLevel();
            pImpl->curiosity->setLevel(curiosityLevel + (1.0f - pImpl->predictionConfidence) * 0.05f);
        }
        
        if (pImpl->predictionError) {
            // PredictionError itself provides a neuromodulatory signal
            pImpl->predictionError->recordError(pImpl->predictionError, pImpl->currentStep);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in prediction-based neuromodulation: ") + e.what());
    }
}

// Predict action consequences
std::vector<float> Brain::predictActionConsequence(ActionType action, const std::vector<float>& currentState) {
    if (!pImpl || !pImpl->neuralPrediction) {
        return currentState;
    }
    
    try {
        return pImpl->neuralPrediction->predictActionConsequence(action, currentState);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error predicting action consequence: ") + e.what());
        return currentState;
    }
}

// Get prediction statistics
void Brain::logPredictionStatus() const {
    if (!pImpl) return;
    
    NLM_LOG_INFO("=== Prediction System Status ===");
    NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionError));
    NLM_LOG_INFO("Prediction confidence: " + std::to_string(pImpl->predictionConfidence));
    NLM_LOG_INFO("Predictions generated: " + std::to_string(pImpl->predictionErrorHistory.size()));
    
    if (pImpl->neuralPrediction) {
        NLM_LOG_INFO("NeuralPrediction neurons: " + std::to_string(pImpl->neuralPrediction->getPredictionNeurons().size()));
        NLM_LOG_INFO("Sequence neurons: " + std::to_string(pImpl->neuralPrediction->getSequenceNeurons().size()));
    }
}

} // namespace nlm
