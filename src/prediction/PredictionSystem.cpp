#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction components
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionPredictor;
    std::unique_ptr<PredictionErrorSignal> errorSignal;
    
    // Current sensory state for comparison
    std::vector<float> currentSensoryState;
    SimulationStep currentStep;
    
    // Prediction and error tracking
    std::vector<float> predictedStates;
    std::vector<float> predictionErrors;
    float predictionAccuracy;
    float predictionConfidence;
    
    Impl() : currentStep(0), predictionAccuracy(0.5f), predictionConfidence(0.5f) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
        actionPredictor = std::make_unique<ActionConsequencePredictor>();
        errorSignal = std::make_unique<PredictionErrorSignal>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // STEP 1: Convert SensoryInput to vector format
    std::vector<float> currentStateData;
    const auto& currentData = currentState.getData();
    if (!currentData.empty()) {
        currentStateData = currentData;
    }
    
    // STEP 2: Use neural prediction system for forward model
    pImpl->currentSensoryState = currentStateData;
    
    // Get next action prediction from planner (if available)
    ActionType predictedAction = ActionType::Wait;
    if (pImpl->neuralPrediction && pImpl->neuralPrediction->isInitialized()) {
        // Use planner to suggest an action
        if (pImpl->brain) {
            auto action = pImpl->brain->produceAction();
            predictedAction = action->getType();
        } else {
            // Default prediction
            predictedAction = ActionType::Wait;
        }
    }
    
    // STEP 3: Generate predicted next state using neural forward model
    std::vector<float> predictedState;
    if (pImpl->neuralPrediction) {
        predictedState = pImpl->neuralPrediction->predictForward(
            currentStateData, predictedAction, 1.0f);
        
        // Store prediction
        pImpl->predictedStates.push_back(predictedState[0]); // Store first dimension for simplicity
        if (pImpl->predictedStates.size() > 100) {
            pImpl->predictedStates.erase(pImpl->predictedStates.begin());
        }
        
        // Update prediction confidence
        pImpl->predictionConfidence = pImpl->neuralPrediction->getPredictionConfidence();
    }
    
    // STEP 4: Convert predicted state back to SensoryInput
    if (!predictedState.empty()) {
        auto result = std::make_unique<SensoryInput>();
        result->setData(predictedState);
        return result;
    } else {
        // Return copy of current state as fallback
        return currentState.clone();
    }
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                       const SensoryInput& actual) {
    // STEP 2: Update prediction accuracy
    pImpl->predictionAccuracy = std::max(0.0f, 1.0f - error);
    pImpl->predictionErrors.push_back(error);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // STEP 3: Learn from prediction error using neural prediction
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->updateForwardModel(
            predictedData, actualData, ActionType::Wait);  // Action will be set when known
        
        // Apply prediction error to neural circuits
        pImpl->neuralPrediction->applyPredictionErrorToNeurons(error);
    }
    
    // STEP 4: Record action consequences if we have action information
    // This will be called from brain integration when we know the action
    
    NLM_LOG_DEBUG("Prediction error: " + std::to_string(error) +
                  ", accuracy: " + std::to_string(predictionAccuracy));
}

void PredictionSystem::storePrediction(const std::vector<float>& prediction) {
    // STEP 1: Store prediction for future comparison
    pImpl->predictedStates.push_back(prediction.size() > 0 ? prediction[0] : 0.0f);
    if (pImpl->predictedStates.size() > 100) {
        pImpl->predictedStates.erase(pImpl->predictedStates.begin());
    }
    
    NLM_LOG_DEBUG("Prediction system stored prediction: " + 
                  std::to_string(prediction.size() > 0 ? prediction[0] : 0.0f));
}

void PredictionSystem::clearPredictions() {
    pImpl->predictedStates.clear();
}

const std::vector<std::vector<float>>& PredictionSystem::getPredictions() const {
    // This is a simplified implementation - in a full version,
    // we would store complete prediction vectors
    static std::vector<std::vector<float>> empty;
    return empty;
}

    // STEP 1: Record experience for action consequence prediction
    if (pImpl->actionPredictor) {
        const auto& beforeData = before.getData();
        const auto& afterData = after.getData();
        
        if (!beforeData.empty() && !afterData.empty()) {
            pImpl->actionPredictor->recordExperience(action, beforeData, afterData, reward);
            
            // Use action predictor to get expected consequence
            std::vector<float> expectedConsequence = 
                pImpl->actionPredictor->predictConsequence(action, beforeData);
            
            // Compare with actual outcome
            float error = computeVectorError(expectedConsequence, afterData);
            
            // Update prediction system with action prediction
            if (pImpl->errorSignal) {
                pImpl->errorSignal->recordError(error, pImpl->currentStep);
            }
        }
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // STEP 1: Train prediction model from actual observation
    const auto& observationData = observation.getData();
    if (observationData.empty()) {
        return;
    }
    
    // STEP 2: Update neural prediction from real observation
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->trainFromExperience(observationData);
    }
    
    // STEP 3: Store observation for future prediction
    pImpl->currentSensoryState = observationData;
    pImpl->currentStep++;
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->predictionErrors.empty()) {
        return 0.0f;
    }
    return pImpl->predictionErrors.back();
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->predictionAccuracy;
}

float PredictionSystem::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

const std::vector<float>& PredictionSystem::getPredictionHistory() const {
    return pImpl->predictedStates;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    // This should return error signal history when implemented
    static std::vector<float> emptyHistory;
    if (pImpl->errorSignal) {
        // Access error signal history
        return pImpl->errorSignal->getErrorHistory();
    }
    return emptyHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->predictedStates.clear();
    pImpl->predictionErrors.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
    if (pImpl->actionPredictor) {
        pImpl->actionPredictor->clear();
    }
    if (pImpl->errorSignal) {
        pImpl->errorSignal->clearHistory();
    }
}

float PredictionSystem::computeVectorError(const std::vector<float>& a,
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 1.0f;
    }
    
    float sumSquared = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sumSquared += diff * diff;
    }
    
    return std::sqrt(sumSquared / a.size());
}

std::vector<float> PredictionSystem::getPredictedState() const {
    if (pImpl->predictedStates.empty()) {
        return std::vector<float>();
    }
    
    // Return the most recent prediction
    std::vector<float> result(1, pImpl->predictedStates.back());
    return result;
}

void PredictionSystem::setBrainReference(Brain* brain) {
    pImpl->brain = brain;
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->initialize(brain);
    }
    if (pImpl->actionPredictor) {
        pImpl->actionPredictor->initialize(brain);
    }
}

} // namespace nlm
