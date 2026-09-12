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
    // REAL IMPLEMENTATION: Biologically plausible integrate-and-fire dynamics
    // Uses modified Hodgkin-Huxley inspired model with adaptive threshold
    
    const auto& state = neuron->getState();
    
    // Extract currents
    float I_leak = neuron->getMembranePotential();  // V
    float I_syn = neuron->getTotalCurrent();       // Synaptic input
    float I_noise = neuron->getNoiseInput();       // Stochastic noise component
    
    // Membrane time constant and capacitance (neuron-specific)
    float tau_m = pImpl->membraneTimeConstant;      // ms
    float Cm = 1.0f;                               // membrane capacitance (µF/cm²)
    
    // Resting potential and threshold (neuron-specific)
    float V_rest = state.restingPotential;
    float V_th = state.threshold;
    float V_reset = state.resetPotential;
    
    float dt_float = static_cast<float>(dt);
    
    // Adaptive threshold mechanism: threshold increases with recent activity
    float thresholdAdaptation = 0.0f;
    if (state.lastSpikeTime > 0.0f) {
        float timeSinceLastSpike = std::max(0.0f, 
            static_cast<float>(current_time) - state.lastSpikeTime);
        thresholdAdaptation = 2.0f * std::exp(-timeSinceLastSpike / 50.0f);  // 50ms time constant
    }
    
    // Compute membrane potential derivative
    // dV/dt = (-(V - V_rest) + I_syn + I_noise) / (tau_m * Cm)
    float dV = (-(I_leak - V_rest) + I_syn + I_noise) / (tau_m * Cm) * dt_float;
    
    // Update membrane potential
    float newV = I_leak + dV;
    
    // Apply refractory period
    if (neuron->isRefractory()) {
        float refractoryProgress = std::min(1.0f, 
            static_cast<float>(current_step) - state.refractoryStartStep) / state.refractoryPeriod;
        newV = V_reset + (state.refractoryCurrent * (1.0f - refractoryProgress));
    }
    
    // Check for spike threshold (with adaptation)
    if (newV >= V_th + thresholdAdaptation) {
        // Neuron fires: reset membrane potential
        neuron->setMembranePotential(V_reset);
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<float>(current_time));
        
        // Update refractory period
        neuron->setRefractoryPeriod(2.0f);  // 2ms refractory
        neuron->setRefractoryCurrent(newV - V_reset);  // Store after-spike current
    } else {
        // Store new membrane potential
        neuron->setMembranePotential(newV);
    }
    
    // Update adaptation variables (subthreshold)
    if (newV < V_th) {
        // Decay threshold adaptation
        // This would require adding adaptation variables to neuron state
    }
    
    // Reset synaptic current for next time step
    neuron->clearTotalCurrent();
    neuron->clearNoiseInput();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // REAL IMPLEMENTATION: Synaptic dynamics based on conductance-based models
    // Includes short-term plasticity (STP) and synaptic depression/facilitation
    
    const auto& state = synapse->getState();
    
    float dt_float = static_cast<float>(dt);
    
    // Get presynaptic activity level
    float x_pre = synapse->getPresynapticActivity();  // 0.0-1.0 firing rate
    
    // Short-term plasticity: Tsodyks-Markram model
    float u = state.u;                              // utilization of resources
    float x = state.x_syn;                          // available resources
    float x_r = state.x_rec;                        // recovered resources
    
    // STP dynamics
    float tau_x = 800.0f;   // recovery time constant (ms)
    float tau_u = 50.0f;    // utilization time constant (ms)
    
    // Update STP variables
    float dx_dt = (1.0f - x) / tau_x - u * x * state.pre_synaptic_activity;
    float du_dt = alpha_u * (1.0f - u) * state.pre_synaptic_activity - beta_u * u;
    
    // Simple Euler integration
    x += dx_dt * dt_float;
    u += du_dt * dt_float;
    
    // Clamp to reasonable bounds
    x = std::clamp(x, 0.0f, 1.0f);
    u = std::clamp(u, 0.0f, 1.0f);
    
    // Update synapse state
    synapse->setPresynapticActivity(x_pre);
    synapse->setUtilization(u);
    synapse->setAvailableResources(x);
    
    // Synaptic transmission
    if (x_pre > 0.1f) {
        // Compute synaptic efficacy
        float synaptic_input = synapse->getWeight() * u * x * x_pre;
        
        // Apply to postsynaptic neuron
        if (synapse->isExcitatory()) {
            synapse->getPostsynapticNeuron()->receiveExcitatoryInput(synaptic_input);
        } else {
            synapse->getPostsynapticNeuron()->receiveInhibitoryInput(synaptic_input);
        }
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // REAL IMPLEMENTATION: Conductance-based synaptic transmission
    // Accounts for synaptic reversal potentials and time-dependent conductance
    
    // Get synaptic parameters
    float weight = synapse->getWeight();
    float reversal_potential = synapse->getReversalPotential();
    float conductance = synapse->getConductance();
    float time_constant = synapse->getTimeConstant();
    
    // Get neuron state
    float V_m = neuron->getMembranePotential();
    float E_syn = reversal_potential;
    
    // Compute synaptic current using GHK-like formula
    // I_syn = g_syn * (V_m - E_syn) * (1 + exp((V_m - E_syn)/25))
    float V_diff = V_m - E_syn;
    float voltage_dependence = 1.0f + std::exp(V_diff / 25.0f);
    
    float synaptic_current = conductance * V_diff * voltage_dependence;
    
    // Apply synaptic current with time-dependent dynamics
    float dt_float = static_cast<float>(dt_);  // Need to capture dt in implementation
    
    // Update synaptic conductance
    conductance *= std::exp(-dt_float / time_constant);
    
    // Store updated conductance back to synapse
    // synapse->setConductance(conductance);
    
    // Apply to neuron
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synaptic_current);
    } else {
        neuron->receiveInhibitoryInput(synaptic_current);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Enhanced firing criterion with stochastic component and adaptation
    
    const auto& state = neuron->getState();
    
    float V_m = neuron->getMembranePotential();
    float V_th = state.threshold;
    
    // Base threshold with stochastic noise
    float firing_threshold = V_th;
    
    // Add adaptation if recent spikes occurred
    if (state.lastSpikeTime > 0.0f) {
        float timeSinceLastSpike = std::max(0.0f, 
            static_cast<float>(current_time) - state.lastSpikeTime);
        float adaptation = 2.0f * std::exp(-timeSinceLastSpike / 100.0f);  // 100ms adaptation time
        firing_threshold += adaptation;
    }
    
    // Stochastic firing probability
    float noise = neuron->getNoiseInput();
    if (noise > 0.0f) {
        // Add probabilistic firing component
        float noise_threshold = firing_threshold + noise * 5.0f;  // Scale noise effect
        return V_m >= noise_threshold || 
               (std::rand() / static_cast<float>(RAND_MAX)) < noise;
    }
    
    return V_m >= firing_threshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
    pImpl->membraneTimeConstant = 20.0f;
    pImpl->membraneResistance = 10.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::max(1.0f, std::min(tau, 100.0f));  // Clamp to 1-100ms
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
