#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<SensoryInput> lastObservation;
    
    Impl() : predictionError(0.0f), confidence(0.5f), lastObservation(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For Phase 6: Use neural substrate to generate predictions
    // For now: return a copy of current state as placeholder
    return currentState.clone();
}

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
    // TODO PHASE 2: Train prediction model using neural substrate
    // For Phase 6: Use neural substrate to generate predictions
    // For now: store observation as potential future state
    pImpl->lastObservation = observation.clone();
}

const SensoryInput* PredictionSystem::getLastObservation() const {
    return pImpl->lastObservation.get();
}

void PredictionSystem::applyPredictionToAttention() {
    // TODO PHASE 6: Apply predictions to attentional selection
    // Prediction error should bias attention toward unexpected sensory input
    // Current placeholder
}

void PredictionSystem::updatePrediction(const SensoryInput& sensoryInput) {
    // TODO PHASE 6: Integrate prediction with neural processing
    // This method updates the prediction system based on actual sensory input
    // Current placeholder
}

void PredictionSystem::generateActionPredictions(const std::vector<float>& internalState) {
    // TODO PHASE 6: Generate action predictions using neural substrate
    // Current placeholder
}

float PredictionSystem::getPredictionUncertainty() const {
    // Simple uncertainty estimate based on prediction error
    return std::min(1.0f, std::max(0.0f, pImpl->predictionError / 10.0f));
}

const SensoryInput* PredictionSystem::getExpectedInput() const {
    // TODO PHASE 6: Store and return expected input
    // Currently returns last observation as placeholder
    return pImpl->lastObservation.get();
}

void PredictionSystem::setExpectedInput(std::unique_ptr<SensoryInput> expected) {
    pImpl->lastObservation = std::move(expected);
}

void PredictionSystem::clearPredictionCache() {
    pImpl->lastObservation.reset();
    pImpl->errorHistory.clear();
    pImpl->predictionError = 0.0f;
}

} // namespace nlm