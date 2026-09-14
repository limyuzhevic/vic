#include "PredictionError.hpp"
#include "NeuromodulatorBase.hpp"
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

PredictionError::PredictionError() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

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

float PredictionError::getLevel() const {
    return currentLevel;
}

void PredictionError::setLevel(float level) {
    currentLevel = level;
}

float PredictionError::getPlasticityFactor() const {
    return std::abs(pImpl->error);
}

void PredictionError::update(TimestepDuration dt) {
    // Decay error magnitude using common helper
    currentLevel = computeDecay(currentLevel, 0.1f, dt);
}

NeuromodulatorType PredictionError::getType() const {
    return NeuromodulatorType::PredictionError;
}

void PredictionError::loadConfig(const std::string& key, float value) {
    // Configuration loading for PredictionError
}

} // namespace nlm
