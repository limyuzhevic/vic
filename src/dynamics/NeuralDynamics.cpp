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
    
    // Null pointer safety checks
    if (!pImpl || !neuron) {
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        return;
    }
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // For simplicity using explicit Euler:
    // V_new = V_old + dt * (-(V_old - V_rest) / tau + I / C)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Validate physical constants
    if (tau <= 0.0f || R <= 0.0f) {
        // Reset to resting potential if constants are invalid
        neuron->setMembranePotential(V_rest);
        return;
    }
    
    // Validate current input
    if (std::isnan(I)) {
        I = 0.0f;
    }
    
    // Simple Euler integration
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    neuron->setMembranePotential(V + dV);
    
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
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    
    // Null pointer safety checks
    if (!pImpl || !synapse) {
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        return;
    }
    
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    
    // Null pointer safety checks
    if (!pImpl || !neuron || !synapse) {
        return;
    }
    
    SynapticWeight weight = synapse->getWeight();
    
    // Validate synaptic weight
    if (std::isnan(weight)) {
        weight = 0.0f;
    }
    
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
    // Null pointer safety check
    if (!pImpl) {
        return;
    }
    
    // Validate time constant
    if (std::isnan(tau) || tau <= 0.0f) {
        return; // Silently reject invalid values
    }
    
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
