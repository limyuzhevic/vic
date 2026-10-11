#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Integrated neuromodulator that affects neural excitability and plasticity
// Based on computational neuroscience models of dopaminergic signaling

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current dopamine level (normalized -1 to 1)
    float getLevel() const;
    
    // Set dopamine level
    void setLevel(float level);
    
    // Get plasticity modulation factor (1.0 = baseline)
    float getPlasticityFactor() const;
    
    // Update dopamine state
    void update(TimestepDuration dt);
    
    // Signal reward (positive) or prediction error (can be negative)
    void signalReward(float reward);
    void signalPredictionError(float error);
    
    // Get error magnitude for internal computation
    float getPredictionError() const;
    
    // Get reward value
    float getReward() const;
    
    // Reset state
    void reset();
    
    // Get history for analysis
    const std::vector<float>& getLevelHistory() const;
    const std::vector<float>& getErrorHistory() const;
    const std::vector<float>& getRewardHistory() const;
    void clearHistory();
    
    // Set neuromodulation parameters
    void setLearningRate(float rate);
    void setEvaporationRate(float rate);
    void setResponseGain(float gain);
    
    // Get current state
    float getLearningRate() const;
    float getEvaporationRate() const;
    float getResponseGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
