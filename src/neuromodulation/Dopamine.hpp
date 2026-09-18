#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Enhanced Dopamine: Realistic reward prediction error system
// Implements temporal difference learning and biological dopamine dynamics

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling and prediction error computation
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Learning system components
    float getPredictionError() const;
    float getExpectedValue() const;
    void setExpectedValue(float value);
    
    // Learning parameters
    void setLearningRate(float rate);
    void setNeuromodulationStrength(float strength);
    
    // Reset system state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
