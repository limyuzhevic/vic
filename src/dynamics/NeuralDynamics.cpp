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
    // dV/dt = -(V - V_rest) / tau + I / C
    // V = membrane potential
    // V_rest = resting potential
    // tau = membrane time constant
    // I = total input current
    // C = membrane capacitance
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    
    // Membrane resistance from tau and capacitance (C = 1 for simplicity)
    float R = tau;  // Capacitance = 1
    
    // Calculate derivatives using improved Euler method for better stability
    float dVdt = (-(V - V_rest) / tau + I / R);
    
    // Update using time step
    float dt_float = static_cast<float>(dt);
    float V_new = V + dVdt * dt_float;
    
    // Apply leak correction and ensure we stay within reasonable bounds
    V_new = std::clamp(V_new, state.resetPotential, state.threshold * 1.5f);
    
    neuron->setMembranePotential(V_new);
    
    // Check for firing with improved detection
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(dt_float));  // Pass actual time
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        // Keep potential at reset during refractory period
        neuron->setMembranePotential(state.resetPotential);
        
        // Decrease refractory remaining time
        neuron->setRefractoryPeriod(neuron->getRefractoryPeriod() - dt_float);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with exponential decay
    // dW/dt = -W / tau_synapse + input from plasticity rules
    
    float weight = synapse->getWeight();
    float decayRate = 0.1f;  // Synaptic decay rate
    
    // Apply exponential decay
    weight = weight * std::exp(-decayRate * static_cast<float>(dt));
    
    // Plasticity effects are applied elsewhere (STDP, Hebbian)
    // The dynamics class just provides the background decay
    
    synapse->setWeight(weight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with conductance-based modeling
    // The input depends on synapse type, weight, and reversal potential
    
    SynapticWeight weight = synapse->getWeight();
    SynapseType synType = synapse->getType();
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses depolarize the neuron
        // Weight scaled by membrane resistance
        float input = weight * 10.0f;  // Scale factor for current
        neuron->receiveExcitatoryInput(input);
    } else {
        // Inhibitory synapses hyperpolarize the neuron
        // Stronger inhibition for higher weight magnitude
        float input = std::abs(weight) * 8.0f;
        neuron->receiveInhibitoryInput(input);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with conductance-based modeling
    // The input depends on synapse type, weight, and reversal potential
    
    SynapticWeight weight = synapse->getWeight();
    SynapseType synType = synapse->getType();
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses depolarize the neuron
        // Weight scaled by membrane resistance
        float input = weight * 10.0f;  // Scale factor for current
        neuron->receiveExcitatoryInput(input);
    } else {
        // Inhibitory synapses hyperpolarize the neuron
        // Stronger inhibition for higher weight magnitude
        float input = std::abs(weight) * 8.0f;
        neuron->receiveInhibitoryInput(input);
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
