#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;             // nF
    
    // Gating variables for HH model
    float m;  // Sodium activation
    float h;  // Sodium inactivation
    float n;  // Potassium activation
    float m_inf, h_inf, n_inf;     // Steady-state values
    float tau_m, tau_h, tau_n;    // Time constants
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    /*
     * Hodgkin-Huxley dynamics implementation
     * 
     * Real biological model of action potential generation based on:
     * - Voltage-gated sodium channels (activation m, inactivation h)
     * - Voltage-gated potassium channels (activation n)
     * - Leak channels
     * 
     * Mathematical formulation:
     * C * dV/dt = I_leak + I_Na + I_K + I_stim - I_app
     * 
     * Where:
     *   I_leak = g_L * (V - E_L)
     *   I_Na = g_Na * m^3 * h * (V - E_Na)
     *   I_K = g_K * n^4 * (V - E_K)
     * 
     * Gating variables follow:
     *   dx/dt = alpha(x) * (1 - x) - beta(x) * x
     *   x_inf = alpha(x) / (alpha(x) + beta(x))
     *   tau_x = 1 / (alpha(x) + beta(x))
     * 
     * Biological parameters (from original HH model):
     *   C_m = 1 μF/cm²
     *   g_Na = 120 mS/cm², E_Na = 115 mV
     *   g_K = 36 mS/cm², E_K = -12 mV  
     *   g_L = 0.3 mS/cm², E_L = 10.613 mV
     */
    
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float I_stim = neuron->getTotalCurrent();
    
    // Convert neuron voltage to mV if needed (HH model uses mV)
    if (V < -100.0f) {  // Assuming typical resting potential around -70mV
        V = V * 1000.0f;  // Convert from V to mV
    }
    
    // Get current HH parameters
    float g_Na = 120.0f;      // Sodium conductance (mS/cm²)
    float g_K = 36.0f;        // Potassium conductance (mS/cm²)  
    float g_L = 0.3f;         // Leak conductance (mS/cm²)
    float E_Na = 115.0f;      // Sodium reversal potential (mV)
    float E_K = -12.0f;       // Potassium reversal potential (mV)
    float E_L = 10.613f;      // Leak reversal potential (mV)
    
    // Compute gate variables using alpha/beta functions (HH model equations)
    float V_mV = V;  // Already in mV
    
    // Sodium activation (m)
    float alpha_m = 0.1f * (V_mV + 40.0f) / (1.0f - std::exp(-(V_mV + 40.0f) / 10.0f));
    float beta_m = 4.0f * std::exp(-(V_mV + 65.0f) / 18.0f);
    pImpl->m_inf = alpha_m / (alpha_m + beta_m);
    pImpl->tau_m = 1.0f / (alpha_m + beta_m);
    
    // Sodium inactivation (h)
    float alpha_h = 0.07f * std::exp(-(V_mV + 65.0f) / 20.0f);
    float beta_h = 1.0f / (std::exp(-(V_mV + 35.0f) / 10.0f) + 1.0f);
    pImpl->h_inf = alpha_h / (alpha_h + beta_h);
    pImpl->tau_h = 1.0f / (alpha_h + beta_h);
    
    // Potassium activation (n)
    float alpha_n = 0.01f * (V_mV + 55.0f) / (1.0f - std::exp(-(V_mV + 55.0f) / 10.0f));
    float beta_n = 0.125f * std::exp(-(V_mV + 65.0f) / 80.0f);
    pImpl->n_inf = alpha_n / (alpha_n + beta_n);
    pImpl->tau_n = 1.0f / (alpha_n + beta_n);
    
    // Update gating variables
    float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    pImpl->m += dt_ms * ((pImpl->m_inf - pImpl->m) / pImpl->tau_m);
    pImpl->h += dt_ms * ((pImpl->h_inf - pImpl->h) / pImpl->tau_h);
    pImpl->n += dt_ms * ((pImpl->n_inf - pImpl->n) / pImpl->tau_n);
    
    // Clamp to [0,1] range
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    
    // Compute currents
    float I_Na = g_Na * std::pow(pImpl->m, 3) * pImpl->h * (V_mV - E_Na);
    float I_K = g_K * std::pow(pImpl->n, 4) * (V_mV - E_K);
    float I_L = g_L * (V_mV - E_L);
    
    // Total membrane current
    float I_total = I_leak + I_Na + I_K + I_stim * 1000.0f;  // Convert I_stim to mA
    
    // Update membrane potential
    float dV = I_total / (capacitance * 1000.0f);  // Convert to appropriate units
    V += dV * static_cast<float>(dt);
    neuron->setMembranePotential(V);
    
    // Check for firing (spike threshold)
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update synapse for HH model (similar to IntegrateAndFire)
    synapse->step(0.0);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Real synaptic input based on HH model
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(static_cast<MembranePotential>(weight));
    } else {
        neuron->receiveInhibitoryInput(std::abs(static_cast<MembranePotential>(weight)));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron && neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->m = pImpl->m_inf;
    pImpl->h = pImpl->h_inf;
    pImpl->n = pImpl->n_inf;
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm