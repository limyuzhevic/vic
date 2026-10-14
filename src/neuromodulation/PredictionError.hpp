#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class PredictionError
 * @brief Neuromodulator for prediction error signals
 * 
 * PredictionError computes the discrepancy between predicted and actual values,
 * serving as a core learning signal in both curiosity-driven and reinforcement
 * learning. Higher prediction errors increase plasticity and exploration.
 * 
 * @details Maintains prediction history and computes error magnitudes for
 * neuromodulatory effects on synaptic plasticity.
 */
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
