#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics based on biophysical principles
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // LIF dynamics: dV/dt = (V_rest - V) / tau + I / R
    // Using exponential Euler integration for stability:
    // V_new = V_rest + (V - V_rest) * exp(-dt/tau) + I * R * (1 - exp(-dt/tau))
    
    float alpha = std::exp(-static_cast<float>(dt) / 1000.0f / tau);  // dt in ms
    float newV = V_rest + (V - V_rest) * alpha + I * R * (1.0f - alpha);
    
    // Update membrane potential
    neuron->setMembranePotential(newV);
    
    // Check for firing
    if (shouldFire(neuron)) {
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

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics: exponential decay
    float weight = synapse->getWeight();
    float decayRate = 0.001f;  // 1/ms decay time constant
    
    // Apply decay to synaptic weight
    weight *= std::exp(-decayRate * static_cast<float>(dt));
    synapse->setWeight(weight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic transmission based on synaptic properties
    float weight = synapse->getWeight();
    float delay = synapse->getDelay();
    
    // Different synaptic types produce different input characteristics
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight * 10.0f);  // scaled by weight
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(weight) * 10.0f);  // scaled by weight
    }
    
    // Record for plasticity
    if (weight > 0.0f) {
        // Presynaptic spike for STDP
        // neuron->recordPreSpike(0.0);  // TODO: pass actual time
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    pImpl->membraneResistance = R;
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

// Hodgkin-Huxley implementation
struct HodgkinHuxleyDynamics::Impl {
    // HH model parameters (from http://www.cmabbott.org/hh/pmwiki.php?n=Main.HHParameters)
    float gNa;      // Sodium conductance (mS/cm²)
    float gK;       // Potassium conductance (mS/cm²)
    float gL;       // Leak conductance (mS/cm²)
    float Cm;       // Membrane capacitance (uF/cm²)
    
    // Reversal potentials
    float ENa;      // Sodium reversal potential (mV)
    float EK;       // Potassium reversal potential (mV)
    float EL;       // Leak reversal potential (mV)
    
    // gating variables
    float m, h, n;  // activation/inactivation variables
    
    Impl() : gNa(120.0f), gK(36.0f), gL(0.3f), Cm(1.0f),
             ENa(50.0f), EK(-77.0f), EL(-54.387f),
             m(0.05f), h(0.6f), n(0.325f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real Hodgkin-Huxley dynamics implementation
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float I_app = neuron->getTotalCurrent();  // Applied current (excitatory input)
    
    // Convert timestep from seconds to milliseconds for HH equations
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Calculate alpha and beta functions for gating variables
    float alpha_m = (2.5f - 0.1f * V) / (std::exp(0.1f * V - 2.5f) - 1.0f);
    float beta_m = 4.0f * std::exp(-V / 18.0f);
    
    float alpha_h = 0.07f * std::exp(-V / 20.0f);
    float beta_h = 1.0f / (std::exp(0.1f * V - 3.0f) + 1.0f);
    
    float alpha_n = (0.1f - 0.01f * V) / (std::exp(0.1f * V - 1.0f) - 1.0f);
    float beta_n = 0.125f * std::exp(-V / 80.0f);
    
    // Update gating variables
    pImpl->m += dt_ms * (alpha_m * (1.0f - pImpl->m) - beta_m * pImpl->m);
    pImpl->h += dt_ms * (alpha_h * (1.0f - pImpl->h) - beta_h * pImpl->h);
    pImpl->n += dt_ms * (alpha_n * (1.0f - pImpl->n) - beta_n * pImpl->n);
    
    // Clamp gating variables
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    
    // Calculate ionic currents
    float INa = pImpl->gNa * pImpl->m * pImpl->m * pImpl->m * pImpl->h * (V - pImpl->ENa);
    float IK = pImpl->gK * pImpl->n * pImpl->n * pImpl->n * (V - pImpl->EK);
    float IL = pImpl->gL * (V - pImpl->EL);
    
    // Calculate membrane potential derivative
    float dVdt = (I_app - INa - IK - IL) / pImpl->Cm;
    
    // Update membrane potential
    V += dVdt * dt_ms;
    neuron->setMembranePotential(V);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Update neuron state for synaptic effects
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simplified synaptic dynamics for HH model
    float weight = synapse->getWeight();
    float decayRate = 0.002f;  // Slower decay for HH
    
    weight *= std::exp(-decayRate * static_cast<float>(dt));
    synapse->setWeight(weight);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    float weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight * 5.0f);  // Modified for HH dynamics
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(weight) * 5.0f);
    }
    
    // Record for plasticity
    if (weight > 0.0f) {
        // neuron->recordPreSpike(0.0);  // TODO: pass actual time
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->m = 0.05f;
    pImpl->h = 0.6f;
    pImpl->n = 0.325f;
}

