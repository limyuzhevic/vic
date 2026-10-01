#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration
class Brain;

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Implements temporal difference learning prediction error

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(std::shared_ptr<Brain> brain);
    
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
    
    // Check if system is properly initialized
    bool isInitialized() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
