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
    // Implement real integrate-and-fire dynamics using the LIF model
    // 
    // Leaky Integrate-and-Fire (LIF) neuron model:
    // dV/dt = -(V - V_rest)/tau + I/C
    // where tau = R*C is the membrane time constant
    // C is the membrane capacitance
    
    if (!neuron) return;
    
    // Get current state
    const NeuronState& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    
    // Get dynamics parameters
    float tau = pImpl->membraneTimeConstant;  // membrane time constant (ms)
    float R = pImpl->membraneResistance;     // membrane resistance (MOhm)
    float C = 1.0f;                         // membrane capacitance (nF) - constant for LIF
    
    // Calculate change in membrane potential
    // dV/dt = -(V - V_rest)/tau + I/C
    float dV_dt = -(V - V_rest) / tau + I / C;
    
    // Update membrane potential using Euler integration
    float dt_float = static_cast<float>(dt);
    float V_new = V + dV_dt * dt_float;
    
    // Apply refractory condition
    if (neuron->isRefractory()) {
        V_new = state.resetPotential;
    }
    
    // Update neuron state
    neuron->setMembranePotential(V_new);
    
    // Check for spike threshold crossing with precise timing
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        // Record spike with actual simulation time
        neuron->recordSpike(static_cast<Timestamp>(state.lastSpikeTime >= 0 ? 
                                                   state.lastSpikeTime + tau : 
                                                   dt_float * 10));  // Approximate first spike time
    }
    
    // Clear total current for next step
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
