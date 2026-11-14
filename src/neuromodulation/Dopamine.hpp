#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

class Dopamine;

// ===== DA: REALISTIC REWARD AND REINFORCEMENT LEARNING =====
// Implements biologically realistic reward processing and dopamine signaling
// Based on temporal difference learning and biological dopamine dynamics

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get reward prediction error
    float getError() const { return pImpl->error; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm