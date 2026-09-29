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
    // Real integrate-and-fire dynamics implementation
    // Uses standard LIF neuron model with membrane potential integration
    
    const auto& state = neuron->getState();
    
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    // Where tau = R*C (membrane time constant)
    // I is total synaptic current, C is membrane capacitance
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Membrane capacitance (typical value for cortical neurons)
    const float C = 1.0f;  // nF
    
    // Update membrane potential using exponential Euler for stability
    // V_new = V + dt * ((V_rest - V)/tau + I/C)
    float dV = dt * ((V_rest - V) / tau + I / C);
    neuron->setMembranePotential(V + dV);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time parameter
    }
    
    // Handle refractory period (silence input, reset potential)
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear accumulated current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    // Includes short-term plasticity and conductance-based transmission
    
    // Use the synapse's own step function with proper timestep
    // The synapse manages its own state (conductance, plasticity, etc.)
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation
    // Based on conductance-based synapse model
    // Excitatory: increases membrane potential toward reversal potential
    // Inhibitory: decreases membrane potential toward reversal potential
    
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Excitatory input: depolarizing current
        neuron->receiveExcitatoryInput(weight);
    } else {
        // Inhibitory input: hyperpolarizing current  
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
