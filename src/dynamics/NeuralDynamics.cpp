#include "NeuralDynamics.hpp"
#include <cmath>
#include <random>
#include <algorithm>

namespace nlm {

// Hodgkin-Huxley dynamics implementation
struct HodgkinHuxleyDynamics::Impl {
    // Membrane parameters (mV, ms, nS, uF)
    float membraneCapacitance = 1.0f;      // C_m (uF/cm^2)
    float membraneResistance = 1.0f;       // R_m (GOhm/cm^2)
    
    // Gating variable steady-state values and time constants
    float alpha_m = 0.1f;                  // mV^-1
    float beta_m = 4.0f;                   // mV^-1
    float alpha_h = 0.07f;                 // mV^-1
    float beta_h = 1.0f;                   // mV^-1
    float alpha_n = 0.01f;                 // mV^-1
    float beta_n = 0.125f;                 // mV^-1
    
    // Current injection
    float injectedCurrent = 0.0f;          // uA/cm^2
    
    // Random number generator for noise
    std::mt19937 rng;
    std::uniform_real_distribution<float> noiseDist;
    
    Impl() : noiseDist(-0.1f, 0.1f) {
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) {
        NLM_LOG_ERROR("HodgkinHuxleyDynamics::updateNeuron called with null neuron");
        return;
    }
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    // Add noise to membrane potential
    float noise = pImpl->noiseDist(pImpl->rng);
    
    // Compute gating variables (steady-state values)
    float m_inf = 1.0f / (1.0f + std::exp(-(V + 40.0f) / 10.0f));
    float h_inf = 1.0f / (1.0f + std::exp((V + 60.0f) / 10.0f));
    float n_inf = 1.0f / (1.0f + std::exp(-(V + 60.0f) / 10.0f));
    
    // Compute derivatives
    float dm_dt = pImpl->alpha_m * m_inf * (1.0f - neuron->getGatingVariable(0)) - 
                  pImpl->beta_m * neuron->getGatingVariable(0);
    float dh_dt = pImpl->alpha_h * h_inf * (1.0f - neuron->getGatingVariable(1)) - 
                  pImpl->beta_h * neuron->getGatingVariable(1);
    float dn_dt = pImpl->alpha_n * n_inf * (1.0f - neuron->getGatingVariable(2)) - 
                  pImpl->beta_n * neuron->getGatingVariable(2);
    
    // Update gating variables
    float m = neuron->getGatingVariable(0) + dm_dt * static_cast<float>(dt);
    float h = neuron->getGatingVariable(1) + dh_dt * static_cast<float>(dt);
    float n = neuron->getGatingVariable(2) + dn_dt * static_cast<float>(dt);
    
    // Clamp gating variables to [0, 1]
    m = std::clamp(m, 0.0f, 1.0f);
    h = std::clamp(h, 0.0f, 1.0f);
    n = std::clamp(n, 0.0f, 1.0f);
    
    // Update gating variables in neuron
    neuron->setGatingVariable(0, m);
    neuron->setGatingVariable(1, h);
    neuron->setGatingVariable(2, n);
    
    // Compute ionic currents
    float I_Na = 120.0f * m * m * m * h * (V - 50.0f);  // Sodium current
    float I_K = 36.0f * n * n * n * n * (V + 77.0f);      // Potassium current
    float I_L = 0.3f * (V + 54.387f);                     // Leak current
    
    // Total current (including injected current and noise)
    float I_total = pImpl->injectedCurrent + noise;
    
    // Membrane potential update: C_m * dV/dt = I_total - I_Na - I_K - I_L
    float dV_dt = (I_total - I_Na - I_K - I_L) / pImpl->membraneCapacitance;
    
    // Update membrane potential
    float new_V = V + dV_dt * static_cast<float>(dt);
    neuron->setMembranePotential(new_V);
    
    // Check for firing and record spike
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<double>(dt));
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear injected current for next step
    pImpl->injectedCurrent = 0.0f;
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simplified synaptic dynamics with exponential decay
    if (!synapse) {
        NLM_LOG_ERROR("HodgkinHuxleyDynamics::updateSynapse called with null synapse");
        return;
    }
    
    // Exponential decay of synaptic current
    float currentStrength = synapse->getWeight() * std::exp(-dt / 5.0f);
    synapse->setWeight(currentStrength);
    
    // Update synaptic state
    synapse->step(static_cast<double>(dt));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) {
        return;
    }
    
    // Determine synaptic type and apply appropriate current
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Add to injected current for HH model
        pImpl->injectedCurrent += weight * 10.0f;  // Scale factor
    } else {
        // Inhibitory input reduces membrane potential
        float V = neuron->getMembranePotential();
        neuron->setMembranePotential(std::max(V - weight, -100.0f));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // Hodgkin-Huxley firing threshold (around -55 mV)
    return neuron->getMembranePotential() >= -55.0f;
}

void HodgkinHuxleyDynamics::reset() {
    // Reset gating variables to resting values
    for (int i = 0; i < 3; ++i) {
        float resting_value = 0.05f;  // Approximate resting value for HH
        // Note: We can't directly access neuron to reset gating variables
        // This would need to be handled by the caller
    }
    
    pImpl->injectedCurrent = 0.0f;
}

void HodgkinHuxleyDynamics::setInjectedCurrent(float current) {
    pImpl->injectedCurrent = current;
}

float HodgkinHuxleyDynamics::getInjectedCurrent() const {
    return pImpl->injectedCurrent;
}

} // namespace nlm
