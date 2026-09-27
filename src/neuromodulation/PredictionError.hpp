#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Real prediction error: neural-based prediction error signaling system

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current prediction error
    float getError() const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction with new value
    void updatePrediction(float newPrediction);
    
    // Get prediction error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Get temporal difference components
    float getTemporalDifference() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Set prediction parameters
    void setPredictionHorizon(float horizon);
    void setErrorSensitivity(float sensitivity);
    void setLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
