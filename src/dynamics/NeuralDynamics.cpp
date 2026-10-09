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
    // Enhanced integrate-and-fire dynamics with biological accuracy
    if (!neuron) {
        return; // Defensive programming
    }
    
    const auto& state = neuron->getState();
    
    // Leaky integration: dV/dt = -(V - V_rest) / tau + I / R
    // Implement exponential integration for better stability:
    // V_new = V_rest + (V_old - V_rest) * exp(-dt/tau) + R * I * (1 - exp(-dt/tau))
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Exponential integration for better numerical stability
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = R * (1.0f - alpha);
    
    float V_new = V_rest + (V - V_rest) * alpha + I * beta;
    neuron->setMembranePotential(V_new);
    
    // Ensure membrane potential stays within biological bounds
    float maxV = V_rest + state.threshold;
    float minV = state.resetPotential;
    
    if (V_new > maxV) {
        V_new = maxV;
    } else if (V_new < minV) {
        V_new = minV;
    }
    neuron->setMembranePotential(V_new);
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<double>(dt));  // Use actual timestep
    }
    
    // Refractory mechanism - clamp to reset potential
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Decay refractoriness over time
    neuron->decayRefractoriness(static_cast<float>(dt));
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Enhanced synaptic dynamics with realistic decay and temporal dynamics
    if (!synapse) {
        return; // Defensive programming
    }
    
    // Apply synaptic dynamics with proper time integration
    synapse->step(static_cast<TimestampDuration>(dt));
    
    // Update short-term plasticity variables (if enabled)
    if (synapse->getPlasticityFlags().reward_modulated) {
        // Reward-modulated plasticity dynamics
        float currentWeight = synapse->getWeight();
        float eligibilityTrace = synapse->getEligibilityTrace();
        
        // Calculate eligibility trace dynamics
        float eligibilityDecayRate = 0.01f; // per ms
        eligibilityTrace *= (1.0f - eligibilityDecayRate * static_cast<float>(dt));
        
        // Apply reward modulation if applicable
        // (This would be called by neuromodulation system)
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Enhanced synaptic input with realistic dynamics and delays
    if (!neuron || !synapse) {
        return; // Defensive programming
    }
    
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        // Excitatory postsynaptic current (EPSC)
        // Add current with synaptic conductance dynamics
        float riseTime = 1.0f;  // ms - synaptic rise time
        float decayTime = 5.0f;  // ms - synaptic decay time
        
        float g_max = weight;  // Maximum synaptic conductance
        float g_syn = g_max * (riseTime / (riseTime + decayTime));  // Steady-state conductance
        
        // Calculate synaptic current: I_syn = g_syn * (V - E_syn)
        // For excitatory synapses, E_syn ≈ 0 mV
        float synapticCurrent = g_syn * (neuron->getMembranePotential() - 0.0f);
        
        neuron->receiveExcitatoryInput(synapticCurrent);
        
    } else {
        // Inhibitory postsynaptic current (IPSC)
        // Subtract current with shunting inhibition
        float riseTime = 1.0f;
        float decayTime = 5.0f;
        
        float g_max = std::abs(weight);
        float g_syn = g_max * (riseTime / (riseTime + decayTime));
        
        // For inhibitory synapses, E_syn ≈ -70 mV
        float synapticCurrent = g_syn * (neuron->getMembranePotential() - (-70.0f));
        
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
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
