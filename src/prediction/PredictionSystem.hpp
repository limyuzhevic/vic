#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Implemented using neural substrate (STDP, Hebbian plasticity, temporal learning)
// NOT a Transformer - prediction emerges from learned synaptic dynamics
//
// Key principles:
// - Prediction is encoded in patterns of neural activity
// - Temporal sequences are learned through STDP and Hebbian plasticity  
// - Prediction error emerges from comparison of predicted vs actual sensory states
// - No token prediction architecture - purely neural dynamics
//
// Integration:
// - Connected to sensory processing through NeuralPrediction system
// - Provides prediction error signals for neuromodulation
// - Affects action selection through predicted consequences
// - Enhances working memory through predictive coding

class NeuralPrediction;
class ActionConsequencePredictor;
class PredictionErrorSignal;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Returns prediction error for neuromodulation
    float updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with observation
    void train(const SensoryInput& observation);
    
    // Get prediction components for neuromodulation
    float getModulationSignal() const;
    
    // Check if prediction error indicates surprising event
    bool isSurprising() const;
    
    // Get temporal predictions (multiple steps ahead)
    std::vector<std::vector<float>> getTemporalPredictions(size_t numSteps = 5);
    
    // Get predicted consequences for an action
    std::vector<float> predictActionConsequence(ActionType action, 
                                               const SensoryInput& currentState);
    
private:
    // Neural prediction and action consequence systems
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor_;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal_;
    
    // Brain reference for neural integration
    class Brain* brain_;
};

} // namespace nlm
