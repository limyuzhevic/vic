#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

PredictionSystem::PredictionSystem() : brain_(nullptr), neuralPrediction_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain);
    NLM_LOG_INFO("PredictionSystem initialized with neural prediction engine");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!brain_ || !neuralPrediction_) {
        return currentState.clone();
    }
    
    // Convert current state to vector for prediction
    const auto& currentData = currentState.getData();
    
    // Get prediction from neural prediction engine
    std::vector<float> predictedVector;
    if (!neuralPrediction_->getRecentSensoryStates().empty()) {
        predictedVector = neuralPrediction_->generatePrediction(0);
    } else {
        predictedVector = currentData;
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedVector);
    
    return predictedInput;
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!brain_ || !neuralPrediction_) {
        return currentState; // No prediction, return current state
    }
    
    return neuralPrediction_->predictActionConsequence(action, currentState);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!neuralPrediction_) return;
    
    // Update predictions with actual observation
    neuralPrediction_->updateWithObservation(actual.getData(), 0);
}

float PredictionSystem::getPredictionError() const {
    if (!neuralPrediction_) return 0.0f;
    return neuralPrediction_->getPredictionError();
}

float PredictionSystem::getConfidence() const {
    if (!neuralPrediction_) return 0.0f;
    return neuralPrediction_->getPredictionConfidence();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    if (!neuralPrediction_) {
        static std::vector<float> empty;
        return empty;
    }
    return neuralPrediction_->getErrorHistory();
}

void PredictionSystem::clearHistory() {
    if (neuralPrediction_) {
        neuralPrediction_->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation, ActionType action, 
                            const SensoryInput& outcome, float reward) {
    if (!neuralPrediction_) return;
    
    // Train neural prediction with new experience
    neuralPrediction_->recordSensoryState(observation.getData(), 0);
    neuralPrediction_->recordAction(action, 0);
    neuralPrediction_->updateWithObservation(outcome.getData(), 0);
    
    NLM_LOG_INFO("Prediction system trained with new experience");
}

std::string PredictionSystem::getStatus() const {
    std::ostringstream status;
    status << "PredictionSystem: ";
    if (!brain_) {
        status << "NOT INITIALIZED";
    } else {
        status << "active, error=" << getPredictionError() 
               << ", confidence=" << getConfidence();
    }
    return status.str();
}

} // namespace nlm
