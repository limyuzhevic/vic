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
    // Phase 2: Real integrate-and-fire dynamics with conductance-based integration
    // Implements: dV/dt = (V_rest - V)/tau + I/C
    // Where I includes synaptic currents and external inputs
    
    const auto& state = neuron->getState();
    
    // Get all inputs contributing to membrane potential
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();  // Includes synaptic inputs
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Apply leak conductance (I_leak = (V - V_rest) / R_leak)
    // and synaptic input (I_syn) to compute total current
    // dV/dt = (V_rest - V)/(R_leak * C) + I_syn / C
    
    // Simple exponential Euler integration for stability
    float dV = (-(V - V_rest) / tau + I_total / R) * static_cast<float>(dt);
    neuron->setMembranePotential(V + dV);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(pImpl->currentTime));
    }
    
    // Apply refractory period if needed
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear accumulated currents for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
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

} // namespace nlm
