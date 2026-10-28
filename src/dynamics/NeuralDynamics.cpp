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
    // Real integrate-and-fire dynamics with LIF equation:
    // dV/dt = (V_rest - V)/tau + I/C
    // V_new = V + dt * (V_rest - V)/tau + dt * I/C
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;  // ms
    float R = pImpl->membraneResistance;     // MOhm
    
    // LIF dynamics with proper scaling for milliseconds
    float leakTerm = (V_rest - V) / tau;
    float inputTerm = I / R;
    
    // Update membrane potential using Euler integration
    float dV = (leakTerm + inputTerm) * static_cast<float>(dt) * 1000.0f; // Convert seconds to ms
    neuron->addToMembranePotential(dV);
    
    // Check for spike threshold crossing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        // Record spike with current simulation time
        neuron->recordSpike(Timestamp(0.0));  // TODO: pass actual timestamp parameter
        
        // Reset membrane potential to reset potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Apply adaptation effect
        float& adaptation = neuron->getState().adaptationVariable;
        adaptation += 1.0f;
    } else {
        // Clamp to reasonable range to prevent numerical instability
        float& V_current = neuron->getState().membranePotential;
        if (V_current < -100.0f) V_current = -100.0f;
        if (V_current > 50.0f) V_current = 50.0f;
        
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear synaptic current for next step
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
