#include "PredictionError.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// PredictionError implementation
struct PredictionError::Impl {
    float currentError;
    float prediction;
    float accumulatedError;
    float learningRate;
    float timeConstant;
    
    // Error dynamics
    float errorGain;        // Scaling factor for prediction error
    float errorThreshold;   // Threshold for significant error
    float errorDecay;       // Rate of error decay towards prediction
    
    // Statistics
    float maxErrorObserved;    // Maximum error observed
    float minErrorObserved;    // Minimum error observed
    size_t errorEvents;        // Number of error events processed
    float totalErrorSquared;   // Sum of squared errors (for variance calculation)
    
    Impl() : currentError(0.0f), prediction(0.0f), accumulatedError(0.0f),
             learningRate(0.1f), timeConstant(50.0f),
             errorGain(1.0f), errorThreshold(0.1f), errorDecay(0.05f),
             maxErrorObserved(0.0f), minErrorObserved(0.0f), errorEvents(0),
             totalErrorSquared(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PE";
}

float PredictionError::getCurrentError() const {
    return pImpl->currentError;
}

float PredictionError::getPrediction() const {
    return pImpl->prediction;
}

float PredictionError::getAccumulatedError() const {
    return pImpl->accumulatedError;
}

float PredictionError::getLearningRate() const {
    return pImpl->learningRate;
}

float PredictionError::getTimeConstant() const {
    return pImpl->timeConstant;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 1.0f);
}

void PredictionError::setTimeConstant(float constant) {
    pImpl->timeConstant = std::clamp(constant, 1.0f, 200.0f);
}

void PredictionError::setErrorGain(float gain) {
    pImpl->errorGain = std::clamp(gain, 0.1f, 5.0f);
}

void PredictionError::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = std::clamp(threshold, 0.01f, 1.0f);
}

void PredictionError::update(float actualReward, float predictedReward, TimestepDuration dt) {
    // Calculate prediction error (delta rule)
    float error = actualReward - predictedReward;
    
    // Update current error
    pImpl->currentError = error * pImpl->errorGain;
    
    // Apply exponential smoothing for prediction
    float predictionUpdate = error * pImpl->learningRate;
    pImpl->prediction += predictionUpdate;
    
    // Update accumulated error (for statistics)
    pImpl->accumulatedError += std::abs(error);
    
    // Update statistics
    pImpl->totalErrorSquared += error * error;
    if (pImpl->errorEvents == 0) {
        pImpl->maxErrorObserved = std::abs(error);
        pImpl->minErrorObserved = std::abs(error);
    } else {
        pImpl->maxErrorObserved = std::max(pImpl->maxErrorObserved, std::abs(error));
        pImpl->minErrorObserved = std::min(pImpl->minErrorObserved, std::abs(error));
    }
    pImpl->errorEvents++;
    
    // Decay prediction error over time
    float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->timeConstant);
    pImpl->prediction *= decayFactor;
}

void PredictionError::reset() {
    pImpl->currentError = 0.0f;
    pImpl->prediction = 0.0f;
    pImpl->accumulatedError = 0.0f;
    pImpl->maxErrorObserved = 0.0f;
    pImpl->minErrorObserved = 0.0f;
    pImpl->errorEvents = 0;
    pImpl->totalErrorSquared = 0.0f;
}

float PredictionError::getVariance() const {
    if (pImpl->errorEvents <= 1) {
        return 0.0f;
    }
    return pImpl->totalErrorSquared / static_cast<float>(pImpl->errorEvents) -
           (pImpl->accumulatedError / static_cast<float>(pImpl->errorEvents)) * 
           (pImpl->accumulatedError / static_cast<float>(pImpl->errorEvents));
}

float PredictionError::getMaxError() const {
    return pImpl->maxErrorObserved;
}

float PredictionError::getMinError() const {
    return pImpl->minErrorObserved;
}

size_t PredictionError::getErrorEvents() const {
    return pImpl->errorEvents;
}

float PredictionError::getMeanError() const {
    return pImpl->errorEvents > 0 ? 
           pImpl->accumulatedError / static_cast<float>(pImpl->errorEvents) : 0.0f;
}

} // namespace nlm
