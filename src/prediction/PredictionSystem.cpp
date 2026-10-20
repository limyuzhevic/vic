// PredictionSystem implementation
#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

PredictionSystem::PredictionSystem()
    : brain_(nullptr)
    , confidence_(0.5f)
    , lastUpdateStep_(0)
{
    // NeuralPrediction is already constructed in initializer list
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::updateWithInput(const std::vector<float>& sensoryInput, SimulationStep step) {
    lastUpdateStep_ = step;
    
    // Generate prediction based on current state
    if (!sensoryInput.empty()) {
        lastPredictedState_ = predictNextState(sensoryInput);
        
        // Update prediction error
        float error = updatePredictions(lastPredictedState_, sensoryInput, step);
        
        // Update confidence based on error
        confidence_ = std::max(0.1f, 1.0f - error);
        
        // Train the prediction model
        train(sensoryInput, step);
    }
}

std::vector<float> PredictionSystem::predictNextState(const std::vector<float>& currentState) {
    if (currentState.empty()) return {};
    
    // Simple prediction: apply neural transformation
    // In a real implementation, this would use learned weights and neural dynamics
    std::vector<float> prediction(currentState.size());
    
    for (size_t i = 0; i < currentState.size(); ++i) {
        // Apply temporal smoothing
        float smoothed = currentState[i] * 0.7f;
        if (i > 0) {
            smoothed += lastPredictedState_[i-1] * 0.3f;
        }
        
        // Add some prediction noise (learning component)
        float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
        prediction[i] = smoothed + noise;
        
        // Apply biological constraints
        prediction[i] = std::clamp(prediction[i], -10.0f, 10.0f);
    }
    
    return prediction;
}

float PredictionSystem::updatePredictions(const std::vector<float>& predicted,
                                        const std::vector<float>& actual,
                                        SimulationStep step) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 1.0f;  // Maximum error
    }
    
    float totalError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        totalError += std::abs(predicted[i] - actual[i]);
    }
    
    float averageError = totalError / predicted.size();
    
    // Update prediction error signal
    predictionErrorSignal_.recordError(averageError, step);
    
    // Apply prediction error to brain systems
    applyToBrain(brain_);
    
    return averageError;
}

void PredictionSystem::train(const std::vector<float>& observation, SimulationStep step) {
    // Train prediction model with new observation
    // This would involve updating weights in the neural prediction system
    neuralPrediction_.recordSensoryState(observation, step);
    
    // Record action if we have a brain with motor output
    if (brain_) {
        auto action = brain_->produceAction();
        if (action) {
            // Record action for action-consequence learning
            // This is a simplified version - real implementation would be more sophisticated
            neuralPrediction_.recordAction(action->getType(), step);
        }
    }
}

void PredictionSystem::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply prediction errors to brain plasticity
    float error = predictionErrorSignal_.getPredictionError();
    float modulation = error * 0.5f;  // Scale error for plasticity
    
    // Modulate STDP learning rates based on prediction error
    if (brain->getSTDP()) {
        float currentLTP = brain->getSTDP()->getLTPWeight();
        float currentLTD = brain->getSTDP()->getLTDWeight();
        
        // Reduce LTP when prediction error is high (avoid overfitting)
        // Enhance LTD when prediction error is high (encourage exploration)
        float ltpMod = std::max(0.001f, currentLTP - modulation * 0.3f);
        float ltdMod = std::max(0.001f, currentLTD + modulation * 0.2f);
        
        brain->getSTDP()->setLTPWeight(ltpMod);
        brain->getSTDP()->setLTDWeight(ltdMod);
    }
}

} // namespace nlm