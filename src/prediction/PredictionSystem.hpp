#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// IMPLEMENTED: Real predictive coding using neural activity patterns and temporal dynamics

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep
    // IMPLEMENTED: Real prediction using neural activity patterns and temporal dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // IMPLEMENTED: Real prediction error computation using neural coding
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // IMPLEMENTED: Train prediction model using neural learning rules
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
