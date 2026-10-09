#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../neuron/NeuralActivityPattern.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain;
class NeuralPrediction;
class ActionConsequencePredictor;
class PredictionErrorSignal;

// Prediction system: predicts future sensory states and consequences
// Uses neural prediction mechanisms for realistic forecasting

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep based on current state
    // Uses neural prediction and action consequence models
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and updates internal models
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, higher = larger error)
    float getPredictionError() const;
    
    // Get prediction confidence based on model consistency
    float getConfidence() const;
    
    // Get prediction history for analysis
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observation
    void train(const SensoryInput& observation);
    
    // Get prediction error signal for neuromodulation
    float getPredictionErrorSignal() const;
    
    // Get current predicted value
    float getLastPrediction() const { return pImpl->lastPredictedValue; }
    
    // Set prediction mode
    void setPredictionMode(PredictionMode mode) { predictionMode_ = mode; }
    PredictionMode getPredictionMode() const { return predictionMode_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PredictionMode predictionMode_;
};

enum class PredictionMode {
    Passive,      // Simply mirror current state
    Active,       // Actively predict future states
    Reactive,     // React to sensory input
    Proactive     // Plan and anticipate
};

} // namespace nlm
