#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Phase 2: Implements real predictive coding using NLM's neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural substrate access
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes neural prediction error and modulates plasticity
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (neural prediction error signal)
    float getPredictionError() const;
    
    // Get prediction confidence based on neural consistency
    float getConfidence() const;
    
    // Get prediction history (neural prediction error over time)
    // Returns a read-only view of the error history
    // Note: This returns a temporary buffer; do not store the reference
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural plasticity mechanisms
    // Implements reward-modulated STDP and Hebbian learning
    void train(const SensoryInput& observation);
    
    // Configure prediction subsystems
    void enableTemporalPrediction(bool enable);
    void enableActionConsequencePrediction(bool enable);
    void setPredictionHorizon(size_t steps);
    
    // Record action for action-consequence learning
    void recordAction(ActionType action);
    
    // Update current simulation step for prediction computations
    void updateStep(SimulationStep step);
    
    // Multi-step prediction
    std::vector<std::unique_ptr<SensoryInput>> predictMultipleSteps(
        SimulationStep currentStep, size_t numSteps);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
