#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Modulation effects
    void applyToExcitability(float& restingPotential) const;
    void applyToLearningRate(float& learningRate) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm