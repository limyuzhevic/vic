#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Now properly inherits from Neuromodulator

class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Get prediction error parameters
    float getLearningRate() const;
    float getErrorThreshold() const;
    
    // Set prediction error parameters
    void setLearningRate(float rate);
    void setErrorThreshold(float threshold);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
