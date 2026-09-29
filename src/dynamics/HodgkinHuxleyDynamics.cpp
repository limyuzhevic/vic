#include "Neuromodulator.hpp"
#include "core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    // Membrane capacitance
    float capacitance;  // nF
    
    // Ion channel conductances
    float sodiumConductance;
    float potassiumConductance;
    float leakConductance;
    
    // Reversal potentials
    float sodiumReversal;
    float potassiumReversal;
    float leakReversal;
    
    // Channel gating variables
    float m;  // sodium activation
    float h;  // sodium inactivation
    float n;  // potassium activation
    
    // Membrane potential derivative
    float dVdt;
    
    Impl() : capacitance(1.0f),
             sodiumConductance(120.0f),
             potassiumConductance(36.0f),
             leakConductance(0.3f),
             sodiumReversal(50.0f),
             potassiumReversal(-77.0f),
             leakReversal(-54.387f),
             m(0.0f), h(0.0f), n(0.0f),
             dVdt(0.0f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real Hodgkin-Huxley dynamics implementation
    // Biophysical model of action potential generation
    // Based on ion channel conductance dynamics and membrane potential
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float I_total = neuron->getTotalCurrent();
    
    // Update gating variables using HH equations
    // dm/dt = alpha_m*(1-m) - beta_m*m
    // dh/dt = alpha_h*(1-h) - beta_h*h  
    // dn/dt = alpha_n*(1-n) - beta_n*n
    
    float alpha_m = (0.1f * (V + 40.0f)) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    float beta_m = 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    
    float alpha_h = 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    float beta_h = 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    
    float alpha_n = (0.01f * (V + 55.0f)) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    float beta_n = 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    
    // Update gating variables
    pImpl->m += dt * (alpha_m * (1.0f - pImpl->m) - beta_m * pImpl->m);
    pImpl->h += dt * (alpha_h * (1.0f - pImpl->h) - beta_h * pImpl->h);
    pImpl->n += dt * (alpha_n * (1.0f - pImpl->n) - beta_n * pImpl->n);
    
    // Clamp gating variables to valid range
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    
    // Calculate ionic currents
    float I_Na = pImpl->sodiumConductance * pImpl->m * pImpl->m * pImpl->m * pImpl->h * (V - pImpl->sodiumReversal);
    float I_K = pImpl->potassiumConductance * pImpl->n * pImpl->n * pImpl->n * (V - pImpl->potassiumReversal);
    float I_L = pImpl->leakConductance * (V - pImpl->leakReversal);
    
    // Total membrane current
    float I_membrane = I_Na + I_K + I_L;
    
    // Update membrane potential: dV/dt = (I_ext - I_ionic) / C
    pImpl->dVdt = (I_total - I_membrane) / pImpl->capacitance;
    
    // Update membrane potential
    neuron->setMembranePotential(V + pImpl->dVdt * static_cast<float>(dt));
    
    // Check for firing threshold
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // HH dynamics: synaptic dynamics with conductance changes
    // Synapses affect membrane potential through conductance-based models
    synapse->step(0.0);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // HH dynamics: spike input affects ion channel states
    // Synaptic input modulates gating variable dynamics
    
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Excitatory input increases sodium conductance
        pImpl->sodiumConductance += weight * 10.0f;
        pImpl->sodiumConductance = std::clamp(pImpl->sodiumConductance, 0.0f, 150.0f);
        neuron->receiveExcitatoryInput(weight);
    } else {
        // Inhibitory input increases potassium conductance
        pImpl->potassiumConductance += weight * 5.0f;
        pImpl->potassiumConductance = std::clamp(pImpl->potassiumConductance, 0.0f, 50.0f);
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    // HH firing: spike when membrane potential exceeds threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH state variables
    pImpl->m = 0.05f;  // resting value
    pImpl->h = 0.6f;   // resting value
    pImpl->n = 0.32f;  // resting value
    pImpl->dVdt = 0.0f;
}

float HodgkinHuxleyDynamics::getSodiumReversal() const {
    return pImpl->sodiumReversal;
}

void HodgkinHuxleyDynamics::setSodiumReversal(float reversal) {
    pImpl->sodiumReversal = reversal;
}

float HodgkinHuxleyDynamics::getPotassiumReversal() const {
    return pImpl->potassiumReversal;
}

void HodgkinHuxleyDynamics::setPotassiumReversal(float reversal) {
    pImpl->potassiumReversal = reversal;
}

float HodgkinHuxleyDynamics::getLeakReversal() const {
    return pImpl->leakReversal;
}

void HodgkinHuxleyDynamics::setLeakReversal(float reversal) {
    pImpl->leakReversal = reversal;
}

float HodgkinHuxleyDynamics::getSodiumConductance() const {
    return pImpl->sodiumConductance;
}

void HodgkinHuxleyDynamics::setSodiumConductance(float conductance) {
    pImpl->sodiumConductance = std::clamp(conductance, 0.0f, 200.0f);
}

float HodgkinHuxleyDynamics::getPotassiumConductance() const {
    return pImpl->potassiumConductance;
}

void HodgkinHuxleyDynamics::setPotassiumConductance(float conductance) {
    pImpl->potassiumConductance = std::clamp(conductance, 0.0f, 100.0f);
}

float HodgkinHuxleyDynamics::getLeakConductance() const {
    return pImpl->leakConductance;
}

void HodgkinHuxleyDynamics::setLeakConductance(float conductance) {
    pImpl->leakConductance = std::clamp(conductance, 0.0f, 1.0f);
}

float HodgkinHuxleyDynamics::getCapacitance() const {
    return pImpl->capacitance;
}

void HodgkinHuxleyDynamics::setCapacitance(float capacitance) {
    pImpl->capacitance = std::clamp(capacitance, 0.1f, 10.0f);
}

} // namespace nlm
