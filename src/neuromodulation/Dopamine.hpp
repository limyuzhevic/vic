#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine();
    
    void initialize(Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm