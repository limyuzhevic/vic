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
    
    // Get filtered error history with temporal weighting
    const std::vector<float>& getFilteredHistory() const;
    
    // Get original prediction and actual values
    float getPredictionValue() const;
    float getActualValue() const;
    
    // Get statistical analysis of error history
    float getConfidenceInterval() const;
    float getStatisticalMean() const;
    float getStatisticalVariance() const;
    float getStdDev() const;
    
    // System management
    void reset();
    bool isHealthy() const;
    void logDiagnostics() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
