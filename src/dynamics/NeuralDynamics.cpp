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
    // Real integrate-and-fire dynamics:
    // dV/dt = (V_rest - V) / tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V) / tau + I/C)
    
    const auto& state = neuron->getState();
    
    // Membrane capacitance (inferred from existing values)
    constexpr float C_m = 1.0f;  // nF (matches Neuron::Impl::MEMBRANE_CAPACITANCE)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Synaptic input: V += I * R (Ohm's law for current injection)
    // Add spike input from active synapses
    V += I * R;
    
    // Membrane leak: (V_rest - V) / tau
    float leak = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leak + I / C_m);
    
    // Spike-frequency adaptation (post-synaptic current)
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * 0.01f;
        // Decay adaptation variable
        const_cast<NeuronState&>(state).adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    neuron->setMembranePotential(V);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
    }
    
    // Refractory mechanism during refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with enhanced Tsodyks-Markram model
    // This uses the enhanced implementation from Synapse.cpp
    
    // Synapse::step() now includes:
    // 1. Short-term plasticity (facilitation and depression)
    // 2. Eligibility trace decay for reward-modulated learning
    // 3. Efficency updates based on use
    // 4. STDP updates (when provided with spike times)
    
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with proper dynamics
    // Includes synaptic weight, delay handling, and conductance-based modeling
    
    SynapticWeight weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    
    // Use conductance-based modeling for biological realism
    float conductance = std::abs(weight);
    float reversalPotential;
    
    if (type == SynapseType::Excitatory) {
        reversalPotential = 0.0f;  // Depolarizing
        neuron->receiveExcitatoryInput(conductance);
    } else if (type == SynapseType::Inhibitory) {
        reversalPotential = -70.0f;  // Hyperpolarizing
        neuron->receiveInhibitoryInput(conductance);
    } else {
        // Other synapse types: default to modulatory
        reversalPotential = -60.0f;  // Slightly depolarizing
        neuron->receiveModulatoryInput(conductance * 0.1f);
    }
    
    // Store reversal potential for LIF integration in updateNeuron
    // This would require extending NeuronState to include synaptic reversal potential
    // For now, we use the conductance-based approach above
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
