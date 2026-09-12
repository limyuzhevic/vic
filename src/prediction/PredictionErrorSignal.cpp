#include "PredictionErrorSignal.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionErrorSignal::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

PredictionErrorSignal::PredictionErrorSignal()
    : pImpl(new Impl)
    , lastError_(0.0f)
    , surpriseThreshold_(0.3f)
{
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
}

PredictionErrorSignal::~PredictionErrorSignal() = default;

void PredictionErrorSignal::initialize(Brain* brain) {
    pImpl->brain = brain;
}

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                       const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return 0.0f;
    }
    
    // Compute overall difference
    float totalDiff = 0.0f;
    float intensityDiff = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalDiff += diff;
        intensityDiff += diff * diff;
    }
    
    float meanError = totalDiff / predicted.size();
    float meanSqError = intensityDiff / predicted.size();
    
    // Compute spatial error (variance of differences)
    float spatialVar = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    float spatialError = std::sqrt(spatialVar / predicted.size());
    
    // Update components
    errorComponents_.intensityError = std::sqrt(meanSqError);
    errorComponents_.spatialError = spatialError;
    
    // Compute total error (combining components)
    lastError_ = std::sqrt(meanSqError + spatialError * spatialError);
    errorComponents_.totalError = lastError_;
    
    return lastError_;
}

float PredictionErrorSignal::getModulationSignal() const {
    // Convert error to modulation signal (0-1)
    // Higher error = stronger modulation for learning
    return std::min(1.0f, lastError_ * 2.0f);
}

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

const std::deque<float>& PredictionErrorSignal::getErrorHistory() const {
    return errorHistory_;
}

bool PredictionErrorSignal::isSurprising() const {
    return lastError_ > surpriseThreshold_;
}

void PredictionErrorSignal::setSurpriseThreshold(float t) {
    surpriseThreshold_ = t;
}

float PredictionErrorSignal::getSurpriseThreshold() const {
    return surpriseThreshold_;
}

} // namespace nlm