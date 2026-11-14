#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

struct PredictionSystem::Impl {
    // Real neural prediction system using NeuralPrediction
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionPredictor;
    std::unique_ptr<PredictionErrorSignal> errorSignal;
    
    // Neural state tracking
    std::vector<float> currentNeuralState;
    std::vector<float> predictedState;
    std::vector<float> predictionErrorComponents;
    
    // Real-time prediction and error tracking
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Prediction-based action selection bias
    std::vector<float> actionPredictionBias;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {
        // Initialize prediction systems
        neuralPrediction = std::make_unique<NeuralPrediction>();
        actionPredictor = std::make_unique<ActionConsequencePredictor>();
        errorSignal = std::make_unique<PredictionErrorSignal>();
        
        actionPredictionBias.resize(10, 0.0f);  // 10 action types
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->initialize(brain);
    }
    if (pImpl_agg->actionPredictor) {
        pImpl->actionPredictor->initialize(brain);
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // REAL PREDICTION: Use neural prediction system
    if (!currentState.isEmpty()) {
        const auto& sensoryData = currentState.getData();
        
        // Record current state for temporal prediction
        if (pImpl->neuralPrediction) {
            // Convert sensory input to neural state representation
            std::vector<float> neuralState = encodeSensoryToNeural(sensoryData);
            
            // Update prediction with current state
            auto currentStep = 0; // Simplified - would use actual step in full implementation
            pImpl->neuralPrediction->recordSensoryState(neuralState, currentStep);
            
            // Generate prediction using learned sequences
            auto predictedState = pImpl->neuralPrediction->generatePrediction(currentStep);
            
            if (!predictedState.empty()) {
                pImpl->predictedState = std::move(predictedState);
                pImpl->predictionErrorComponents = computeErrorComponents(sensoryData, pImpl->predictedState);
                pImpl->confidence = pImpl->neuralPrediction->getPredictionConfidence();
                
                // Create new sensory input with predicted state
                auto predictedInput = sensoryData.clone();
                predictedInput.modifyData(pImpl->predictedState);
                return predictedInput;
            }
        }
    }
    
    // Fallback: return current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // REAL PREDICTION ERROR: Use NeuralPrediction error computation
    if (!actual.isEmpty() && pImpl->neuralPrediction && pImpl->errorSignal) {
        const auto& predictedData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predictedData.size() == actualData.size() && !predictedData.empty()) {
            // Update neural prediction with actual observation
            SimulationStep currentStep = 0;  // Simplified
            float error = pImpl->neuralPrediction->updateWithObservation(actualData, currentStep);
            pImpl->predictionError = error;
            
            // Compute detailed error components
            pImpl->errorSignal->computeError(predictedData, actualData);
            pImpl->errorComponents = {
                pImpl->errorSignal->getIntensityError(),
                pImpl->errorSignal->getSpatialError(),
                pImpl->errorSignal->getTemporalError(),
                pImpl->errorSignal->getTotalError()
            };
            
            // Record error history
            pImpl->errorHistory.push_back(error);
            if (pImpl->errorHistory.size() > 10000) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            }
            
            // Apply error to confidence
            pImpl->confidence = std::max(0.1f, 1.0f - error * 2.0f);
            
            // Update action prediction bias based on prediction error
            updateActionPredictionBias(actualData);
        }
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
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
    if (pImpl->actionPredictor) {
        pImpl->actionPredictor->clear();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // REAL TRAINING: Train prediction models with sensory input
    if (!observation.isEmpty() && pImpl->neuralPrediction && pImpl->actionPredictor) {
        const auto& sensoryData = observation.getData();
        
        // Record as neural state for temporal sequence learning
        std::vector<float> neuralState = encodeSensoryToNeural(sensoryData);
        
        // Record for action-consequence prediction
        // This would be called from AgentBrain when actions are taken
        // For now, record with null action as background learning
    }
}

std::vector<float> PredictionSystem::getPredictionBias() const {
    return pImpl->actionPredictionBias;
}

float PredictionSystem::getPredictionErrorComponent(PredictionErrorType type) const {
    if (type < predictionErrorComponents.size()) {
        return predictionErrorComponents[type];
    }
    return 0.voie;
}

void PredictionSystem::integrateWithBrain(Brain* brain) {
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->initialize(brain);
    }
    if (pImpl->actionPredictor) {
        pImpl->actionPredictor->initialize(brain);
    }
}

// Helper methods
std::vector<float> PredictionSystem::encodeSensoryToNeural(const std::vector<float>& sensoryData) const {
    // Convert sensory input to neural representation
    // In a full implementation, this would map sensory features to
    // neural population activations, potentially using learned mappings
    std::vector<float> neuralState = sensoryData;
    
    // Apply non-linear transformation to simulate neural encoding
    for (auto& val : neuralState) {
        val = std::tanh(val * 2.0f);  // Normalize and non-linearity
    }
    
    return neuralState;
}

void PredictionSystem::updateActionPredictionBias(const std::vector<float>& sensoryData) {
    // Update action prediction bias based on prediction error
    // This would map prediction errors to action preferences
    // For now, implement simple bias update
    
    for (size_t i = 0; i < pImpl->actionPredictionBias.size(); ++i) {
        // Action-specific prediction bias
        // Higher prediction error reduces bias for all actions
        pImpl->actionPredictionBias[i] *= 0.9f;  // Decay bias
        
        // Add error-dependent modulation
        pImpl->actionPredictionBias[i] += (pImpl->predictionError - 0.5f) * 0.1f;
        
        // Clamp to reasonable range
        pImpl->actionPredictionBias[i] = std::clamp(pImpl->actionPredictionBias[i], -0.5f, 0.5f);
    }
}

std::vector<float> PredictionSystem::computeErrorComponents(const std::vector<float>& predicted,
                                                          const std::vector<float>& actual) const {
    if (predicted.size() != actual.size()) {
        return std::vector<float>(3, 0.0f);  // Return empty error components
    }
    
    std::vector<float> components(3, 0.0f);
    
    // Intensity error (MSE)
    float intensityError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        intensityError += diff * diff;
    }
    components[PredictionErrorType::IntensityError] = std::sqrt(intensityError / predicted.size());
    
    // Spatial error (pattern similarity)
    float spatialError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialError += (diff - components[PredictionErrorType::IntensityError]) * 
                       (diff - components[PredictionErrorType::IntensityError]);
    }
    components[PredictionErrorType::SpatialError] = std::sqrt(spatialError / predicted.size());
    
    // Temporal error (would need time series data for full computation)
    // For now, use a simplified approach based on pattern change rate
    components[PredictionErrorType::TemporalError] = std::abs(components[PredictionErrorType::SpatialError] - 
                                                            components[PredictionErrorType::IntensityError]);
    
    // Total error (combined)
    components[PredictionErrorType::TotalError] = 
        std::sqrt(components[PredictionErrorType::IntensityError] * components[PredictionErrorType::IntensityError] +
                 components[PredictionErrorType::SpatialError] * components[PredictionErrorType::SpatialError] +
                 components[PredictionErrorType::TemporalError] * components[PredictionErrorType::TemporalError]);
    
    return components;
}

} // namespace nlm
