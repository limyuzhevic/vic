#pragma once

#include <string>

namespace nlm {

// ============================================================================
// APPLICATION-LEVEL CONSTANTS (Config.hpp)
// ============================================================================
// These constants define the default values for configuration parameters
// used across the NLM system. They can be overridden via configuration files
// or command line arguments.

namespace ConfigConstants {

// ============================================================================
// BRAIN STRUCTURE PARAMETERS
// ============================================================================

// Default number of neurons for testing and basic operation
static constexpr size_t DEFAULT_NEURON_COUNT = 1000;

// Default number of brain regions
static constexpr size_t DEFAULT_REGION_COUNT = 1;

// Default connection probability between neurons (10%)
static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;

// ============================================================================
// SIMULATION PARAMETERS
// ============================================================================

// Default simulation timestep (1 millisecond)
static constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001; // seconds

// ============================================================================
// SPATIAL TEMPORAL DYNAMICS (STDP) PARAMETERS
// ============================================================================

// Standard STDP LTP (Long-Term Potentiation) weight change
static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;

// Standard STDP LTD (Long-Term Depression) weight change  
static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;

// STDP synaptic time constant (20 milliseconds)
static constexpr float DEFAULT_STDP_TAU = 20.0f; // ms

// ============================================================================
// STRUCTURAL PLASTICITY PARAMETERS
// ============================================================================

// Synaptogenesis rate (creation of new synapses)
static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;

// Pruning rate (elimination of weak synapses)
static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// ============================================================================
// MEMORY SYSTEM PARAMETERS
// ============================================================================

// Working memory capacity (number of neuron traces to maintain)
static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;

// Maximum number of episodic memory episodes
static constexpr size_t DEFAULT_MAX_EPISODES = 1000;

// Replay interval (steps between memory replay)
static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;

// Consolidation interval (steps between memory consolidation)
static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;

// ============================================================================
// DEVELOPMENT PARAMETERS
// ============================================================================

// Random seed for reproducible simulations
static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;

// ============================================================================
// NEUROMODULATION PARAMETERS
// ============================================================================

// Default neuromodulation scaling factor
static constexpr float DEFAULT_NEUROMODULATION_FACTOR = 1.0f;

// ============================================================================
// SCALING AND NORMALIZATION FACTORS
// ============================================================================

// Synaptic current scaling factor for spike-induced current
static constexpr float DEFAULT_SYNAPTIC_CURRENT_SCALE = 10.0f;

// Input normalization factor (convert to [-10, 10] mV range)
static constexpr float DEFAULT_INPUT_NORMALIZATION_FACTOR = 10.0f;

// Working memory storage normalization factor
static constexpr float DEFAULT_WORKING_MEMORY_NORMALIZATION = 10.0f;

// Episodic memory activation normalization factor
static constexpr float DEFAULT_EPISODIC_MEMORY_NORMALIZATION = 20.0f;

// Spike-frequency adaptation decay rate
static constexpr float DEFAULT_ADAPTATION_DECAY_RATE = 0.01f;

// Spike-frequency adaptation decay factor
static constexpr float DEFAULT_ADAPTATION_DECAY_FACTOR = 0.95f;

// ============================================================================
// RANGE AND BOUND PARAMETERS
// ============================================================================

// Minimum synaptic weight to prevent zero values
static constexpr float MIN_SYNAPTIC_WEIGHT = 0.01f;

// Maximum synaptic weight to prevent instability
static constexpr float MIN_INHIBitory_WEIGHT = -0.01f;

// Maximum excitatory synaptic weight
static constexpr float MAX_EXCITATORY_WEIGHT = 1.0f;

// Maximum inhibitory synaptic weight
static constexpr float MAX_INHIBITORY_WEIGHT = -1.0f;

// ============================================================================
// CELL-TYPE PARAMETERS
// ============================================================================

// Membrane capacitance (nF)
static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;

// Membrane time constant (ms)
static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;

// ============================================================================
// TARGET LEVELS FOR ACTION SELECTION
// ============================================================================

// Minimum firing count to trigger forward movement action
static constexpr size_t MIN_FORWARD_FIRING_COUNT = 0;

} // namespace ConfigConstants

} // namespace nlm
