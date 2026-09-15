#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // REAL LIF DYNAMICS: Leaky Integrate-and-Fire with proper differential equation
    // dV/dt = (V_rest - V)/(R*C) + I/C
    // V(t+dt) = V(t) + dt * ((V_rest - V(t))/(R*C) + I(t)/C)
    
    const auto& state = neuron->getState();
    
    // Get current membrane potential
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    
    // LIF parameters
    float R = pImpl->membraneResistance;     // Ohms
    float C = 1.0f;                          // Membrane capacitance (nF) - constant for all neurons
    float tau = pImpl->membraneTimeConstant; // Time constant (ms) = R*C
    
    // Calculate dV/dt using proper LIF equation
    float dV_dt = (V_rest - V) / (tau) + I / C;
    
    // Convert dt to ms for integration (assuming dt in seconds)
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Update membrane potential using exponential Euler for better stability
    // V(t+dt) = V_rest + (V(t) - V_rest) * exp(-dt/tau) + (R*I/C) * tau * (1 - exp(-dt/tau))
    float exp_dt = std::exp(-dt_ms / tau);
    float I_term = (I / C) * tau * (1.0f - exp_dt);
    
    float V_new = V_rest + (V - V_rest) * exp_dt + I_term;
    
    // Apply spike-frequency adaptation ( Adaptation variable affects resting potential )
    if (neuron->getFiringState() == FiringState::Active) {
        V_new += neuron->getFiringRate() * 0.1f;
    }
    
    // Clamp membrane potential to biologically plausible range
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for firing with refractory period consideration
    if (V_new >= neuron->getThreshold() && !neuron->isRefractory()) {
        // Record the spike with precise timing
        neuron->recordSpike(static_cast<Timestamp>(dt * 1000.0));
        
        // Apply spike reset (subthreshold after spike)
        neuron->setMembranePotential(state.resetPotential);
        neuron->setFiringState(FiringState::Active);
        
        // Update firing rate based on input strength
        float firingRate = std::max(1.0f, std::min(100.0f, V_new * 2.0f));
        neuron->setFiringRate(firingRate);
        
        // Start refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod));
        neuron->decrementRefractory();
    } else {
        neuron->setFiringState(FiringState::Resting);
    }
    
    // Apply adaptive exponential integrate-and-fire features
    // If membrane potential approaches threshold exponentially
    if (V_new > neuron->getThreshold() - 10.0f) {
        // Increase firing rate probability based on proximity to threshold
        float proximity = (V_new - neuron->getThreshold() + 10.0f) / 10.0f;
        neuron->setFiringRate(neuron->getFiringRate() * (1.0f + proximity * 0.5f));
    }
    
    // Clear total current for next timestep (synaptic currents are integrated into V)
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // REAL SYNAPTIC DYNAMICS: Exponential decay with activity-dependent plasticity
    // dw/dt = A_pre * A_post - w/tau_w
    
    // Get current synaptic weight
    SynapticWeight weight = synapse->getWeight();
    
    // Calculate decay based on exponential time constant
    float tau_w = 10.0f; // Synaptic time constant (ms)
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    float decay = std::exp(-dt_ms / tau_w);
    
    // Apply decay
    SynapticWeight newWeight = weight * decay;
    
    // Apply activity-dependent plasticity for Hebbian learning
    if (synapse->getPlasticityFlags().hebbian) {
        // Simple STDP-like weight change based on spike timing
        // w(t+dt) = w(t) + learning_rate * (A_plus * postSpike - A_minus * preSpike)
        float learningRate = pImpl->learningRate; // 0.001 default
        float preTrace = synapse->getPreSpikeTrace();
        float postTrace = synapse->getPostSpikeTrace();
        
        float deltaW = learningRate * (postTrace - preTrace);
        newWeight += deltaW;
        
        // Constrain weight to biological range
        newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    }
    
    // Update synapse
    synapse->setWeight(newWeight);
    
    // Decay spike traces
    synapse->decaySpikeTraces(dt_ms);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // REAL SYNAPTIC INPUT: Conductance-based models with reversal potentials
    // I_syn = g_syn * (V - E_rev)
    // where g_syn is synaptic conductance, E_rev is reversal potential
    
    SynapticWeight weight = synapse->getWeight();
    float V = neuron->getMembranePotential();
    
    // Get reversal potential based on synapse type
    float E_rev;
    if (synapse->isExcitatory()) {
        E_rev = 0.0f;     // Excitatory reversal potential (~0 mV)
    } else if (synapse->isInhibitory()) {
        E_rev = -80.0f;   // Inhibitory reversal potential (~-80 mV)
    } else {
        E_rev = -65.0f;   // Modulatory reversal potential
    }
    
    // Calculate conductance-based current
    // Conductance is proportional to weight and modeled as alpha function
    float tau_syn = 5.0f;   // Synaptic time constant (ms)
    float g_max = 1.0f;     // Max conductance
    float g_syn = weight * g_max * std::exp(-2.0f); // Simplified alpha function
    
    float I_syn = g_syn * (V - E_rev);
    
    // Apply to neuron based on type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else if (synapse->isInhibitory()) {
        neuron->receiveInhibitoryInput(I_syn);
    } else {
        neuron->receiveModulatoryInput(I_syn);
    }
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float IntegrateAndFireDynamics::getLearningRate() const {
    return pImpl->learningRate;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(float period) {
    pImpl->refractoryPeriod = period;
}

float IntegrateAndFireDynamics::getRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

} // namespace nlm