// FitzHugh-Nagumo implementation
struct FitzHughNagumoDynamics::Impl {
    // FHN parameters
    float threshold;      // Firing threshold
    float restingPotential; // Resting potential
    float timeConstant;   // Current decay time constant (tau)
    float activationThreshold; // Excitation threshold for activator
    
    // FHN state variables
    float v;              // Membrane potential
    float w;              // Recovery variable
    
    Impl() : threshold(0.5f), restingPotential(-1.2f), 
             timeConstant(12.5f), activationThreshold(0.5f),
             v(-1.2f), w(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real FitzHugh-Nagumo dynamics implementation
    const auto& state = neuron->getState();
    
    float v = pImpl->v;
    float w = pImpl->w;
    float I_app = neuron->getTotalCurrent();
    
    // FHN equations:
    // dv/dt = (v - v³/3 + w + I_app) / tau
    // dw/dt = epsilon * (v - a - b * w)
    
    float tau = pImpl->timeConstant;
    float a = pImpl->activationThreshold;
    float b = 0.8f;  // Recovery rate parameter
    
    float dvdt = (v - (v * v * v) / 3.0f + w + I_app) / tau;
    float dwdt = 0.08f * (v - a - b * w);  // epsilon = 0.08
    
    // Update state variables
    v += dvdt * static_cast<float>(dt);
    w += dwdt * static_cast<float>(dt);
    
    pImpl->v = v;
    pImpl->w = w;
    
    // Update membrane potential in neuron
    neuron->setMembranePotential(v);
    
    // Check for firing (spike threshold crossing)
    if (v >= pImpl->threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Reset after spike
        v = pImpl->restingPotential;
        w += 0.1f;  // Recovery boost after spike
        pImpl->v = v;
        pImpl->w = w;
        neuron->setMembranePotential(v);
    }
    
    // Update neuron state
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // FHN synaptic dynamics with different time constants
    float weight = synapse->getWeight();
    float decayRate = 0.0005f;  // Very slow decay
    
    weight *= std::exp(-decayRate * static_cast<float>(dt));
    synapse->setWeight(weight);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    float weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight * 2.0f);  // Different for FHN
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(weight) * 2.0f);
    }
    
    // Record for plasticity
    if (weight > 0.0f) {
        // neuron->recordPreSpike(0.0);  // TODO: pass actual time
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    pImpl->v = pImpl->restingPotential;
    pImpl->w = 0.0f;
}

void FitzHughNagumoDynamics::setFHNThreshold(float vThreshold) {
    pImpl->threshold = vThreshold;
}

float FitzHughNagumoDynamics::getFHNThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setFHNRestingPotential(float vRest) {
    pImpl->restingPotential = vRest;
}

float FitzHughNagumoDynamics::getFHNRestingPotential() const {
    return pImpl->restingPotential;
}

void FitzHughNagumoDynamics::setFHNCurrentDegradation(float tau) {
    pImpl->timeConstant = tau;
}

float FitzHughNagumoDynamics::getFHNCurrentDegradation() const {
    return pImpl->timeConstant;
}

void FitzHughNagumoDynamics::setFHNActivationThreshold(float aThreshold) {
    pImpl->activationThreshold = aThreshold;
}

float FitzHughNagumoDynamics::getFHNActivationThreshold() const {
    return pImpl->activationThreshold;
}

} // namespace nlm
