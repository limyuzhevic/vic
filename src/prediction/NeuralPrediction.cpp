#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralPrediction::Impl {
    float sequenceSmoothing;      // How much to smooth sequence learning
    float confidenceThreshold;    // Minimum confidence for reliable predictions
    float predictionDecayRate;    // How predictions decay over time
    float temporalLearningRate;   // Learning rate for temporal sequences
    
    Impl() : sequenceSmoothing(0.9f), confidenceThreshold(0.8f), predictionDecayRate(0.95f), temporalLearningRate(0.01f) {}
};

NeuralPrediction::NeuralPrediction() : pImpl(new Impl), predictionError_(0.0f), predictionConfidence_(0.0f),
    temporalPredictionEnabled_(true), actionConsequenceEnabled_(true), sequenceLearningEnabled_(true),
    sequenceMemorySize_(100), predictionHorizon_(5) {}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    brain_ = brain;
    actionConsequencePredictor_ = std::make_unique<ActionConsequencePredictor>();
    predictionErrorSignal_ = std::make_unique<PredictionErrorSignal>();
    
    actionConsequencePredictor_->initialize(brain);
    predictionErrorSignal_->initialize(brain);
    
    NLM_LOG_INFO("NeuralPrediction initialized with brain reference");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || sensoryState.empty()) {
        return;
    }
    
    // Add to sequence memory
    sensorySequence_.push_back(sensoryState);
    
    if (sensorySequence_.size() > sequenceMemorySize_) {
        sensorySequence_.erase(sensorySequence_.begin());
    }
    
    NLM_LOG_TRACE("Recorded sensory state at step " + std::to_string(currentStep));
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    std::vector<float> prediction;
    
    if (!temporalPredictionEnabled_ || sensorySequence_.empty()) {
        return prediction;
    }
    
    // Simple prediction: use last few states to predict next
    size_t lookback = std::min(sensorySequence_.size(), 5);
    
    if (lookback == 0) return prediction;
    
    prediction.resize(sensorySequence_.front().size(), 0.0f);
    
    // Average last few states
    float weightSum = 0.0f;
    for (size_t i = 1; i <= lookback; ++i) {
        const auto& state = sensorySequence_.at(sensorySequence_.size() - i);
        for (size_t j = 0; j < state.size(); ++j) {
            prediction[j] += state[j] * 1.0f;
        }
        weightSum += 1.0f;
    }
    
    // Normalize prediction
    for (float& val : prediction) {
        val /= weightSum;
    }
    
    NLM_LOG_TRACE("Generated prediction at step " + std::to_string(currentStep));
    
    return prediction;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState, 
                                            SimulationStep currentStep) {
    float error = 0.0f;
    
    if (!actualState.empty() && predictionErrorSignal_) {
        error = predictionErrorSignal_->computeError(std::vector<float>(), actualState);
        predictionError_ = error;
        
        errorHistory_.push_back(error);
        if (errorHistory_.size() > 100) {
            errorHistory_.erase(errorHistory_.begin());
        }
        
        // Update confidence
        updatePredictionConfidence(error);
        
        // Apply error to learning
        applyPredictionErrorToLearning(error, currentStep);
        
        NLM_LOG_INFO("Prediction updated with actual state, error=" + std::to_string(error));
    }
    
    return error;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                             const std::vector<float>& currentState) {
    std::vector<float> consequence;
    
    if (actionConsequenceEnabled_ && action != ActionType::Wait && actionConsequencePredictor_) {
        consequence = actionConsequencePredictor_->predictConsequence(action, currentState);
        
        NLM_LOG_TRACE("Predicted consequence for action " + std::to_string(static_cast<int>(action)));
    }
    
    return consequence;
}

void NeuralPrediction::updatePredictionConfidence(float actualError) {
    // Confidence based on error consistency
    if (errorHistory_.size() > 1) {
        float avgError = 0.0f;
        for (float err : errorHistory_) {
            avgError += err;
        }
        avgError /= static_cast<float>(errorHistory_.size());
        
        // Lower confidence for higher average error
        predictionConfidence_ = std::max(0.0f, 1.0f - avgError);
        predictionConfidence_ = std::min(1.0f, predictionConfidence_);
    }
}

void NeuralPrediction::applyPredictionErrorToLearning(float error, SimulationStep step) {
    if (predictionErrorSignal_) {
        // Record error for history
        predictionErrorSignal_->recordError(error, step);
        
        // Apply error to neuromodulation
        float dopamineLevel = 0.0f, acetylcholineLevel = 0.0f, norepinephrineLevel = 0.0f;
        predictionErrorSignal_->applyToNeuromodulation(dopamineLevel, acetylcholineLevel, norepinephrineLevel);
        
        NLM_LOG_TRACE("Applied prediction error to learning at step " + std::to_string(step));
    }
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                 size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    if (!temporalPredictionEnabled_ || sensorySequence_.empty()) {
        return predictions;
    }
    
    for (size_t i = 0; i < numSteps; ++i) {
        predictions.push_back(generatePrediction(currentStep + i));
    }
    
    return predictions;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    if (actionConsequenceEnabled_ && actionConsequencePredictor_) {
        actionConsequencePredictor_->recordExperience(action, std::vector<float>(), std::vector<float>(), 0.0f);
        
        NLM_LOG_TRACE("Recorded action at step " + std::to_string(step));
    }
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    predictionError_ = 0.0f;
    predictionConfidence_ = 0.0f;
    NLM_LOG_INFO("NeuralPrediction history cleared");
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> neurons;
    
    if (brain_) {
        // Get neurons involved in prediction
        for (const auto& region : brain_->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                // Check if neuron is part of prediction system
                // For now, return all neurons as a simple implementation
                neurons.push_back(neuron->getId());
            }
        }
    }
    
    return neurons;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    return getPredictionNeurons();  // Simplified implementation
}

} // namespace nlm

