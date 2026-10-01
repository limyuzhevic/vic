#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction engine for detailed predictive capabilities
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    std::unique_ptr<PredictionErrorSignal> errorSignal;
    
    Impl() : neuralPrediction(nullptr), actionConsequencePredictor(nullptr), errorSignal(nullptr) {}
};

PredictionSystem::PredictionSystem()
    : pImpl(new Impl)
    , brain_(nullptr)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , predictionHorizon_(1)
    , sequenceMemorySize_(10)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , predictionCount_(0)
    , trainingCount_(0)
{
    NLM_LOG_INFO("PredictionSystem created (Phase 6 integration)");
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural prediction components
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->actionConsequencePredictor = std::make_unique<ActionConsequencePredictor>();
    pImpl->errorSignal = std::make_unique<PredictionErrorSignal>();
    
    pImpl->neuralPrediction->initialize(brain);
    pImpl->actionConsequencePredictor->initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized with neural prediction integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    predictionCount_++;
    
    if (!temporalPredictionEnabled_) {
        return currentState.clone();
    }
    
    // Use neural prediction for advanced prediction
    std::vector<float> currentData = currentState.getData();
    
    // Generate prediction using neural prediction system
    auto predictedData = pImpl->neuralPrediction->generatePrediction(0);
    
    if (predictedData.empty()) {
        // Fallback to current state
        return currentState.clone();
    }
    
    // Create new sensory input with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // Update confidence based on prediction quality
    predictionConfidence_ = pImpl->neuralPrediction->getPredictionConfidence();
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                        const SensoryInput& actual) {
    // Convert to vector format for neural prediction
    std::vector<float> predictedData = predicted.getData();
    std::vector<float> actualData = actual.getData();
    
    // Update neural prediction with actual observation
    float error = pImpl->neuralPrediction->updateWithObservation(actualData, 0);
    
    // Update action consequence predictor
    pImpl->actionConsequencePredictor->updatePrediction(
        ActionType::Wait, predictedData, actualData, 1.0f - error);
    
    // Update prediction error signal
    float neuromodulationSignal = pImpl->errorSignal->computeError(predictedData, actualData);
    pImpl->errorSignal->recordError(error, 0);
    
    // Update internal state
    predictionError_ = error;
    predictionConfidence_ = 1.0f - error;
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    // Apply neuromodulation signal to brain
    if (brain_) {
        pImpl->errorSignal->getModulationSignal();
    }
}

float PredictionSystem::getPredictionError() const {
    return predictionError_;
}

float PredictionSystem::getConfidence() const {
    return predictionConfidence_;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return errorHistory_;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    trainingCount_++;
    
    if (pImpl->neuralPrediction) {
        std::vector<float> obsData = observation.getData();
        pImpl->neuralPrediction->recordSensoryState(obsData, 0);
    }
}

std::vector<float> PredictionSystem::predictActionConsequence(
    ActionType action,
    const std::vector<float>& currentState) {
    
    if (!actionConsequenceEnabled_ || !pImpl->actionConsequencePredictor) {
        return currentState;
    }
    
    return pImpl->actionConsequencePredictor->predictConsequence(action, currentState);
}

std::vector<std::vector<float>> PredictionSystem::predictMultipleSteps(
    const std::vector<float>& currentState,
    size_t numSteps) {
    
    std::vector<std::vector<float>> predictions;
    
    if (pImpl->neuralPrediction) {
        // Record current state if needed
        if (predictions.empty() || predictions.back() != currentState) {
            // Use neural prediction system for sequence generation
            auto multiPred = pImpl->neuralPrediction->predictMultipleSteps(0, numSteps);
            return multiPred;
        }
    }
    
    return predictions;
}

void PredictionSystem::clearPredictions() {
    predictionCount_ = 0;
    trainingCount_ = 0;
    predictionError_ = 0.0f;
    predictionConfidence_ = 0.5f;
    
    clearHistory();
    
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
    if (pImpl->actionConsequencePredictor) {
        pImpl->actionConsequencePredictor->clear();
    }
}

} // namespace nlm
