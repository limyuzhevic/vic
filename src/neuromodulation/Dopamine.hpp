#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Real dopamine neuromodulation system with phasic and tonic components

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getEligibilityTrace() const override;
    void updateEligibilityTrace(float delta) override;
    
    // Reward signaling and prediction error computation
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get burst information for external observers
    float getBurstIntensity() const;
    Timestamp getLastBurstTime() const;
    
    // Configuration methods for research
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayTimeConstant(float tau);
    void setRewardGain(float gain);
    void setPredictionErrorGain(float gain);
    
    // Get current configuration
    float getBaseline() const;
    float getPeak() const;
    float getDecayTimeConstant() const;
    float getRewardGain() const;
    float getPredictionErrorGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm