#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Implements temporal difference learning and prediction error propagation
class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error using temporal difference learning
    // delta = r + gamma * V(s') - V(s)
    void computeTemporalDifferenceError(float reward, float currentValue, float nextValue, float discountFactor = 0.95f);
    
    // Compute prediction error between predicted and actual observations
    void computeObservationError(const class Observation& predicted, const class Observation& actual);
    
    // Update prediction based on new information
    void updatePrediction(float newPrediction, float confidence = 1.0f);
    
    // Get history of prediction errors
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Get prediction error statistics
    float getMeanError() const;
    float getMaxError() const;
    float getMinError() const;
    
    // Check if prediction error is significant
    bool isSignificant() const;
    
    // Get recent prediction error (for curiosity and exploration)
    float getRecentError() const { return recentError_; }
    
    // Get prediction error for specific context (neural representation)
    float getContextError(const class NeuralPattern& context) const;
    
    // Update context-specific prediction errors
    void updateContextError(const class NeuralPattern& context, float error);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Recent error for feedback
    float recentError_;
};

} // namespace nlm
