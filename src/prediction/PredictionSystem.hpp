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
    
    // Integration methods for brain loop
    float updateWithObservation(const std::vector<float>& sensoryState, SimulationStep currentStep);
    void recordSensoryState(const std::vector<float>& state, SimulationStep step);
    std::unique_ptr<std::vector<float>> getRecentSensoryState(SimulationStep stepsAgo) const;
    void learnSequence(const std::vector<float>& fromState, 
                       const std::vector<float>& toState, 
                       SimulationStep currentStep);
    void updatePredictionsWithEpisode(const class EpisodicMemoryItem& episode);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
