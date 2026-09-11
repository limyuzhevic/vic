#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values with multi-dimensional error components

class Brain;

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error with multi-dimensional components
    void computeError(float predicted, float actual);
    
    // Get error components (intensity, spatial, temporal)
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were
        float temporalError;     // When things happened
        float totalError;        // Combined error magnitude
    };
    
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Is this a surprising event (error > threshold)?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }
    
    // Get neuromodulation signal strength (0-1)
    float getModulationSignal() const;
    
    // Record error with step information
    void recordError(float error, SimulationStep step);
    
    // Get recent errors for learning
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal state
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    std::vector<float> errorHistory_;
};

} // namespace nlm
