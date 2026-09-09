#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// Real prediction error computation
// Surprise computation
// Plasticity modulation

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction using delta rule
    void updatePrediction(float newPrediction);
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Surprise computation
    float computeSurprise() const;
    float getSurprise() const;
    
    // Get prediction error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set parameters
    void setDecayRate(float rate);
    void setIntegrationFactor(float factor);
    void setLearningRate(float lr);
    void setSurpriseFactor(float factor);
    
    // Plasticity modulation based on prediction error
    float getPlasticityModulation() const;
    
    // Get error statistics
    float getMaxError() const;
    float getMinError() const;
    float getAverageError() const;
    
    // Get exploration value for prediction error-driven exploration
    float getExplorationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
