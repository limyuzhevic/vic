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
    const auto& state = neuron->getState();
    
    // Real integrate-and-fire dynamics: dV/dt = (V_rest - V)/tau + I/C
    // Using exponential Euler integration for better stability
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Exponential Euler integration: V_new = V_rest + (V - V_rest) * exp(-dt/tau) + I * R * (1 - exp(-dt/tau))
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float V_new = V_rest + (V - V_rest) * alpha + I * R * (1.0f - alpha);
    
    // Apply spike-frequency adaptation if active
    if (neuron->getState().adaptationVariable > 0.0f) {
        V_new -= neuron->getState().adaptationVariable * 0.01f;
    }
    
    neuron->setMembranePotential(V_new);
    
    // Check for firing threshold crossing
    if (shouldFire(neuron)) {
        // Record spike with current time
        neuron->recordSpike(0.0f);  // TODO: pass actual simulation time
        
        // Reset membrane potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Increment adaptation for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
        
        neuron->setFiringState(FiringState::Active);
    }
    
    // Refractory mechanism: hold at reset potential during refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
        // During refractory, don't integrate further
        neuron->clearTotalCurrent();
        return;
    }
    
    // Clear synaptic current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with exponential decay
    // dW/dt = -W/τ + activity-dependent changes
    
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    
    // Synaptic decay: exponential decay towards baseline
    float decayTimeConstant = 50.0f;  // ms
    float decayFactor = std::exp(-static_cast<float>(dt) / decayTimeConstant);
    float decayedWeight = weight * decayFactor;
    
    // Add small baseline noise to prevent exact zeros
    float noise = (std::rand() % 100) / 100.0f * 0.001f - 0.0005f;
    
    // Apply synaptic plasticity updates if enabled
    if (synapse->isPlastic()) {
        // Get eligibility trace from synapse for reward-modulated plasticity
        float trace = synapse->getEligibilityTrace();
        
        // Decay eligibility trace
        trace *= decayFactor;
        
        // Apply trace to weight if significant
        if (std::abs(trace) > 1e-6f) {
            float plasticityDelta = trace * 0.01f;  // Learning rate
            decayedWeight += plasticityDelta;
            
            // Reset trace after application
            synapse->setEligibilityTrace(0.0f);
        }
    }
    
    // Clamp weight to valid range
    decayedWeight = std::clamp(decayedWeight, -1.0f, 1.0f);
    
    synapse->setWeight(decayedWeight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with conductance-based dynamics
    // Post-synaptic current: I_syn = g_syn * (V - E_syn)
    
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -80.0f;  // mV
    
    // Convert weight to conductance (simplified)
    float conductance = std::abs(weight) * 0.1f;  // nS
    
    // Get current membrane potential
    float V = neuron->getMembranePotential();
    
    // Calculate synaptic current
    float synapticCurrent = conductance * (V - reversalPotential);
    
    // Apply synaptic input to neuron based on type
    if (synapse->isExcitatory()) {
        // Excitatory input: add current to membrane
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        // Inhibitory input: subtract current from membrane
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
    
    // Update synapse timestamp for STDP
    synapse->updateLastSpikeTime(0.0f);  // TODO: pass actual time
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
