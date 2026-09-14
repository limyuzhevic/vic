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
    
    // Multiple error components for different prediction aspects
    float intensityError;
    float spatialError;
    float temporalError;
    float totalError;
    float confidence;
    
    class Dopamine* dopamine;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f)
           , intensityError(0.0f), spatialError(0.0f), temporalError(0.0f)
           , totalError(0.0f), confidence(0.5f), dopamine(nullptr) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    surpriseThreshold_ = 0.3f;
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
    errorComponents_.confidence = 0.5f;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        pImpl->error = 0.0f;
        return;
    }
    
    // Compute intensity error (magnitude of changes)
    float intensitySum = 0.0f;
    float spatialSum = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        intensitySum += diff;
        
        // Spatial error based on pattern of differences
        if (i > 0) {
            float prevDiff = std::abs(predicted[i-1] - actual[i-1]);
            spatialSum += std::abs(diff - prevDiff);
        }
    }
    
    pImpl->intensityError = intensitySum / predicted.size();
    pImpl->spatialError = spatialSum / (predicted.size() - 1);
    
    // Temporal error (assuming last element represents time)
    if (!predicted.empty() && !actual.empty()) {
        pImpl->temporalError = std::abs(predicted.back() - actual.back());
    }
    
    // Compute total error (weighted combination)
    pImpl->totalError = std::sqrt(
        pImpl->intensityError * pImpl->intensityError +
        pImpl->spatialError * pImpl->spatialError +
        pImpl->temporalError * pImpl->temporalError
    );
    
    pImpl->error = pImpl->totalError;
    pImpl->predictedValue = predicted.back();
    pImpl->actualValue = actual.back();
    
    // Update error components
    errorComponents_.intensityError = pImpl->intensityError;
    errorComponents_.spatialError = pImpl->spatialError;
    errorComponents_.temporalError = pImpl->temporalError;
    errorComponents_.totalError = pImpl->totalError;
    
    // Update confidence based on error magnitude
    // Higher error = lower confidence
    errorComponents_.confidence = 1.0f - std::min(1.0f, pImpl->totalError / 0.5f);
    
    pImpl->history.push_back(pImpl->error);
    
    // Signal to dopamine system if connected
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(pImpl->error);
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
    return std::abs(pImpl->error);
}

float PredictionError::getModulationSignal() const {
    // Convert error to neuromodulatory signal (0-1)
    // Higher error = stronger modulation for learning
    return std::min(1.0f, pImpl->totalError * 2.0f);
}

bool PredictionError::isSurprising(float threshold) const {
    return pImpl->totalError > threshold;
}

void PredictionError::connectToDopamine(class Dopamine* dopamine) {
    pImpl->dopamine = dopamine;
}

void PredictionError::updateWithTD(float predictedValue, float actualValue, float reward) {
    // Temporal difference learning update
    float tdError = reward + 0.9f * actualValue - predictedValue;
    pImpl->error = tdError;
    pImpl->predictedValue = predictedValue;
    pImpl->actualValue = actualValue;
    
    // Update components based on TD error
    pImpl->totalError = std::abs(tdError);
    pImpl->intensityError = tdError;
    errorComponents_.totalError = pImpl->totalError;
    errorComponents_.confidence = 1.0f - std::min(1.0f, std::abs(tdError));
    
    pImpl->history.push_back(pImpl->error);
}

} // namespace nlm
