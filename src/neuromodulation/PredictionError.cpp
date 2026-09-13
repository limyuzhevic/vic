#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    float magnitude;
    float confidence;
    float predictionVariance;
    float learningRate;
    float decayRate;
    std::vector<float> history;
    std::vector<float> predictionHistory;
    Timestamp lastUpdateTime;
    TimestepDuration lastUpdateDuration;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             magnitude(0.0f), confidence(1.0f), predictionVariance(0.01f),
             learningRate(0.1f), decayRate(0.05f), lastUpdateTime(0.0f),
             lastUpdateDuration(0.001) {}
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
    Timestamp currentTime = static_cast<Timestamp>(pImpl->currentStep) * pImpl->lastUpdateDuration;
    
    // Calculate raw prediction error
    float rawError = actual - predicted;
    
    // Update prediction model based on actual outcome (learning)
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Apply inverse variance weighting to error
    float weight = 1.0f / (pImpl->predictionVariance + 0.01f);
    pImpl->error = rawError * weight;
    
    // Compute error magnitude
    pImpl->magnitude = std::abs(pImpl->error);
    
    // Update prediction variance based on error magnitude
    pImpl->predictionVariance = 0.9f * pImpl->predictionVariance + 0.1f * pImpl->magnitude * pImpl->magnitude;
    
    // Update prediction history
    pImpl->predictionHistory.push_back(predicted);
    
    // Add error to history for tracking
    pImpl->history.push_back(pImpl->error);
    
    // Adjust confidence based on error magnitude
    // High confidence when prediction is accurate (small error)
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->magnitude);
    
    // Record timing for temporal dynamics
    pImpl->lastUpdateTime = currentTime;
    
    NLM_LOG_DEBUG(std::string("Prediction error computed: ") + std::to_string(pImpl->error) +
                 std::string(" (magnitude: ") + std::to_string(pImpl->magnitude) +
                 std::string(", confidence: ") + std::to_string(pImpl->confidence) + std::string(")"));
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update predicted value with temporal decay
    pImpl->predictedValue = newPrediction;
    
    // Decay error towards zero between updates
    pImpl->error *= 0.9f;
    pImpl->magnitude *= 0.9f;
    
    // Increase confidence in prediction
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

float PredictionError::getPredictionVariance() const {
    return pImpl->predictionVariance;
}

void PredictionError::update(TimestepDuration dt) {
    // Decay prediction error over time
    pImpl->error *= 0.95f;
    pImpl->magnitude *= 0.95f;
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    
    // Slowly decay prediction variance
    pImpl->predictionVariance = std::max(0.001f, pImpl->predictionVariance * 0.99f);
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->magnitude = 0.0f;
    pImpl->confidence = 1.0f;
    pImpl->predictionVariance = 0.01f;
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

} // namespace nlm