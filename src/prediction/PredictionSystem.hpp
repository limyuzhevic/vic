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
    
    // Initialize prediction system with neural integration
    // Connects to brain's neural substrate for real prediction
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses neural patterns instead of just copying sensory input
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Uses neural prediction error signals from neuromodulation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural plasticity
    void train(const SensoryInput& observation);
    
    // Get neural prediction neurons
    const std::vector<NeuronId>& getPredictionNeurons() const { return pImpl->predictionNeurons; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
