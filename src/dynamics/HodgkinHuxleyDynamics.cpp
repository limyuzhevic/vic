// HodgkinHuxleyDynamics.cpp implementation
#include "HodgkinHuxleyDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    float Cm;                    // Membrane capacitance
    float gNa_max;               // Max sodium conductance
    float gK_max;                // Max potassium conductance
    float gL;                    // Leak conductance
    
    // Currents
    float I_Na;                 // Sodium current
    float I_K;                  // Potassium current
    float I_L;                  // Leak current
    float I_ext;                // External current
    
    // Time constants and rates
    float dm_dt;                // dm/dt
    float dh_dt;                // dh/dt
    float dn_dt;                // dn/dt
    
    // Stability parameters
    float voltageSlope;         // For numerical stability
    float maxVoltageChange;     // Maximum voltage change per timestep
    
    Impl() : Cm(1.0f), gNa_max(120.0f), gK_max(36.0f), gL(0.3f),
             I_Na(0.0f), I_K(0.0f), I_L(0.0f), I_ext(0.0f),
             dm_dt(0.0f), dh_dt(0.0f), dn_dt(0.0f),
             voltageSlope(1.0f), maxVoltageChange(100.0f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : 
    pImpl(new Impl()),
    membraneVoltage_(-65.0f), m_(0.05f), h_(0.6f), n_(0.32f),
    gNa_(120.0f), gK_(36.0f), gL_(0.3f) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) {
    const float dt_sec = static_cast<float>(dt) / 1000.0f;  // Convert to seconds
    
    // Get current state
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    // External current (from current injection)
    float I_ext = neuron->getTotalCurrent();
    pImpl->I_ext = I_ext;
    
    // Update gating variables using forward Euler method
    // dm/dt = alpha_m(V)*(1-m) - beta_m(V)*m
    // dh/dt = alpha_h(V)*(1-h) - beta_h(V)*h  
    // dn/dt = alpha_n(V)*(1-n) - beta_n(V)*n
    
    float alpha_m = 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    float beta_m = 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    
    float alpha_h = 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    float beta_h = 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    
    float alpha_n = 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    float beta_n = 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    
    // Update gating variables
    m_ += (alpha_m * (1.0f - m_) - beta_m * m_) * dt_sec;
    h_ += (alpha_h * (1.0f - h_) - beta_h * h_) * dt_sec;
    n_ += (alpha_n * (1.0f - n_) - beta_n * n_) * dt_sec;
    
    // Clamp gating variables to [0, 1]
    m_ = std::clamp(m_, 0.0f, 1.0f);
    h_ = std::clamp(h_, 0.0f, 1.0f);
    n_ = std::clamp(n_, 0.0f, 1.0f);
    
    // Calculate ionic currents
    // I_Na = gNa * m^3 * h * (V - V_Na)
    // I_K = gK * n^4 * (V - V_K)
    // I_L = gL * (V - V_L)
    
    float I_Na = gNa_ * m_ * m_ * m_ * h_ * (V - V_Na);
    float I_K = gK_ * n_ * n_ * n_ * n_ * (V - V_K);
    float I_L = gL_ * (V - V_L);
    
    pImpl->I_Na = I_Na;
    pImpl->I_K = I_K;
    pImpl->I_L = I_L;
    
    // Total ionic current
    float I_ion = I_Na + I_K + I_L;
    
    // Membrane potential equation
    // Cm * dV/dt = -I_ion + I_ext - I_leak
    // dV/dt = (-I_ion + I_ext) / Cm
    
    float dV_dt = (-I_ion + I_ext) / pImpl->Cm;
    
    // Limit voltage change for stability
    float maxVoltageChange = pImpl->maxVoltageChange * dt_sec;
    float dV = std::clamp(dV_dt * dt_sec, -maxVoltageChange, maxVoltageChange);
    
    // Update membrane potential
    V += dV;
    neuron->setMembranePotential(V);
    
    // Store state for external access
    membraneVoltage_ = V;
    
    // Check for spike (threshold crossing)
    if (V >= neuron->getThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(currentTime);  // FIXED: Pass actual time
        
        // After spike, reset gating variables to specific values
        // (Hodgkin-Huxley model has specific recovery dynamics)
        h_ = 0.295f;  // After spike, h decreases rapidly
        // m and n continue their natural dynamics
        
        // Set membrane potential to threshold (or reset to specific value)
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synaptic dynamics for HH neurons
    // In HH model, synaptic transmission affects membrane potential directly
    // rather than conductance changes
    
    const float tau_syn = 5.0f;  // Synaptic time constant (ms)
    float decayFactor = std::exp(-static_cast<float>(dt) / tau_syn);
    
    // Get current synaptic conductance
    float g_syn = synapse->getConductance();
    
    // Apply decay
    g_syn *= decayFactor;
    synapse->setConductance(g_syn);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // For HH neurons, spike input affects membrane potential directly
    // through conductance changes
    
    SynapticWeight weight = synapse->getWeight();
    float conductance = synapse->getConductance();
    
    if (synapse->isExcitatory()) {
        // Excitatory input: increase conductance
        float deltaConductance = weight * 0.05f;  // Larger effect for HH
        synapse->setConductance(conductance + deltaConductance);
        
        // Add to neuron's synaptic conductance
        neuron->addSynapticConductance(deltaConductance);
        
        // Also add to total current (simplified model)
        neuron->receiveExcitatoryInput(deltaConductance * 10.0f);
    } else {
        // Inhibitory input: increase conductance
        float deltaConductance = std::abs(weight) * 0.05f;
        synapse->setConductance(conductance + deltaConductance);
        
        // Inhibitory neurons hyperpolarize postsynaptic neuron
        neuron->receiveInhibitoryInput(deltaConductance * 10.0f);
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    // For HH neurons, firing is determined by membrane potential crossing threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH model state variables
    membraneVoltage_ = V_rest;
    m_ = 0.05f;   // Steady-state at rest
    h_ = 0.6f;    // Steady-state at rest
    n_ = 0.32f;   // Steady-state at rest
    
    // Reset currents
    pImpl->I_Na = 0.0f;
    pImpl->I_K = 0.0f;
    pImpl->I_L = 0.0f;
    pImpl->I_ext = 0.0f;
}

// Setters and getters for HH-specific parameters
void HodgkinHuxleyDynamics::setSodiumConductance(float gNa) { gNa_ = gNa; }
float HodgkinHuxleyDynamics::getSodiumConductance() const { return gNa_; }

void HodgkinHuxleyDynamics::setPotassiumConductance(float gK) { gK_ = gK; }
float HodgkinHuxleyDynamics::getPotassiumConductance() const { return gK_; }

void HodgkinHuxleyDynamics::setLeakConductance(float gL) { gL_ = gL; }
float HodgkinHuxleyDynamics::getLeakConductance() const { return gL_; }

// Getters for gating variables
float HodgkinHuxleyDynamics::getMembraneVoltage() const { return membraneVoltage_; }
float HodgkinHuxleyDynamics::getSodiumActivation() const { return m_; }
float HodgkinHuxleyDynamics::getSodiumInactivation() const { return h_; }
float HodgkinHuxleyDynamics::getPotassiumActivation() const { return n_; }

} // namespace nlm
