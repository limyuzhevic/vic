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
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
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
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system with current sensory input
    void update(const class SensoryInput& input, TimestepDuration dt);
    
    // Process prediction for action selection
    ActionType selectActionBasedOnPrediction(const std::vector<float>& currentState);
    
    // Get current predictions for planning
    std::vector<float> getCurrentPredictions() const;
    
    // Set prediction parameters
    void setPredictionConfidence(float confidence) { confidence_ = confidence; }
    
private:
    Brain* brain_;
    float confidence_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
