// NLM (熙然) - Neural Learning Machine
// Constants and configuration definitions
//
// This file contains constants used throughout the NLM project to improve
code maintainability, reduce magic numbers, and standardize parameters.

#pragma once

#include <cstdint>
#include "Types.hpp" // For SimulationStep type

namespace nlm {
namespace constants {

// ============================================================================
// NEURAL COMPUTATION CONSTANTS
// ============================================================================

// Neuron dynamics parameters
constexpr float LIF_TAU = 0.001f;              // Membrane time constant (10ms)
constexpr float LIF_V_REST = -65.0f;           // Resting potential
constexpr float LIF_V_RESET = -75.0f;          // Reset potential
constexpr float LIF_V_THRESH = -50.0f;         // Firing threshold

// Current injection constants
constexpr float DEFAULT_CURRENT_INJECTION = 50.0f;    // Strong excitatory input
constexpr float STDP_CURRENT_INJECTION = 60.0f;       // For correlated activity
constexpr float SENSORY_CURRENT_INJECTION = 30.0f;   // For sensory stimulation

// Plasticity thresholds
constexpr float PLASTICITY_STRENGTHEN_THRESHOLD = 0.01f;
constexpr float PLASTICITY_WEAKEN_THRESHOLD = 0.01f;
constexpr float LEARNING_THRESHOLD = 0.001f;

// ============================================================================
// SIMULATION CONSTANTS
// ============================================================================

// Time and step constants
constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;  // 1ms timestep
constexpr SimulationStep BASIC_CONNECTIVITY_STEPS = 50;
constexpr SimulationStep PLASTICITY_EXPERIMENT_STEPS = 1000;
constexpr SimulationStep STDP_TRIALS = 50;

// ============================================================================
// TESTING CONSTANTS
// ============================================================================

// Neuron and synapse testing
constexpr size_t NEURONS_FOR_CURRENT_INJECTION = 10;
constexpr size_t SENSORY_NEURONS_INJECT = 20;
constexpr size_t SYNAPSES_FOR_STDP_TEST = 5;
constexpr size_t MOST_ACTIVE_NEURONS_TO_TRACK = 10;

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

// Default configuration values
constexpr int64_t DEFAULT_RANDOM_SEED = 42;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
constexpr int64_t DEFAULT_NEURON_COUNT = 500;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr int64_t DEFAULT_VISION_WIDTH = 16;
constexpr int64_t DEFAULT_VISION_HEIGHT = 16;
constexpr int64_t DEFAULT_ENVIRONMENT_WIDTH = 20;
constexpr int64_t DEFAULT_ENVIRONMENT_HEIGHT = 20;

// ============================================================================
// PLASTICITY CONSTANTS
// ============================================================================

// STDP parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
constexpr float DEFAULT_STDP_TAU = 20.0f;

// Structural plasticity
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// ============================================================================
// LOGGING CONSTANTS
// ============================================================================

constexpr size_t MAX_LOG_MESSAGE_LENGTH = 4096;
constexpr size_t MAX_CONSOLE_LOG_LINES = 1000;
constexpr size_t MAX_FILE_LOG_SIZE_MB = 100;

// ============================================================================
// MEMORY MANAGEMENT CONSTANTS
// ============================================================================

// Pool sizes for memory optimization
constexpr size_t NEURON_POOL_SIZE = 1000;
constexpr size_t SYNAPSE_POOL_SIZE = 5000;
constexpr size_t SPIKE_EVENT_POOL_SIZE = 10000;

// ============================================================================
// AGENT SYSTEM CONSTANTS
// ============================================================================

// World configuration
constexpr float DEFAULT_WORLD_WIDTH = 20.0f;
constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;
constexpr float WORLD_AGENT_SIZE = 1.0f;
constexpr float WORLD_OBSTACLE_PROBABILITY = 0.1f;

// Agent behavior
constexpr float AGENT_ENERGY_MAX = 100.0f;
constexpr float AGENT_ENERGY_REGEN_RATE = 0.5f;
constexpr float AGENT_ENERGY_CONSUMPTION_PER_STEP = 0.1f;
constexpr float AGENT_SPEED = 2.0f;

// ============================================================================
// SENSOR CONSTANTS
// ============================================================================

// Sensory input ranges
constexpr float SENSOR_INPUT_MIN = 0.0f;
constexpr float SENSOR_INPUT_MAX = 1.0f;
constexpr float SENSOR_NOISE_STDDEV = 0.05f;

// ============================================================================
// PREDICTION CONSTANTS
// ============================================================================

// Prediction parameters
constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;
constexpr float PREDICTION_LEARNING_RATE = 0.01f;
constexpr int64_t PREDICTION_WINDOW_SIZE = 10;

// ============================================================================
// MEMORY CONSTANTS
// ============================================================================

// Working memory
constexpr size_t WORKING_MEMORY_CAPACITY = 50;
constexpr float WORKING_MEMORY_DECAY_RATE = 0.95f;

// Episodic memory
constexpr size_t MAX_EPISODES_RECORDED = 1000;
constexpr float EPISODIC_RECALL_THRESHOLD = 0.8f;

// ============================================================================
// DEVELOPMENT CONSTANTS
// ============================================================================

// Developmental stages
constexpr float DEVELOPMENT_CRITICAL_PERIOD_START = 10.0f;
constexpr float DEVELOPMENT_CRITICAL_PERIOD_END = 100.0f;
constexpr float DEVELOPMENT_MATURATION_THRESHOLD = 0.8f;

// ============================================================================
// END OF CONSTANTS
// ============================================================================

} // namespace constants
} // namespace nlm
