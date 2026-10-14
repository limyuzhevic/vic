#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2 will implement real reward computation
// 
// The Reward class provides the foundation for reinforcement learning in the NLM.
// It computes reward signals from environmental states and provides feedback
// to guide learning and behavior optimization.
// 
// Key features:
// - Reward computation from environmental observations
// - Reward prediction error calculation for temporal difference learning
// - Integration with dopamine neuromodulation system
// - Reward history tracking for analysis and debugging
// - Reward normalization and scaling for stable learning
// 
// Biological inspiration:
// - Reward prediction errors in dopamine neurons
// - Temporal difference learning in basal ganglia
// - Reward magnitude and probability coding
// - Reward learning and extinction
// - Incentive salience and motivation
// 
// Phase 2 Implementation:
// - Real-time reward computation from sensory inputs
// - Reward prediction error signaling with dopamine
// - Temporal difference learning algorithms
// - Reward-based synaptic plasticity
// - Exploration-exploitation trade-off control
// - Reward-based action selection and policy learning

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
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward prediction error computation
    float computePredictionError(float actualReward, float predictedReward);
    
    // Temporal difference learning
    void updatePrediction(float predictionError, float learningRate);
    
    // Reward history management
    const std::vector<float>& getHistory() const;
    void clearHistory();
    size_t getHistorySize() const;
    
    // Reward statistics
    float getMaxReward() const;
    float getMinReward() const;
    float getAverageReward() const;
    float getStandardDeviation() const;
    
    // Reward processing
    void normalizeRewards(float minReward = 0.0f, float maxReward = 1.0f);
    void scaleRewards(float scaleFactor);
    
    // Reward-based learning control
    void enableRewardLearning(bool enable);
    bool isRewardLearningEnabled() const;
    
    // Integration with neuromodulation
    void setDopamineIntegration(bool enable);
    bool hasDopamineIntegration() const;
    
    // Reward-based action selection
    float getExplorationBonus() const;
    void setExplorationBonus(float bonus);
    
    // Reward validation
    bool validateReward(float reward) const;
    
private:
    struct Impl;
    Impl* pImpl;
};
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
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
