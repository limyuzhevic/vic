#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;

// Prediction system: predicts future sensory states and consequences
// IMPLEMENTED - Uses neural predictive coding with brain neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural predictive coding
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
    
    // Neural prediction interface
    std::vector<NeuronId> getPredictionNeurons() const;
    std::vector<NeuronId> getSequenceNeurons() const;
    
private:
    // Internal implementations
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    float computeNeuralSimilarity(const std::vector<float>& a, const std::vector<float>& b) const;
    void applyPredictiveErrorMinimization(const std::vector<float>& predicted, const std::vector<float>& actual);
    void updateForwardModelWeights(const std::vector<float>& predicted, const std::vector<float>& actual, float similarity);
    std::vector<float> applyForwardModel(const std::vector<float>& sensoryState, const std::vector<float>& neuralPrediction) const;
    void providePredictionErrorToNeuromodulation();
};

} // namespace nlm
