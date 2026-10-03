#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values

class PredictionError {
public:
    PredictionError(nullptr);
    ~PredictionError(nullptr);
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError(nullptr) const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory(nullptr) const;
    void clearHistory(nullptr);
    
    // Get error magnitude for neuromodulation
    float getMagnitude(nullptr) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
