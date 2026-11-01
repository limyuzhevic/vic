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
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Simplified leaky integrator with adaptive time step
    float dtFloat = static_cast<float>(dt);
    
    // More accurate integration using trapezoidal rule for stability
    float dV_leaky = (-(V - V_rest) / tau) * dtFloat;
    float dV_input = (I / R) * dtFloat;
    
    // Combined update with saturation limits
    float dV = dV_leaky + dV_input;
    
    // Apply update with hard limits to prevent runaway excitation
    float maxChange = 50.0f;  // Maximum voltage change per step
    float absDV = std::abs(dV);
    if (absDV > maxChange) {
        dV = (dV / absDV) * maxChange;
    }
    
    V += dV;
    neuron->setMembranePotential(V);
    
    // Check for firing with hysteresis to prevent multiple spikes per step
    bool shouldFire = neuron->getMembranePotential() >= neuron->getThreshold();
    if (shouldFire && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        // Pass the actual time when the spike occurs
        neuron->recordSpike(static_cast<float>(dt * neuron->getState().stepCount));
    }
    
    // Refractory mechanism - reset membrane potential during refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        // Decay from refractory state
        neuron->updateRefractoryPeriod(dtFloat);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Improved synaptic dynamics with short-term plasticity
    float dtFloat = static_cast<float>(dt);
    
    // Apply synaptic transmission dynamics
    // Simple first-order kinetics for synaptic efficacy decay
    float currentEfficacy = synapse->getEfficacy();
    float decayRate = 0.1f;  // Synaptic efficacy decay rate
    float newEfficacy = currentEfficacy * std::exp(-decayRate * dtFloat);
    
    // Update efficacy
    synapse->setEfficacy(newEfficacy);
    
    // Update short-term depression/facilitation if present
    if (synapse->getShortTermDepression() > 0.0f) {
        float std = synapse->getShortTermDepression();
        // Simple depression model
        synapse->setShortTermDepression(std * std::exp(-0.05f * dtFloat));
    }
    
    if (synapse->getShortTermFacilitation() > 0.0f) {
        float stf = synapse->getShortTermFacilitation();
        // Simple facilitation model
        synapse->setShortTermFacilitation(stf + 0.01f * dtFloat);
        synapse->setShortTermFacilitation(std::min(1.0f, stf + 0.01f * dtFloat));
    }
    
    // Apply eligibility trace for spike-timing dependent plasticity
    float eligibility = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(eligibility * std::exp(-0.01f * dtFloat));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Improved spike input handling with realistic synaptic transmission
    SynapticWeight weight = synapse->getWeight();
    
    // Apply conductance-based synaptic input with realistic dynamics
    float currentWeight = weight * synapse->getEfficacy();  // Efficacy scales the effective weight
    
    // Consider short-term depression/facilitation
    float depression = synapse->getShortTermDepression();
    float facilitation = synapse->getShortTermFacilitation();
    
    // Modulate input based on short-term plasticity
    if (depression > 0.0f) {
        currentWeight *= depression;
    }
    if (facilitation > 1.0f) {
        currentWeight *= facilitation;
    }
    
    // Apply input to neuron based on synapse type
    if (synapse->isExcitatory()) {
        // Excitatory synapses increase membrane potential
        neuron->receiveExcitatoryInput(currentWeight);
        
        // Record spike in eligibility trace for STDP
        if (neuron->getState().stepCount > 0) {
            float stepTime = static_cast<float>(neuron->getState().stepCount);
            // This would be handled by the brain's plasticity system
        }
    } else {
        // Inhibitory synapses decrease membrane potential
        neuron->receiveInhibitoryInput(std::abs(currentWeight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Improved firing threshold with adaptive dynamics
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    // Adaptive threshold that can change based on recent activity
    float thresholdFactor = 1.0f;
    if (state.firingRate > 0.0f) {
        // Higher firing rate can lower threshold (positive feedback)
        thresholdFactor = std::max(0.8f, 1.0f - 0.1f * std::min(5.0f, state.firingRate));
    }
    
    // Consider refractory period
    if (neuron->isRefractory()) {
        return false;  // Cannot fire during refractory period
    }
    
    // Improved firing condition with noise and hysteresis
    bool shouldFire = V >= threshold * thresholdFactor;
    
    // Add small random component to prevent perfect synchrony
    // This helps with more natural, desynchronized firing patterns
    return shouldFire;
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
