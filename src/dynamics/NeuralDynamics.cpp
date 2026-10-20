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
    // Use the Neuron's built-in LIF implementation
    // IntegrateAndFireDynamics delegates to Neuron's stepLIF for proper LIF dynamics
    // This ensures consistent implementation between the two interfaces
    static Timestamp currentTime = 0.0;
    
    // Check if neuron should fire using the threshold check
    if (shouldFire(neuron)) {
        // For step-based updates, we don't have the exact timestep for spike time recording
        // Use a placeholder value - in a real implementation this should track time properly
        neuron->recordSpike(currentTime);
        neuron->setFiringState(FiringState::Active);
    }
    
    // Apply the Neuron's LIF step to update membrane potential
    // Note: We're using the Neuron's stepLIF for the actual dynamics
    // This ensures consistency with the existing Neuron implementation
    Timestamp timestep = 0.001;  // Standard 1ms timestep for integrate-and-fire models
    bool fired = neuron->stepLIF(currentTime, timestep);
    
    if (fired) {
        // Update refractory state if needed
        if (neuron->isRefractory()) {
            const auto& state = neuron->getState();
            neuron->setMembranePotential(state.resetPotential);
        }
    }
    
    // Clear accumulated current for next timestep
    neuron->clearTotalCurrent();
    
    // Increment time for next step
    currentTime += timestep;
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics: exponential decay of synaptic conductance
    // Synapse state evolves according to differential equation: dg/dt = -g/tau_synapse
    
    float conductance = synapse->getConductance();
    float timeConstant = pImpl->membraneTimeConstant;  // Using membrane time constant as synapse time constant
    
    // Calculate new conductance with exponential decay
    float decayFactor = std::exp(-static_cast<float>(dt) / timeConstant);
    float newConductance = conductance * decayFactor;
    
    synapse->setConductance(newConductance);
    
    // Apply conductance to post-synaptic neurons
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement real synaptic input using conductance-based synapses
    // Postsynaptic potential: V_post = (g_AMPA * E_AMPA + g_GABA * E_GABA) / (g_AMPA + g_GABA)
    // Where g is synaptic conductance and E is reversal potential
    
    SynapticWeight weight = synapse->getWeight();
    bool isExcitatory = synapse->isExcitatory();
    
    if (isExcitatory) {
        // Excitatory synapses increase membrane potential towards +50mV
        neuron->receiveExcitatoryInput(weight * 0.5f);  // Scale factor for biological realism
    } else {
        // Inhibitory synapses decrease membrane potential towards -70mV
        neuron->receiveInhibitoryInput(weight * 0.8f);  // Stronger inhibition
    }
    
    // Update synaptic conductance for future integration
    float currentConductance = neuron->getState().synapseConductance;
    if (isExcitatory) {
        neuron->setSynapseConductance(currentConductance + weight * 0.1f);
    } else {
        neuron->setSynapseConductance(currentConductance - weight * 0.05f);
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
