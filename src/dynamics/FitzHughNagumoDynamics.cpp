#include "FitzHughNagumoDynamics.hpp"
#include <cmath>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    float threshold;           // Spike threshold (mV)
    float resetVoltage;         // Reset voltage after spike (mV)
    float timeConstant;         // Time constant for recovery variable (ms)
    float recoveryVariable;     // Recovery variable (u)
    float maxRecovery;          // Maximum recovery value
    float minRecovery;          // Minimum recovery value
    
    Impl() : threshold(20.0f), resetVoltage(0.0f), timeConstant(10.0f),
             recoveryVariable(0.0f), maxRecovery(2.0f), minRecovery(-2.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    /*
     * FitzHugh-Nagumo (FHN) dynamics implementation
     * 
     * Simplified excitable system based on:
     * - Morris-Lecar model variant for action potential generation
     * - Fast activator variable (v) and slow recovery variable (u)
     * - Produces spike trains and oscillatory behavior
     * 
     * Mathematical formulation:
     * dv/dt = (v - v^3/3 - u + I_inj)/tau
     * du/dt = (a*v + b - c*u)/tau_u
     * 
     * Where:
     *   v = membrane potential (activator)
     *   u = recovery variable (inhibitory)
     *   I_inj = injected current
     *   tau, tau_u = time constants
     *   a, b, c = parameters controlling excitability
     * 
     * Biological inspiration:
     *   - Models excitable membrane systems
     *   - Captures threshold dynamics and refractoriness
     *   - Produces realistic spike patterns
     *   - Used in neuron models for neural coding
     */
    
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float I_stim = neuron->getTotalCurrent();
    
    // Convert voltage to FHN model units (v, u)
    // Scale to match FHN mathematical formulation
    float v = (V - state.restingPotential) / 50.0f;  // Scale to reasonable range
    
    // FHN model parameters
    float a = 0.7f;      // Recovery parameter
    float b = 0.8f;      // Recovery parameter  
    float c = 0.08f;     // Recovery parameter
    float tau_v = 1.0f;  // Voltage time constant (ms)
    float tau_u = 10.0f; // Recovery time constant (ms)
    
    // Compute recovery variable dynamics
    float dv_dt = (v - (v*v*v)/3.0f - pImpl->recoveryVariable + I_stim) / tau_v;
    float du_dt = (a*v + b - c*pImpl->recoveryVariable) / tau_u;
    
    // Update variables using Euler integration
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    v += dv_dt * dt_ms;
    pImpl->recoveryVariable += du_dt * dt_ms;
    
    // Clamp recovery variable to bounds
    pImpl->recoveryVariable = std::clamp(pImpl->recoveryVariable, pImpl->minRecovery, pImpl->maxRecovery);
    
    // Convert back to membrane potential
    V = v * 50.0f + state.restingPotential;
    neuron->setMembranePotential(V);
    
    // Check for firing (threshold crossing)
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        
        // Reset after spike with recovery variable adjustment
        V = pImpl->resetVoltage;
        neuron->setMembranePotential(V);
        
        // Increase recovery variable to prevent immediate re-firing
        pImpl->recoveryVariable += 0.5f;
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update synapse for FHN model (similar to other dynamics)
    synapse->step(0.0);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Real synaptic input based on FHN model
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(static_cast<MembranePotential>(weight));
    } else {
        neuron->receiveInhibitoryInput(std::abs(static_cast<MembranePotential>(weight)));
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron && neuron->getMembranePotential() >= neuron->getThreshold();
}

void FitzHughNagumoDynamics::reset() {
    pImpl->recoveryVariable = 0.0f;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setResetVoltage(float voltage) {
    pImpl->resetVoltage = voltage;
}

float FitzHughNagumoDynamics::getResetVoltage() const {
    return pImpl->resetVoltage;
}

void FitzHughNagumoDynamics::setTimeConstant(float tau) {
    pImpl->timeConstant = tau;
}

float FitzHughNagumoDynamics::getTimeConstant() const {
    return pImpl->timeConstant;
}

} // namespace nlm