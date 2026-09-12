#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/ActionConsequencePredictor.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Brain reference for integration
    Brain* brain;
    
    // Integrated prediction components
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    
    // Create integrated prediction components
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    actionConsequencePredictor_ = std::make_unique<ActionConsequencePredictor>();
    predictionErrorSignal_ = std::make_unique<PredictionErrorSignal>();
    
    neuralPrediction_->initialize(brain);
    actionConsequencePredictor_->initialize(brain);
    
    // Register prediction error signal with brain
    if (brain) {
        brain->registerPredictionErrorHandler([this](float error) {
            if (pImpl->predictionErrorSignal) {
                pImpl->predictionErrorSignal->recordError(error, 0);
            }
        });
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural prediction system for advanced predictions
    if (neuralPrediction_) {
        // Convert sensory input to vector representation for prediction
        const auto& data = currentState.getData();
        auto predicted = neuralPrediction_->generatePrediction(0);
        
        // Create a new vision input with predicted data
        auto predictedInput = currentState.clone();
        
        // Use prediction error signal for neuromodulation
        if (predictionErrorSignal_) {
            // This would normally compute error against actual observation
            // For now just return the current state as prediction
        }
        
        return predictedInput;
    }
    
    // Fallback: return current state as prediction
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Use neural prediction for sequence learning
    if (neuralPrediction_) {
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            // Compute prediction error
            float error = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                error += diff * diff;
            }
            error = std::sqrt(error / predData.size());
            
            pImpl->predictionError = error;
            pImpl->errorHistory.push_back(error);
            
            // Record in prediction error signal for neuromodulation
            if (predictionErrorSignal_) {
                predictionErrorSignal_->computeError(predData, actualData);
            }
            
            // Use error to modulate learning
            neuralPrediction_->updateWithObservation(actualData, 0);
        }
    }
}

void PredictionSystem::updateWithSensoryInput(const SensoryInput& currentInput) {
    // Convert current sensory input to vector for prediction system
    const auto& data = currentInput.getData();
    
    // Update neural prediction with current sensory state
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(data, 0);
    }
    
    // Generate prediction for next state
    auto prediction = predictNextState(currentInput);
    
    // Store prediction for comparison
    if (prediction) {
        // Update neural prediction system with prediction
        if (neuralPrediction_) {
            const auto& predData = prediction->getData();
            const auto& actualData = currentInput.getData();
            
            // Compute and record prediction error
            neuralPrediction_->updateWithObservation(actualData, 0);
            
            // Update action consequence predictor if needed
            if (actionConsequencePredictor_) {
                // This would record the action that led to this state
                // For now just update with the prediction
                const auto& predVec = prediction->getData();
                const auto& actualVec = currentInput.getData();
                actionConsequencePredictor_->updatePrediction(
                    ActionType::Wait, predVec, actualVec, 1.0f - pImpl->predictionError);
            }
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(data, 0);
    }
}

const std::vector<NeuronId>& PredictionSystem::getPredictionNeurons() const {
    static std::vector<NeuronId> empty;
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionNeurons();
    }
    return empty;
}

const std::vector<NeuronId>& PredictionSystem::getSequenceNeurons() const {
    static std::vector<NeuronId> empty;
    if (neuralPrediction_) {
        return neuralPrediction_->getSequenceNeurons();
    }
    return empty;
}

} // namespace nlm
