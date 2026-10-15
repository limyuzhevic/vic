#pragma once

#include <cmath>
#include <algorithm>
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Common initialization constants for neurons (removing magic numbers)
struct NeuronDefaults {
    // Membrane properties
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;        // ms
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float DEFAULT_THRESHOLD = -55.0f;   // mV
    static constexpr float DEFAULT_RESET_POTENTIAL_OFFSET = 5.0f;  // mV
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;  // nS
    static constexpr float DEFAULT_SPIKE_VARIATION = 3.0f;  // mV
    static constexpr float DEFAULT_THRESHOLD_VARIATION = 2.0f;  // mV
    static constexpr float DEFAULT_POTENTIAL_VARIATION = 2.0f;  // mV
    static constexpr float DEFAULT_ADAPTATION_INCREMENT = 1.0f;  // unit
    static constexpr float DEFAULT_ADAPTATION_DECAY_RATE = 0.95f;  // per ms
    static constexpr float DEFAULT_ADAPTATION_EFFECT = 0.01f;  // mV per adaptation unit
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // mV
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;   // mV
    static constexpr uint32_t MIN_REFECTORY_PERIOD = 2;      // ms
    static constexpr uint32_t MAX_REFECTORY_PERIOD = 10;     // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;         // count
    static constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001f;  // 1ms
};

// Common initialization constants for synapses (removing magic numbers)
struct SynapseDefaults {
    // Weight bounds
    static constexpr SynapticWeight MIN_WEIGHT = -1.0f;
    static constexpr SynapticWeight MAX_WEIGHT = 1.0f;
    static constexpr SynapticWeight EXCITATORY_WEIGHT_MIN = 0.1f;
    static constexpr SynapticWeight EXCITATORY_WEIGHT_MAX = 0.4f;
    static constexpr SynapticWeight INHIBITORY_WEIGHT_ABS_MIN = 0.1f;
    static constexpr SynapticWeight INHIBITORY_WEIGHT_ABS_MAX = 0.4f;
    static constexpr SynapticWeight OTHER_WEIGHT_MIN = -0.1f;
    static constexpr SynapticWeight OTHER_WEIGHT_MAX = 0.1f;
    
    // Delay
    static constexpr Delay MIN_DELAY = 1;      // simulation steps
    static constexpr Delay MAX_DELAY = 5;      // simulation steps
    
    // Efficacy
    static constexpr float EFFICIENCY_BASE = 1.0f;
    static constexpr float EFFICIENCY_EXCITATORY_MIN = 0.8f;
    static constexpr float EFFICIENCY_EXCITATORY_MAX = 1.0f;
    static constexpr float EFFICIENCY_INHIBITORY_MIN = 0.8f;
    static constexpr float EFFICIENCY_INHIBITORY_MAX = 1.0f;
    static constexpr float EFFICIENCY_OTHER_MIN = 0.9f;
    static constexpr float EFFICIENCY_OTHER_MAX = 1.0f;
    static constexpr float EFFICIENCY_MIN = 0.0f;
    static constexpr float EFFICIENCY_MAX = 2.0f;
    
    // Short-term plasticity
    static constexpr float SHORT_TERM_DEPRESSION_INIT = 1.0f;   // fully recovered
    static constexpr float SHORT_TERM_FACILITATION_INIT = 0.0f; // no initial facilitation
    static constexpr float STP_FACILITATION_TAU = 100.0f;       // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;         // ms
    static constexpr float STP_U_MAX = 1.0f;                     // max utilization
    
    // Learning parameters
    static constexpr float ELIGIBILITY_TRACE_INIT = 0.0f;
    static constexpr float LAST_SPIKE_TIME_INVALID = -1.0f;
    static constexpr float ELIGIBILITY_TRACE_DECAY_RATE = 0.001f; // per ms
    static constexpr float WEIGHT_PRECISION = 0.001f;            // for validation
};

