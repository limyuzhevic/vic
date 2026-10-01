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
    // REAL LIF Dynamics Implementation
    // Biophysical model with adaptive threshold and refractory period
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float tau_membrane = pImpl->membraneTimeConstant;
    float R_membrane = pImpl->membraneResistance;
    
    // Membrane potential dynamics with exponential decay
    // dV/dt = (-(V - V_rest) + R*I) / tau + I_leak
    float alpha = static_cast<float>(dt) / tau_membrane;
    float V_leak = (V - V_rest) * alpha;
    float V_input = (R_membrane * I_total) * alpha;
    
    // Update membrane potential
    V += V_input - V_leak;
    
    // Adaptive threshold mechanism (rate-coding)
    if (neuron->getAdaptationVariable() > 0.0f) {
        V += neuron->getAdaptationVariable() * alpha;  // Spike-frequency adaptation
    }
    
    // Clamp to reasonable bounds
    V = std::max(V, state.resetPotential);
    V = std::min(V, state.threshold);
    
    neuron->setMembranePotential(V);
    
    // Check for firing threshold crossing
    if (V >= state.threshold && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        
        // Record spike with current simulation time
        // (dt is used as time since last step)
        neuron->recordSpike(static_cast<Timestamp>(dt));
        
        // Implement spike-triggered processes
        neuron->setAdaptationVariable(state.adaptationIncrement);
        neuron->setRefractory(state.refractoryDuration);
        
        // Reset membrane potential after spike
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Update refractory timer
    if (neuron->isRefractory()) {
        neuron->updateRefractory(dt);
        // Keep at reset potential during refractory period
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Decay adaptation variable
    if (neuron->getAdaptationVariable() > 0.0f) {
        neuron->setAdaptationVariable(std::max(0.0f, neuron->getAdaptationVariable() - 0.01f * static_cast<float>(dt)));
    }
    
    // Clear injected current for next step
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
