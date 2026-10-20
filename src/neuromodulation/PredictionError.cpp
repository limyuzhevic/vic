// PredictionError implementation
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float lastError;
    float surpriseThreshold;
    float errorIntensity;
    float errorSpatial;
    float errorTemporal;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f), lastError(0.0f), surpriseThreshold(0.5f),
             errorIntensity(0.0f), errorSpatial(0.0f), errorTemporal(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}
PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->plasticityFactor = 1.0f + pImpl->level * 1.5f;  // Range: 1.0 to 2.5
}

float PredictionError::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void PredictionError::update(TimestepDuration dt) {
    // Decay prediction error level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
}

float PredictionError::computeError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual) {
    if (predicted.size() != actual.size()) {
        return 1.0f;  // Maximum error for size mismatch
    }
    
    float totalError = 0.0f;
    float intensityError = 0.0f;
    float spatialError = 0.0f;
    float temporalError = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalError += diff;
        
        // Classify error type (simplified)
        if (i < 3) {  // First 3 elements assumed to be intensity
            intensityError += diff;
        } else if (i < 6) {  // Next 3 elements assumed to be spatial
            spatialError += diff;
        } else {  // Remaining elements assumed to be temporal
            temporalError += diff;
        }
    }
    
    float averageError = totalError / predicted.size();
    
    // Update error components
    pImpl->errorIntensity = intensityError / 3.0f;
    pImpl->errorSpatial = spatialError / 3.0f;
    pImpl->errorTemporal = temporalError / std::max(1.0f, static_cast<float>(predicted.size()) - 6.0f);
    
    // Update last error and level
    pImpl->lastError = averageError;
    pImpl->level = std::min(1.0f, averageError * 2.0f);
    
    return averageError;
}

void PredictionError::recordError(float error, SimulationStep step) {
    pImpl->lastError = error;
    
    if (error > pImpl->surpriseThreshold) {
        // Surprise detected - level increases more rapidly
        pImpl->level = std::min(1.0f, pImpl->level + error * 0.5f);
        NLM_LOG_DEBUG(std::string("Surprise detected: error = ") + std::to_string(error));
    }
}

} // namespace nlm