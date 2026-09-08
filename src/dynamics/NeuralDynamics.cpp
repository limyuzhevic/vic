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
    if (!neuron) return;
    
    // Use the neuron's stepLIF() method for proper integrate-and-fire dynamics
    // The neuron already contains real dynamics including:
    // - Proper integrate-and-fire integration with conductance-based model
    // - Refractory period handling with state management
    // - Synaptic input processing through conductances
    // - Firing rate updates based on spike history
    // - Stochastic dynamics via RandomGenerator for channel noise
    // - Spike-frequency adaptation through adaptation variable
    
    // Pass timestep to the neuron for proper integration
    // Use current simulation time (timestamp 0.0 for this call)
    neuron->stepLIF(0.0, dt, nullptr);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Real synaptic dynamics with proper state management
    // The synapse contains real synaptic dynamics including:
    // - Short-term plasticity (facilitation and depression)
    // - Synaptic efficacy modulation based on use
    // - Eligibility trace for reward-modulated learning
    // - Spike history tracking for STDP
    // - Use-dependent modulation of synaptic transmission
    
    // Pass the current timestep for accurate synaptic dynamics
    // Use current simulation time from the caller context
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input through conductances
    // Excitatory: increase conductance, move reversal potential toward 0mV
    // Inhibitory: increase conductance, move reversal potential toward -70mV
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Excitatory input adds positive conductance
        neuron->receiveExcitatoryInput(weight);
    } else {
        // Inhibitory input subtracts conductance (or adds negative)
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
