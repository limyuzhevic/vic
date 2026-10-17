#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr), confidence_(0.5f) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::update(const SensoryInput& input, TimestepDuration dt) {
    // Generate prediction for current sensory input
    auto predictedState = predictNextState(input);
    
    if (predictedState) {
        // Update predictions based on actual observation
        updatePredictions(*predictedState, input);
        
        // Train prediction model with new observation
        train(input);
    }
}

ActionType PredictionSystem::selectActionBasedOnPrediction(const std::vector<float>& currentState) {
    // Simple action selection based on prediction error
    // Lower prediction error -> more confident predictions -> better actions
    float error = getPredictionError();
    
    if (error < 0.1f) {
        return ActionType::MoveForward;  // Confident predictions, move forward
    } else if (error < 0.5f) {
        return ActionType::Wait;  // Moderate uncertainty, wait and observe
    } else {
        return ActionType::TurnLeft;  // High uncertainty, try new direction
    }
}

std::vector<float> PredictionSystem::getCurrentPredictions() const {
    // Return simplified prediction vector
    // In a full implementation, this would be more sophisticated
    return std::vector<float>(10, 0.5f);  // 10-dimensional prediction vector
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
}

// Update predictions based on actual observation
void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm