#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

class Brain;

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Integrates with neuromodulation systems to modulate plasticity and exploration

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update prediction error system with time step
    void update(TimestepDuration dt);
    
    // Configure system parameters
    void configure(float errorThreshold, float sensitivity, float timeWindow);
    
    // Process new observation to compute prediction error
    void processObservation(const std::vector<float>& observation);
    
    // Get current error level
    float getLevel() const;
    
    // Get prediction error level (alias for getLevel())
    float getPredictionErrorLevel() const;
    
    // Get error signal strength for neuromodulation
    float getErrorSignalStrength() const;
    
    // Get error significance factor for plasticity modulation
    float getErrorSignificance() const;
    
    // Signal prediction error to neuromodulation systems
    void signalPredictionError();
    
    // Set error threshold parameter
    void setErrorThreshold(float threshold);
    
    // Set error sensitivity parameter
    void setErrorSensitivity(float sensitivity);
    
    // Set error time window parameter
    void setErrorTimeWindow(TimestepDuration timeWindow);
    
    // Compute prediction error from predicted and actual values
    void computeError(float predicted, float actual);
    
    // Update prediction value
    void updatePrediction(float newPrediction);
    
    // Reset system state
    void reset();
    
    // Get current error value
    float getError() const;
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Get observation history
    const std::vector<float>& getHistory() const;
    
    // Clear observation history
    void clearHistory();
    
    // Get neuromodulation factor
    float getNeuromodulationFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
