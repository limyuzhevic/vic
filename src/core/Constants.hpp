#pragma once

namespace nlm {

// === LEARNING AND PLASTICITY CONSTANTS ===

// Learning detection threshold for synaptic weight changes
constexpr float LEARNING_THRESHOLD = 0.001f;  // Minimum weight change to detect learning

// Plasticity rule parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;      // Spike-timing dependent plasticity LTP weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;     // Spike-timing dependent plasticity LTD weight
constexpr float DEFAULT_STDP_TAU = 20.0f;              // STDP time constant (ms)

// Structural plasticity parameters
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // Rate of new synapse formation
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;        // Rate of synapse elimination

// Neural dynamics constants
constexpr float LIF_MEMBRANE_CAPACITANCE = 1.0f;         // Membrane capacitance (nF)
constexpr float LIF_TIME_CONSTANT = 20.0f;              // Membrane time constant (ms)
constexpr float LIF_RESTING_POTENTIAL = -70.0f;        // Default resting potential (mV)
constexpr float LIF_THRESHOLD_POTENTIAL = -55.0f;       // Default firing threshold (mV)
constexpr float LIF_RESET_POTENTIAL = -70.0f;           // Post-spike reset potential (mV)
constexpr float LIF_LEAK_CONDUCTANCE = 10.0f;            // Leak conductance (nS)

// Synapse parameters
constexpr float SYNAPSE_MIN_WEIGHT = -1.0f;             // Minimum synaptic weight
constexpr float SYNAPSE_MAX_WEIGHT = 1.0f;              // Maximum synaptic weight
constexpr Delay DEFAULT_SYNAPSE_DELAY = 1;               // Default synaptic delay (1ms)

// Memory system constants
constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000; // Default working memory capacity
constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000; // Max episodes in episodic memory
constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;  // Memory consolidation threshold

// Development system constants
constexpr float DEVELOPMENT_INITIAL_PLASTICITY = 1.0f;   // High initial plasticity
constexpr float DEVELOPMENT_CRITICAL_PERIOD_PLASTICITY = 0.8f;
constexpr float DEVELOPMENT_MATURATION_PLASTICITY = 0.5f;
constexpr float DEVELOPMENT_ADULT_PLASTICITY = 0.2f;    // Low adult plasticity

// Neuromodulation constants
constexpr float DEFAULT_DOPAMINE_LEVELS = 0.5f;          // Baseline dopamine level
constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.3f;        // Novelty detection threshold
constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.2f;     // Curiosity threshold

// Simulation constants
constexpr SimulationStep DEFAULT_REPLAY_INTERVAL = 100;     // Memory replay interval
constexpr SimulationStep DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Memory consolidation interval
constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001;          // Default simulation timestep (1ms)

} // namespace nlm
