// FitzHughNagumoDynamics.hpp
#pragma once

#include "NeuralDynamics.hpp"

namespace nlm {

// FitzHugh-Nagumo dynamics implementation
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    // From NeuralDynamics base class
    void updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // FitzHugh-Nagumo specific parameters
    void setRestingPotential(float v0) { v0_ = v0; }
    float getRestingPotential() const { return v0_; }
    
    void setTimeConstant(float tau) { tau_ = tau; }
    float getTimeConstant() const { return tau_; }
    
    void setThreshold(float threshold) { threshold_ = threshold; }
    float getThreshold() const { return threshold_; }
    
    void setRecoveryRate(float epsilon) { epsilon_ = epsilon; }
    float getRecoveryRate() const { return epsilon_; }
    
    void setRecoverySlope(float a) { a_ = a; }
    float getRecoverySlope() const { return a_; }
    
    void setRecoveryOffset(float b) { b_ = b; }
    float getRecoveryOffset() const { return b_; }
    
    // Get state variables
    float getMembranePotential() const { return V_; }
    float getRecoveryVariable() const { return w_; }
    
    // FitzHugh-Nagumo model parameters (biological values)
    static constexpr float V_rest = -1.5f;      // Resting potential (dimensionless)
    static constexpr float tau = 12.5f;        // Time constant (ms)
    static constexpr float epsilon = 0.08f;    // Recovery rate
    static constexpr float a = 0.7f;           // Recovery slope
    static constexpr float b = 0.8f;           // Recovery offset
    static constexpr float threshold = 0.5f;   // Firing threshold
    
    // External current parameters
    static constexpr float I_ext_min = -2.0f;  // Minimum external current
    static constexpr float I_ext_max = 2.0f;   // Maximum external current
    
private:
    struct Impl;
    Impl* pImpl;
    
    // State variables for FHN model
    float V_;    // Membrane potential
    float w_;    // Recovery variable
    
    // Integration helpers
    float clamp(float value, float min, float max) const {
        return std::max(min, std::min(max, value));
    }
};

} // namespace nlm