// Common validation functions (improving error handling)
inline bool isValidMembranePotential(float potential) {
    return potential >= NeuronDefaults::MIN_MEMBRANE_POTENTIAL && 
           potential <= NeuronDefaults::MAX_MEMBRANE_POTENTIAL;
}

inline bool isValidSynapticWeight(float weight) {
    return weight >= SynapseDefaults::MIN_WEIGHT && 
           weight <= SynapseDefaults::MAX_WEIGHT;
}

inline bool isValidDelay(Delay delay) {
    return delay >= SynapseDefaults::MIN_DELAY && delay <= SynapseDefaults::MAX_DELAY;
}

inline bool isValidEfficiency(float efficiency) {
    return efficiency >= SynapseDefaults::EFFICIENCY_MIN && 
           efficiency <= SynapseDefaults::EFFICIENCY_MAX;
}

inline bool validateNeuronParameters(const NeuronState& state) {
    if (!isValidMembranePotential(state.membranePotential)) {
        return false;
    }
    if (!isValidMembranePotential(state.restingPotential)) {
        return false;
    }
    if (!isValidMembranePotential(state.threshold)) {
        return false;
    }
    if (!isValidMembranePotential(state.resetPotential)) {
        return false;
    }
    if (state.refractoryPeriod < NeuronDefaults::MIN_REFECTORY_PERIOD || 
        state.refractoryPeriod > NeuronDefaults::MAX_REFECTORY_PERIOD) {
        return false;
    }
    if (state.lastSpikeTime != -1.0f && state.lastSpikeTime < 0.0f) {
        return false;
    }
    return true;
}

inline bool validateSynapseParameters(const Synapse::Impl& impl) {
    if (!isValidSynapticWeight(impl.weight)) {
        return false;
    }
    if (!isValidDelay(impl.delay)) {
        return false;
    }
    if (!isValidEfficiency(impl.efficacy)) {
        return false;
    }
    return true;
}

// Common initialization helper functions (extracting duplicated logic)
inline void initializeNeuronState(NeuronState& state) {
    state = NeuronState();  // This sets defaults from NeuronState constructor
}

inline void initializeSynapseState(Synapse::Impl& impl) {
    // Reset to synaptic defaults
    impl.weight = 0.0f;
    impl.delay = SynapseDefaults::MIN_DELAY;
    impl.type = SynapseType::Excitatory;
    impl.efficacy = SynapseDefaults::EFFICIENCY_BASE;
    impl.shortTermDepression = SynapseDefaults::SHORT_TERM_DEPRESSION_INIT;
    impl.shortTermFacilitation = SynapseDefaults::SHORT_TERM_FACILITATION_INIT;
    impl.lastPreSpikeTime = SynapseDefaults::LAST_SPIKE_TIME_INVALID;
    impl.lastPostSpikeTime = SynapseDefaults::LAST_SPIKE_TIME_INVALID;
    impl.eligibilityTrace = SynapseDefaults::ELIGIBILITY_TRACE_INIT;
}

// Common random initialization helper functions
inline void initializeNeuronRandomParameters(NeuronState& state, RandomGenerator& rng) {
    state.membranePotential = state.restingPotential + rng.uniformReal(
        -NeuronDefaults::DEFAULT_SPIKE_VARIATION, 
        NeuronDefaults::DEFAULT_SPIKE_VARIATION
    );
    state.threshold = NeuronDefaults::DEFAULT_THRESHOLD + rng.uniformReal(-2.0f, 2.0f);
    state.restingPotential = NeuronDefaults::DEFAULT_RESTING_POTENTIAL + rng.uniformReal(-2.0f, 2.0f);
    state.resetPotential = state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(
        NeuronDefaults::MIN_REFECTORY_PERIOD,
        NeuronDefaults::MAX_REFECTORY_PERIOD
    ));
    state.firingState = FiringState::Resting;
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    state.lastSpikeTime = -1.0f;
}

