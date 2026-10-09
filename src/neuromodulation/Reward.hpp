#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation

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
    // Combines outcome reward, prediction error, and novelty signals
    // Integrates with neural prediction system and neuromodulation
    float computeReward(const class Observation& observation,
                       const PredictionError& predictionError,
                       const Curiosity& curiosity,
                       const Novelty& novelty,
                       const NeuralPrediction& predictionSystem,
                       float predictedReward = 0.0f,
                       float outcomeReward = 0.0f) const;
    
    // Configuration methods
    void setRewardWeights(float outcomeWeight, float predictionErrorWeight,
                         float noveltyWeight, float curiosityWeight, float temporalWeight);
    void setRewardScaling(float scalingFactor, float offset);
    void setPredictionConfidenceWeight(float weight);
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get reward components for debugging/analysis
    float getOutcomeRewardComponent() const;
    float getPredictionErrorRewardComponent() const;
    float getNoveltyRewardComponent() const;
    float getCuriosityRewardComponent() const;
    float getTemporalRewardComponent() const;
    
    // Get reward statistics
    float getMaxObservedReward() const;
    float getMinObservedReward() const;
    float getHistoricalMeanReward() const;
    
private:
    struct Impl;
    Impl* pImpl;
}
};

} // namespace nlm
