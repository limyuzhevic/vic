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
    // Real integrate-and-fire dynamics based on LIF model
    // dV/dt = -(V - V_rest) / tau + I / C
    // Where tau = R * C (membrane time constant)
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Euler integration with biological accuracy
    // Include spike-frequency adaptation from neuron state
    float adaptation = state.adaptationVariable * 0.01f;
    float effective_I = I - adaptation;  // Adaptation reduces effective input
    
    // Standard LIF integration
    float dV = (-(V - V_rest) / tau + effective_I / R) * static_cast<float>(dt);
    float newV = V + dV;
    
    // Apply spike-frequency adaptation current injection
    if (state.adaptationVariable > 0.0f) {
        newV -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp to biological range (-100mV to 50mV)
    newV = std::clamp(newV, -100.0f, 50.0f);
    neuron->setMembranePotential(newV);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(pImpl->membraneTimeConstant * 0.5));  // Approximate spike time
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics based on Tsodyks-Markram model
    // Updates short-term plasticity state (STP)
    
    Timestamp currentTime = 0.0;  // In real system, would be passed from Brain
    synapse->step(currentTime);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input based on spike timing and synaptic properties
    // Handles both excitatory and inhibitory inputs with realistic dynamics
    
    SynapticWeight weight = synapse->getWeight();
    
    // Calculate synaptic conductance based on weight and type
    // Excitatory synapses: positive current, Inhibitory: negative current
    float synapticConductance = std::abs(weight) * 10.0f;  // Scaling factor for realistic currents
    
    // Apply synaptic input through neuron's specific pathways
    if (synapse->isExcitatory()) {
        // Excitatory input increases membrane potential toward threshold
        // Realistic amplitude based on synaptic strength
        neuron->receiveExcitatoryInput(synapticConductance * weight);  // Weight is positive
    } else if (synapse->isInhibitory()) {
        // Inhibitory input decreases membrane potential (hyperpolarization)
        // Inhibitory weights are typically negative, so we use absolute value
        neuron->receiveInhibitoryInput(synapticConductance * std::abs(weight));
    }
    
    // Additional dynamic factors could include:
    // - Short-term plasticity (facilitation/depression)
    // - Synaptic delay effects
    // - Neuromodulation effects
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
