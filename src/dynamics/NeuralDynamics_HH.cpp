#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics() {
        reset();
    }
    
    ~HodgkinHuxleyDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        if (!neuron) return;
        
        const NeuronState& state = neuron->getState();
        float V = neuron->getMembranePotential();
        
        // Compute gating variables using Euler integration
        float V_norm = V / 100.0f; // Convert to normalized units
        
        // m-gate (Na+ activation)
        float alpha_m = (0.1f * (V_norm + 40.0f)) / (1.0f - std::exp(-(V_norm + 40.0f) / 10.0f));
        float beta_m = 4.0f * std::exp(-(V_norm + 65.0f) / 18.0f);
        float dm_dt = alpha_m * (1.0f - m_) - beta_m * m_;
        m_ += dm_dt * dt * 0.001f; // dt in ms
        m_ = std::clamp(m_, 0.0f, 1.0f);
        
        // h-gate (Na+ inactivation)
        float alpha_h = 0.07f * std::exp(-(V_norm + 65.0f) / 20.0f);
        float beta_h = 1.0f / (1.0f + std::exp(-(V_norm + 35.0f) / 10.0f));
        float dh_dt = alpha_h * (1.0f - h_) - beta_h * h_;
        h_ += dh_dt * dt * 0.001f;
        h_ = std::clamp(h_, 0.0f, 1.0f);
        
        // n-gate (K+ activation)
        float alpha_n = (0.01f * (V_norm + 55.0f)) / (1.0f - std::exp(-(V_norm + 55.0f) / 10.0f));
        float beta_n = 0.125f * std::exp(-(V_norm + 65.0f) / 80.0f);
        float dn_dt = alpha_n * (1.0f - n_) - beta_n * n_;
        n_ += dn_dt * dt * 0.001f;
        n_ = std::clamp(n_, 0.0f, 1.0f);
        
        // Compute ionic currents
        float I_Na = g_Na_ * std::pow(m_, 3) * h_ * (V_norm - V_Na_);
        float I_K = g_K_ * std::pow(n_, 4) * (V_norm - V_K_);
        float I_L = g_L_ * (V_norm - V_L_);
        
        // Get synaptic input from neuron
        float I_syn = neuron->getTotalCurrent() / C_m_ * 1000.0f; // Convert to appropriate units
        
        // Total ionic current (negative for inward currents)
        float I_ion = -(I_Na + I_K + I_L);
        
        // Update membrane potential: dV/dt = -(I_ion - I_syn) / C_m
        float dV_dt = -(I_ion - I_syn) / C_m_;
        
        // Euler integration
        V += dV_dt * dt * 0.001f; // dt in ms
        
        // Update neuron state
        neuron->setMembranePotential(V * 100.0f); // Convert back to mV
        
        // Check for spike threshold
        if (neuron->checkThreshold()) {
            neuron->recordSpike(0.0); // TODO: pass actual time
            neuron->setFiringState(FiringState::Active);
        }
        
        // Clear synaptic current
        neuron->clearTotalCurrent();
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        if (!synapse) return;
        
        // Simple synaptic dynamics for HH
        // Apply exponential decay to synaptic efficacy
        float currentEfficacy = synapse->getEfficacy();
        float decay = std::exp(-dt * 0.001f / tau_s_); // Convert dt to ms
        float newEfficacy = currentEfficacy * decay;
        synapse->setEfficacy(newEfficacy);
        
        // Update eligibility trace for plasticity
        float eligibility = synapse->getEligibilityTrace();
        eligibility *= decay;
        synapse->setEligibilityTrace(eligibility);
    }
    
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {
        if (!neuron || !synapse) return;
        
        // Apply synaptic input based on synapse type and weight
        SynapticWeight weight = synapse->getWeight();
        
        // HH-specific synaptic scaling
        float synWeight = weight * synapse->getEfficacy();
        
        if (synapse->isExcitatory()) {
            // Excitatory synapses increase membrane potential
            float excitatoryInput = synWeight * 10.0f; // Scale factor
            neuron->receiveExcitatoryInput(excitatoryInput);
        } else if (synapse->isInhibitory()) {
            // Inhibitory synapses decrease membrane potential
            float inhibitoryInput = synWeight * 10.0f; // Scale factor
            neuron->receiveInhibitoryInput(inhibitoryInput);
        }
    }
    
    bool shouldFire(const Neuron* neuron) const override {
        if (!neuron) return false;
        return neuron->checkThreshold();
    }
    
    void reset() override {
        // Reset gating variables to steady-state at resting potential
        m_ = compute_m_gate(state_resting_);
        h_ = compute_h_gate(state_resting_);
        n_ = compute_n_gate(state_resting_);
        
        // Reset internal state
        last_spike_time_ = -1.0f;
        adaptation_counter_ = 0;
    }
    
    // HH parameter configuration methods
    void setMembraneCapacitance(float C_m) { C_m_ = C_m; }
    void setMaxSodiumConductance(float g_Na) { g_Na_ = g_Na; }
    void setMaxPotassiumConductance(float g_K) { g_K_ = g_K; }
    void setLeakConductance(float g_L) { g_L_ = g_L; }
    void setSodiumReversalPotential(float V_Na) { V_Na_ = V_Na; }
    void setPotassiumReversalPotential(float V_K) { V_K_ = V_K; }
    void setLeakReversalPotential(float V_L) { V_L_ = V_L; }
    void setSodiumActivationTimeConstant(float tau_m) { tau_m_ = tau_m; }
    void setSodiumInactivationTimeConstant(float tau_h) { tau_h_ = tau_h; }
    void setPotassiumActivationTimeConstant(float tau_n) { tau_n_ = tau_n; }
    void setSynapticTimeConstant(float tau_s) { tau_s_ = tau_s; }
    
    // Get parameter values
    float getMembraneCapacitance() const { return C_m_; }
    float getMaxSodiumConductance() const { return g_Na_; }
    float getMaxPotassiumConductance() const { return g_K_; }
    float getLeakConductance() const { return g_L_; }
    float getSodiumReversalPotential() const { return V_Na_; }
    float getPotassiumReversalPotential() const { return V_K_; }
    float getLeakReversalPotential() const { return V_L_; }
    
