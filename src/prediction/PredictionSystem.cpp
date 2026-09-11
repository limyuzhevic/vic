#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (neuralPrediction_) {
        neuralPrediction_->initialize(brain);
    }
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::update(TimestepDuration dt) {
    if (!pImpl->brain || !neuralPrediction_) return;
    
    // Update neural prediction with current brain state
    neuralPrediction_->update(dt);
    
    // Update prediction system error and confidence
    pImpl->predictionError = neuralPrediction_->getPredictionError();
    pImpl->confidence = neuralPrediction_->getPredictionConfidence();
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain || !neuralPrediction_) {
        return currentState.clone();
    }
    
    // Use neural prediction to generate next state prediction
    auto currentData = currentState.getData();
    neuralPrediction_->recordSensoryState(currentData);
    auto prediction = neuralPrediction_->generatePrediction(pImpl->brain->getConfig()->getOr<size_t>("current_step", 0));
    
    // Create internal signals for prediction
    auto predictedInput = std::make_unique<InternalSignals>();
    for (float val : prediction) {
        predictedInput->addSignal(val);
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain || !neuralPrediction_) return;
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
    }
    
    // Update neural prediction with actual observation
    if (!actualData.empty()) {
        float error = neuralPrediction_->updateWithObservation(actualData, 
                                       pImpl->brain->getConfig()->getOr<size_t>("current_step", 0));
        pImpl->predictionError = error;
        pImpl->confidence = 1.0f - error;
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
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain || !neuralPrediction_) return;
    
    const auto& data = observation.getData();
    if (!data.empty()) {
        neuralPrediction_->recordSensoryState(data);
    }
}

std::vector<float> PredictionSystem::getCurrentPrediction() const {
    if (!neuralPrediction_) return std::vector<float>();
    return neuralPrediction_->generatePrediction(pImpl->brain->getConfig()->getOr<size_t>("current_step", 0));
}

float PredictionSystem::getPredictionErrorSignal() const {
    if (!pImpl->brain) return 0.0f;
    
    auto predictionError = pImpl->brain->getPredictionErrorSignal();
    if (predictionError) {
        return predictionError->getModulationSignal();
    }
    return 0.0f;
}

void PredictionSystem::recordSensoryState(const std::vector<float>& sensoryState) {
    if (neuralPrediction_) {
        neuralPrediction_->recordSensoryState(sensoryState, pImpl->brain->getConfig()->getOr<size_t>("current_step", 0));
    }
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!neuralPrediction_) return currentState;
    return neuralPrediction_->predictActionConsequence(action, currentState);
}

float PredictionSystem::updateWithObservation(const std::vector<float>& actualState) {
    if (!pImpl->brain || !neuralPrediction_) return 0.0f;
    return neuralPrediction_->updateWithObservation(actualState, 
                                   pImpl->brain->getConfig()->getOr<size_t>("current_step", 0));
}

} // namespace nlm
