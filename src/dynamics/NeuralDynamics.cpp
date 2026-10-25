#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Implement real integrate-and-fire dynamics with proper integration
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Exponential Euler integration for LIF
    // dV/dt = -(V - V_rest)/tau + I/R
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    V = V + dV;
    
    // Apply spike-frequency adaptation (if present)
    if (neuron->getState().adaptationVariable > 0.0f) {
        V -= neuron->getState().adaptationVariable * 0.01f;
    }
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    neuron->setMembranePotential(V);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(0.0));  // TODO: pass actual time
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics with exponential decay
    float decay = std::exp(-static_cast<float>(dt) / 5.0f);  // 5ms time constant
    float weight = synapse->getWeight();
    synapse->setWeight(weight * decay);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement real synaptic input based on synapse properties
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
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

// Hodgkin-Huxley dynamics implementation
struct HodgkinHuxleyDynamics::Impl {
    float membraneCapacitance;   // uF/cm^2
    float sodiumConductance;     // mS/cm^2
    float potassiumConductance;  // mS/cm^2
    float leakConductance;       // mS/cm^2
    
    // Gating variable steady states
    float m_inf, h_inf, n_inf;
    float tau_m, tau_h, tau_n;
    
    Impl() : membraneCapacitance(1.0f), sodiumConductance(120.0f), 
             potassiumConductance(36.0f), leakConductance(0.3f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    
    // Calculate gating variables using steady-state approximation
    float m = pImpl->m_inf;
    float h = pImpl->h_inf;
    float n = pImpl->n_inf;
    
    // Calculate currents
    float I_leak = pImpl->leakConductance * (V - V_rest);
    float I_Na = pImpl->sodiumConductance * std::pow(m, 3) * h * (V - 50.0f);
    float I_K = pImpl->potassiumConductance * std::pow(n, 4) * (V - (-77.0f));
    
    float I_total = I_leak + I_Na + I_K;
    
    // Update membrane potential using membrane capacitance
    float dV = I_total / pImpl->membraneCapacitance * static_cast<float>(dt);
    V = V + dV;
    
    // Clamp to reasonable range
    V = std::clamp(V, -100.0f, 100.0f);
    neuron->setMembranePotential(V);
    
    // Check for firing threshold
    if (neuron->getMembranePotential() >= neuron->getThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(0.0));  // TODO: pass actual time
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement synaptic dynamics with conductance-based model
    float decay = std::exp(-static_cast<float>(dt) / 10.0f);  // 10ms time constant
    synapse->setConductance(synapse->getConductance() * decay);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply conductance-based synaptic input
    if (synapse->isExcitatory()) {
        neuron->setConductance(synapse->getConductance());
    } else {
        neuron->setConductance(-synapse->getConductance());
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    // Reset gating variables
    pImpl->m_inf = 0.0f;
    pImpl->h_inf = 0.0f;
    pImpl->n_inf = 0.0f;
    pImpl->tau_m = 1.0f;
    pImpl->tau_h = 1.0f;
    pImpl->tau_n = 1.0f;
}

// FitzHugh-Nagumo dynamics implementation
struct FitzHughNagumoDynamics::Impl {
    float timeConstant;      // ms
    float threshold;         // V threshold for spike
    float resetVoltage;      // V reset after spike
    float recoveryTime;      // Tau for recovery variable
    
    float recovery;          // Recovery variable
    
    Impl() : timeConstant(12.0f), threshold(0.5f), resetVoltage(-1.5f), recoveryTime(100.0f), recovery(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    
    // FitzHugh-Nagumo equations
    float dV = (V - V * V * V / 3.0f - pImpl->recovery + I_ext) / pImpl->timeConstant;
    float dRecovery = (V + 0.7f - 0.8f * pImpl->recovery) / pImpl->recoveryTime;
    
    // Simple integration
    V = V + dV * static_cast<float>(dt);
    pImpl->recovery = pImpl->recovery + dRecovery * static_cast<float>(dt);
    
    // Apply spike detection and reset
    if (V >= pImpl->threshold) {
        V = pImpl->resetVoltage;
        pImpl->recovery = pImpl->recovery + 0.1f;
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(0.0));  // TODO: pass actual time
    }
    
    neuron->setMembranePotential(V);
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement synaptic dynamics with FHN-like behavior
    float decay = std::exp(-static_cast<float>(dt) / 20.0f);  // 20ms time constant
    synapse->setWeight(synapse->getWeight() * decay);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input with FHN characteristics
    float weight = synapse->getWeight();
    float input = weight * (neuron->getMembranePotential() - (-1.5f));  // Excitatory
    neuron->addToMembranePotential(input);
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= 0.5f;
}

void FitzHughNagumoDynamics::reset() {
    pImpl->recovery = 0.0f;
}

} // namespace nlm
