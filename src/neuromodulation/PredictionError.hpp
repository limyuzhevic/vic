#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// PLACEHOLDER - Phase 2 will implement real prediction error computation

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Get error value
    float getError() const;
    
    // Compute prediction error
    // TODO PHASE 2: Implement real prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
