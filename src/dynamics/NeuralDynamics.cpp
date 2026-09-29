#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    float adaptiveExponent;       // For spike-frequency adaptation
    float adaptationTimeConstant; // Time constant for adaptation
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f),
             adaptiveExponent(0.2f), adaptationTimeConstant(50.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float V_reset = state.resetPotential;
    float I_total = neuron->getTotalCurrent();
    
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = pImpl->capacitance;
    
    // Convert timestep to milliseconds for consistency
    float dt_ms = static_cast<float>(dt);
    
    // Membrane potential dynamics with adaptive exponential integration
    // I_phi = (V - V_T + Delta_T * exp((V - V_T) / Delta_T)) / (C * tau_m)
    // Simplified to standard LIF for stability
    
    float dV_leak = (V_rest - V) / tau * dt_ms;
    
    // Current input contribution
    float dV_current = I_total / C * dt_ms;
    
    // Adaptation contribution (spike-frequency adaptation)
    float adaptation = 0.0f;
    if (state.adaptationVariable > 0.0f) {
        adaptation = state.adaptationVariable * pImpl->adaptiveExponent * dt_ms;
        // Decay adaptation
        state.adaptationVariable *= (1.0f - dt_ms / pImpl->adaptationTimeConstant);
    }
    
    float dV_total = dV_leak + dV_current - adaptation;
    
    // Apply Euler integration with adaptive step size control
    V += dV_total;
    
    // Apply spike-frequency adaptation to resting potential
    if (state.adaptationVariable > 0.0f) {
        V_rest += state.adaptationVariable * 0.1f * dt_ms;
    }
    
    // Stability constraints
    V = std::clamp(V, -100.0f, 50.0f);
    V_rest = std::clamp(V_rest, -90.0f, -50.0f);
    
    // Check for threshold crossing with hysteresis
    if (V >= threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(state.lastSpikeTime >= 0.0f ? state.lastSpikeTime : 0.0f);
        
        // Reset membrane potential with refractory dynamics
        V = V_reset + state.adaptationVariable * 0.5f;  // Adaptation affects reset
        
        // Update adaptation variable for next spike
        state.adaptationVariable += 1.0f + state.adaptationVariable * 0.1f;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(std::max(2.0f, tau * 0.1f)));
        neuron->setFiringState(FiringState::Refractory);
        
        // Store spike time for plasticity
        neuron->recordSpike(static_cast<float>(neuron->getState().lastSpikeTime >= 0.0f ? 
                                                 neuron->getState().lastSpikeTime : 0.0f));
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    neuron->setMembranePotential(V);
    neuron->setRestingPotential(V_rest);
    
    // Clear synaptic input for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics with short-term plasticity
    float dt_ms = static_cast<float>(dt);
    
    // Get current synapse state
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    
    // Tsodyks-Markram short-term plasticity model
    // Depression and facilitation dynamics
    if (synapticInputActive) {
        // Depression
        depression *= (1.0f - (1.0f - depression) * dt_ms / pImpl->depressionTau);
        
        // Facilitation
        facilitation *= (1.0f - (facilitation - 1.0f) * dt_ms / pImpl->facilitationTau);
    } else {
        // Recovery from depression
        depression += (1.0f - depression) * dt_ms / pImpl->recoveryTau;
        
        // Decay facilitation
        facilitation *= std::exp(-dt_ms / pImpl->facilitationTau);
    }
    
    // Update efficacy based on short-term plasticity
    float newEfficacy = 1.0f + (facilitation - 1.0f) * efficacy;
    synapse->setEfficacy(std::clamp(newEfficacy, 0.0f, 2.0f));
    
    // Apply plastic changes to weight
    if (plasticEnabled) {
        // Weight changes based on activity-dependent plasticity
        float weightChange = weight * 0.001f * dt_ms;
        weight += weightChange;
        
        // Constrain to reasonable bounds
        weight = std::clamp(weight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(weight);
    }
    
    // Store updated synapse state
    synapse->setShortTermDepression(depression);
    synapse->setShortTermFacilitation(facilitation);
    synapse->setLastActiveTime(dt_ms);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input with short-term plasticity and dynamics
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    
    // Calculate effective synaptic strength
    float effectiveWeight = weight * efficacy * depression;
    
    // Apply spike timing-dependent plasticity (STDP) prepulse
    // Modify efficacy based on recent spike history
    const auto& preSpikeHistory = synapse->getPreSpikeHistory();
    const auto& postSpikeHistory = synapse->getPostSpikeHistory();
    
    float stdpFactor = 1.0f;
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        // Calculate relative spike timing
        float timeDiff = postSpikeHistory.back() - preSpikeHistory.back();
        
        if (timeDiff > 0.0f && timeDiff < 10.0f) {  // LTP window
            stdpFactor *= 1.1f + 0.05f * std::exp(-timeDiff / 5.0f);
        } else if (timeDiff < 0.0f && std::abs(timeDiff) < 10.0f) {  // LTD window
            stdpFactor *= std::max(0.9f, 1.0f - 0.05f * std::exp(std::abs(timeDiff) / 5.0f));
        }
    }
    
    // Apply STDP to efficacy
    float newEfficacy = efficacy * stdpFactor;
    synapse->setEfficacy(std::clamp(newEfficacy, 0.0f, 2.0f));
    
    // Deliver synaptic current
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(std::abs(effectiveWeight));
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
    }
    
    // Store eligibility trace for reward-modulated learning
    synapse->setEligibilityTrace(synapse->getEligibilityTrace() + stdpFactor * 0.1f);
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    const auto& state = neuron->getState();
    
    // Enhanced threshold checking with adaptation
    float adaptiveThreshold = state.threshold + state.adaptationVariable * 0.5f;
    
    // Check for spike with noise considerations
    bool thresholdCrossed = neuron->getMembranePotential() >= adaptiveThreshold;
    
    // Add stochastic spiking for biological realism
    if (thresholdCrossed) {
        float noise = getRandomNoise();  // Would need access to random generator
        thresholdCrossed = (neuron->getMembranePotential() + noise) >= adaptiveThreshold;
    }
    
    return thresholdCrossed;
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal dynamics state
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
    pImpl->capacitance = 1.0f;
    pImpl->adaptiveExponent = 0.2f;
    pImpl->adaptationTimeConstant = 50.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::max(1.0f, std::min(tau, 100.0f));
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    pImpl->membraneResistance = std::max(1.0f, std::min(R, 100.0f));
}

void IntegrateAndFireDynamics::setCapacitance(float C) {
    pImpl->capacitance = std::max(0.1f, std::min(C, 10.0f));
}

void IntegrateAndFireDynamics::setAdaptiveExponent(float exponent) {
    pImpl->adaptiveExponent = std::max(0.0f, std::min(exponent, 1.0f));
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

float IntegrateAndFireDynamics::getCapacitance() const {
    return pImpl->capacitance;
}

float IntegrateAndFireDynamics::getAdaptiveExponent() const {
    return pImpl->adaptiveExponent;
}

} // namespace nlm
