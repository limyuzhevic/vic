#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <random>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 6: Real predictive coding using neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const { return predictionError_; }
    
    // Get prediction confidence
    float getConfidence() const { return confidence_; }
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    void clearHistory() { errorHistory_.clear(); predictionError_ = 0.0f; }
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Update with current brain state (for integration)
    void update(const std::vector<float>& currentSensory, float timestep);
    
    // Get current prediction strength
    float getPredictionStrength() const { return predictionStrength_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float predictionError_;
    float confidence_;
    float predictionStrength_;
    std::vector<float> errorHistory_;
    std::mt19937 rng_;
    
    // Generate prediction based on current neural activity
    std::unique_ptr<SensoryInput> generatePrediction(const SensoryInput& currentState);
    
    // Compute prediction error between predicted and actual
    float computeError(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update neural prediction circuits
    void updatePredictionCircuits(const SensoryInput& currentState);
};

} // namespace nlm
