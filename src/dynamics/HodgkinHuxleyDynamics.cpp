#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// Hodgkin-Huxley dynamics implementation
struct HodgkinHuxleyDynamics::Impl {
    float temperature;          // Q10 temperature factor
    float membrane_potential;   // Current membrane potential (mV)
    float sodium_conductance;   // Sodium conductance
    float potassium_conductance; // Potassium conductance
    float leak_conductance;     // Leak conductance
    float sodium_reversal;      // Sodium reversal potential
    float potassium_reversal;   // Potassium reversal potential
    float leak_reversal;        // Leak reversal potential
    
    // Gate variables
    float m;  // Na+ activation
    float h;  // Na+ inactivation
    float n;  // K+ activation
    
    // Time constants and steady states
    float alpha_m(float V) { return (0.1 * (V + 40)) / (1 - std::exp(-(V + 40) / 10)); }
    float beta_m(float V) { return 4 * std::exp(-(V + 65) / 18); }
    float alpha_h(float V) { return 0.07 * std::exp(-(V + 65) / 20); }
    float beta_h(float V) { return 1 / (1 + std::exp(-(V + 35) / 10)); }
    float alpha_n(float V) { return (0.01 * (V + 55)) / (1 - std::exp(-(V + 55) / 10)); }
    float beta_n(float V) { return 0.125 * std::exp(-(V + 65) / 80); }
    
    float m_infinity(float V) { return alpha_m(V) / (alpha_m(V) + beta_m(V)); }
    float h_infinity(float V) { return alpha_h(V) / (alpha_h(V) + beta_h(V)); }
    float n_infinity(float V) { return alpha_n(V) / (alpha_n(V) + beta_n(V)); }
    
    float tau_m(float V) { return 1.0f / (alpha_m(V) + beta_m(V)); }
    float tau_h(float V) { return 1.0f / (alpha_h(V) + beta_h(V)); }
    float tau_n(float V) { return 1.0f / (alpha_n(V) + beta_n(V)); }
    
    Impl() : temperature(6.3f), membrane_potential(-70.0f),
             sodium_conductance(120.0f), potassium_conductance(36.0f), leak_conductance(0.3f),
             sodium_reversal(50.0f), potassium_reversal(-77.0f), leak_reversal(-54.387f),
             m(m_infinity(-70.0f)), h(h_infinity(-70.0f)), n(n_infinity(-70.0f)) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics(float temperature) : pImpl(new Impl) {
    setTemperature(temperature);
}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0f) return;
    
    float V = neuron->getMembranePotential();
    float dt_ms = static_cast<float>(dt);
    
    // Update gate variables using differential equations
    // dm/dt = (alpha_m*(1-m) - beta_m*m) / tau_m
    // Similar for h and n
    
    float alpha_m = pImpl->alpha_m(V);
    float beta_m = pImpl->beta_m(V);
    pImpl->m += dt_ms * (alpha_m * (1 - pImpl->m) - beta_m * pImpl->m);
    
    float alpha_h = pImpl->alpha_h(V);
    float beta_h = pImpl->beta_h(V);
    pImpl->h += dt_ms * (alpha_h * (1 - pImpl->h) - beta_h * pImpl->h);
    
    float alpha_n = pImpl->alpha_n(V);
    float beta_n = pImpl->beta_n(V);
    pImpl->n += dt_ms * (alpha_n * (1 - pImpl->n) - beta_n * pImpl->n);
    
    // Clamp gate variables between 0 and 1
    pImpl->m = std::max(0.0f, std::min(1.0f, pImpl->m));
    pImpl->h = std::max(0.0f, std::min(1.0f, pImpl->h));
    pImpl->n = std::max(0.0f, std::min(1.0f, pImpl->n));
    
    // Calculate ionic currents
    float I_Na = pImpl->sodium_conductance * std::pow(pImpl->m, 3) * pImpl->h * (V - pImpl->sodium_reversal);
    float I_K = pImpl->potassium_conductance * std::pow(pImpl->n, 4) * (V - pImpl->potassium_reversal);
    float I_L = pImpl->leak_conductance * (V - pImpl->leak_reversal);
    
    // Get external currents
    float I_inj = neuron->getTotalCurrent();
    
    // Total current: I_total = I_inj - I_Na - I_K - I_L
    float dVdt = (I_inj - I_Na - I_K - I_L) / 1.0f;  // Capacitance = 1 μF/cm²
    
    // Update membrane potential
    V += dVdt * dt_ms;
    neuron->setMembranePotential(V);
    
    // Check for threshold crossing (spike)
    if (V > 0.0f) {  // HH model threshold at 0 mV
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0); // TODO: pass actual time
        neuron->setMembranePotential(pImpl->potassium_reversal); // Reset to K+ reversal
        
        // Update gates after spike
        pImpl->m = pImpl->m_infinity(pImpl->potassium_reversal);
        pImpl->h = pImpl->h_infinity(pImpl->potassium_reversal);
        pImpl->n = pImpl->n_infinity(pImpl->potassium_reversal);
    }
    
    // Clear injected current
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) return;
    
    // HH synapse dynamics: conductance-based transmission
    synapse->step(static_cast<float>(dt));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // HH synaptic transmission: increase conductance
    float weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Increase sodium conductance for excitatory inputs
        neuron->receiveExcitatoryInput(weight);
    } else {
        // Decrease potassium conductance for inhibitory inputs
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() > 0.0f; // HH threshold
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->membrane_potential = -70.0f;
    pImpl->m = pImpl->m_infinity(-70.0f);
    pImpl->h = pImpl->h_infinity(-70.0f);
    pImpl->n = pImpl->n_infinity(-70.0f);
}

void HodgkinHuxleyDynamics::setTemperature(float temperature) {
    pImpl->temperature = temperature;
    // Adjust rates based on temperature (Q10 factor)
    // For now, just store the temperature
}

float HodgkinHuxleyDynamics::getTemperature() const {
    return pImpl->temperature;
}

} // namespace nlm