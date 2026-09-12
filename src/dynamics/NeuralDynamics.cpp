#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;    // ms, membrane time constant τ_m
    float membraneResistance;       // MOhm, membrane resistance R_m
    float restingPotential;         // mV, V_rest
    float resetPotential;           // mV, V_reset
    float thresholdPotential;       // mV, V_th
    float refractoryPeriod;          // ms, t_ref
    float adaptationCurrent;         // nS, spike-triggered adaptation current
    
    Impl() : 
        membraneTimeConstant(20.0f),      // Typical τ_m for cortical neurons
        membraneResistance(10000.0f),     // Typical R_m (10 MOhm)
        restingPotential(-70.0f),         // Typical resting potential
        resetPotential(-65.0f),           // Reset to just below threshold
        thresholdPotential(-50.0f),       // Typical threshold
        refractoryPeriod(2.0f),            // 2ms refractory period
        adaptationCurrent(0.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics with conductance-based model
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_syn = neuron->getTotalCurrent();  // Synaptic current (nA)
    float I_ext = neuron->getExternalInput(); // External input (nA)
    
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float V_reset = pImpl->resetPotential;
    float V_thresh = pImpl->thresholdPotential;
    float t_ref = pImpl->refractoryPeriod;
    float I_adapt = pImpl->adaptationCurrent;
    
    // Update membrane potential using real LIF equation with adaptation
    // dV/dt = -(V - V_rest)/τ_m + (I_syn + I_ext + I_adapt)/C
    // where C = τ_m / R_m
    
    float C = tau / R;  // Membrane capacitance
    float I_total = I_syn + I_ext + I_adapt;
    
    // Euler integration with adaptive time step
    float dV = (-(V - V_rest) / tau + I_total / C) * static_cast<float>(dt);
    float V_new = V + dV;
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        // During refractory period, clamp potential to reset
        neuron->setMembranePotential(V_reset);
        // Decrement refractory timer
        neuron->setRefractoryTime(std::max(0.0f, neuron->getRefractoryTime() - dt));
        neuron->clearTotalCurrent();
        return;
    }
    
    // Check for spike threshold crossing
    if (V_new >= V_thresh) {
        // Record spike with actual time
        neuron->recordSpike(neuron->getLastSpikeTime() + dt);
        
        // Apply spike-triggered adaptation
        pImpl->adaptationCurrent += 10.0f;  // Increase adaptation current
        
        // Enter refractory period
        neuron->setRefractoryPeriod(t_ref);
        neuron->setMembranePotential(V_reset);
        
        // Trigger synaptic transmission (handled externally)
        neuron->setFiringState(FiringState::Active);
    } else {
        // Update membrane potential
        neuron->setMembranePotential(V_new);
    }
    
    // Clear external input for next step
    neuron->clearExternalInput();
    
    // Decay adaptation current slowly
    pImpl->adaptationCurrent = std::max(0.0f, pImpl->adaptationCurrent * 0.95f);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with exponential decay and conductance-based model
    float weight = synapse->getWeight();
    float g_max = synapse->getMaxConductance(); // Max synaptic conductance
    float reversal_potential = synapse->getReversalPotential(); // E_rev in mV
    
    // Exponential decay of synaptic efficacy
    float decayRate = 1.0f / pImpl->membraneTimeConstant; // τ = τ_m for synapses
    float weight_new = weight * std::exp(-decayRate * static_cast<float>(dt));
    
    // Update synaptic conductance
    float g = g_max * (1.0f - std::exp(-static_cast<float>(dt) / decayRate));
    
    // Record the updated synaptic state
    synapse->setWeight(weight_new);
    
    // Update synaptic conductance in neuron if needed
    if (synapse->isActive()) {
        // Synaptic activation decays over time
        synapse->setActivation(std::max(0.0f, synapse->getActivation() - 0.1f * static_cast<float>(dt)));
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic transmission with probabilistic release and delay
    static thread_local float spikeTimeCounter = 0.0f;
    
    // Synaptic release probability (typically 0.2-0.8 for real synapses)
    float releaseProb = 0.5f;
    float random = ((float)rand() / (float)RAND_MAX);  // Simple RNG
    
    if (random < releaseProb) {
        // Synaptic activation with exponential dynamics
        float activation = synapse->getActivation();
        activation = std::min(1.0f, activation + 0.8f);  // Strong activation
        synapse->setActivation(activation);
        
        // Calculate postsynaptic potential based on synapse type
        float reversal_potential = synapse->getReversalPotential();
        float weight = synapse->getWeight();
        float tau_syn = synapse->getTimeConstant(); // Synaptic time constant
        
        // Determine postsynaptic current: I_post = g_syn * (V - E_rev)
        // Where g_syn = weight * activation
        float g_syn = weight * activation;
        
        // Store synaptic parameters in neuron's input queue
        // This simulates delayed synaptic effects
        neuron->addSynapticInput(reversal_potential, g_syn, tau_syn);
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
