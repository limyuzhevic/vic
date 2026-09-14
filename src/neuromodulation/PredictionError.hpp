#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Integrates with dopamine system and provides neuromodulatory signals

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error with multiple components
    // intensity: how much change occurred
    // spatial: where change occurred
    // temporal: when change occurred
    void computeError(const std::vector<float>& predicted, const std::vector<float>& actual);
    
    // Update prediction based on learning
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Get error components for detailed analysis
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were  
        float temporalError;     // When things happened
        float totalError;
        float confidence;        // Confidence in prediction
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
    // Neuromodulation integration
    float getModulationSignal() const;  // Convert error to neuromodulatory signal
    bool isSurprising(float threshold = 0.3f) const;  // For curiosity triggering
    
    // Dopamine system integration
    void connectToDopamine(class Dopamine* dopamine);
    
    // Temporal difference learning
    void updateWithTD(float predictedValue, float actualValue, float reward);
    
private:
    struct Impl;
    Impl* pImpl;
    
    ErrorComponents errorComponents_;
    float surpriseThreshold_;
    class Dopamine* dopamine_;
};

} // namespace nlm
