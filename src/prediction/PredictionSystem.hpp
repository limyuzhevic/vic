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
    
    // Connect episodic memory for pattern completion and prediction
    void setEpisodicMemory(NeuralEpisodicMemory* episodicMemory);
    
    // Get episodes similar to current pattern for prediction
    std::vector<const EpisodicMemoryItem*> getSimilarEpisodes(const std::vector<float>& pattern, size_t maxResults = 5) const;
    
    // Make prediction based on episodic memory patterns
    std::unique_ptr<SensoryInput> predictFromEpisodicMemory(const std::vector<float>& currentPattern) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
