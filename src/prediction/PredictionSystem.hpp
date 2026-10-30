#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// INTEGRATED WITH NEURAL PREDICTION

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural prediction
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    // Uses NeuralPrediction for real predictive coding
    std::vector<float> predictNextState(const std::vector<float>& currentState);
    
    // Update predictions based on actual observation
    // Uses NeuralPrediction to learn from experience
    void updatePredictions(const std::vector<float>& predicted, const std::vector<float>& actual);
    
    // Get prediction error
    // Delegates to NeuralPrediction
    float getPredictionError() const;
    
    // Get prediction confidence
    // Delegates to NeuralPrediction
    float getConfidence() const;
    
    // Get prediction history
    // Delegates to NeuralPrediction
    const std::deque<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    // Delegates to NeuralPrediction
    void train(const std::vector<float>& observation);
    
    // Enable/disable prediction mechanisms
    void enableTemporalPrediction(bool enable) { neuralPrediction_.enableTemporalPrediction(enable); }
    void enableActionConsequencePrediction(bool enable) { neuralPrediction_.enableActionConsequencePrediction(enable); }
    
    // Configuration
    void setSequenceMemorySize(size_t size) { neuralPrediction_.setSequenceMemorySize(size); }
    void setPredictionHorizon(size_t steps) { neuralPrediction_.setPredictionHorizon(steps); }
    
    // Get neural components involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const { return neuralPrediction_.getPredictionNeurons(); }
    std::vector<NeuronId> getSequenceNeurons() const { return neuralPrediction_.getSequenceNeurons(); }
    
private:
    NeuralPrediction neuralPrediction_;
};

} // namespace nlm
