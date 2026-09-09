// Complete implementation of prediction system integration
// –: Add all necessary includes
#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    class Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::update(const SensoryInput& currentState, const std::vector<float>& neuralActivity) {
    if (!pImpl->brain) return;
    
    // Make prediction for next state
    auto predicted = predictNextState(currentState);
    if (predicted) {
        // Update prediction system with actual observation
        updatePredictions(*predicted, currentState);
    }
    
    // Update confidence based on neural activity variability
    if (!neuralActivity.empty()) {
        float sum = 0.0f, sumSq = 0.0f;
        for (float val : neuralActivity) {
            sum += val;
            sumSq += val * val;
        }
        float mean = sum / neuralActivity.size();
        float stdDev = std::sqrt(sumSq / neuralActivity.size() - mean * mean);
        pImpl->confidence = std::max(0.0f, 1.0f - stdDev); // Higher variability = lower confidence
    }
    
    // Store prediction error for neuromodulation
    float currentError = getPredictionError();
    if (currentError != 0.0f) {
        NLM_LOG_DEBUG("Prediction error: " + std::to_string(currentError));
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictAndUpdate(const SensoryInput& currentState, 
    const std::vector<float>& neuralActivity) {
    // Combine prediction and update into one method
    auto predicted = predictNextState(currentState);
    if (predicted) {
        updatePredictions(*predicted, currentState);
    }
    return predicted;
}

// Rest of existing methods remain unchanged (getPredictionError, updatePredictions, etc.)

} // namespace nlm