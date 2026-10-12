#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class Brain;
class SensoryInput;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system with current state
    // This is called during each brain step to update predictions
    void update(const Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Set the current sensory input for prediction
    void setCurrentInput(const SensoryInput& input);
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::unique_ptr<SensoryInput> currentInput_;
};

} // namespace nlm
