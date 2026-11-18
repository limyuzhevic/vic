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
    // Phase 2: Real integrate-and-fire dynamics
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation with exponential Euler for stability:
    // V_new = V + (V_rest - V)/tau * dt + I/C * dt
    
    NeuronState& state = neuron->getState();
    
    // Current membrane potential
    float V = neuron->getMembranePotential();
    
    // Extract parameters
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float V_reset = state.resetPotential;
    float tau = pImpl->membraneTimeConstant;  // ms
    float R = pImpl->membraneResistance;     // MOhm (membrane resistance)
    
    // Get current input and convert to membrane potential contribution
    float totalCurrent = neuron->getTotalCurrent();
    
    // Membrane potential derivative: dV/dt = (V_rest - V)/tau + I/R
    float leakTerm = (V_rest - V) / tau;  // Resting contribution
    float inputTerm = totalCurrent / R;   // Synaptic current contribution
    
    // Update membrane potential with Euler integration
    float dV = (leakTerm + inputTerm) * static_cast<float>(dt);
    V = V + dV;
    
    // Apply spike-frequency adaptation if present
    if (state.adaptationVariable > 0.0f) {
        // Adaptation causes slow hyperpolarization
        V -= state.adaptationVariable * 0.01f * static_cast<float>(dt);
        // Decay adaptation variable exponentially
        state.adaptationVariable *= std::pow(0.95f, dt * 1000.0f);
    }
    
    // Clamp membrane potential to realistic bounds
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        // Record spike with current time (placeholder: 0.0)
        neuron->recordSpike(0.0);  // TODO: pass actual time from caller
        
        // Update firing rate metric
        float firingRate = neuron->getFiringRate();
        if (dt > 0.0) {
            firingRate = firingRate * 0.9f + 0.1f * static_cast<float>(1.0 / dt);
        }
        neuron->setFiringRate(firingRate);
        
        // Reset membrane potential to reset value
        V = V_reset;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Update adaptation variable for next spike
        state.adaptationVariable += 1.0f;
    }
    
    // Update membrane potential
    neuron->setMembranePotential(V);
    
    // Handle refractory period
    if (state.refractoryRemaining > 0) {
        // During refractory period, clamp to reset potential
        neuron->setMembranePotential(V_reset);
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            state.firingState = FiringState::Resting;
        }
    }
    
    // Clear total current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Phase 2: Real synaptic dynamics
    // Synaptic weight decay and potential modulation
    
    // Apply synaptic weight decay (synaptic plasticity homeostasis)
    float currentWeight = synapse->getWeight();
    if (currentWeight > 0.0f) {
        // Exponential weight decay
        float decayFactor = std::pow(0.995f, static_cast<float>(dt) * 1000.0f);  // ms
        float newWeight = currentWeight * decayFactor;
        synapse->addToWeight(newWeight - currentWeight);  // Apply decay
    }
    
    // Apply synaptic adaptation if enabled
    if (synapse->hasPlasticityEnabled()) {
        // Simple synaptic scaling based on postsynaptic activity
        auto* postNeuron = synapse->getDestinationNeuron();
        if (postNeuron && postNeuron->isFiring()) {
            // Slight strengthening for correlated activity
            float adaptation = 0.001f * static_cast<float>(dt) * 1000.0f;
            synapse->addToWeight(adaptation);
        }
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Phase 2: Real synaptic input
    // Compute synaptic current based on weight, reversal potential, and conductance
    
    float weight = synapse->getWeight();
    float reversalPotential = synapse->getReversalPotential();
    
    // Get current membrane potential of postsynaptic neuron
    float V_post = neuron->getMembranePotential();
    
    // Calculate synaptic current using conductance-based model
    // I_syn = g_syn * (V_post - E_rev) where g_syn is proportional to synaptic weight
    float conductance = std::abs(weight) * 0.1f;  // Weight scaling factor
    float synapticCurrent = conductance * (V_post - reversalPotential);
    
    // For excitatory synapses, current is positive (depolarizing)
    // For inhibitory synapses, current is negative (hyperpolarizing)
    if (synapse->isExcitatory()) {
        neuron->injectCurrent(synapticCurrent);
    } else {
        neuron->injectCurrent(-synapticCurrent);  // Negative current for inhibition
    }
    
    // Apply short-term synaptic plasticity if enabled
    if (synapse->hasPlasticityEnabled()) {
        // Facilitation and depression based on recent activity
        static thread_local float utilization = 0.0f;
        utilization = utilization * 0.95f + 1.0f;  // Facilitation
        if (utilization > 1.0f) utilization = 1.0f;
        
        float depressionFactor = 1.0f - utilization * 0.3f;
        float effectiveWeight = weight * depressionFactor;
        
        // Update synaptic efficacy with depression
        neuron->injectCurrent(-effectiveWeight * 0.5f);  // Depression feedback
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
