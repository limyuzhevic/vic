#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class NeuralPrediction;

// Prediction system: predicts future sensory states and consequences
// Phase 2: Real neural-based prediction using NLM's neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and neural prediction system
    void initialize(class Brain* brain, class NeuralPrediction* neuralPrediction);
    
    // Make prediction for next timestep using neural substrate
    // Uses brain's regions and neurons to generate predictions based on neural patterns
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    // Compare predicted vs actual neural activity patterns
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction accuracy
    float getPredictionAccuracy() const;
    
    // Get neural synchronization metrics
    float getNeuralSynchronization() const;
    float getPopulationCorrelation() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Training and adaptation
    void setLearningRate(float rate);
    void setErrorDecayRate(float rate);
    void train(const SensoryInput& observation);
    
private:
    // Helper methods for neural prediction
    void updatePredictionNeurons(const SensoryInput& currentState, const SensoryInput& predictedState);
    float computeNeuralPredictionError(const std::vector<std::vector<float>>& predicted, const std::vector<std::vector<float>>& actual) const;
    void updatePredictionConfidence(const std::vector<std::vector<float>>& predicted, const std::vector<std::vector<float>>& actual);
    void applyPredictionLearning(const std::vector<std::vector<float>>& predicted, const std::vector<std::vector<float>>& actual);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
