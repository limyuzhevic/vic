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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // PLACEHOLDER: Simple leaky integrator
    
    const auto& state = neuron->getState();
    
    // Bounds checking: ensure neuron has valid parameters
    if (!neuron) return;
    
    // Bound membrane potential to prevent numerical instability
    float V = neuron->getMembranePotential();
    V = std::clamp(V, -100.0f, 50.0f);
    neuron->setMembranePotential(V);
    
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Bound parameters to prevent division by zero or extreme values
    if (tau <= 0.0f || R <= 0.0f) {
        tau = std::max(tau, 0.1f);
        R = std::max(R, 0.1f);
    }
    
    // Simple Euler integration with bounds checking
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    dV = std::clamp(dV, -10.0f, 10.0f); // Prevent excessive voltage changes
    
    neuron->setMembranePotential(V + dV);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Refractory mechanism with bounds checking
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Ensure membrane potential doesn't exceed bounds
    float finalV = neuron->getMembranePotential();
    finalV = std::clamp(finalV, -100.0f, 50.0f);
    neuron->setMembranePotential(finalV);
    
    // Clear current for next step
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
