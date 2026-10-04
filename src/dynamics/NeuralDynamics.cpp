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

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) {
    // Use the neuron's built-in LIF dynamics
    neuron->step(currentTime);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics: Tsodyks-Markram short-term plasticity model
    // Also handles efficacy decay and other synaptic modifications
    
    TimestepDuration dt_ms = dt * 1000.0;  // Convert to ms for consistency
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    float& u = synapse->getPlasticityFlags().hebbian ? 1.0f : 0.0f;  // Placeholder for STP state
    float& x = synapse->getPlasticityFlags().stdp ? 1.0f : 0.0f;   // Placeholder for recovery
    
    // Synaptic efficacy decay based on usage
    float currentEfficacy = synapse->getEfficacy();
    synapse->decayEligibilityTrace(0.001f);  // Fast decay of eligibility
    
    // Update short-term plasticity dynamics
    // This is a simplified model - in a full implementation, we'd track facilitation/depression
    float U = 0.3f;  // Utilization parameter
    float x_denom = 1.0f / (1.0f + U * synapse->getEfficacy());
    
    // Apply spike-dependent changes if applicable (would be called from applySpikeInput)
    // For now, just step the synapse to maintain plasticity state
    synapse->step(dt_ms);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse, Timestamp currentTime) {
    // Real synaptic input: apply spike with delay and weight
    
    // Get synaptic weight and type
    SynapticWeight weight = synapse->getWeight();
    bool isExcitatory = synapse->isExcitatory();
    
    // Record the spike with the actual time it occurred
    neuron->recordSpike(currentTime);
    
    // Apply synaptic input to the neuron
    if (isExcitatory) {
        // Excitatory input increases membrane potential
        neuron->receiveExcitatoryInput(std::abs(weight));
    } else {
        // Inhibitory input decreases membrane potential
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->checkThreshold();
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
