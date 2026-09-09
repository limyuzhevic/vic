#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
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
    
    // Update prediction system (called each step)
    void update(Brain* brain, TimestepDuration dt);
    
    // Get most recent prediction
    const SensoryInput* getLastPrediction() const { return lastPrediction_.get(); }
    
    // Process sensory input for prediction
    void processSensoryInput(const SensoryInput& input);
    
    // Get prediction confidence in range [0, 1]
    float getPredictionConfidence() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<SensoryInput> lastPrediction_;
};

} // namespace nlm
