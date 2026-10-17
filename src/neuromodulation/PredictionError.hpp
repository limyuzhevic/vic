#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values with sophisticated error tracking

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current error value
    float getError() const;
    
    // Compute prediction error with enhanced statistics
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Enhanced error statistics for analysis
    float getMeanAbsoluteError() const;
    float getMeanSquaredError() const;
    float getMaxAbsoluteError() const;
    float getConfidenceInterval() const;
    float getTemporalDecay() const;
    void setTemporalDecay(float decay);
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError implementation notes:
// - Real-time error computation with automatic statistics calculation
// - Historical error tracking with temporal decay
// - Comprehensive error statistics for analysis and debugging
// - Confidence interval estimation for error reliability
// - Integration with neuromodulation systems for adaptive learning
