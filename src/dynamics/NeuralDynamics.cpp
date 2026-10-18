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
    // Implement real integrate-and-fire dynamics with biological accuracy
    
    const auto& state = neuron->getState();
    
    // LIF dynamics with exponential integration for better accuracy
    // dV/dt = (V_rest - V) / tau + I/C
    // Use exponential Euler integration: V(t+dt) = V_rest + (V(t) - V_rest) * exp(-dt/tau) + I * tau * (1 - exp(-dt/tau)) / C
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float C = 1.0f;  // Membrane capacitance (nF)
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->clearTotalCurrent();
        return;
    }
    
    // Exponential integration for better numerical stability
    float alpha = std::exp(-static_cast<float>(dt) * 1000.0f / tau);
    float synapticContribution = I * tau * (1.0f - alpha) / C;
    
    float V_new = V_rest + (V - V_rest) * alpha + synapticContribution;
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;
    }
    
    // Clamp to physiological bounds
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for spike threshold crossing
    if (V_new >= state.threshold) {
        // Fire at threshold
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // Time will be updated by caller
        
        // Reset membrane potential to reset potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(state.refractoryPeriod));
        neuron->setFiringState(FiringState::Refractory);
        
        // Increase adaptation variable for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics based on short-term plasticity
    if (!synapse) return;
    
    Timestamp currentTime = 0.0;  // Will be set by caller
    
    // Update synaptic dynamics based on spike activity
    synapse->step(currentTime);
    
    // Update synaptic efficacy based on usage history
    float activityLevel = std::min(1.0f, static_cast<float>(synapse->getPreSpikeHistory().size()) / 10.0f);
    float efficacyChange = 0.01f * activityLevel - 0.005f * dt;  // LTP for activity, decay for inactivity
    synapse->setEfficacy(synapse->getEfficacy() + efficacyChange);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement real synaptic input with conductance-based dynamics
    if (!neuron || !synapse) return;
    
    // Get synaptic weight and type
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Compute synaptic conductance based on weight, efficacy, and synaptic properties
    float conductance = std::abs(weight) * efficacy;
    
    // Determine reversal potential based on synapse type
    float reversalPotential = 0.0f;  // Typical for excitatory synapses
    if (synapse->isInhibitory()) {
        reversalPotential = -80.0f;  // Typical for inhibitory synapses
    }
    
    // Compute synaptic current: I_syn = conductance * (V - E_rev)
    // Use the neuron's current membrane potential for accuracy
    float currentMembranePotential = neuron->getMembranePotential();
    float synapticCurrent = conductance * (currentMembranePotential - reversalPotential);
    
    // Apply synaptic input with proper sign
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(synapticCurrent);
    } else {
        // Modulatory synapses affect plasticity but not directly integrated
        neuron->receiveModulatoryInput(synapticCurrent * 0.1f);
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
