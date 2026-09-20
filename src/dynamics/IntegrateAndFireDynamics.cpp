#include "IntegrateAndFireDynamics.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Membrane time constant
    float membraneTimeConstant;
    
    // Refractory period
    float refractoryPeriod;
    
    // Synaptic capacitance
    float capacitance;
    
    // Adaptation parameters
    float adaptationStrength;
    float adaptationTimeConstant;
    
    // Short-term plasticity parameters
    float depressionTimeConstant;
    float facilitationTimeConstant;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    Impl() : membraneTimeConstant(20.0f), // ms
             refractoryPeriod(2.0f),      // ms
             capacitance(1.0f),            // nF
             adaptationStrength(0.01f),
             adaptationTimeConstant(100.0f),  // ms
             depressionTimeConstant(200.0f),   // ms
             facilitationTimeConstant(100.0f) {} // ms
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() {
    delete pImpl;
}

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    NeuronState& state = neuron->getState();
    
    // Skip if in refractory period
    if (state.refractoryRemaining > 0) {
        return;
    }
    
    // Get current membrane potential
    MembranePotential V = state.membranePotential;
    
    // Calculate leak contribution: (V_rest - V) / tau
    float leakContribution = (state.restingPotential - V) / pImpl->membraneTimeConstant;
    
    // Calculate synaptic contribution: I / C (where I is synaptic input)
    // Note: We need access to synaptic input from neuron state
    // For now, use adaptation variable as proxy for synaptic input
    float synapticContribution = state.adaptationVariable / pImpl->capacitance;
    
    // Apply exponential Euler integration
    // dV/dt = (V_rest - V)/tau + I/C
    float dV = (leakContribution + synapticContribution) * static_cast<float>(dt) * 1000.0f; // Convert ms to s
    
    // Update membrane potential
    state.membranePotential = V + dV;
    
    // Apply adaptation (spike-frequency adaptation)
    if (state.adaptationVariable > 0.0f) {
        // Slow hyperpolarization after spike
        state.membranePotential -= state.adaptationVariable * pImpl->adaptationStrength;
        
        // Decay adaptation variable
        state.adaptationVariable *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->adaptationTimeConstant);
    }
    
    // Clamp to reasonable bounds
    state.membranePotential = std::clamp(state.membranePotential, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    if (state.membranePotential >= state.threshold) {
        neuron->setFiringState(FiringState::Active);
        
        // Record spike for plasticity
        neuron->recordSpike(static_cast<Timestamp>(pImpl->membraneTimeConstant));
        
        // Reset membrane potential
        state.membranePotential = state.resetPotential;
        
        // Enter refractory period
        state.refractoryRemaining = static_cast<uint32_t>(pImpl->refractoryPeriod / dt * 1000.0f);
        
        // Increase adaptation variable for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update short-term plasticity state
    float timeSinceLastActivity = 0.0f;
    
    // Get last spike times from synapse
    const auto& preSpikeHistory = synapse->getPreSpikeHistory();
    const auto& postSpikeHistory = synapse->getPostSpikeHistory();
    
    if (!preSpikeHistory.empty()) {
        timeSinceLastActivity = std::max(timeSinceLastActivity, 
            static_cast<float>(preSpikeHistory.back()) / 1000.0f);
    }
    if (!postSpikeHistory.empty()) {
        timeSinceLastActivity = std::max(timeSinceLastActivity, 
            static_cast<float>(postSpikeHistory.back()) / 1000.0f);
    }
    
    // Apply Tsodyks-Markram short-term plasticity model
    float& depression = synapse->getEfficacy();  // Using efficacy as depression factor
    float& facilitation = synapse->getEligibilityTrace();  // Using eligibility as facilitation
    
    // Decay depression (recovery toward 1.0)
    depression += (1.0f - depression) * (1.0f - std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->depressionTimeConstant));
    
    // Decay facilitation (recovery toward 0.0)
    facilitation *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->facilitationTimeConstant);
    
    // Update synaptic weight based on STP
    float effectiveWeight = synapse->getWeight() * depression * (1.0f + facilitation);
    synapse->setWeight(effectiveWeight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Calculate synaptic current
    // I = g * (V_rev - V) where g is conductance, V_rev is reversal potential
    // For simplicity, use weight as conductance contribution
    float weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        // Excitatory input: depolarizes neuron
        neuron->receiveExcitatoryInput(weight);
    } else if (synapse->isInhibitory()) {
        // Inhibitory input: hyperpolarizes neuron
        neuron->receiveInhibitoryInput(weight);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    
    // Check if membrane potential has crossed threshold
    if (state.membranePotential >= state.threshold) {
        return true;
    }
    
    return false;
}

void IntegrateAndFireDynamics::reset() {
    if (pImpl) {
        pImpl->membraneTimeConstant = 20.0f;
        pImpl->refractoryPeriod = 2.0f;
        pImpl->capacitance = 1.0f;
        pImpl->adaptationStrength = 0.01f;
        pImpl->adaptationTimeConstant = 100.0f;
        pImpl->depressionTimeConstant = 200.0f;
        pImpl->facilitationTimeConstant = 100.0f;
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->membraneTimeConstant = std::max(1.0f, tau);
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->membraneTimeConstant : 20.0f;
}

} // namespace nlm
