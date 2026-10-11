# NeuralDynamics.cpp - Real Neural Dynamics Implementation
// This file implements the Phase 2 neural dynamics models
// Replaces the placeholder implementations with real, working physics

#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Integrate-and-Fire Dynamics Implementation
// Real LIF dynamics based on biophysical principles
// dV/dt = (V_rest - V)/tau + I/C
// where: V=membrane potential, tau=time constant, I=input current, C=capacitance

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl()) {
    // Initialize LIF parameters with biologically plausible values
    pImpl->timeConstant = 20.0f;  // Membrane time constant (ms)
    pImpl->capacitance = 1.0f;    // Membrane capacitance (nF)
    pImpl->synapticConductance = 0.0f;  // Current synaptic conductance
    pImpl->lastUpdateTime = 0.0f;
}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->timeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->timeConstant;
}

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const NeuronState& state = neuron->getState();
    MembranePotential& V = const_cast<MembranePotential&>(state.membranePotential);
    
    // Real LIF integration using Euler method
    // V(t+dt) = V(t) + dt * ((V_rest - V(t))/tau + I(t)/C)
    
    float timeConstant = pImpl->timeConstant;
    float capacitance = pImpl->capacitance;
    
    if (timeConstant > 0.0f && capacitance > 0.0f) {
        float leakTerm = (state.restingPotential - V) / timeConstant;
        float inputTerm = pImpl->synapticConductance / capacitance;
        
        // Update membrane potential
        V = V + dt * 1000.0f * (leakTerm + inputTerm);  // Convert ms to seconds
        
        // Apply spike-frequency adaptation (simple model)
        if (state.adaptationVariable > 0.0f) {
            V -= state.adaptationVariable * 0.01f;
            pImpl->synapticConductance *= 0.95f;  // Decay adaptation
        }
        
        // Clamp to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
    }
    
    // Record time of last update
    pImpl->lastUpdateTime = static_cast<float>(dt);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update synaptic conductance with short-term plasticity
    // Simple model: conductance decays exponentially when not activated
    float &conductance = const_cast<float&>(synapse->getConductance());
    if (conductance > 0.0f) {
        conductance *= (1.0f - dt * 0.1f);  // Decay rate 0.1/ms
        if (conductance < 0.001f) conductance = 0.0f;
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    const NeuronState& state = neuron->getState();
    MembranePotential synapseConductance = synapse->getConductance();
    SynapseType synType = synapse->getType();
    
    // Convert synaptic conductance to membrane current
    // I = g * (V_reversal - V_memb)
    // For excitatory synapses: V_reversal ~ 0mV
    // For inhibitory synapses: V_reversal ~ -70mV
    
    float reversalPotential = 0.0f;  // Excitatory default
    if (synType == SynapseType::Inhibitory || synType == SynapseType::Inhibitory) {
        reversalPotential = state.restingPotential;
    }
    
    float inputCurrent = synapseConductance * (reversalPotential - state.membranePotential);
    
    // Apply to neuron (store in synaptic input for LIF integration)
    // The neuron will integrate this over the next timestep
    if (synapseConductance > 0.0f) {
        neuron->injectCurrent(inputCurrent);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const NeuronState& state = neuron->getState();
    return state.membranePotential >= state.threshold;
}

void IntegrateAndFireDynamics::reset() {
    pImpl->synapticConductance = 0.0f;
    pImpl->lastUpdateTime = 0.0f;
}

// NeuralDynamics abstract base class implementation
// Provides default implementations for convenience

void NeuralDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Default: no update (placeholder)
}

void NeuralDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Default: no update (placeholder)
}

void NeuralDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Default: no application (placeholder)
}

bool NeuralDynamics::shouldFire(const Neuron* neuron) const {
    // Default: never fire (placeholder)
    return false;
}

void NeuralDynamics::reset() {
    // Default: nothing to reset
}

} // namespace nlm
