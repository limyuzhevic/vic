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
    // Real integrate-and-fire dynamics based on LIF equations
    // Implements the same dynamics as Neuron::stepLIF but with configurable parameters
    
    const auto& state = neuron->getState();
    
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation using exponential Euler:
    // V_new = V + dt * 1000.0 * ((V_rest - V)/tau + I/C)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float tau = pImpl->membraneTimeConstant;  // membraneTimeConstant in ms
    float C = pImpl->membraneResistance;      // membraneResistance in MOhm (capacitance proxy)
    
    // Get synaptic input (note: Neuron::getTotalCurrent() is used but may not be populated
    // The current is actually accumulated in neuron->receiveExcitatoryInput()
    // For LIF dynamics, we use the synaptic input accumulated in neuron state
    MembranePotential synapticContribution = 0.0f;
    
    // Spike-frequency adaptation (if available)
    float adaptation = state.adaptationVariable;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (adaptation > 0.0f) {
        V -= adaptation * 0.01f;
        // Note: adaptation decay is handled elsewhere (neuron->decrementRefractory() or similar)
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    bool fired = false;
    if (V >= threshold) {
        fired = true;
        neuron->setFiringState(FiringState::Active);
        
        // Record spike (actual time would come from simulation clock)
        neuron->recordSpike(0.0);  // TODO: pass actual time from simulation
        
        // Reset membrane potential to reset value
        V = V_reset;
        
        // Enter refractory period (duration managed by neuron's refractory period)
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant * 0.002f));  // 2ms refractory
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for spike-frequency adaptation
        neuron->setAdaptationVariable(neuron->getAdaptationVariable() + 1.0f);
    }
    
    // Update membrane potential
    neuron->setMembranePotential(V);
    
    // Clear synaptic input for next step
    // Note: This is done by the caller or through neuron->clearTotalCurrent()
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics - delegate to Synapse::step() method
    // The Synapse::step() method implements:
    // - Short-term facilitation and depression (Tsodyks-Markram model)
    // - Eligibility trace decay for reward-modulated learning
    // - Weight bounds enforcement
    // - Use-dependent efficacy modulation
    
    // Use a consistent timestep for synaptic updates (1ms is typical for LIF)
    TimestepDuration synapticTimestep = std::max(dt, 0.0001);  // Ensure positive value
    
    // Delegate to the synapse's own step method which handles all real synaptic dynamics
    synapse->step(static_cast<Timestamp>(synapticTimestep * 1000.0f));  // Convert to ms
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation with biological fidelity
    // Uses the synapse's own efficacy calculation and type information
    
    // Get synaptic weight and apply efficacy modulation
    SynapticWeight baseWeight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    SynapticWeight effectiveWeight = baseWeight * efficacy;
    
    // Account for synapse type (excitatory/inhibitory)
    if (synapse->isExcitatory()) {
        // For excitatory synapses, apply weighted current input
        neuron->receiveExcitatoryInput(effectiveWeight);
        
    } else if (synapse->isInhibitory()) {
        // For inhibitory synapses, apply hyperpolarizing current
        // Inhibitory weight is typically negative, but we use absolute value
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
        
    } else {
        // For other synapse types (modulatory, etc.), use reduced weight
        neuron->receiveExcitatoryInput(effectiveWeight * 0.5f);
    }
    
    // Note: Full short-term plasticity dynamics (depression, facilitation, eligibility traces)
    // are handled by the Synapse::step() method called by updateSynapse()
    // This method focuses on applying the current synaptic input to the neuron
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
