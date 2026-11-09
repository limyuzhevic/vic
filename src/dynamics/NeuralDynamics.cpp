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
    // Implement real integrate-and-fire dynamics with biological plausibility
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // More biologically accurate dynamics: exponential integration
    // dV/dt = (-(V - V_rest) + I*R) / tau
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + I*R * (1 - exp(-dt/tau))
    
    float exp_dt_tau = std::exp(-static_cast<float>(dt) / tau);
    float I_term = I * R * (1.0f - exp_dt_tau);
    
    neuron->setMembranePotential(V_rest + (V - V_rest) * exp_dt_tau + I_term);
    
    // Check for firing with refractory period consideration
    if (shouldFire(neuron) && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(dt));  // Pass actual time
    }
    
    // Refractory mechanism with adaptive reset potential
    if (neuron->isRefractory()) {
        // During refractory period, membrane potential clamped to reset value
        neuron->setMembranePotential(state.resetPotential);
        
        // Track refractory time
        float refractoryProgress = state.refractoryRemaining / state.refractoryPeriod;
        // Adaptive reset potential that recovers during refractory period
        float adaptiveReset = state.resetPotential + (V_rest - state.resetPotential) * refractoryProgress;
        neuron->setMembranePotential(adaptiveReset);
    }
    
    // Leak current during refractory period (slow recovery)
    if (neuron->isRefractory()) {
        float leakCurrent = -0.1f;  // Weak leak during refractory
        neuron->injectCurrent(leakCurrent);
    }
    
    // Clear current for next step (currents are transient inputs)
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement real synaptic dynamics with short-term plasticity
    // Modified by presynaptic spikes, with depression and facilitation
    
    // Get current synapse state
    float weight = synapse->getWeight();
    float eligibility = synapse->getEligibilityTrace();
    
    // Short-term synaptic depression: repeated use reduces efficacy
    float depressionFactor = 1.0f;
    const auto& depressionHistory = synapse->getDepressionHistory();
    if (!depressionHistory.empty()) {
        // Use oldest depression value (exponential decay of depression)
        depressionFactor = depressionHistory.front();
    }
    
    // Short-term facilitation: recent activity increases efficacy
    float facilitationFactor = 1.0f;
    const auto& facilitationHistory = synapse->getFacilitationHistory();
    if (!facilitationHistory.empty()) {
        // Use newest facilitation value
        facilitationFactor = facilitationHistory.back();
    }
    
    // Apply synaptic dynamics: weight changes based on recent activity
    float effectiveWeight = weight * depressionFactor * facilitationFactor;
    
    // Synaptic decay: weights drift toward baseline over time
    float baselineWeight = synapse->getBaselineWeight();
    float decayRate = synapse->getDecayRate();
    
    effectiveWeight += (baselineWeight - effectiveWeight) * decayRate * static_cast<float>(dt);
    
    // Update synapse with dynamic weight
    synapse->setWeight(effectiveWeight);
    
    // Update synaptic history for plasticity
    synapse->recordSynapticEvent(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Implement real spike-timing-dependent synaptic input with dynamics
    // Includes both instantaneous postsynaptic current and longer-lasting effects
    
    // Get synaptic parameters
    SynapticWeight weight = synapse->getWeight();
    Delay delay = synapse->getDelay();
    SynapseType type = synapse->getType();
    
    // Scale factor for biological realism
    float scaleFactor = 1.0f;
    if (synapse->isExcitatory()) {
        scaleFactor = 0.8f;  // Excitatory synapses have slightly reduced transmission
    } else {
        scaleFactor = 0.6f;  // Inhibitory synapses are slightly stronger
    }
    
    // Apply scaled current
    if (type == SynapseType::Excitatory) {
        neuron->receiveExcitatoryInput(weight * scaleFactor);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight) * scaleFactor);
    }
    
    // Record synaptic activation for plasticity
    neuron->recordSynapticInput(static_cast<float>(delay) * static_cast<float>(dt));
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
