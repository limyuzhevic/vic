#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal
// Computes prediction error between expected and actual outcomes

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
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get prediction error magnitude
    float getMagnitude() const;
    
    // Get prediction error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm