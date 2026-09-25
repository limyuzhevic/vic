#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized (Phase 2: Real Predictive Coding)");
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    neuralPrediction_.initialize(brain);
    actionConsequencePredictor_.initialize(brain);
    NLM_LOG_INFO("PredictionSystem fully initialized with neural prediction engine");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Extract sensory state for neural prediction
    const auto& sensoryData = currentState.getData();
    
    // Generate prediction using neural temporal dynamics
    auto predicted = neuralPrediction_.generatePrediction(pImpl->brain->getCurrentStep());
    
    // Convert back to SensoryInput format
    // For simplicity, we'll use InternalSignals which contains neural state
    auto predictedInput = std::make_unique<InternalSignals>();
    
    // Add predicted neural activity pattern
    for (float value : predicted) {
        predictedInput->addSignal(value);
    }
    
    // Update with observation to compute error
    updatePredictions(*predictedInput, currentState);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
        // Update neural prediction with actual observation to compute error
        float error = neuralPrediction_.updateWithObservation(actualData, pImpl->brain->getCurrentStep());
        
        // Record error for learning
        errorHistory_.push_back(error);
        if (errorHistory_.size() > 1000) {
            errorHistory_.erase(errorHistory_.begin());
        }
        
        // Connect prediction error to neuromodulation system
        pImpl->brain->getPredictionErrorSignal()->computeError(
            neuralPrediction_.getPredictionError(),
            neuralPrediction_.getPredictionConfidence()
        );
    }

float PredictionSystem::getPredictionError() const {
    return neuralPrediction_.getPredictionError();
}

float PredictionSystem::getConfidence() const {
    return neuralPrediction_.getPredictionConfidence();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return errorHistory_;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    neuralPrediction_.clearHistory();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    // Train the neural prediction system with new observations
    const auto& obsData = observation.getData();
    neuralPrediction_.recordSensoryState(obsData, pImpl->brain->getCurrentStep());
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action, const std::vector<float>& currentState) {
    return actionConsequencePredictor_.predictConsequence(action, currentState);
}

void PredictionSystem::recordAction(ActionType action) {
    actionConsequencePredictor_.recordAction(action, pImpl->brain->getCurrentStep());
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    return neuralPrediction_.getPredictionNeurons();
}

std::vector<NeuronId> PredictionSystem::getSequenceNeurons() const {
    return neuralPrediction_.getSequenceNeurons();
}

std::vector<float> PredictionSystem::getRecentConfidence(size_t window) const {
    std::vector<float> recent;
    size_t start = errorHistory_.size() > window ? errorHistory_.size() - window : 0;
    for (size_t i = start; i < errorHistory_.size(); ++i) {
        recent.push_back(1.0f - errorHistory_[i]);  // Convert error to confidence
    }
    return recent;
}

} // namespace nlm
