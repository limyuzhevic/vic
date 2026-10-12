#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <vector>

namespace nlm {

// Reward signal for reinforcement learning
// REAL IMPLEMENTATION - Phase 2 complete

class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Integrates prediction error, novelty, outcome, consolidation, and exploration
    float computeReward(const Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward component computation methods
    float computePredictionErrorReward(float predictionError, 
                                       float predictionConfidence,
                                       float dopamine) const;
    float computeNoveltyReward(float novelty, float curiosity,
                              const std::vector<float>& sensoryState) const;
    float computeOutcomeReward(const Observation& observation,
                               float predictionError,
                               float dopamine) const;
    float computeConsolidationReward(class NeuralEpisodicMemory* episodicMemory,
                                     float dopamine) const;
    float computeExplorationReward(float curiosity, float predictionError,
                                   const std::vector<float>& sensoryState) const;
    
    // Reward statistics and analysis
    void initialize(class Brain* brain);
    float getPredictionErrorRewardAverage() const;
    float getNoveltyRewardAverage() const;
    float getOutcomeRewardAverage() const;
    float getConsolidationRewardAverage() const;
    float getExplorationRewardAverage() const;
    float getTotalRewardAverage() const;
    
    // Integration with episodic memory
    void signalMemoryConsolidation(const EpisodicMemoryItem& episode);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Reference to brain for system integration
    class Brain* brain_;
};

} // namespace nlm
