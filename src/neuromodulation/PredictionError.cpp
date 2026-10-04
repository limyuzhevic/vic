#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float learningRate;
    float predictionUncertainty;
    float adaptationRate;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             learningRate(0.1f), predictionUncertainty(0.1f), adaptationRate(0.05f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Compute prediction error using temporal difference learning
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Basic prediction error: difference between expected and actual
    pImpl->error = actual - predicted;
    
    // Apply uncertainty weighting - higher uncertainty reduces error impact
    float uncertaintyWeight = 1.0f / std::max(0.01f, 1.0f - pImpl->predictionUncertainty);
    pImpl->error *= uncertaintyWeight;
    
    // Apply learning rate to smooth error signal
    pImpl->error = pImpl->error * pImpl->learningRate;
    
    // Store in history for temporal dynamics
    pImpl->history.push_back(pImpl->error);
    
    // Keep history bounded
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction with new information
    pImpl->predictedValue = newPrediction;
    
    // Adapt prediction uncertainty based on prediction error
    if (std::abs(pImpl->error) > 0.1f) {
        // Large prediction errors increase uncertainty
        pImpl->predictionUncertainty = std::min(1.0f, 
            pImpl->predictionUncertainty + pImpl->adaptationRate * std::abs(pImpl->error));
    } else {
        // Small errors decrease uncertainty
        pImpl->predictionUncertainty = std::max(0.0f,
            pImpl->predictionUncertainty - pImpl->adaptationRate * 0.1f);
    }
    
    // Reset error after prediction update
    pImpl->error = 0.0f;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getPredictionUncertainty() const {
    return pImpl->predictionUncertainty;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void PredictionError::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 1.0f);
}

float PredictionError::getRecentError(size_t numSamples) const {
    if (pImpl->history.empty()) return 0.0f;
    
    size_t samples = std::min(numSamples, pImpl->history.size());
    if (samples == 0) return 0.0f;
    
    float sum = std::accumulate(pImpl->history.end() - samples, pImpl->history.end(), 0.0f);
    return sum / samples;
}

} // namespace nlm
