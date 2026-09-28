#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain;

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set random seed for prediction generation
    void setRandomSeed(uint64_t seed);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Enhanced update with brain integration
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual, Brain* brain);
    
    // Integration with attention system
    void integrateWithAttention(Brain* brain);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction influence level
    float getPredictionInfluence() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
