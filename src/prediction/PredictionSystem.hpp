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
    
    // Get predicted active neurons based on learned patterns
    const std::vector<NeuronId>& getPredictedActiveNeurons() const;
    
    // Get predicted sensory state as vector
    const std::vector<float>& getPredictedState() const;
    
    // Get current sensory state
    const std::vector<float>& getCurrentSensoryState() const;
    
    // Set current sensory state (for integration)
    void setCurrentSensoryState(const std::vector<float>& state);
    
    // Update prediction system with current time step
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Get prediction neurons (learned pattern representations)
    const std::vector<NeuronId>& getPatternNeurons() const;
    
    // Get sequence associations (what predicts what)
    const std::vector<std::pair<NeuronId, NeuronId>>& getSequenceAssociations() const;
    
    // Add pattern neuron to system
    void addPatternNeuron(NeuronId id, const std::vector<float>& representation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
