// Constants.h
// Centralized constants for NLM (Phase 2)
// Extracted from Brain.cpp and main.cpp to eliminate magic numbers

#pragma once

#include <cstdint>

namespace nlm {

// Forward declarations for SimulationStep type
class SimulationStep;

// ========== SIMULATION TIMESTEP CONSTANTS ==========

// Default simulation timestep in seconds (1 millisecond)
constexpr double SIMULATION_TIMESTEP_SECONDS = 0.001;

// ========== STEP INTERVALS FOR PERIODIC OPERATIONS ==========

// Replay interval (steps between memory replay)
constexpr size_t REPLAY_INTERVAL_STEPS = 100;

// Consolidation interval (steps between memory consolidation)
constexpr size_t CONSOLIDATION_INTERVAL_STEPS = 1000;

// Structural plasticity update interval
constexpr size_t STRUCTURAL_PLASTICITY_INTERVAL_STEPS = 100;

// Development system update interval
constexpr size_t DEVELOPMENT_UPDATE_INTERVAL_STEPS = 1000;

// Episode storage interval
constexpr size_t EPISODE_STORAGE_INTERVAL_STEPS = 10;

// Test simulation duration
extern constexpr SimulationStep BASIC_CONNECTIVITY_TEST_STEPS;
extern constexpr SimulationStep PLASTICITY_EXPERIMENT_STEPS;

// ========== PLASTICITY PARAMETERS ==========

// STDP parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_STDP_TAU = 20.0f;

// Structural plasticity rates
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// Plasticity modulation factors
constexpr float DEFAULT_WEIGHT_ADJUSTMENT_FACTOR = 0.001f;
constexpr float NEUROMODULATION_EXCITABILITY_FACTOR = 0.5f;
constexpr float NEUROMODULATION_EXCITATION_FACTOR = NEUROMODULATION_EXCITABILITY_FACTOR + 1.0f;
constexpr float NEUROMODULATION_PLASTICITY_FACTOR = 1.0f;

// Development stage plasticity modifiers
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;      // High plasticity
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
constexpr float PLASTICITY_MODIFIER_Maturation = 0.5f;
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;       // Stable

// ========== MEMORY SYSTEM THRESHOLDS ==========

// Working memory capacity fraction
constexpr float WORKING_MEMORY_CAPACITY_FRACTION = 0.1f;

// Episodic memory maximum episodes
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;

// Memory consolidation threshold
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;

// ========== NEURON PARAMETERS ==========

// Default neuron count
constexpr size_t DEFAULT_NEURON_COUNT = 500;
constexpr size_t SMALL_NEURON_COUNT = 1000;  // For tests

// Region configuration
constexpr size_t DEFAULT_REGION_COUNT = 1;
constexpr size_t NEURONS_PER_REGION_DIVISOR = 2;  // Half for internal neurons
constexpr size_t SENSORY_MOTOR_FRACTION = 4;      // Quarter each

// Neuron activation and inhibition
constexpr float NEURON_ACTIVATION_THRESHOLD = 5.0f;
constexpr float SYNAPTIC_CURRENT_SCALE_FACTOR = 10.0f;
constexpr float NEURON_EXCITABILITY_THRESHOLD = 0.5f;

// Membrane potential normalization
constexpr float POTENTIAL_NORMALIZATION_FACTOR = 20.0f;
constexpr float INPUT_NORMALIZATION_MAX = 10.0f;  // mV range
constexpr float INPUT_NORMALIZATION_MIN = -10.0f; // mV range

// Working memory normalization
constexpr float WORKING_MEMORY_NORMALIZATION_FACTOR = 10.0f;

// Connection ratio parameters
constexpr float DEFAULT_EXCITATION_INHIBITION_RATIO = 0.2f;
constexpr float DEFAULT_CONNECTION_INHIBITION_RATIO = 0.1f;

// ========== MOTOR AND SENSOR PARAMETERS ==========

// Input injection counts
constexpr size_t SENSOR_INPUT_INJECTION_COUNT = 10;
constexpr size_t SENSOR_PATTERN_INPUT_COUNT = 20;

// Current injection levels
constexpr float EXCITATORY_INPUT_CURRENT = 50.0f;
constexpr float MODERATE_INPUT_CURRENT = 30.0f;

// Action selection thresholds
constexpr size_t MOTOR_NEURON_FIRING_THRESHOLD = 0;  // Any firing triggers action

// ========== SAFETY AND BOUNDARY VALUES ==========

// Small epsilon for floating point comparisons
constexpr float EPSILON = 0.001f;

// Weight change significance threshold
constexpr float WEIGHT_CHANGE_SIGNIFICANCE = 0.01f;

// Time step safety margin
constexpr double TIME_STEP_SAFETY_MARGIN = 2.0;

// ========== CONFIGURATION CONSTANTS ==========

// Default configuration values for Phase 2
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
constexpr float DEFAULT_STDP_LTP_WEIGHT_CONFIG = 0.02f;
constexpr float DEFAULT_STDP_LTD_WEIGHT_CONFIG = 0.015f;
constexpr float DEFAULT_STDP_TAU_CONFIG = 20.0f;
constexpr float DEFAULT_SYNAPTOGENESIS_RATE_CONFIG = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE_CONFIG = 0.00001f;

// ========== LOG AND OUTPUT CONSTANTS ==========

// Most active neurons to track
constexpr size_t MOST_ACTIVE_NEURONS_TO_TRACK = 10;

// ========== RNG AND SEED CONSTANTS ==========

// Default random seed
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;

} // namespace nlm

// Definition of constexpr SimulationStep objects that need to be defined in a .cpp file
extern constexpr nlm::SimulationStep nlm::BASIC_CONNECTIVITY_TEST_STEPS;
extern constexpr nlm::SimulationStep nlm::PLASTICITY_EXPERIMENT_STEPS;