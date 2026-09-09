#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrates with episodic memory for context-aware predictions

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with episodic memory reference
    void initialize(NeuralEpisodicMemory* episodicMemory);
    
    // Make prediction for next timestep using episodic memory context
    // Retrieves relevant past experiences to inform prediction
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
    
    // Train prediction model using episodic memory patterns
    void train(const SensoryInput& observation);
    
    // Store prediction in episodic memory
    void storePrediction(const SensoryInput& prediction, const SensoryInput& actual, float error, float confidence);
    
    // Retrieve similar past predictions for pattern matching
    std::vector<const EpisodicMemoryItem*> retrieveSimilarPredictions(const SensoryInput& currentState) const;
    
    // Update episodic memory based on prediction outcomes
    void updateEpisodicMemoryBasedOnPrediction(const SensoryInput& currentState, const SensoryInput& predicted, const SensoryInput& actual);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
