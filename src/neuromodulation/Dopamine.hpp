#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Memory consolidation effects
    void enhanceMemoryConsolidation(float strength);
    
    // Learning modulation
    void modulateSTDP(float predictedError);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