private:
    // Hodgkin-Huxley gating variables
    float m_;   // Na+ activation gate
    float h_;   // Na+ inactivation gate
    float n_;   // K+ activation gate
    
    // Model parameters (standard HH values)
    float C_m_;   // Membrane capacitance (μF/cm²)
    float g_Na_;  // Max Na+ conductance (mS/cm²)
    float g_K_;   // Max K+ conductance (mS/cm²)
    float g_L_;   // Leak conductance (mS/cm²)
    
    float V_Na_;  // Na+ reversal potential (mV)
    float V_K_;   // K+ reversal potential (mV)
    float V_L_;   // Leak reversal potential (mV)
    
    float tau_m_; // Na+ activation time constant (ms)
    float tau_h_; // Na+ inactivation time constant (ms)
    float tau_n_; // K+ activation time constant (ms)
    float tau_s_; // Synaptic time constant (ms)
    
    float state_resting_;  // Steady-state membrane potential (mV)
    
    // Internal state tracking
    float last_spike_time_;
    int adaptation_counter_;
    
    // Helper functions for steady-state gating variables
    float compute_m_gate(float V) const {
        float V_norm = V / 100.0f;
        return alpha_m(V_norm) / (alpha_m(V_norm) + beta_m(V_norm));
    }
    
    float compute_h_gate(float V) const {
        float V_norm = V / 100.0f;
        return alpha_h(V_norm) / (alpha_h(V_norm) + beta_h(V_norm));
    }
    
    float compute_n_gate(float V) const {
        float V_norm = V / 100.0f;
        return alpha_n(V_norm) / (alpha_n(V_norm) + beta_n(V_norm));
    }
    
    // Alpha/beta rate functions (standard HH equations)
    float alpha_m(float V) const {
        return (0.1f * (V + 40.0f)) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    }
    
    float beta_m(float V) const {
        return 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    }
    
    float alpha_h(float V) const {
        return 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    }
    
    float beta_h(float V) const {
        return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    }
    
    float alpha_n(float V) const {
        return (0.01f * (V + 55.0f)) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    }
    
    float beta_n(float V) const {
        return 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    }
};

} // namespace nlm