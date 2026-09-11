#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
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
    
    // Compute error from neural state
    void computeFromNeuralState(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               SimulationStep step);
    
    // Get neuromodulation signal strength (0-1)
    float getModulationSignal() const;
    
    // Get error components
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were
        float temporalError;     // When things happened
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    std::vector<float> errorHistory_;
};

} // namespace nlm
