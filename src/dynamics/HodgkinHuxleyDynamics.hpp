// HodgkinHuxleyDynamics.hpp
#pragma once

#include "NeuralDynamics.hpp"
#include <array>

namespace nlm {

// Hodgkin-Huxley dynamics implementation
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    // From NeuralDynamics base class
    void updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Hodgkin-Huxley specific parameters
    void setMembraneVoltage(float voltage) { membraneVoltage_ = voltage; }
    float getMembraneVoltage() const { return membraneVoltage_; }
    
    void setSodiumActivation(float m) { m_ = m; }
    float getSodiumActivation() const { return m_; }
    
    void setSodiumInactivation(float h) { h_ = h; }
    float getSodiumInactivation() const { return h_; }
    
    void setPotassiumActivation(float n) { n_ = n; }
    float getPotassiumActivation() const { return n_; }
    
    // Channel conductances
    void setSodiumConductance(float gNa) { gNa_ = gNa; }
    float getSodiumConductance() const { return gNa_; }
    
    void setPotassiumConductance(float gK) { gK_ = gK; }
    float getPotassiumConductance() const { return gK_; }
    
    void setLeakConductance(float gL) { gL_ = gL; }
    float getLeakConductance() const { return gL_; }
    
    // HH model parameters (biophysical constants)
    static constexpr float V_rest = -65.0f;      // Resting potential (mV)
    static constexpr float V_Na = 50.0f;        // Sodium reversal potential (mV)
    static constexpr float V_K = -77.0f;        // Potassium reversal potential (mV)
    static constexpr float V_L = -54.4f;        // Leak reversal potential (mV)
    
    static constexpr float gNa_max = 120.0f;    // Max sodium conductance (mS/cm²)
    static constexpr float gK_max = 36.0f;      // Max potassium conductance (mS/cm²)
    static constexpr float gL = 0.3f;           // Leak conductance (mS/cm²)
    
    static constexpr float Cm = 1.0f;           // Membrane capacitance (μF/cm²)
    
    // Time constants and rates
    static constexpr float alpha_m(V) { return 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f)); }
    static constexpr float beta_m(V) { return 4.0f * std::exp(-(V + 65.0f) / 18.0f); }
    static constexpr float alpha_h(V) { return 0.07f * std::exp(-(V + 65.0f) / 20.0f); }
    static constexpr float beta_h(V) { return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f)); }
    static constexpr float alpha_n(V) { return 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f)); }
    static constexpr float beta_n(V) { return 0.125f * std::exp(-(V + 65.0f) / 80.0f); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // State variables for HH model
    float membraneVoltage_;
    float m_;  // Sodium activation
    float h_;  // Sodium inactivation
    float n_;  // Potassium activation
};

} // namespace nlm
