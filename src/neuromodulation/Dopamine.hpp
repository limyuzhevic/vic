#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

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
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm