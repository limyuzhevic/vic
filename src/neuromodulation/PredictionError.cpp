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
    float errorThreshold;
    float sensitivity;
    TimestepDuration timeWindow;
    float errorSignificance;
    float neuromodulationFactor;
    TimestepDuration elapsedTime;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             errorThreshold(0.5f), sensitivity(1.0f), timeWindow(1.0f),
             errorSignificance(0.0f), neuromodulationFactor(0.0f), elapsedTime(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

void PredictionError::update(TimestepDuration dt) {
    pImpl->elapsedTime += dt;
    
    // Apply temporal decay to error significance
    if (pImpl->timeWindow > 0.0f) {
        float decayFactor = std::exp(-dt / pImpl->timeWindow);
        pImpl->errorSignificance *= decayFactor;
    }
    
    // Decay neuromodulation factor
    pImpl->neuromodulationFactor *= 0.95f;
}

void PredictionError::configure(float errorThreshold, float sensitivity, float timeWindow) {
    pImpl->errorThreshold = std::max(0.0f, errorThreshold);
    pImpl->sensitivity = std::max(0.0f, sensitivity);
    pImpl->timeWindow = std::max(0.0f, timeWindow);
    
    nlm::Logger::getGlobal().info(
        "PredictionError configured: threshold=" + std::to_string(pImpl->errorThreshold) +
        ", sensitivity=" + std::to_string(pImpl->sensitivity) +
        ", timeWindow=" + std::to_string(pImpl->timeWindow));
}

void PredictionError::processObservation(const std::vector<float>& observation) {
    // Simple implementation: use last observation as actual value
    if (!observation.empty()) {
        // For now, we'll just store the observation and compute error if we have a prediction
        // In a full implementation, this would compare predicted vs actual
        pImpl->actualValue = observation.back();
        nlm::Logger::getGlobal().debug(
            "Processed observation: actual=" + std::to_string(pImpl->actualValue));
    }
}

float PredictionError::getLevel() const {
    return getPredictionErrorLevel();
}

float PredictionError::getPredictionErrorLevel() const {
    return pImpl->error;
}

float PredictionError::getErrorSignalStrength() const {
    float magnitude = std::abs(pImpl->error);
    return magnitude * pImpl->sensitivity;
}

float PredictionError::getErrorSignificance() const {
    return pImpl->errorSignificance;
}

void PredictionError::signalPredictionError() {
    float errorSignal = getErrorSignalStrength();
    
    if (errorSignal > pImpl->errorThreshold) {
        // Compute significance based on threshold crossing
        pImpl->errorSignificance = (errorSignal - pImpl->errorThreshold) / pImpl->errorThreshold;
        pImpl->errorSignificance = std::min(1.0f, pImpl->errorSignificance);
        
        // Convert to neuromodulation factor
        pImpl->neuromodulationFactor = pImpl->errorSignificance * pImpl->sensitivity;
        
        nlm::Logger::getGlobal().info(
            "Prediction error signaled: signal=" + std::to_string(errorSignal) +
            ", significance=" + std::to_string(pImpl->errorSignificance) +
            ", factor=" + std::to_string(pImpl->neuromodulationFactor));
    }
}

void PredictionError::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = std::max(0.0f, threshold);
}

void PredictionError::setErrorSensitivity(float sensitivity) {
    pImpl->sensitivity = std::max(0.0f, sensitivity);
}

void PredictionError::setErrorTimeWindow(TimestepDuration timeWindow) {
    pImpl->timeWindow = std::max(0.0f, timeWindow);
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
    
    // Update error significance based on error magnitude
    pImpl->errorSignificance = std::abs(pImpl->error) * pImpl->sensitivity;
    
    nlm::Logger::getGlobal().debug(
        "Computed prediction error: predicted=" + std::to_string(predicted) +
        ", actual=" + std::to_string(actual) +
        ", error=" + std::to_string(pImpl->error));
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->history.clear();
    pImpl->errorSignificance = 0.0f;
    pImpl->neuromodulationFactor = 0.0f;
    pImpl->elapsedTime = 0.0f;
    
    NLM_LOG_INFO("PredictionError system reset");
}

float PredictionError::getError() const {
    return pImpl->error;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getNeuromodulationFactor() const {
    return pImpl->neuromodulationFactor;
}

} // namespace nlm
