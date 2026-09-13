#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Additional functionality
    float getConfidence() const;
    float getPredictionVariance() const;
    void update(TimestepDuration dt);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline PredictionError::PredictionError() : pImpl(new Impl) {}

inline PredictionError::~PredictionError() = default;

inline void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

inline float PredictionError::getError() const {
    return pImpl->error;
}

inline void PredictionError::computeError(float predicted, float actual) {
    Timestamp currentTime = static_cast<Timestamp>(pImpl->currentStep) * pImpl->lastUpdateDuration;
    
    float rawError = actual - predicted;
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    float weight = 1.0f / (pImpl->predictionVariance + 0.01f);
    pImpl->error = rawError * weight;
    
    pImpl->magnitude = std::abs(pImpl->error);
    
    pImpl->predictionVariance = 0.9f * pImpl->predictionVariance + 0.1f * pImpl->magnitude * pImpl->magnitude;
    
    pImpl->predictionHistory.push_back(predicted);
    
    pImpl->history.push_back(pImpl->error);
    
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->magnitude);
    
    pImpl->lastUpdateTime = currentTime;
    
    NLM_LOG_DEBUG(std::string("Prediction error computed: ") + std::to_string(pImpl->error) +
                 std::string(" (magnitude: ") + std::to_string(pImpl->magnitude) +
                 std::string(", confidence: ") + std::to_string(pImpl->confidence) + std::string(")"));
}

inline void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    
    pImpl->error *= 0.9f;
    pImpl->magnitude *= 0.9f;
    
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f);
}

inline const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

inline void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

inline float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

inline float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

inline float PredictionError::getPredictionVariance() const {
    return pImpl->predictionVariance;
}

inline void PredictionError::update(TimestepDuration dt) {
    pImpl->error *= 0.95f;
    pImpl->magnitude *= 0.95f;
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
    
    pImpl->predictionVariance = std::max(0.001f, pImpl->predictionVariance * 0.99f);
}

inline void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->magnitude = 0.0f;
    pImpl->confidence = 1.0f;
    pImpl->predictionVariance = 0.01f;
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

} // namespace nlm
