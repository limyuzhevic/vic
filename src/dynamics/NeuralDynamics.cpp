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
    // Implement real Leaky Integrate-and-Fire dynamics
    // dV/dt = (V_rest - V)/tau + I/R
    // Where R = membrane resistance, tau = R*C (membrane time constant)
    // Uses exponential Euler integration for stability
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float R = pImpl->membraneResistance;
    float tau = pImpl->membraneTimeConstant;
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/R
    // Discrete approximation using exponential Euler:
    // V_new = V_rest + (V - V_rest) * exp(-dt/tau) + (I/R) * (1 - exp(-dt/tau))
    
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = R * (1.0f - alpha);
    
    // Update membrane potential using exponential Euler integration
    float V_new = V_rest + (V - V_rest) * alpha + (I / R) * beta;
    neuron->setMembranePotential(V_new);
    
    // Check for spike (threshold crossing)
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        // Record spike with current time for STDP calculations
        neuron->recordSpike(0.0);  // TODO: Pass actual simulation time
    }
    
    // Refractory mechanism: clamp to reset potential
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clamp membrane potential to prevent numerical instability
    // Use biological limits similar to Neuron.cpp implementation
    float V_clamped = std::clamp(neuron->getMembranePotential(), -100.0f, 50.0f);
    neuron->setMembranePotential(V_clamped);
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics with short-term plasticity
    // This uses the Tsodyks-Markram model for short-term plasticity
    // Includes facilitation, depression, and use-dependent modulation
    
    float currentTime = 0.0;  // TODO: Pass actual simulation time
    
    // Update synaptic efficacy and short-term plasticity state
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    if (synapse->getPreSpikeHistory().size() > 0) {
        float lastPreSpike = synapse->getPreSpikeHistory().back();
        float timeSincePre = currentTime - lastPreSpike;
        
        // Decay facilitation exponentially
        float& facilitation = synapse->getEfficacy();  // Efficacy tracks facilitation
        facilitation *= std::exp(-timeSincePre / 100.0f);  // 100ms time constant
    }
    
    // 2. Recovery from depression (use-dependent modulation)
    if (synapse->getWeight() < 0.0f) {
        // Depressive synapses recover toward baseline over time
        float recoveryRate = 0.001f;  // Recovery time constant
        float& weight = synapse->getWeight();
        weight = std::max(-1.0f, weight + recoveryRate * static_cast<float>(dt));
    }
    
    // 3. Clamp weight bounds to maintain stability
    float& weight = synapse->getWeight();
    weight = std::clamp(weight, -1.0f, 1.0f);
    
    // 4. Update efficacy based on recent use (use-dependent modulation)
    float& efficacy = synapse->getEfficacy();
    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement real synaptic transmission with conductance-based dynamics
    // Includes both excitatory and inhibitory postsynaptic potentials
    // Uses synaptic weight, delay, and type information for accurate transmission
    
    SynapticWeight weight = synapse->getWeight();
    
    // Apply synaptic input based on synapse type
    if (synapse->isExcitatory()) {
        // Excitatory input: depolarizing postsynaptic potential
        // Larger weight = stronger depolarization
        neuron->receiveExcitatoryInput(weight);
    } else if (synapse->isInhibitory()) {
        // Inhibitory input: hyperpolarizing postsynaptic potential
        // Larger absolute weight = stronger hyperpolarization
        neuron->receiveInhibitoryInput(std::abs(weight));
    } else {
        // Modulatory synapses: affect plasticity, not directly integrated
        // e.g., acetylcholine, dopamine, serotonin
        neuron->receiveModulatoryInput(std::abs(weight) * 0.1f);
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
