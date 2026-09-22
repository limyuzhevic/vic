#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Dopaminergic system implementation
// Integrates reward prediction error, reinforcement learning, and motivation

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const { return "DA"; }
    float getLevel() const { return level_; }
    void setLevel(float level);
    
    float getPlasticityFactor() const;
    void update(float dt);
    
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // ACh/NE/5-HT interactions
    void integrateAcetylcholine(float achLevel);
    void integrateNorepinephrine(float neLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // Memory modulation
    float getMemoryModulationFactor() const;
    float getAttentionModulationFactor() const;
    
    // State management
    void reset();
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
    float plasticityMod_;
    float memoryMod_;
    float attentionMod_;
    
    // Integration state
    float achInfluence_;
    float neInfluence_;
    float serotoninInfluence_;
    
    // Prediction tracking
    float lastPredictionError_;
    float predictionErrorHistory_[100];
    int historyIndex_;
};

} // namespace nlm
