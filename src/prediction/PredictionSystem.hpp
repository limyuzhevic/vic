#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain;
class SensoryInput;

// Prediction system: predicts future sensory states and consequences
// PHASE 2: Real neural prediction using predictive coding and brain dynamics
//
// Mechanisms:
// - Neural prediction using sensory and internal region activity patterns
// - Predictive coding: top-down predictions vs bottom-up prediction errors
// - Neural prediction error signals that influence learning and behavior
// - Integration with working memory and attention systems
// - Real-time prediction updated with brain step loop

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses neural activity patterns from sensory and internal regions
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Compare predicted vs actual using neural similarity metrics
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // Update internal model based on prediction errors
    void train(const SensoryInput& observation);
    
    // Get current prediction
    const std::vector<float>& getLastPrediction() const { return lastPrediction_; }
    
    // Update confidence based on prediction accuracy
    void updateConfidence(float error);
    
    // Get working memory prediction contribution
    float getWorkingMemoryContribution() const;
    
    // Get attention contribution
    float getAttentionContribution() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Last prediction generated
    std::vector<float> lastPrediction_;
    
    // Brain reference for accessing neural patterns
    Brain* brain_;
};

} // namespace nlm
