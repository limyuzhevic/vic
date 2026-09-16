#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrated with neural substrate for real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural substrate
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
    std::vector<float> predictNextState(const std::vector<float>& currentState,
                                       SimulationStep currentStep);
    
    // Update predictions based on actual observation
    float updateWithObservation(const std::vector<float>& actualState,
                               SimulationStep currentStep);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Get prediction history
    const std::deque<float>& getErrorHistory() const;
    void clearHistory();
    
    // Record sensory state for future prediction
    void recordSensoryState(const std::vector<float>& sensoryState,
                          SimulationStep currentStep);
    
    // Record action that was taken for action-consequence learning
    void recordAction(ActionType action, SimulationStep step);
    
    // Get the prediction error signal for neuromodulation
    float getPredictionErrorSignal() const;
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable);
    void enableActionConsequencePrediction(bool enable);
    
    // Configuration
    void setSequenceMemorySize(size_t size);
    void setPredictionHorizon(size_t steps);
    
    // Get prediction system reference
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
