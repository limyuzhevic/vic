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
    // Implement real integrate-and-fire dynamics based on LIF equation
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Leaky integrate-and-fire dynamics: dV/dt = (V_rest - V)/(R*tau) + I/C
    // where C = tau/R is membrane capacitance
    // Discrete form: V_new = V + dt * ((V_rest - V)/(R*tau) + I/C)
    
    float C = tau / R;  // Membrane capacitance
    float dV = dt * ((V_rest - V) / (R * tau) + I / C);
    
    // Apply Euler integration
    neuron->setMembranePotential(V + dV);
    
    // Clamp to biologically plausible range
    neuron->setMembranePotential(std::clamp(neuron->getMembranePotential(), -100.0f, 50.0f));
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Reset membrane potential
        neuron->setMembranePotential(state.resetPotential);
        neuron->enterRefractoryPeriod();
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear accumulated current for next step
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
