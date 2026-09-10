#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Integrated with working memory and episodic memory for context-aware predictions

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Make prediction for next timestep based on working memory and episodic memory
    // Takes current working memory state and relevant episodes as context
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState,
                                                  const std::vector<float>& workingMemoryPattern,
                                                  const std::vector<const EpisodicMemoryItem*>& relevantEpisodes);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update prediction error signal
    void updatePredictionError(float error, TimestepDuration dt);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using episode patterns
    void trainFromEpisode(const EpisodicMemoryItem& episode);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
