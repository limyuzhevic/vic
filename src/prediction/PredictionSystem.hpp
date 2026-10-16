#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Error/Error.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class NeuralPrediction;
class PredictionError;
class ActionConsequencePredictor;

// Prediction system: predicts future sensory states and consequences
// Uses neural prediction infrastructure for real-time predictions

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses neural substrate to generate actual predictions
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Uses NeuralPrediction error computation and PredictionErrorSignal
    // Provides multi-component error analysis
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // Uses NeuralPrediction's learning mechanisms
    // Trains temporal and action-consequence models
    // Integrates with neuromodulation systems
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
