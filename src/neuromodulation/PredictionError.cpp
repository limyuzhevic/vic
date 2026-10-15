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
    float learningRate;
    float errorThreshold;
    float magnitude;
    
    Impl() 
        : brain(nullptr)
        , error(0.0f)
        , predictedValue(0.0f)
        , actualValue(0.0f)
        , learningRate(0.1f)
        , errorThreshold(0.1f)
        , magnitude(0.0f) {}
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

float PredictionError::getLevel() const {
    return getMagnitude();
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
    pImpl->magnitude = std::abs(level);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error modulates learning and plasticity
    float effectiveLevel = std::min(getMagnitude() / pImpl->errorThreshold, 1.0f);
    
    // Prediction error drives learning and adaptation
    float plasticityFactor = 1.0f + 1.5f * effectiveLevel;
    
    // Higher prediction error leads to more rapid learning
    plasticityFactor = std::min(plasticityFactor, 3.0f);
    
    return plasticityFactor;
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error updates continuously
    // For now, just maintain current state
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    pImpl->history.push_back(pImpl->magnitude);
    
    // Log significant prediction errors
    if (pImpl->magnitude > pImpl->errorThreshold) {
        NLM_LOG_INFO("Prediction error detected: magnitude=" + std::to_string(pImpl->magnitude));
    }
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
    return pImpl->magnitude;
}

float PredictionError::getLearningRate() const {
    return pImpl->learningRate;
}

float PredictionError::getErrorThreshold() const {
    return pImpl->errorThreshold;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void PredictionError::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = std::max(0.01f, threshold);
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->magnitude = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm