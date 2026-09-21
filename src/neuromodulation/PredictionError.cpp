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
    float decayRate;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f), decayRate(0.1f) {}
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
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
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

const char* PredictionError::getName() const {
    return "PE";
}

float PredictionError::getLevel() const {
    return pImpl->error;
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
}

float PredictionError::getPlasticityFactor() const {
    return 0.5f + 0.5f * std::abs(pImpl->error);
}

void PredictionError::update(TimestepDuration dt) {
    pImpl->error = pImpl->error * std::max(0.0f, 1.0f - pImpl->decayRate * static_cast<float>(dt));
}

} // namespace nlm