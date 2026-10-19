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
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / C
    // For simplicity using explicit Euler:
    // V_new = V_old + dt * (-(V_old - V_rest) / tau + I / C)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Validate parameters to prevent division by zero
    if (tau <= 0.0f || R <= 0.0f) {
        // Invalid parameters - skip integration but record spike if threshold reached
        if (shouldFire(neuron)) {
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(0.0);  // TODO: pass actual time
        }
        neuron->clearTotalCurrent();
        return;
    }
    
    // Simple Euler integration with improved numerical stability
    float leakTerm = (V - V_rest) / tau;
    float inputTerm = I / R;
    
    // Add safety check for NaN/inf values
    if (!std::isfinite(leakTerm) || !std::isfinite(inputTerm)) {
        // Invalid calculation - reset to resting potential
        neuron->setMembranePotential(V_rest);
    } else {
        float dV = (leakTerm + inputTerm) * static_cast<float>(dt);
        neuron->setMembranePotential(V + dV);
    }
    
    // Check for firing with improved threshold detection
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Proper refractory mechanism with countdown
    if (neuron->isRefractory()) {
        // During refractory period, reset potential and prevent integration
        neuron->setMembranePotential(state.resetPotential);
        // Note: Refractory countdown is handled by Neuron class
    }
    
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
