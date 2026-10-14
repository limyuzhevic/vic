#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class NeuralEpisodicMemory;

// Prediction system: predicts future sensory states and consequences
// Implements real predictive coding using brain's neural substrate
// and episodic memory for experience-based learning

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural substrate access
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural forward model
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation and learn from error
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error (0 = perfect prediction, 1 = total error)
    float getPredictionError() const;
    
    // Get prediction confidence (0 = no confidence, 1 = certain)
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observation
    void train(const SensoryInput& observation);
    
    // Predict sensory consequences of an action
    std::unique_ptr<SensoryInput> predictActionConsequence(ActionType action,
                                                          const SensoryInput& currentState);
    
    // Store an outcome/sequence for prediction error computation
    void recordOutcome(const std::vector<float>& sensoryState, float reward);
    
    // Enable/disable episodic memory training
    void enableEpisodicTraining(bool enable);
    
    // Set maximum number of prediction examples
    void setMaxPredictionExamples(size_t max);
    
    // Set learning rate for prediction updates
    void setPredictionLearningRate(float rate);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Modulate prediction-related synapses based on error
    void modulatePredictionPathways(float error, float modulation);
    
    // Learn from episodic memory to improve predictions
    void learnFromEpisodicMemory();
};

} // namespace nlm