inline void initializeSynapseRandomParameters(Synapse::Impl& impl, RandomGenerator& rng) {
    if (impl.type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        impl.weight = rng.uniformReal(
            SynapseDefaults::EXCITATORY_WEIGHT_MIN,
            SynapseDefaults::EXCITATORY_WEIGHT_MAX
        );
        // Excitatory synapses have moderate initial efficacy
        impl.efficacy = rng.uniformReal(
            SynapseDefaults::EFFICIENCY_EXCITATORY_MIN,
            SynapseDefaults::EFFICIENCY_EXCITATORY_MAX
        );
    } else if (impl.type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        impl.weight = -rng.uniformReal(
            SynapseDefaults::INHIBITORY_WEIGHT_ABS_MIN,
            SynapseDefaults::INHIBITORY_WEIGHT_ABS_MAX
        );
        impl.efficacy = rng.uniformReal(
            SynapseDefaults::EFFICIENCY_INHIBITORY_MIN,
            SynapseDefaults::EFFICIENCY_INHIBITORY_MAX
        );
    } else {
        // Other types: small random weights
        impl.weight = rng.uniformReal(
            SynapseDefaults::OTHER_WEIGHT_MIN,
            SynapseDefaults::OTHER_WEIGHT_MAX
        );
        impl.efficacy = rng.uniformReal(
            SynapseDefaults::EFFICIENCY_OTHER_MIN,
            SynapseDefaults::EFFICIENCY_OTHER_MAX
        );
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    impl.delay = static_cast<Delay>(rng.uniformInt(
        SynapseDefaults::MIN_DELAY,
        SynapseDefaults::MAX_DELAY
    ));
    
    // Initialize short-term plasticity state
    impl.shortTermDepression = SynapseDefaults::SHORT_TERM_DEPRESSION_INIT;
    impl.shortTermFacilitation = SynapseDefaults::SHORT_TERM_FACILITATION_INIT;
    
    // Initialize eligibility trace to 0
    impl.eligibilityTrace = SynapseDefaults::ELIGIBILITY_TRACE_INIT;
    
    // Initialize last spike times
    impl.lastPreSpikeTime = SynapseDefaults::LAST_SPIKE_TIME_INVALID;
    impl.lastPostSpikeTime = SynapseDefaults::LAST_SPIKE_TIME_INVALID;
}

// Common reset helper functions (ensuring consistent reset patterns)
inline void resetNeuronState(NeuronState& state) {
    // Reset to defaults using the existing NeuronState constructor
    state = NeuronState();
}

inline void resetSynapseState(Synapse::Impl& impl) {
    // Reset synaptic state to defaults using CommonInit constants
    impl.weight = 0.0f;
    impl.eligibilityTrace = SynapseDefaults::ELIGIBILITY_TRACE_INIT;
    impl.efficacy = SynapseDefaults::EFFICIENCY_BASE;
}

// Common step helpers for LIF dynamics
inline void applyLIFDynamics(NeuronState& state, float dt) {
    MembranePotential& V = state.membranePotential;
    MembranePotential V_rest = state.restingPotential;
    float tau = NeuronDefaults::TIME_CONSTANT;
    float C = NeuronDefaults::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = state.synapseConductance / C;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + dt * 1000.0f * (leakContribution + synapticContribution);
    
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * SynapseDefaults::ADAPTATION_EFFECT;
        state.adaptationVariable *= SynapseDefaults::ADAPTATION_DECAY_RATE;
    }
    
    V = std::clamp(V, NeuronDefaults::MIN_MEMBRANE_POTENTIAL, 
                  NeuronDefaults::MAX_MEMBRANE_POTENTIAL);
}

// Common plasticity application helper
inline void applyPlasticityToWeight(float& weight, float eligibilityTrace, float learningRate = 0.01f) {
    weight += eligibilityTrace * learningRate;
    weight = std::clamp(weight, SynapseDefaults::MIN_WEIGHT, SynapseDefaults::MAX_WEIGHT);
}

} // namespace nlm
