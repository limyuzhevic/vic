#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "NeuralPrediction.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implements real predictive coding using neural temporal dynamics
// Integrates with brain loop, working memory, episodic memory, and neuromodulation

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for integration
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and compute error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0-1, 0=perfect prediction)
    float getPredictionError() const;
    
    // Get prediction confidence (0-1, 1=high confidence)
    float getConfidence() const;
    
    // Get prediction error history for learning
    const std::vector<float>& getErrorHistory() const;
    
    // Clear prediction history and reset state
    void clearHistory();
    
    // Train prediction model using recent experiences
    void train(const SensoryInput& observation);
    
    // Enable/disable temporal prediction
    void enableTemporalPrediction(bool enable) { neuralPrediction_.enableTemporalPrediction(enable); }
    
    // Enable/disable action consequence prediction
    void enableActionConsequencePrediction(bool enable) { neuralPrediction_.enableActionConsequencePrediction(enable); }
    
    // Predict consequences of an action
    std::vector<float> predictActionConsequence(ActionType action, const std::vector<float>& currentState);
    
    // Record action for learning action-consequence associations
    void recordAction(ActionType action);
    
    // Get prediction neurons for monitoring
    std::vector<NeuronId> getPredictionNeurons() const;
    
    // Get sequence prediction neurons
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Get prediction confidence over time window
    std::vector<float> getRecentConfidence(size_t window = 10) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core neural prediction engine
    NeuralPrediction neuralPrediction_;
    
    // Error signal for neuromodulation
    std::vector<float> errorHistory_;
};

} // namespace nlm
