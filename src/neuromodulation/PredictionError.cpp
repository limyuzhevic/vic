#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError()
    : pImpl(new Impl)
    , lastError_(0.0f)
    , surpriseThreshold_(0.3f)
{
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return lastError_;
}

void PredictionError::computeError(float predicted, float actual) {
    float diff = actual - predicted;
    lastError_ = diff;
    errorComponents_.totalError = std::abs(diff);
    errorComponents_.intensityError = diff * diff;
    
    errorHistory_.push_back(lastError_);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    errorComponents_.totalError = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return errorHistory_;
}

void PredictionError::clearHistory() {
    errorHistory_.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(lastError_);
}

void PredictionError::computeFromNeuralState(const std::vector<float>& predicted,
                                           const std::vector<float>& actual,
                                           SimulationStep step) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return;
    }
    
    // Compute total difference
    float totalDiff = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        totalDiff += std::abs(predicted[i] - actual[i]);
    }
    float meanError = totalDiff / predicted.size();
    
    // Compute intensity error (squared differences)
    float intensityError = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        intensityError += diff * diff;
    }
    errorComponents_.intensityError = std::sqrt(intensityError / predicted.size());
    
    // Compute spatial error (variance of differences)
    float spatialVar = 0.0f;
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    errorComponents_.spatialError = std::sqrt(spatialVar / predicted.size());
    
    // Compute temporal error (step difference)
    static SimulationStep lastStep = 0;
    if (step > lastStep) {
        errorComponents_.temporalError = static_cast<float>(step - lastStep);
    }
    lastStep = step;
    
    // Total error combines all components
    lastError_ = std::sqrt(errorComponents_.intensityError * errorComponents_.intensityError +
                          errorComponents_.spatialError * errorComponents_.spatialError +
                          errorComponents_.temporalError * errorComponents_.temporalError);
    
    errorHistory_.push_back(lastError_);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

float PredictionError::getModulationSignal() const {
    // Convert error to modulation signal (0-1)
    return std::min(1.0f, lastError_ * 2.0f);
}

} // namespace nlm
