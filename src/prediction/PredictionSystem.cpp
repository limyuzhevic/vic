#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brainReference;
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 1.0f;
    pImpl->errorHistory.clear();
    pImpl->brainReference = nullptr;
}

PredictionSystem::~PredictionSystem() = default;

// Initialize prediction system with brain reference
void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    if (brain) {
        pImpl->brainReference = brain;
    }
    NLM_LOG_INFO("PredictionSystem initialized");
}

// Get brain reference (for integration)
Brain* PredictionSystem::getBrain() const {
    return brain_ ? brain_ : pImpl->brainReference;
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
        // Maintain history size
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
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
    // For now, just update confidence based on prediction error
    // Positive learning when error is low
    if (pImpl->predictionError < 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    }
    // Reduce confidence when error is high
    else if (pImpl->predictionError > 0.5f) {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.02f);
    }
}

} // namespace nlm