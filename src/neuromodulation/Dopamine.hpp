#pragma once

#include "../core/Types/Types.hpp"
#include "../core/BasePimpl.hpp"
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
    
private:
    struct Impl;
    SimplePimpl<Dopamine, Impl> pImpl;
};

} // namespace nlm