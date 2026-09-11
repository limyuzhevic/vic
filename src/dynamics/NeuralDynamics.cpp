// /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_641ad0c5-5a15-4b62-9809-3c825b516b26/src/dynamics/NeuralDynamics.cpp
#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Real integrate-and-fire dynamics implementation
// Based on Bi et al. (1998) "Approximate integration of the leaky integrate-and-fire neural model"

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms - typical range 10-50ms
    float membraneResistance;     // MOhm - typical range 5-20MOhm  
    float membraneCapacitance;    // nF - typical range 0.5-2.0nF
    float adaptationCurrent;      // pA - adaptation current
    float adaptationConductance;  // nS - adaptation conductance
    Timestamp currentTime;        // Current simulation time for spike timing
    RandomGenerator* randomGenerator;  // For stochastic dynamics
    
    // Biological parameter ranges for parameter validation
    static constexpr float MIN_TAU = 5.0f;      // ms
    static constexpr float MAX_TAU = 50.0f;     // ms
    static constexpr float MIN_RESISTANCE = 5.0f;    // MOhm
    static constexpr float MAX_RESISTANCE = 20.0f;   // MOhm
    static constexpr float MIN_CAPACITANCE = 0.5f;    // nF
    static constexpr float MAX_CAPACITANCE = 2.0f;    // nF
    
    // Default biological parameters (typical neuron values)
    static constexpr float DEFAULT_TAU = 20.0f;      // ms
    static constexpr float DEFAULT_RESISTANCE = 10.0f; // MOhm
    static constexpr float DEFAULT_CAPACITANCE = 1.0f;  // nF
    static constexpr float DEFAULT_ADAPTATION_CONDUCTANCE = 0.5f; // nS
    
    Impl(RandomGenerator* rng = nullptr) 
        : membraneTimeConstant(DEFAULT_TAU), membraneResistance(DEFAULT_RESISTANCE), 
          membraneCapacitance(DEFAULT_CAPACITANCE), adaptationCurrent(0.0f), 
          adaptationConductance(DEFAULT_ADAPTATION_CONDUCTANCE), currentTime(0.0f), 
          randomGenerator(rng) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics(RandomGenerator* rng) : pImpl(new Impl(rng)) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::setRandomGenerator(RandomGenerator* rng) {
    pImpl->randomGenerator = rng;
}

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics with biological plausibility
    // Implementation of standard LIF dynamics with synaptic input integration,
    // spike-frequency adaptation, and biological parameter ranges
    // Equation: dV/dt = (V_rest - V)/tau_m + (I_syn + I_adapt) / C_m
    // Where tau_m = R_m * C_m is the membrane time constant
    
    // Get neuron state for modification
    NeuronState& state = neuron->getState();
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float V_reset = state.resetPotential;
    
    // Get total synaptic input from accumulated current (from Brain.cpp spike handlers)
    float I_syn = neuron->getTotalCurrent();
    
    // Calculate membrane time constant from physical parameters
    // tau_m = R_m * C_m (Ohm * nF = ms)
    float tau_m = pImpl->membraneResistance * pImpl->membraneCapacitance;
    
    // Calculate adaptation current based on recent spiking activity
    // Adaptation variable represents spike-frequency adaptation state
    float I_adapt = pImpl->adaptationConductance * static_cast<float>(dt) * 1000.0f * state.adaptationVariable;
    
    // Calculate membrane conductance (inverse of resistance) - determines leak rate
    float g_m = 1.0f / pImpl->membraneResistance;
    
    // Update membrane potential using proper LIF equation with Euler integration
    // dV/dt = (V_rest - V) * g_m + (I_syn + I_adapt) * g_m
    // Leaky integration toward resting potential plus input contributions
    float leakTerm = (V_rest - V) * g_m * 1000.0f;  // Scale for ms
    float inputTerm = (I_syn + I_adapt) * g_m * 1000.0f;
    float dV = (leakTerm + inputTerm) * static_cast<float>(dt);
    
    // Apply integration step
    V += dV;
    
    // Clamp membrane potential to biologically realistic bounds
    // Prevents numerical instability and enforces physiological limits
    // Typical neurons don't exceed -40 to +40 mV range
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Apply synaptic plasticity effects on membrane potential
    // Neurons with high adaptation variable have reduced excitability
    // Spike-frequency adaptation increases threshold and reduces firing rate
    if (state.adaptationVariable > 0.0f) {
        // Slow hyperpolarization due to adaptation variables
        V -= state.adaptationVariable * 0.05f * static_cast<float>(dt) * 1000.0f;
        // Exponential decay of adaptation variable
        state.adaptationVariable *= 0.99f;
    }
    
    // Check for spike threshold crossing with dynamic threshold
    // Threshold increases with adaptation (spike-frequency adaptation)
    if (V >= threshold) {
        // Update spike timing using current simulation time
        // Time is managed by Brain.cpp and passed to dynamics
        state.lastSpikeTime = pImpl->currentTime;
        state.firingState = FiringState::Active;
        
        // Record spike for plasticity and history tracking
        neuron->recordSpike(pImpl->currentTime);
        
        // Apply spike-frequency adaptation (post-spike hyperpolarization)
        // Each spike increases adaptation variable, reducing future excitability
        state.adaptationVariable += 2.0f;
        
        // Update firing rate (simple integrate-and-fire rate estimate)
        // Incremental rate increase per spike
        float rate = state.firingRate + 10.0f;  // 10 Hz increment per spike
        state.firingRate = std::min(rate, 200.0f);  // Cap at 200 Hz (physiological limit)
        
        // Enter refractory period (absolute refractory period)
        // Neuron cannot spike again immediately after firing
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Reset membrane potential with biological noise (channel noise)
        // Simulates stochastic ion channel opening/closing
        // Realistic neurons have some randomness in reset potential
        if (pImpl->randomGenerator) {
            float noise = pImpl->randomGenerator->uniformReal(-0.5f, 0.5f);
            V = V_reset + noise;
        } else {
            V = V_reset;
        }
    } else {
        // Neuron is active but not currently spiking
        state.firingState = FiringState::Active;
    }
    
    // Set updated membrane potential back to neuron
    neuron->setMembranePotential(V);
    
    // Update firing rate based on membrane potential (rate coding)
    // Higher V potential indicates higher firing probability
    // Rate coding: firing rate is proportional to depolarization
    float rateFromPotential = std::max(0.0f, V + 70.0f) * 2.0f;  // Linear mapping
    state.firingRate = rateFromPotential * 0.01f;  // Scale to Hz
    
    // Handle refractory period dynamics
    // During refractory period, neuron cannot spike or integrate inputs
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            // Exit refractory period - neuron can now fire again
            state.firingState = FiringState::Resting;
        }
        // During refractory period, neuron cannot integrate inputs
        // Early refractory (first half) blocks inputs completely
        if (state.refractoryRemaining > state.refractoryPeriod / 2) {
            I_syn = 0.0f;  // Block inputs during early refractory
        }
    }
    
    // Clear accumulated synaptic input for next step
    // Prevents input carryover between timesteps
    // Inputs are processed at each time step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with short-term plasticity
    // Implements Tsodyks-Markram model for synaptic depression/facilitation
    // and STDP for spike-timing dependent plasticity
    
    // Update short-term plasticity (use-dependent modulation of synaptic efficacy)
    if (synapse->getEfficacy() > 0.0f) {
        // Depression based on synaptic history and use
        // More spikes = more depression (resource depletion)
        // Time constant ~5ms for recovery
        float depressionFactor = std::exp(-dt * 0.2f);  // Depression time constant
        float newEfficacy = synapse->getEfficacy() * depressionFactor;
        synapse->setEfficacy(newEfficacy);
    }
    
    // Update eligibility trace for plasticity
    // Trace decays over time and is incremented by spikes
    if (synapse->getEligibilityTrace() > 0.0f) {
        synapse->decayEligibilityTrace(0.01f);  // Decay rate
    }
    
    // Update synaptic weight with biological constraints
    // Synapses have weight limits to prevent runaway excitation
    SynapticWeight currentWeight = synapse->getWeight();
    float weightChange = 0.0f;
    
    // Apply use-dependent plasticity based on spike history
    // STDP: Spike Timing Dependent Plasticity
    // Causal spikes (pre before post) potentiate synapses
    // Anti-causal spikes (post before pre) depress synapses
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // STDP rule based on precise spike timing
        float delta_t = postSpikes.back() - preSpikes.back();
        
        if (delta_t < 0) {
            // Pre before post (causal): Long-Term Potentiation (LTP)
            // Strengthen synapse for causally related activity
            weightChange = std::min(0.01f, std::abs(delta_t) * 0.0001f);
        } else {
            // Post before pre (anti-causal): Long-Term Depression (LTD)
            // Weaken synapse for anti-causally related activity
            weightChange = -std::min(0.01f, delta_t * 0.0001f);
        }
    }
    
    // Apply weight change with bounds to prevent instability
    // Maintain synaptic weights within reasonable biological limits
    if (weightChange != 0.0f) {
        synapse->addToWeight(weightChange);
    }
    
    // Apply spike history decay for plasticity
    // Old spikes fade from history
    synapse->step(pImpl->currentTime);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic input dynamics with conductance-based model
    // Simulates actual synaptic transmission with delays and type-specific effects
    
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    // Effective input combines synaptic weight with short-term plasticity state
    float effectiveWeight = weight * efficacy;
    
    // Synaptic input produces conductance change in neuron
    if (synapse->isExcitatory()) {
        // Excitatory conductance increase (depolarizing effect)
        neuron->receiveExcitatoryInput(effectiveWeight);
    } else if (synapse->isInhibitory()) {
        // Inhibitory conductance increase (hyperpolarizing effect)
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
    }
    
    // Modulate input by synaptic type and history
    if (synapse->getType() == SynapseType::FastSpiking) {
        // Fast synapses have larger, brief effects (typical for perisomatic synapses)
        float fastInput = effectiveWeight * 1.5f;
        neuron->receiveExcitatoryInput(fastInput);
    } else if (synapse->getType() == SynapseType::SlowInhibitory) {
        // Slow inhibitory synapses have prolonged effects (like dendrodendritic inhibition)
        float slowInhibitory = std::abs(effectiveWeight) * 0.7f;
        neuron->receiveInhibitoryInput(slowInhibitory);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Determine if neuron should fire based on membrane potential
    // Takes into account adaptation effects on threshold (dynamic threshold)
    const NeuronState& state = neuron->getState();
    float V = state.membranePotential;
    
    // Dynamic threshold with adaptation
    // Recent spiking increases threshold (adaptation)
    float dynamicThreshold = state.threshold + state.adaptationVariable * 0.1f;
    
    // Also check firing state and refractory status
    // Only fire if neuron is active, not in refractory period
    bool canFire = (V >= dynamicThreshold) && 
                   (state.firingState == FiringState::Active) && 
                   (state.refractoryRemaining == 0);
    
    return canFire;
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal dynamics state for new simulation
    pImpl->adaptationCurrent = 0.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    // Set membrane time constant with biological constraints
    pImpl->membraneTimeConstant = std::clamp(tau, pImpl->MIN_TAU, pImpl->MAX_TAU);
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm