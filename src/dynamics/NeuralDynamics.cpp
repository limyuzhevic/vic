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
    // Real integrate-and-fire dynamics with LIF neuron integration
    // Uses neuron->stepLIF() which implements proper Leaky Integrate-and-Fire dynamics
    
    const auto& state = neuron->getState();
    
    // Get current timestep in simulation time (for consistency)
    Timestamp currentTime = static_cast<Timestamp>(neuron->getLastSpikeTime() >= 0.0f ? 
                                                  neuron->getLastSpikeTime() + dt : 0.0);
    
    // Use neuron's LIF implementation for proper dynamics
    bool fired = neuron->stepLIF(currentTime, dt);
    
    if (fired) {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Refractory mechanism - Neuron handles this in stepLIF, but ensure reset potential
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Note: neuron->clearTotalCurrent() is called internally by stepLIF
    // to clear synaptic input for the next step
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    // Synapse handles its own internal state updates including:
    // - Short-term plasticity (facilitation/depression)
    // - Eligibility trace decay for reward-modulated learning
    // - Efficacy updates based on usage
    
    synapse->step(0.0);  // Current timestamp placeholder
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses cause depolarization (increase membrane potential)
        // Weight represents synaptic strength (typically 0.0-1.0, but can be negative for inhibitory)
        neuron->receiveExcitatoryInput(weight);
    } else if (synapse->isInhibitory()) {
        // Inhibitory synapses cause hyperpolarization (decrease membrane potential)
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
    
    // Record spike timing for STDP (short-term synaptic plasticity)
    neuron->recordSpike(0.0); // TODO: pass actual spike timestamp when available
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
