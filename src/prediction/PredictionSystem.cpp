#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neuromodulation integration
    Dopamine* dopamine;
    Curiosity* curiosity;
    PredictionErrorSignal* predictionErrorSignal;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize with default values
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize neural prediction component
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain);
    
    // Initialize prediction error signal
    predictionErrorSignal_ = std::make_unique<PredictionErrorSignal>();
    predictionErrorSignal_->initialize(brain);
    
    // Get neuromodulation references
    if (brain) {
        pImpl->dopamine = brain->getDopamine();
        pImpl->curiosity = brain->getCuriosity();
        pImpl->predictionErrorSignal = brain->getPredictionErrorSignal();
    }
    
    NLM_LOG_INFO("PredictionSystem initialized with real neural forward models");
}

void PredictionSystem::enableTemporalPrediction(bool enable) {
    if (neuralPrediction_) {
        neuralPrediction_->enableTemporalPrediction(enable);
    }
}

void PredictionSystem::enableActionConsequencePrediction(bool enable) {
    if (neuralPrediction_) {
        neuralPrediction_->enableActionConsequencePrediction(enable);
    }
}

void PredictionSystem::setSequenceMemorySize(size_t size) {
    if (neuralPrediction_) {
        neuralPrediction_->setSequenceMemorySize(size);
    }
}

void PredictionSystem::setPredictionHorizon(size_t steps) {
    if (neuralPrediction_) {
        neuralPrediction_->setPredictionHorizon(steps);
    }
}

void PredictionSystem::setPredictionSimilarityThreshold(float threshold) {
    if (neuralPrediction_) {
        neuralPrediction_->setPredictionSimilarityThreshold(threshold);
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralPrediction_) {
        return currentState.clone();
    }
    
    // Get current state data
    const auto& data = currentState.getData();
    
    // Generate neural prediction
    auto prediction = neuralPrediction_->generatePrediction(pImpl->brain->getCurrentStep());
    
    // Convert prediction back to SensoryInput
    // This is a simplified implementation - in reality would create appropriate
    // sensory input type based on the original input type
    auto result = currentState.clone();
    result->setData(prediction);
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                        const SensoryInput& actual) {
    if (!neuralPrediction_ || !predictionErrorSignal_) return;
    
    // Get prediction and actual data
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error using neural prediction system
    float error = neuralPrediction_->updateWithObservation(actualData, 
                                                         pImpl->brain->getCurrentStep());
    
    // Update prediction error signal
    predictionErrorSignal_->computeError(predData, actualData);
    predictionErrorSignal_->recordError(error, pImpl->brain->getCurrentStep());
    
    // Update system state
    pImpl->predictionError = error;
    pImpl->confidence = neuralPrediction_->getPredictionConfidence();
    pImpl->errorHistory.push_back(error);
    
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Apply neuromodulation based on prediction error
    applyNeuromodulationEffects(error);
}

void PredictionSystem::applyNeuromodulationEffects(float predictionError) {
    if (!pImpl->dopamine || !pImpl->curiosity) return;
    
    // Dopamine modulates based on prediction error (reward prediction error)
    // Higher prediction error = more dopamine release
    float dopamineLevel = std::min(1.0f, predictionError * 3.0f);
    pImpl->dopamine->setLevel(dopamineLevel);
    
    // Curiosity modulates based on surprise (unexpected prediction error)
    float curiosityLevel = 0.0f;
    if (predictionErrorSignal_->isSurprising()) {
        curiosityLevel = predictionError * 2.0f;
    }
    pImpl->curiosity->setLevel(curiosityLevel);
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!neuralPrediction_) return;
    
    // Train neural prediction system with observation
    const auto& data = observation.getData();
    neuralPrediction_->recordSensoryState(data, pImpl->brain->getCurrentStep());
    
    // Update action-consequence prediction with current observation
    // This would be called when actions are taken
}

void PredictionSystem::recordAction(ActionType action, SimulationStep step) {
    if (neuralPrediction_) {
        neuralPrediction_->recordAction(action, step);
    }
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!neuralPrediction_) {
        return currentState;
    }
    
    return neuralPrediction_->predictActionConsequence(action, currentState);
}

std::vector<std::vector<float>> PredictionSystem::predictMultipleSteps(SimulationStep currentStep,
                                                                         size_t numSteps) {
    if (!neuralPrediction_) {
        return std::vector<std::vector<float>>();
    }
    
    return neuralPrediction_->predictMultipleSteps(currentStep, numSteps);
}

float PredictionSystem::getPredictionError() const {
    if (predictionErrorSignal_) {
        return predictionErrorSignal_->getErrorComponents().totalError;
    }
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionConfidence();
    }
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
    if (predictionErrorSignal_) {
        predictionErrorSignal_->clearHistory();
    }
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    if (!neuralPrediction_) {
        return std::vector<NeuronId>();
    }
    return neuralPrediction_->getPredictionNeurons();
}

std::vector<NeuronId> PredictionSystem::getSequenceNeurons() const {
    if (!neuralPrediction_) {
        return std::vector<NeuronId>();
    }
    return neuralPrediction_->getSequenceNeurons();
}

float PredictionSystem::getTemporalPredictionAccuracy() const {
    if (!neuralPrediction_) {
        return 0.5f;
    }
    return neuralPrediction_->getTemporalPredictionAccuracy();
}

float PredictionSystem::getActionConsequenceAccuracy() const {
    if (!neuralPrediction_) {
        return 0.5f;
    }
    return neuralPrediction_->getActionConsequenceAccuracy();
}

float PredictionSystem::getPredictionErrorMean() const {
    if (!predictionErrorSignal_) {
        return pImpl->predictionError;
    }
    return predictionErrorSignal_->getMeanError();
}

float PredictionSystem::getPredictionErrorVariance() const {
    if (!predictionErrorSignal_) {
        return 0.0f;
    }
    return predictionErrorSignal_->getErrorVariance();
}

void PredictionSystem::reset() {
    clearHistory();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
    
    NLM_LOG_INFO("PredictionSystem reset");
}

} // namespace nlm
