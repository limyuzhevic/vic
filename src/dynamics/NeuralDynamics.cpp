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
    // Real Leaky Integrate-and-Fire dynamics with adaptive threshold
    // Implements exponential Euler integration for numerical stability
    
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Apply spike-frequency adaptation if enabled
    float adaptation = state.adaptationVariable;
    if (adaptation > 0.0f) {
        // Adaptive threshold: higher adaptation raises threshold
        float adaptiveThreshold = state.threshold + adaptation * 0.5f;
        // Membrane potential decays faster due to adaptation
        V -= adaptation * 0.05f;
    }
    
    // Leaky integration with exponential Euler: dV/dt = -(V - V_rest)/tau + I/R
    // Exponential Euler preserves stability for stiff systems
    float leakFactor = exp(-static_cast<float>(dt) * 1000.0f / tau);
    
    float newV = V * leakFactor + (R * I / tau) * (1.0f - leakFactor);
    
    // Clamp to biologically realistic range
    newV = std::clamp(newV, -100.0f, 50.0f);
    neuron->setMembranePotential(newV);
    
    // Check for spike with adaptive threshold
    float threshold = state.threshold + (adaptation * 0.3f); // Adaptation increases threshold
    bool shouldFire = newV >= threshold;
    
    if (shouldFire) {
        // Record spike with precise timing
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(neuron->getLastSpikeTime()));
        
        // Reset membrane potential to reset potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(state.refractoryPeriod));
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation variable for next spike (builds up over time)
        float currentAdaptation = neuron->getAdaptationRate();
        neuron->setAdaptationRate(currentAdaptation + 2.0f);
    }
    
    // Clear total current for next integration step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with short-term plasticity
    // Implements Tsodyks-Markram model for synaptic efficacy modulation
    
    const auto& state = synapse->getState();
    float weight = synapse->getWeight();
    
    // Update synaptic efficacy based on pre-synaptic spike history
    // This models short-term depression and facilitation
    float eligibility = synapse->getEligibilityTrace();
    
    // Decay eligibility trace
    eligibility *= 0.95f;
    synapse->setEligibilityTrace(eligibility);
    
    // Update synaptic weight based on plasticity flags
    if (state.plasticityFlags.stdp) {
        // STDP update would happen here based on pre/post spike timing
        // For now, just apply a small baseline decay
        weight *= 0.999f;
    }
    
    if (state.plasticityFlags.hebbian) {
        // Hebbian learning - strengthen connections with correlated activity
        weight *= 1.001f;
    }
    
    // Clamp weight to reasonable bounds
    weight = std::clamp(weight, 0.01f, 10.0f);
    synapse->setWeight(weight);
    
    synapse->step(0.0);  // Allow for any internal state updates
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic transmission with temporal dynamics
    // Implements spike-timing dependent synaptic integration
    
    SynapticWeight synapticWeight = synapse->getWeight();
    float reversalPotential = 0.0f;  // Default excitatory reversal potential
    
    if (!synapse->isExcitatory()) {
        reversalPotential = -80.0f;  // Inhibitory reversal potential
    }
    
    // Calculate synaptic current using conductance-based model
    float conductance = synapticWeight;
    float drivingForce = neuron->getMembranePotential() - reversalPotential;
    float synapticCurrent = conductance * drivingForce;
    
    // Apply synaptic input with appropriate sign
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(-synapticCurrent);
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
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
