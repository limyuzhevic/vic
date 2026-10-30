#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Enhanced with biological accuracy including release dynamics and receptor modulation

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Reward signaling with dopamine burst-pause dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Motor excitation modulation
    float getMotorExcitationFactor() const;
    void setMotorExcitationFactor(float factor);
    
    // Learning rate modulation
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Receptor dynamics (D1, D2, D3, D4, D5)
    void setReceptorAffinity(float d1, float d2);
    float getD1ReceptorEffect() const;
    float getD2ReceptorEffect() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm