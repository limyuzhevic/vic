#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

PredictionSystem::PredictionSystem() 
    : pImpl(new Impl)
    , neuralPrediction_(new NeuralPrediction())
    , actionConsequence_(new ActionConsequencePredictor())
    , errorSignal_(new PredictionErrorSignal())
    , brain_(nullptr)
{
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural components with brain reference
    neuralPrediction_->initialize(brain);
    actionConsequence_->initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized with NeuralPrediction backend");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Delegate to NeuralPrediction for advanced temporal prediction
    if (!neuralPrediction_) {
        return currentState.clone();
    }
    
    // Get current sensory state as vector
    const auto& currentData = currentState.getData();
    
    // Generate prediction using neural substrate
    std::vector<float> predictedVector = neuralPrediction_->generatePrediction(
        pImpl->brain ? pImpl->brain->getCurrentStep() : 0);
    
    if (predictedVector.empty()) {
        // Fallback to current state
        return currentState.clone();
    }
    
    // Create new SensoryInput with predicted values
    // Note: This assumes SensoryInput can be constructed from vector
    auto predictedInput = std::make_unique<SensoryInput>(predictedVector);
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Delegate to NeuralPrediction for learning and error computation
    if (!neuralPrediction_) return;
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using NeuralPrediction
        float error = neuralPrediction_->updateWithObservation(actualData,
                                                              pImpl->brain ? 
                                                              pImpl->brain->getCurrentStep() : 0);
        
        // Also update error signal for neuromodulation
        if (errorSignal_) {
            errorSignal_->recordError(error, pImpl->brain ? 
                                          pImpl->brain->getCurrentStep() : 0);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    if (neuralPrediction_) {
        return neuralPrediction_->getPredictionError();
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
    static std::vector<float> emptyHistory;
    if (neuralPrediction_) {
        return neuralPrediction_->getErrorHistory();
    }
    return emptyHistory;
}

void PredictionSystem::clearHistory() {
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
    if (errorSignal_) {
        // Clear error signal history (access private member)
        errorSignal_->getErrorHistory().clear();
    }
}

void PredictionSystem::train(const std::vector<float>& sensoryState) {
    // Record sensory state using NeuralPrediction
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(sensoryState,
                                            pImpl->brain ? 
                                            pImpl->brain->getCurrentStep() : 0);
    }
}

std::vector<std::vector<float>> PredictionSystem::predictMultipleSteps(size_t numSteps) {
    if (!neuralPrediction_) return std::vector<std::vector<float>>();
    
    return neuralPrediction_->predictMultipleSteps(
        pImpl->brain ? pImpl->brain->getCurrentStep() : 0,
        numSteps);
}

void PredictionSystem::recordAction(ActionType action) {
    if (neuralPrediction_) {
        neuralPrediction_->recordAction(action,
                                       pImpl->brain ? 
                                       pImpl->brain->getCurrentStep() : 0);
    }
    
    if (actionConsequence_) {
        actionConsequence_->recordAction(action);
    }
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

const std::vector<std::vector<float>>& PredictionSystem::getRecentSensoryStates() const {
    static std::vector<std::vector<float>> emptyStates;
    if (neuralPrediction_) {
        return neuralPrediction_->getRecentSensoryStates();
    }
    return emptyStates;
}

} // namespace nlm
