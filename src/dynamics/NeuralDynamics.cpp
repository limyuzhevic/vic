#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <memory>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    float adaptiveThreshold;      // mV/ms
    float adaptationCurrent;       // pA
    float adaptationTau;           // ms
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), 
             capacitance(1.0f), adaptiveThreshold(0.01f), 
             adaptationCurrent(0.0f), adaptationTau(100.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    // LIF dynamics with adaptive threshold and spike-frequency adaptation
    // dV/dt = (V_rest - V)/tau + I/C + I_adaptive/C
    // where I_adaptive accounts for spike-frequency adaptation
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    
    float I_total = neuron->getTotalCurrent();
    float I_adaptive = pImpl->adaptationCurrent;
    
    float tau = pImpl->membraneTimeConstant;
    float C = pImpl->capacitance;
    
    // Synaptic current contributes to membrane potential change
    float synapticCurrent = I_total;
    
    // Adaptive current contributes to threshold modulation
    float adaptiveCurrent = I_adaptive;
    
    // Leaky integration: V_new = V + dt * ((V_rest - V)/tau + (I_total + I_adaptive)/C)
    float dV_leak = (V_rest - V) / tau * static_cast<float>(dt);
    float dV_synaptic = synapticCurrent / C * static_cast<float>(dt);
    float dV_adaptive = adaptiveCurrent / C * static_cast<float>(dt);
    
    float V_new = V + dV_leak + dV_synaptic + dV_adaptive;
    
    // Apply spike-frequency adaptation (slow negative feedback)
    if (neuron->hasRecentSpike(0.01f)) {  // Spike in last 10ms
        pImpl->adaptationCurrent += 5.0f;  // Increase adaptation current
        pImpl->adaptationCurrent = std::clamp(pImpl->adaptationCurrent, 0.0f, 20.0f);
    }
    
    // Decay adaptation current
    pImpl->adaptationCurrent *= std::exp(-static_cast<float>(dt) / pImpl->adaptationTau);
    
    // Check for spike with adaptive threshold
    float effective_threshold = threshold + neuron->getFiringRate() * 0.5f; // Rate-dependent threshold
    
    if (V_new >= effective_threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(state.lastSpikeTime + static_cast<float>(dt));
        
        // Reset membrane potential
        neuron->setMembranePotential(V_reset);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(2);  // 2ms refractory
        
        // Increase adaptation current (spike-triggered adaptation)
        pImpl->adaptationCurrent += 10.0f;
        pImpl->adaptationCurrent = std::clamp(pImpl->adaptationCurrent, 0.0f, 20.0f);
    } else {
        neuron->setMembranePotential(V_new);
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear synaptic current for next step
    neuron->clearTotalCurrent();
    
    // Clamp to reasonable bounds
    float V_clamped = std::clamp(neuron->getMembranePotential(), -100.0f, 50.0f);
    neuron->setMembranePotential(V_clamped);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics using Tsodyks-Markram model
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    if (synapse->getLastPreSpikeTime() >= 0.0f) {
        float timeSincePre = static_cast<float>(dt) + synapse->getLastPreSpikeTime();
        synapse->setShortTermFacilitation(
            synapse->getShortTermFacilitation() * std::exp(-timeSincePre / pImpl->membraneTimeConstant)
        );
    }
    
    // 2. Decay short-term depression
    synapse->decayShortTermDepression(0.001f);  // Fast depression
    
    // 3. Update synaptic efficacy based on recent activity
    if (synapse->hasRecentPreSpike(0.01f) || synapse->hasRecentPostSpike(0.01f)) {
        // Use-dependent modulation
        float currentEfficacy = synapse->getEfficacy();
        synapse->setEfficacy(currentEfficacy * 1.01f);  // Slight potentiation
    }
    
    // 4. Decay eligibility trace for reward-modulated learning
    synapse->decayEligibilityTrace(0.001f);  // Fast decay
    
    // 5. Apply synaptic plasticity if enabled
    if (synapse->getPlasticityFlags().stdp) {
        // Apply STDP based on pre- and post-synaptic spike history
        synapse->applySTDP(0.001f); // Learning rate
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with short-term plasticity
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    
    // Modified weight: W' = W * depression * facilitation * efficacy
    float effectiveWeight = weight * depression * facilitation * efficacy;
    
    if (synapse->isExcitatory()) {
        // For excitatory synapses: I = g * (V - E), where g = weight * depression * facilitation
        // For simplicity: I_excitatory = effectiveWeight
        neuron->receiveExcitatoryInput(effectiveWeight);
        
        // Update post-synaptic trace for STDP
        neuron->addToAdaptationVariable(effectiveWeight * 0.1f);
        
    } else {
        // For inhibitory synapses: I_inhibitory = -g * (V - E) ≈ -effectiveWeight for reversal potential
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
        
        // Update adaptation
        neuron->addToAdaptationVariable(-effectiveWeight * 0.1f);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    const auto& state = neuron->getState();
    
    // Check if membrane potential exceeds adaptive threshold
    float V = neuron->getMembranePotential();
    float threshold = state.threshold;
    
    // Adjust threshold based on firing rate (adaptation)
    float rateDependentThreshold = threshold + neuron->getFiringRate() * 0.5f;
    
    return V >= rateDependentThreshold;
}

void IntegrateAndFireDynamics::reset() {
    pImpl->adaptationCurrent = 0.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
    // Update resistance to maintain time constant consistency
    pImpl->membraneResistance = pImpl->capacitance * pImpl->membraneTimeConstant / 1000.0f;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm

