#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements forward model with neural substrate integration

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Update with current brain state for integration
    void updateWithCurrentState();
    
    // Get current prediction confidence based on attention
    float getCurrentConfidence() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
