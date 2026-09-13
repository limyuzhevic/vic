#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Uses neural substrate for prediction, prediction error, and action-consequence learning

class Brain;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural substrate
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
    
    // Train prediction model using neural learning
    void train(const SensoryInput& observation);
    
    // Get access to neural prediction component
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    const NeuralPrediction* getNeuralPrediction() const { return neuralPrediction_.get(); }
    
    // Get access to action consequence predictor
    ActionConsequencePredictor* getActionConsequencePredictor() { return actionPredictor_.get(); }
    const ActionConsequencePredictor* getActionConsequencePredictor() const { return actionPredictor_.get(); }
    
    // Get access to prediction error signal
    PredictionErrorSignal* getPredictionErrorSignal() { return errorSignal_.get(); }
    const PredictionErrorSignal* getPredictionErrorSignal() const { return errorSignal_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural prediction components
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    std::unique_ptr<ActionConsequencePredictor> actionPredictor_;
    std::unique_ptr<PredictionErrorSignal> errorSignal_;
    Brain* brain_;
};

} // namespace nlm
