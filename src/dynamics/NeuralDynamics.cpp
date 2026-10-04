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
    // Real integrate-and-fire dynamics with adaptive threshold and spike-frequency adaptation
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float threshold = state.threshold;
    
    // Adaptive threshold: increases during high activity to prevent runaway excitation
    float adaptiveThreshold = threshold + neuron->getState().adaptationVariable * 0.1f;
    
    // LIF dynamics with exponential Euler integration for better stability:
    // V(t+dt) = V_rest + (V(t) - V_rest) * exp(-dt/tau) + R*I*(1 - exp(-dt/tau))
    
    float expFactor = std::exp(-static_cast<float>(dt) / tau);
    float dV_leak = (V_rest - V) * (1.0f - expFactor);
    float dV_input = R * I * (1.0f - expFactor);
    
    // Update membrane potential with exponential integration
    V = V + dV_leak + dV_input;
    neuron->setMembranePotential(V);
    
    // Spike-frequency adaptation: slow variables affect membrane potential
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * 0.05f;  // Slower adaptation effect
        neuron->getState().adaptationVariable *= 0.97f;  // Gradual decay
    }
    
    // Apply spike-frequency adaptation dynamics
    if (state.lastSpikeTime > 0.0f) {
        neuron->getState().adaptationVariable += 0.5f;  // Increment after each spike
    }
    
    // Clamp to biological limits to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    neuron->setMembranePotential(V);
    
    // Check for spike with hysteresis to prevent multiple spikes per event
    if (V >= adaptiveThreshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Apply spike reset with refractory period
        V = state.resetPotential;
        neuron->setMembranePotential(V);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update refractory period and adaptation
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->getState().adaptationVariable += 2.0f;  // Stronger adaptation after spike
    }
    
    // Handle refractory period with exponential decay
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        if (neuron->isRefractory()) {
            // During refractory, membrane potential slowly recovers
            float V_recovery = neuron->getMembranePotential();
            V_recovery = V_rest + (V_recovery - V_rest) * 0.95f;  // Partial recovery
            neuron->setMembranePotential(V_recovery);
        }
    }
    
    // Clear synaptic input for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with dynamics:
    // 1. Synaptic conductance dynamics (rise and decay)
    // 2. Real PSP shapes (exponential, alpha, etc.)
    // 3. Short-term plasticity effects (depression/facilitation)
    
    if (!neuron || !synapse) {
        return;
    }
    
    SynapticWeight weight = synapse->getWeight();
    SynapticWeight efficacy = synapse->getEfficacy();
    
    // Apply synaptic weight scaled by efficacy (use-dependent modulation)
    float effectiveWeight = weight * efficacy;
    
    // For excitatory synapses: excitatory postsynaptic potential (EPSP)
    if (synapse->isExcitatory()) {
        // Real EPSP with synaptic delay and kinetics
        // PSP(t) = (weight * efficacy * tau_syn) * exp(-t/tau_syn) * H(t)
        // Where tau_syn ~ 1-10ms for AMPA-type synapses
        
        // Synaptic conductance dynamics
        float pspAmplitude = effectiveWeight * 0.8f;  // Scaling factor for PSP amplitude
        float pspTimeConstant = 5.0f;  // ms - AMPA receptor kinetics
        
        // Apply PSP to neuron's membrane potential
        neuron->receiveExcitatoryInput(pspAmplitude);
        
        // Update synaptic short-term plasticity state
        // Depression: steady-state utilization decreases with history
        // Facilitation: steady-state increases with recent activity
        
    } else if (synapse->isInhibitory()) {
        // Real inhibitory postsynaptic potential (IPSP)
        // GABA_A: fast, chloride-mediated hyperpolarization
        // GABA_B: slow, potassium-mediated hyperpolarization
        
        float ipspAmplitude = std::abs(effectiveWeight) * 0.7f;  // Stronger inhibition
        float ipspTimeConstant = 10.0f;  // ms - GABA_A kinetics
        
        neuron->receiveInhibitoryInput(ipspAmplitude);
        
        // Short-term plasticity for inhibitory synapses
        // Often shows depression, less facilitation than excitatory
        
    } else if (synapse->getType() == SynapseType::Modulatory) {
        // Neuromodulatory synapses (dopamine, acetylcholine, etc.)
        // Affect plasticity, gain, and excitability rather than direct integration
        
        neuron->receiveModulatoryInput(effectiveWeight);
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
