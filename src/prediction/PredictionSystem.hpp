#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include <vector>
#include <memory>

namespace nlm {

// PredictionSystem: Integrated prediction system for the brain
// NOT a standalone system - provides prediction capabilities to Brain
//
// Key components:
// - NeuralPrediction: Temporal sequence prediction
// - ActionConsequencePredictor: Learned action outcomes
// - PredictionErrorSignal: Prediction error computation and neuromodulation
//
// Prediction is encoded in neural activity patterns, not symbolic representations

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Make prediction for next sensory state based on current state
    // Returns predicted sensory state as neural activity pattern
    std::vector<float> predictNextState(const std::vector<float>& currentState);

    // Update predictions based on actual observation
    // Computes prediction error and modulates learning
    float updatePredictions(const std::vector<float>& predicted,
                           const std::vector<float>& actual,
                           SimulationStep currentStep);

    // Get current prediction error
    float getPredictionError() const { return predictionErrorSignal_.getPredictionError(); }

    // Get prediction confidence
    float getPredictionConfidence() const { return confidence_; }

    // Get prediction history for analysis
    const std::deque<float>& getErrorHistory() const { return predictionErrorSignal_.getErrorHistory(); }

    // Clear prediction history
    void clearHistory() {
        neuralPrediction_.clearHistory();
        actionConsequencePredictor_.clear();
        predictionErrorSignal_.clearHistory();
    }

    // Train prediction model on observed data
    void train(const std::vector<float>& observation, SimulationStep step);

    // Record action for action-consequence learning
    void recordAction(ActionType action, SimulationStep step);

    // Predict consequences of taking an action
    std::vector<float> predictActionConsequence(ActionType action,
                                             const std::vector<float>& currentState);

    // Update system with new sensory input
    void updateWithInput(const std::vector<float>& sensoryInput, SimulationStep step);

    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const {
        return neuralPrediction_.getPredictionNeurons();
    }

    // Get sequence learning neurons
    std::vector<NeuronId> getSequenceNeurons() const {
        return neuralPrediction_.getSequenceNeurons();
    }

    // Configure prediction system parameters
    void setTemporalPredictionEnabled(bool enable) {
        neuralPrediction_.enableTemporalPrediction(enable);
    }

    void setActionConsequencePredictionEnabled(bool enable) {
        neuralPrediction_.enableActionConsequencePrediction(enable);
    }

    void setSequenceMemorySize(size_t size) {
        neuralPrediction_.setSequenceMemorySize(size);
    }

    void setPredictionHorizon(size_t steps) {
        neuralPrediction_.setPredictionHorizon(steps);
    }

    // Is prediction system active and providing useful predictions?
    bool isActive() const {
        return predictionErrorSignal_.getPredictionError() < 0.5f;
    }

    // Get prediction quality (inverse of error)
    float getPredictionQuality() const {
        return std::max(0.0f, 1.0f - getPredictionError());
    }

    // Apply prediction errors to brain plasticity
    void applyToBrain(Brain* brain);

private:
    Brain* brain_;
    
    // Core prediction components
    NeuralPrediction neuralPrediction_;
    ActionConsequencePredictor actionConsequencePredictor_;
    PredictionErrorSignal predictionErrorSignal_;
    
    // System state
    float confidence_;
    SimulationStep lastUpdateStep_;
    
    // Prediction results cache
    std::vector<float> lastPredictedState_;
};

// NeuralPrediction: Implements temporal prediction using the spiking substrate
// NOT a Transformer - prediction emerges from learned synaptic dynamics
//
// Key principles:
// - Prediction is encoded in patterns of neural activity
// - Temporal sequences are learned through STDP and Hebbian plasticity
// - Prediction error emerges from comparison of predicted vs actual sensory states
// - No token prediction architecture - purely neural dynamics

// ActionConsequencePredictor: Learns action -> consequence mappings
// Uses the neural substrate to store and retrieve action consequences

// PredictionErrorSignal: Computes and broadcasts prediction error
// Acts as neuromodulatory signal to enhance learning

} // namespace nlm
