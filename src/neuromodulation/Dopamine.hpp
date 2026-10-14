#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Dopamine: Reward prediction error and reinforcement learning signal
// Real dopamine signaling with temporal difference learning
// 
// Dopamine neurons encode reward prediction errors (RPEs), which represent the difference
// between expected and actual rewards. This is fundamental for reinforcement learning
// and enables the brain to update value functions and adapt behavior.
// 
// Key features:
// - Reward prediction error computation: δ = r + γV(s') - V(s)
// - Temporal difference learning: V(s) ← V(s) + α × δ
// - Phasic vs tonic dopamine signaling modes
// - Neuromodulation of synaptic plasticity through dopamine concentration
// - Integration with reward-modulated STDP for enhanced learning
// 
// Mathematical formulation:
// Δw = (1 + β × δ) × exp(-Δt / τ) for potentiation
// Δw = (1 - β × |δ|) × exp(Δt / τ) for depression
// 
// Where:
// - δ = reward prediction error
// - β = reward sensitivity parameter
// - τ = STDP time constant
// - Δw = weight change
// 
// Biological inspiration:
// - Dopamine neurons in ventral tegmental area (VTA)
// - Response to unexpected rewards (positive prediction error)
// - Suppression to omitted rewards (negative prediction error)
// - Role in reinforcement learning and habit formation
// 
// Phase 2 Implementation:
// - Real dopamine concentration dynamics with decay and release
// - Reward prediction error computation based on temporal difference
// - Plasticity factor modulation based on dopamine levels
// - Integration with reward-modulated STDP rules
// - Behavioral output generation through motor systems

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
    Impl* pImpl;
};
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
    Impl* pImpl;
};

} // namespace nlm
