#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Episode for prediction system
struct PredictionEpisode {
    SimulationStep timestamp;
    std::vector<float> sensoryPattern;
    std::vector<float> motorPattern;
    float reward;
    float predictionError;
};

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Update prediction system
    void update(TimestepDuration dt);
    
    // Make prediction for next timestep
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
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
    
    // Incorporate episodic memory into predictions
    void incorporateMemoryContext(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // Update prediction error based on neuromodulation
    void updatePredictionError(float error, float confidence);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
