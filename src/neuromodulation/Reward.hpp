#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "PredictionError.hpp"

namespace nlm {

// Enhanced Reward signal for reinforcement learning
// Now integrates with prediction error and novelty detection
// Implements real reward prediction error (RPE) computation
// Provides exploration bonuses and decay mechanisms

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward();
    
    // Neuromodulator interface
    void update(TimestepDuration dt) override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Enhanced version with prediction error integration
    float computeReward(const Observation& observation) const;
    
    // Enhanced reward computation with prediction error
    void integratePredictionError(float predictionError);
    void integrateNovelty(float novelty, TimestepDuration dt);
    void integratePrediction(float predictedValue, float actualValue);
    void applyExplorationBonus(float novelty, TimestepDuration dt);
    
    // Set brain reference for integration
    void setBrain(class Brain* brain);
    
    // Reward history and statistics
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Decay control
    void resetDecay();
    void increaseDecay();
    
    // Get prediction error contribution
    float getPredictionErrorContribution() const;
    
    // Get novelty contribution
    float getNoveltyContribution() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
