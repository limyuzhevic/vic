#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::update() {
    if (!brain_) {
        return;
    }
    
    // Simple prediction error computation for now
    // This will be enhanced with real prediction capabilities
    float error = 0.0f;
    
    // If we have dopamine, compute prediction error
    // Dopamine level is inversely related to prediction error
    if (brain_->getDopamine()) {
        float dopamineLevel = brain_->getDopamine()->getLevel();
        // Higher dopamine means lower prediction error (better predictions)
        error = 1.0f - dopamineLevel;
    }
    
    // Store error in history for analysis
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Simple confidence modulation based on error
    if (error < 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.05f);
    } else if (error > 0.5f) {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.1f);
    }
}


std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
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
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm