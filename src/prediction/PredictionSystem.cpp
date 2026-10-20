#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

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

// Integration methods for brain loop
float PredictionSystem::updateWithObservation(const std::vector<float>& sensoryState, 
                                             SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = predictNextState(SensoryInput(sensoryState));
    
    // Compute error
    if (!predicted || predicted->getData().empty()) {
        pImpl->predictionError = 0.0f;
    } else {
        updatePredictions(*predicted, SensoryInput(sensoryState));
        pImpl->predictionError = getPredictionError();
    }
    
    // Record the state for future learning
    recordSensoryState(sensoryState, currentStep);
    
    return pImpl->predictionError;
}

void PredictionSystem::recordSensoryState(const std::vector<float>& state, SimulationStep step) {
    pImpl->errorHistory.push_back(state.empty() ? 0.0f : 1.0f);
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

std::unique_ptr<std::vector<float>> PredictionSystem::getRecentSensoryState(SimulationStep stepsAgo) const {
    if (stepsAgo >= pImpl->errorHistory.size()) {
        return nullptr;
    }
    
    size_t idx = pImpl->errorHistory.size() - 1 - stepsAgo;
    if (idx < pImpl->errorHistory.size()) {
        auto result = std::make_unique<std::vector<float>>();
        result->push_back(pImpl->errorHistory[idx]);
        return result;
    }
    
    return nullptr;
}

void PredictionSystem::learnSequence(const std::vector<float>& fromState,
                                   const std::vector<float>& toState,
                                   SimulationStep currentStep) {
    // Simple sequence learning - in real implementation, this would use neural patterns
    // For now, just record that we learned this sequence
    recordSensoryState(toState, currentStep);
}

void PredictionSystem::updatePredictionsWithEpisode(const EpisodicMemoryItem& episode) {
    // Update prediction model based on episodic memory
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        updatePredictions(SensoryInput(episode.sensoryState), 
                         SensoryInput(episode.resultingSensoryState));
    }
}

} // namespace nlm
