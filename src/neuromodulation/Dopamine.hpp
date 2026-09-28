#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine neuromodulator for reward prediction error and reinforcement learning
// IMPLEMENTED - Phase 2 implementation of real dopamine signaling

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Get current dopamine level (prediction error signal)
    float getLevel() const override;
    
    // Set dopamine level (used for testing)
    void setLevel(float level);
    
    // Update dopamine level based on reward and prediction error
    // phase: 0.0 = predict, 1.0 = act
    void update(double dt, float reward, float predictedReward);
    
    // Get plasticity factor (modulates STDP learning rates)
    float getPlasticityFactor() const;
    
    // Set plasticity factor
    void setPlasticityFactor(float factor);
    
    // Get dopamine level for neuromodulation effects
    float getDopamineLevel() const { return getLevel(); }
    
    // Reset to initial state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm