#include "HodgkinHuxleyDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    // HH model parameters
    float capacitance;           // Membrane capacitance (μF/cm²)
    float membraneResistance;    // Membrane resistance (MΩ)
    float leakConductance;       // Leak conductance (mS/cm²)
    
    // Gating variables (dimensionless, 0-1 range)
    float m;                     // Sodium activation
    float h;                     // Sodium inactivation  
    float n;                     // Potassium activation
    
    // Current variables
    float I_Na;                  // Sodium current
    float I_K;                   // Potassium current
    float I_L;                   // Leak current
    
    // State variables
    float V;                     // Membrane potential (mV)
    float I_stim;                // External stimulus current
    
    Impl() 
        : capacitance(1.0f), membraneResistance(10.0f), leakConductance(0.3f)
        , m(0.05f), h(0.6f), n(0.32f)
        , I_Na(0.0f), I_K(0.0f), I_L(0.0f)
        , V(-65.0f), I_stim(0.0f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    pImpl->V = V;
    pImpl->I_stim = neuron->getTotalCurrent();
    
    // Update gating variables using Euler integration
    float alpha_m = (2.5f - 0.1f * V) / (exp((2.5f - V) / 10.0f) - 1.0f);
    float beta_m = 4.0f * exp(-V / 18.0f);
    float alpha_h = 0.07f * exp(-V / 20.0f);
    float beta_h = 1.0f / (exp((30.0f - V) / 10.0f) + 1.0f);
    float alpha_n = (0.1f - 0.01f * V) / (exp((1.0f - V) / 10.0f) - 1.0f);
    float beta_n = 0.125f * exp(-V / 80.0f);
    
    // Correct for numerical stability when denominator is zero
    if (std::abs(2.5f - 0.1f * V) < 1e-6f) {
        alpha_m = 0.2f;
    }
    if (std::abs(1.0f - V) < 1e-6f) {
        alpha_n = 0.1f;
    }
    
    // Update gating variables
    pImpl->m += dt * (alpha_m * (1.0f - pImpl->m) - beta_m * pImpl->m);
    pImpl->h += dt * (alpha_h * (1.0f - pImpl->h) - beta_h * pImpl->h);
    pImpl->n += dt * (alpha_n * (1.0f - pImpl->n) - beta_n * pImpl->n);
    
    // Clamp gating variables to valid range
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    
    // Calculate ion currents (Hodgkin-Huxley equations)
    pImpl->I_Na = 120.0f * pImpl->m * pImpl->m * pImpl->m * pImpl->h * (V - 50.0f);
    pImpl->I_K = 36.0f * pImpl->n * pImpl->n * pImpl->n * (V - (-12.0f));
    pImpl->I_L = 0.3f * (V - (-60.0f));
    
    // Total current (including external stimulus)
    float I_total = pImpl->I_Na + pImpl->I_K + pImpl->I_L + pImpl->I_stim;
    
    // Update membrane potential using Euler integration
    // dV/dt = I_total / (C_m * R_m)
    float dV = pImpl->I_total / (pImpl->capacitance * pImpl->membraneResistance);
    V += dt * dV;
    
    // Clamp to reasonable bounds
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update neuron state
    neuron->setMembranePotential(V);
    neuron->clearTotalCurrent();
    
    // Check for spike
    if (V >= 10.0f) {  // Spike threshold
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // HH synaptic dynamics: alpha and beta functions
    float V = synapse->getPostSynapticPotential();
    
    // Calcium-dependent potassium current modulation
    float alpha = 0.1f * (V + 50.0f) / (1.0f - exp(-(V + 50.0f) / 10.0f));
    float beta = 0.1f * (50.0f - V) / (exp((50.0f - V) / 10.0f) - 1.0f);
    
    // Update synaptic variables
    float s = synapse->getSynapticVariable();  // For Gsyn
    s += dt * (alpha * (1.0f - s) - beta * s);
    s = std::clamp(s, 0.0f, 1.0f);
    
    // Apply synaptic conductance modulation
    synapse->setSynapticConductance(s);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Spike-induced synaptic input with HH kinetics
    float weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 0.0f : (-80.0f);
    
    // Calculate synaptic conductance based on spike timing
    float g_syn = weight * 0.1f;  // Synaptic conductance
    float I_syn = g_syn * (neuron->getMembranePotential() - reversalPotential);
    
    // Add to neuron's total current
    neuron->receiveExcitatoryInput(I_syn);
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= 10.0f;
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH internal state
    pImpl->m = 0.05f;
    pImpl->h = 0.6f;
    pImpl->n = 0.32f;
    pImpl->I_Na = 0.0f;
    pImpl->I_K = 0.0f;
    pImpl->I_L = 0.0f;
    pImpl->V = -65.0f;
    pImpl->I_stim = 0.0f;
}

void HodgkinHuxleyDynamics::setCapacitance(float capacitance) {
    pImpl->capacitance = capacitance;
}

float HodgkinHuxleyDynamics::getCapacitance() const {
    return pImpl->capacitance;
}

void HodgkinHuxleyDynamics::setMembraneResistance(float resistance) {
    pImpl->membraneResistance = resistance;
}

float HodgkinHuxleyDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

void HodgkinHuxleyDynamics::setLeakConductance(float conductance) {
    pImpl->leakConductance = conductance;
}

float HodgkinHuxleyDynamics::getLeakConductance() const {
    return pImpl->leakConductance;
}

} // namespace nlm
