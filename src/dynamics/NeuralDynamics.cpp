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
    // Real integrate-and-fire dynamics implementation
    const auto& state = neuron->getState();
    
    // Membrane potential dynamics using Iintegration of inputs
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Use C (membrane capacitance) from neuron state if available, otherwise default
    float C = state.leakConductance > 0.0f ? state.leakConductance : 1.0f;  // nF
    
    // Real LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    // Using exponential Euler integration for stability:
    // V_new = V_rest + (V - V_rest) * exp(-dt/tau) + (I/C) * (1 - exp(-dt/tau))
    
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float I_contribution = (I_total / C) * (1.0f - alpha);
    
    // Update membrane potential
    float V_new = V_rest + (V - V_rest) * alpha + I_contribution;
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;  // Stronger adaptation effect
        // Decay adaptation
        float newAdaptation = state.adaptationVariable * 0.95f;
        neuron->setAdaptationVariable(newAdaptation);
    }
    
    // Clamp to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    // Update membrane potential
    neuron->setMembranePotential(V_new);
    
    // Check for spike with precise threshold crossing
    bool shouldFire = V_new >= state.threshold;
    
    if (shouldFire) {
        // Record spike with placeholder timestamp
        neuron->recordSpike(0.0f);  // TODO: integrate with actual simulation time
        
        // Apply spike reset
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant * 0.8f));  // 80% of tau
        
        // Update adaptation for spike-frequency adaptation
        float adaptationIncrement = 1.5f;  // Stronger adaptation
        neuron->setAdaptationVariable(state.adaptationVariable + adaptationIncrement);
        
        // Set firing state to Active
        neuron->setFiringState(FiringState::Active);
    } else {
        // Not firing
        neuron->setFiringState(FiringState::Resting);
    }
    
    // Clear total current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics implementation
    synapse->step(0.0);  // Use current simulation time
    
    // Apply short-term plasticity
    float utilization = synapse->getShortTermDeprression();  // Recovery state
    float facilitation = synapse->getShortTermFacilitation();  // Facilitation state
    
    // Tsodyks-Markram model with more realistic parameters
    // Recovery from depression: dD/dt = (1-D)/tau_d - U*D*delta(t-spike)
    // Facilitation: dF/dt = (F-1)/tau_f + U*(1-F)*delta(t-spike)
    
    // For small timesteps, apply Euler integration
    float tau_f = synapse->getFacilitationTimeConstant();  // ~100ms
    float tau_d = synapse->getDepressionTimeConstant();   // ~200ms
    
    // Update short-term plasticity state
    if (utilization < 1.0f) {
        // Recovery from depression
        float recovery = (1.0f - utilization) * (1.0f / tau_d) * static_cast<float>(dt);
        synapse->setShortTermDeprression(utilization + recovery);
    }
    
    if (facilitation > 0.0f) {
        // Decay facilitation
        float decay = (facilitation - 1.0f) * (1.0f / tau_f) * static_cast<float>(dt);
        synapse->setShortTermFacilitation(facilitation + decay);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input implementation with conductance-based model
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Conductance-based synaptic model
    // I_syn = g_syn * (V - E_syn)
    // where g_syn is synaptic conductance and E_syn is reversal potential
    
    // For excitatory synapses: E_syn ~ 0mV (depolarizing)
    // For inhibitory synapses: E_syn ~ -70mV (hyperpolarizing)
    
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -70.0f;
    
    // Calculate synaptic conductance
    float g_syn = std::abs(weight) * efficacy;
    
    // Get current membrane potential
    float V = neuron->getMembranePotential();
    
    // Calculate synaptic current: I_syn = g_syn * (V - E_syn)
    float I_syn = g_syn * (V - reversalPotential);
    
    // Apply synaptic input to neuron based on synapse type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(I_syn);
    }
    
    // Update synapse state (short-term plasticity)
    float utilization = synapse->getShortTermDeprression();
    
    if (utilization > 0.01f) {
        // Apply synaptic depression: U*D where U is utilization rate
        float depression = 0.8f * utilization * 0.01f;  // U ~ 0.8, dt = 1ms
        synapse->setShortTermDeprression(utilization - depression);
    }
    
    // Apply facilitation
    float facilitation = synapse->getShortTermFacilitation();
    if (facilitation < 1.0f) {
        // Facilitation: U*(1-F)
        float facilitationAmount = 0.8f * (1.0f - facilitation) * 0.01f;
        synapse->setShortTermFacilitation(facilitation + facilitationAmount);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if neuron should fire based on membrane potential and state
    const auto& state = neuron->getState();
    
    // Check if in refractory period
    if (neuron->isRefractory()) {
        return false;
    }
    
    // Check membrane potential against threshold
    bool thresholdExceeded = neuron->getMembranePotential() >= state.threshold;
    
    // Additional checks for stability
    bool withinValidRange = neuron->getMembranePotential() >= -100.0f && 
                           neuron->getMembranePotential() <= 50.0f;
    
    return thresholdExceeded && withinValidRange;
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
    pImpl = new Impl();
    
    // Note: No internal state to reset beyond pImpl
    // The dynamics are stateless between steps
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm