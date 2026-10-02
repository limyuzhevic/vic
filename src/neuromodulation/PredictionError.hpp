#pragma once

#include "../core/Types/Types.hpp"
#include <deque>

namespace nlm {

// PredictionErrorSignal: Computes and broadcasts prediction error
// Enhanced prediction error computation with neuromodulatory effects

class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();
    
    // Compute error between predicted and actual
    float computeError(const std::vector<float>& predicted,
                       const std::vector<float>& actual);
    
    // Get neuromodulation signal strength (0-1)
    float getModulationSignal() const;
    
    // Update history
    void recordError(float error, SimulationStep step);
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    
    // Is this a surprising event (error > threshold)?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }
    
    // Configuration
    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    float getSurpriseThreshold() const { return surpriseThreshold_; }
    void setNeuromodulatoryStrength(float strength) { neuromodulatoryStrength_ = strength; }
    float getNeuromodulatoryStrength() const { return neuromodulatoryStrength_; }
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reset
    void reset();
    
    // Get current error
    float getCurrentError() const { return predictionError_; }
    float getConfidence() const { return confidence_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Current error
    float predictionError_;
    float confidence_;
    float lastError_;
    float surpriseThreshold_;
    float neuromodulatoryStrength_;
    std::deque<float> errorHistory_;
};

} // namespace nlm