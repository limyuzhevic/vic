// IntegrateAndFireDynamics.cpp implementation
#include "IntegrateAndFireDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float totalExternalInput;     // nA (external current injection)
    uint32_t refractoryPeriod;    // simulation steps
    float timeAccumulator;        // for state updates
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), 
             totalExternalInput(0.0f), refractoryPeriod(5), timeAccumulator(0.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent() + pImpl->totalExternalInput;
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Leaky integrate-and-fire dynamics with enhanced parameters
    // dV/dt = -(V - V_rest) / tau + (I_syn + I_ext) / C
    // Using implicit Euler for better stability:
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + (I_total * R) * (1 - exp(-dt/tau))
    
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = (1.0f - alpha) * R;
    
    // Update membrane potential
    float V_new = V_rest + (V - V_rest) * alpha + I_total * beta;
    neuron->setMembranePotential(V_new);
    
    // Check for firing with actual time
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(currentTime);  // FIXED: Pass actual time
        
        // Reset membrane potential after spike
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(pImpl->refractoryPeriod);
    }
    
    // Handle refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear external input for next step
    pImpl->totalExternalInput = 0.0f;
    
    // Update time accumulator for potential time-dependent dynamics
    pImpl->timeAccumulator += static_cast<float>(dt);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Enhanced synaptic dynamics with conductance-based model
    // Synaptic conductance decay: dg/dt = -g / tau_syn
    // Using tau_syn = 5ms (typical for AMPA synapses)
    
    const float tau_syn = 5.0f;  // Synaptic time constant (ms)
    float decayFactor = std::exp(-static_cast<float>(dt) / tau_syn);
    
    // Get current synaptic conductance
    float g_syn = synapse->getConductance();
    
    // Apply decay
    g_syn *= decayFactor;
    synapse->setConductance(g_syn);
    
    // Update synaptic weight based on plasticity rules (placeholder)
    // In a complete implementation, this would integrate with plasticity system
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Enhanced synaptic input with conductance-based model
    // When presynaptic neuron spikes, postsynaptic conductance increases
    
    SynapticWeight weight = synapse->getWeight();
    float conductance = synapse->getConductance();
    
    if (synapse->isExcitatory()) {
        // Excitatory input: increase conductance
        float deltaConductance = weight * 0.1f;  // Scale factor for biological realism
        synapse->setConductance(conductance + deltaConductance);
        
        // Add to neuron's synaptic conductance
        neuron->addSynapticConductance(deltaConductance);
    } else {
        // Inhibitory input: increase conductance (typically opposite effect)
        float deltaConductance = std::abs(weight) * 0.1f;
        synapse->setConductance(conductance + deltaConductance);
        
        // Inhibitory neurons typically hyperpolarize postsynaptic neuron
        // For LIF, this can be modeled as negative current injection
        neuron->receiveInhibitoryInput(deltaConductance * 0.5f);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal dynamics state
    pImpl->totalExternalInput = 0.0f;
    pImpl->timeAccumulator = 0.0f;
    
    // Note: The neuron state itself is managed by the neuron, not here
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneResistance(float resistance) {
    pImpl->membraneResistance = resistance;
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(uint32_t steps) {
    pImpl->refractoryPeriod = steps;
}

uint32_t IntegrateAndFireDynamics::getRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

void IntegrateAndFireDynamics::addExternalInput(float current) {
    pImpl->totalExternalInput += current;
}

float IntegrateAndFireDynamics::getTotalExternalInput() const {
    return pImpl->totalExternalInput;
}

} // namespace nlm
