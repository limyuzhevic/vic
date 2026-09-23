#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
}

float PredictionError::getLevel() const {
    return pImpl->error;
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error affects learning - higher error increases plasticity
    // This is the classic error-driven learning signal
    return 0.5f + 0.5f * std::abs(pImpl->error);
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error decays over time (for stability)
    pImpl->error = pImpl->error * std::exp(-0.1f * static_cast<float>(dt));
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    // Update error based on new prediction
    computeError(newPrediction, pImpl->actualValue);
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

} // namespace nlm
