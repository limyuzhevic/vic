#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float predictionConfidence;
    std::deque<float> errorHistory;
    float lastPredictionError;
    
    Impl() : brain(nullptr), predictionError(0.0f), predictionConfidence(0.5f), lastPredictionError(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), neuralPrediction_(std::make_unique<NeuralPrediction>()) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (brain) {
        pImpl->brain = brain;
        neuralPrediction_->initialize(brain);
        NLM_LOG_INFO("PredictionSystem initialized with neural substrate");
    }
}

std::vector<float> PredictionSystem::predictNextState(const std::vector<float>& currentState,
                                                     SimulationStep currentStep) {
    if (!pImpl->brain || !neuralPrediction_) {
        return std::vector<float>();
    }
    
    // Use neural prediction system to generate prediction
    return neuralPrediction_->generatePrediction(currentStep);
}

float PredictionSystem::updateWithObservation(const std::vector<float>& actualState,
                                           SimulationStep currentStep) {
    if (!pImpl->brain || !neuralPrediction_) {
        return 0.0f;
    }
    
    // Record the actual sensory state
    neuralPrediction_->recordSensoryState(actualState, currentStep);
    
    // Compute prediction error
    float error = neuralPrediction_->updateWithObservation(actualState, currentStep);
    
    pImpl->predictionError = error;
    pImpl->predictionConfidence = 1.0f - error;
    pImpl->lastPredictionError = error;
    
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.pop_front();
    }
    
    return error;
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

const std::deque<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
}

void PredictionSystem::recordSensoryState(const std::vector<float>& sensoryState,
                                        SimulationStep currentStep) {
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(sensoryState, currentStep);
    }
}

void PredictionSystem::recordAction(ActionType action, SimulationStep step) {
    if (neuralPrediction_) {
        neuralPrediction_->recordAction(action, step);
    }
}

float PredictionSystem::getPredictionErrorSignal() const {
    // Convert prediction error to neuromodulatory signal
    return std::min(1.0f, pImpl->predictionError * 2.0f);
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionNeurons();
    }
    return std::vector<NeuronId>();
}

std::vector<NeuronId> PredictionSystem::getSequenceNeurons() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getSequenceNeurons();
    }
    return std::vector<NeuronId>();
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

} // namespace nlm
