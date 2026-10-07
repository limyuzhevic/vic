#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralPrediction.hpp"
#include <memory>
#include <vector>
#include <deque>

namespace nlm {

// PredictionSystem: Core prediction engine using neural substrate
// Replaces placeholder implementation with real forward models and learning
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Make real prediction for next timestep using neural forward model
    // Uses NeuralPrediction class with spiking neural dynamics
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Computes prediction error and drives learning through neuromodulation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error from neural prediction system
    float getPredictionError() const;
    
    // Get prediction confidence from neural prediction system
    float getConfidence() const;
    
    // Get prediction error history from neural system
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using neural learning mechanisms
    // Uses Hebbian, STDP, and reward-modulated learning
    void train(const SensoryInput& observation);
    
    // Record action for action-consequence prediction
    void recordAction(ActionType action, SimulationStep step);
    
    // Predict action consequences using neural predictive coding
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    // Multi-step prediction using neural forward model
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                         size_t numSteps);
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable);
    void enableActionConsequencePrediction(bool enable);
    
    // Configuration
    void setSequenceMemorySize(size_t size);
    void setPredictionHorizon(size_t steps);
    void setPredictionSimilarityThreshold(float threshold);
    
    // Neural substrate access
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Prediction statistics
    float getTemporalPredictionAccuracy() const;
    float getActionConsequenceAccuracy() const;
    float getPredictionErrorMean() const;
    float getPredictionErrorVariance() const;
    
    // Get the underlying NeuralPrediction class
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
    // Reset prediction system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core prediction engine
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal_;
};

} // namespace nlm
