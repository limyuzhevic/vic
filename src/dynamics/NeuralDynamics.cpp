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
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // Real LIF dynamics with synaptic conductance integration
    // dV/dt = (V_rest - V)/tau + I_syn/C + I_inj/C
    
    const auto& state = neuron->getState();
    
    // Get neuron state
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_inj = neuron->getTotalCurrent();  // External input
    
    // Membrane time constant and capacitance
    float tau = pImpl->membraneTimeConstant;  // ms
    float C = 1.0f;  // nF, membrane capacitance
    
    // Synaptic conductance contribution (from NeuronState)
    float g_syn = state.synapseConductance;  // nS
    float E_syn = -10.0f;  // Excitatory reversal potential (mV)
    
    // Synaptic current: I_syn = g_syn * (E_syn - V)
    float I_syn = g_syn * (E_syn - V);
    
    // Total current
    float I_total = I_inj + I_syn;
    
    // Real LIF dynamics using exponential Euler integration
    // For stability, use dt < tau for explicit integration
    float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    
    if (dt_ms < tau) {
        // Exponential integration: V_new = V_rest + (V - V_rest) * exp(-dt/tau) + I_total * R
        float R = pImpl->membraneResistance;  // MOhm
        float alpha = std::exp(-dt_ms / tau);
        neuron->setMembranePotential(V_rest + (V - V_rest) * alpha + I_total * R);
    } else {
        // Simple Euler for large dt
        float dV = (-(V - V_rest) / tau + I_total / C) * dt_ms;
        neuron->setMembranePotential(V + dV);
    }
    
    // Spike detection and reset
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
        neuron->setMembranePotential(state.resetPotential);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        neuron->decrementRefractory();
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Update synaptic conductance (exponential decay) - need to access mutable state
    NeuronState& mutableState = const_cast<NeuronState&>(state);
    float g_syn_new = mutableState.synapseConductance * std::exp(-dt_ms / 20.0f);  // Time constant 20ms
    mutableState.synapseConductance = g_syn_new;
    
    // Clear synaptic current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // Real synaptic dynamics with conductance-based transmission
    // Synaptic state: g(t) = g * efficacy * STP_state
    // Decay: dg/dt = -g/tau_syn
    
    if (!synapse) return;
    
    TimestepDuration dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Get current synaptic state
    float efficacy = synapse->getEfficacy();
    float depression = 1.0f;  // Default recovery state
    float facilitation = 0.0f;  // Default baseline
    
    // Update short-term plasticity (Tsodyks-Markram model)
    // Facilitation dynamics: df/dt = (1-f)/tau_f + U*(1-f)*δ(t)
    // Depression dynamics: dg/dt = (1-g)/tau_d - x*g*δ(t)
    // Here we use the synapse's internal STP state
    Synapse::Impl* synapseImpl = reinterpret_cast<Synapse::Impl*>(synapse->pImpl);
    if (synapseImpl) {
        float lastPreTime = synapseImpl->lastPreSpikeTime;
        float lastPostTime = synapseImpl->lastPostSpikeTime;
        
        if (lastPreTime >= 0.0f) {
            float timeSincePre = static_cast<float>(dt_ms - lastPreTime);
            // Decay facilitation
            synapseImpl->shortTermFacilitation *= std::exp(-timeSincePre / 100.0f);
        }
        
        if (lastPostTime >= 0.0f || lastPreTime >= 0.0f) {
            float timeSinceActivity = std::max(
                lastPostTime >= 0.0f ? static_cast<float>(dt_ms - lastPostTime) : 0.0f,
                lastPreTime >= 0.0f ? static_cast<float>(dt_ms - lastPreTime) : 0.0f
            );
            // Recovery from depression
            synapseImpl->shortTermDepression += (1.0f - synapseImpl->shortTermDepression) * 
                (1.0f - std::exp(-timeSinceActivity / 200.0f));
        }
    }
    
    // Update synapse internal state
    synapse->step(dt_ms);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // Real synaptic transmission with conductance-based dynamics
    // I_syn = g_max * w * efficacy * STP_state * (V - E_syn)
    
    if (!neuron || !synapse) return;
    
    // Get synaptic parameters
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Check synapse type and reversal potential
    float E_syn;
    if (synapse->isExcitatory()) {
        E_syn = 0.0f;  // Excitatory reversal potential (near 0 mV)
    } else {
        E_syn = -80.0f;  // Inhibitory reversal potential
    }
    
    // Get current neuron membrane potential
    float V = neuron->getMembranePotential();
    
    // Compute synaptic conductance
    // Synaptic conductance depends on weight, efficacy, and STP state
    float g_max = std::abs(weight);  // Maximum conductance proportional to weight
    float g_syn = g_max * efficacy;  // Actual conductance
    
    // Short-term plasticity modulation (if implemented in Synapse)
    // In full implementation, this would include depression/facilitation factors
    
    // Apply synaptic input as conductance-based current
    // I_syn = g_syn * (E_syn - V)
    float I_syn = g_syn * (E_syn - V);
    
    // Add to neuron's synaptic input
    neuron->injectCurrent(I_syn);
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
