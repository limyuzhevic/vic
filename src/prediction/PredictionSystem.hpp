#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "NeuralPrediction.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// PredictionSystem: Core prediction interface with NeuralPrediction backend
// Implements temporal and action-consequence prediction using spiking neural substrate
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next sensory state
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
    
    // Train prediction model with current sensory state
    void train(const std::vector<float>& sensoryState);
    
    // Generate multi-step prediction
    std::vector<std::vector<float>> predictMultipleSteps(size_t numSteps);
    
    // Record action that was taken for action-consequence learning
    void recordAction(ActionType action);
    
    // Get current prediction neurons
    std::vector<NeuronId> getPredictionNeurons() const;
    
    // Get current sequence neurons
    std::vector<NeuronId> getSequenceNeurons() const;
    
    // Get neural prediction for advanced users
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
    // Get action consequence predictor
    ActionConsequencePredictor* getActionConsequencePredictor() { return actionConsequence_.get(); }
    
    // Get prediction error signal
    PredictionErrorSignal* getPredictionErrorSignal() { return errorSignal_.get(); }
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { 
        if (neuralPrediction_) neuralPrediction_->enableTemporalPrediction(enable); 
    }
    void enableActionConsequencePrediction(bool enable) { 
        if (neuralPrediction_) neuralPrediction_->enableActionConsequencePrediction(enable); 
    }
    
    // Configuration
    void setSequenceMemorySize(size_t size) { 
        if (neuralPrediction_) neuralPrediction_->setSequenceMemorySize(size); 
    }
    void setPredictionHorizon(size_t steps) { 
        if (neuralPrediction_) neuralPrediction_->setPredictionHorizon(steps); 
    }
    
    // Get current state for debugging
    const std::vector<std::vector<float>>& getRecentSensoryStates() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural components (Phase 2 implementation)
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    std::unique_ptr<ActionConsequencePredictor> actionConsequence_;
    std::unique_ptr<PredictionErrorSignal> errorSignal_;
    
    Brain* brain_;
};

} // namespace nlm
