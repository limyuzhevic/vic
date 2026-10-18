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
    
    // Initialize prediction system
    void initialize(Brain* brain);
    
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
    
    // Update prediction system with sensory input
    void update(const SensoryInput& input, SimulationStep currentStep);
    
    // Get prediction neurons for attention
    std::vector<NeuronId> getPredictionNeurons() const;
    
    // Get prediction errors for neuromodulation
    float getPredictionErrorSignal() const;
    
    // Get prediction activity level
    float getPredictionActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Brain* brain_;
};

} // namespace nlm
