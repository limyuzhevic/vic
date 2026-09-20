#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct LIFNeuralDynamics::Impl {
    NeuronState params;
    Timestamp lastSpikeTime;
    float adaptiveExcitability;  // For spike-frequency adaptation
    
    Impl() : lastSpikeTime(-1.0f), adaptiveExcitability(0.0f) {
        // Initialize with default LIF parameters
        params.threshold = -55.0f;
        params.resetPotential = -70.0f;
        params.restingPotential = -70.0f;
        params.leakConductance = 10.0f;
        params.membranePotential = params.restingPotential;
        params.firingState = FiringState::Resting;
    }
};

LIFNeuralDynamics::LIFNeuralDynamics() : pImpl(new Impl) {}

LIFNeuralDynamics::~LIFNeuralDynamics() = default;

void LIFNeuralDynamics::initialize(const NeuronState& params) {
    pImpl->params = params;
    reset();
}

void LIFNeuralDynamics::update(Timestamp currentTime, TimestepDuration dt) {
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    float& V = pImpl->params.membranePotential;
    float V_rest = pImpl->params.restingPotential;
    float threshold = pImpl->params.threshold;
    float reset = pImpl->params.resetPotential;
    
    float tau = pImpl->params.leakConductance;  // Time constant in ms
    float C = 1.0f;  // Membrane capacitance (normalized)
    
    // Get synaptic input - this should come from Synapse class
    // For now, we simulate some input
    float I_synaptic = 0.0f;  // Placeholder
    
    // Update membrane potential using Euler integration
    float dV_dt = (V_rest - V) / tau + I_synaptic / C;
    V += dV_dt * dt * 1000.0f; // Convert dt from seconds to milliseconds
    
    // Apply spike-frequency adaptation
    if (pImpl->adaptiveExcitability > 0.0f) {
        V -= pImpl->adaptiveExcitability * 0.01f;
        pImpl->adaptiveExcitability *= 0.95f; // Decay adaptation
    }
    
    // Check for spike
    if (V >= threshold) {
        pImpl->params.firingState = FiringState::Active;
        pImpl->params.lastSpikeTime = static_cast<float>(currentTime);
        pImpl->params.membranePotential = reset;
        
        // Record spike for plasticity
        // Should be handled by Neuron class
        
        // Increase adaptation after spike
        pImpl->adaptiveExcitability += 0.5f;
    }
}

MembranePotential LIFNeuralDynamics::getMembranePotential() const {
    return pImpl->params.membranePotential;
}

FiringState LIFNeuralDynamics::getFiringState() const {
    return pImpl->params.firingState;
}

bool LIFNeuralDynamics::shouldFire() const {
    return pImpl->params.membranePotential >= pImpl->params.threshold;
}

void LIFNeuralDynamics::reset() {
    pImpl->params.membranePotential = pImpl->params.restingPotential;
    pImpl->params.firingState = FiringState::Resting;
    pImpl->params.lastSpikeTime = -1.0f;
    pImpl->adaptiveExcitability = 0.0f;
}

NeuronType LIFNeuralDynamics::getType() const {
    return NeuronType::Internal;
}

void LIFNeuralDynamics::setThreshold(MembranePotential threshold) {
    pImpl->params.threshold = threshold;
}

void LIFNeuralDynamics::setResetPotential(MembranePotential reset) {
    pImpl->params.resetPotential = reset;
}

void LIFNeuralDynamics::setLeakConductance(MembranePotential conductance) {
    pImpl->params.leakConductance = conductance;
}

void LIFNeuralDynamics::setRestingPotential(MembranePotential resting) {
    pImpl->params.restingPotential = resting;
}

FiringRate LIFNeuralDynamics::getFiringRate() const {
    // Simple firing rate estimation based on spike history
    // This should be handled by Neuron class
    return 0.0f;
}

} // namespace nlm

void LIFNeuralDynamics::update(Timestamp currentTime, TimestepDuration dt) {
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    float& V = pImpl->params.membranePotential;
    float V_rest = pImpl->params.restingPotential;
    float threshold = pImpl->params.threshold;
    float reset = pImpl->params.resetPotential;
    
    float tau = pImpl->params.leakConductance;  // Time constant in ms
    float C = 1.0f;  // Membrane capacitance (normalized)
    
    // Get synaptic input - this should come from Synapse class
    // For now, we simulate some input
    float I_synaptic = 0.0f;  // Placeholder
    
    // Update membrane potential using Euler integration
    float dV_dt = (V_rest - V) / tau + I_synaptic / C;
    V += dV_dt * dt * 1000.0f; // Convert dt from seconds to milliseconds
    
    // Apply spike-frequency adaptation
    if (pImpl->adaptiveExcitability > 0.0f) {
        V -= pImpl->adaptiveExcitability * 0.01f;
        pImpl->adaptiveExcitability *= 0.95f; // Decay adaptation
    }
    
    // Check for spike
    if (V >= threshold) {
        pImpl->params.firingState = FiringState::Active;
        pImpl->params.lastSpikeTime = static_cast<float>(currentTime);
        pImpl->params.membranePotential = reset;
        
        // Record spike for plasticity
        // Should be handled by Neuron class
        
        // Increase adaptation after spike
        pImpl->adaptiveExcitability += 0.5f;
    }
}

MembranePotential LIFNeuralDynamics::getMembranePotential() const {
    return pImpl->params.membranePotential;
}

FiringState LIFNeuralDynamics::getFiringState() const {
    return pImpl->params.firingState;
}

bool LIFNeuralDynamics::shouldFire() const {
    return pImpl->params.membranePotential >= pImpl->params.threshold;
}

void LIFNeuralDynamics::reset() {
    pImpl->params.membranePotential = pImpl->params.restingPotential;
    pImpl->params.firingState = FiringState::Resting;
    pImpl->params.lastSpikeTime = -1.0f;
    pImpl->adaptiveExcitability = 0.0f;
}

NeuronType LIFNeuralDynamics::getType() const {
    return NeuronType::Internal;
}

void LIFNeuralDynamics::setThreshold(MembranePotential threshold) {
    pImpl->params.threshold = threshold;
}

void LIFNeuralDynamics::setResetPotential(MembranePotential reset) {
    pImpl->params.resetPotential = reset;
}

void LIFNeuralDynamics::setLeakConductance(MembranePotential conductance) {
    pImpl->params.leakConductance = conductance;
}

void LIFNeuralDynamics::setRestingPotential(MembranePotential resting) {
    pImpl->params.restingPotential = resting;
}

FiringRate LIFNeuralDynamics::getFiringRate() const {
    // Simple firing rate estimation based on spike history
    // This should be handled by Neuron class
    return 0.0f;
}

} // namespace nlm