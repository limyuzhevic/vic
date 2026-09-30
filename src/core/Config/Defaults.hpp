#pragma once

// NLM Configuration
//
// This file contains configuration options for the NLM (Neural Learning Machine) project.
// These constants can be modified to tune system behavior for different experiments.

namespace nlm {

// ========== SIMULATION PARAMETERS ==========
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;     // Core simulation timestep in seconds
constexpr uint64_t DEFAULT_MAX_SIMULATION_STEPS = 10000;  // Maximum simulation steps
constexpr double SIMULATION_TIME_LIMIT = 0.0;            // No time limit unless specified

// ========== BRAIN CONFIGURATION ==========
constexpr size_t DEFAULT_NEURON_COUNT = 1000;            // Default number of neurons
constexpr size_t DEFAULT_REGION_COUNT = 2;               // Default number of brain regions
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;  // Connectivity probability
constexpr float DEFAULT_INITIAL_WEIGHT_MEAN = 0.5f;      // Mean initial synaptic weight
constexpr float DEFAULT_INITIAL_WEIGHT_STD = 0.1f;       // Standard deviation of initial weights

// ========== NEURON PARAMETERS ==========
constexpr float DEFAULT_V_THRESH = -50.0f;               // Neuron threshold potential (mV)
constexpr float DEFAULT_V_REST = -70.0f;                // Resting potential (mV)
constexpr float DEFAULT_V_RESET = -75.0f;               // Reset potential after spike (mV)
constexpr float DEFAULT_TAU_MEM = 20.0f;                 // Membrane time constant (ms)
constexpr float DEFAULT_TAU_REF = 2.0f;                  // Refractory period (ms)

// ========== PLASTICITY PARAMETERS ==========
// STDP parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;         // STDP Long-Term Potentiation weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;        // STDP Long-Term Depression weight
constexpr float DEFAULT_STDP_TAU = 20.0f;                // STDP time constant

// Hebbian parameters
constexpr bool DEFAULT_HEBBIAN_ENABLED = true;          // Enable Hebbian learning
constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;  // Hebbian learning rate

// Structural plasticity parameters
constexpr bool DEFAULT_STRUCTURAL_ENABLED = true;       // Enable structural plasticity
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // New synapse formation rate
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // Weak synapse removal rate

// ========== NEUROMODULATION PARAMETERS ==========
// Dopamine parameters
constexpr float DEFAULT_DOPAMINE_SCALE = 1.0f;           // Dopamine modulation scale

// Curiosity parameters
constexpr bool DEFAULT_CURIOUSITY_ENABLED = true;       // Enable curiosity-driven exploration

// Novelty detection parameters
constexpr bool DEFAULT_NOVELTY_ENABLED = true;           // Enable novelty detection

// Prediction error parameters
constexpr float DEFAULT_PREDICTION_ERROR_SCALE = 1.0f;   // Prediction error signal scale

// ========== MEMORY PARAMETERS ==========
// Working memory
constexpr bool DEFAULT_WORKING_MEMORY_ENABLED = true;   // Enable working memory
constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000; // Working memory capacity

// Episodic memory
constexpr bool DEFAULT_EPISODIC_MEMORY_ENABLED = true; // Enable episodic memory
constexpr size_t DEFAULT_EPISODE_CAPACITY = 10000;       // Maximum number of stored episodes
constexpr float DEFAULT_EPISODE_STORED_THRESHOLD = 5.0f; // Activation threshold for storing episodes

// Associative memory
constexpr bool DEFAULT_ASSOCIATIVE_MEMORY_ENABLED = true; // Enable associative memory
constexpr float DEFAULT_ASSOCIATION_STRENGTH = 0.5f;     // Strength of associative connections

// ========== DEVELOPMENT PARAMETERS ==========
constexpr bool DEFAULT_DEVELOPMENT_ENABLED = true;       // Enable developmental processes
constexpr DevelopmentalStage DEFAULT_DEVELOPMENTAL_STAGE = DevelopmentalStage::Initial; // Initial developmental stage
constexpr float DEFAULT_CRITICAL_PERIOD_DURATION = 60.0f; // Duration of critical period (arbitrary units)

// ========== WORLD AND ENVIRONMENT ==========
// World simulation parameters
constexpr size_t DEFAULT_WORLD_WIDTH = 20;              // World width (cells)
constexpr size_t DEFAULT_WORLD_HEIGHT = 20;             // World height (cells)
constexpr size_t DEFAULT_VISION_WIDTH = 8;              // Vision field width
constexpr size_t DEFAULT_VISION_HEIGHT = 8;             // Vision field height
constexpr size_t DEFAULT_TOUCH_SENSORS = 8;             // Number of touch sensors
constexpr size_t DEFAULT_INTERNAL_SIGNALS = 4;          // Number of internal signals
constexpr size_t DEFAULT_PROPRIOCEPTION_SENSORS = 6;    // Number of proprioception sensors
constexpr float DEFAULT_MAX_WORLD_ENERGY = 100.0f;       // Maximum energy in world

// ========== EXPERIMENT PARAMETERS ==========
// Phase 3 experiment
constexpr size_t PHASE3_EXPERIMENT_NEURONS = 500;       // Number of neurons for Phase 3
constexpr size_t PHASE3_EXPERIMENT_STEPS = 1000;        // Number of steps for Phase 3

// Phase 4 experiment
constexpr size_t PHASE4_EXPERIMENT_NEURONS = 1000;      // Number of neurons for Phase 4
constexpr size_t PHASE4_EXPERIMENT_STEPS = 2000;        // Number of steps for Phase 4

// Phase 6 integration
constexpr size_t PHASE6_INTEGRATION_NEURONS = 500;       // Number of neurons for Phase 6
constexpr size_t PHASE6_INTEGRATION_MAX_STEPS = 2000;    // Maximum steps for Phase 6 integration
constexpr bool PHASE6_ENABLE_CHECKPOINTING = true;       // Enable checkpointing in Phase 6
constexpr bool PHASE6_ENABLE_REPLAY = true;              // Enable memory replay in Phase 6
constexpr bool PHASE6_ENABLE_DEVELOPMENT = true;        // Enable development in Phase 6

// ========== LOGGING AND VISUALIZATION ==========
constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::Info;   // Default logging level
constexpr bool DEFAULT_LOGGING_TO_FILE = false;         // Whether to log to file
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log"; // Default log filename
constexpr float DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0f; // Visualization update rate (Hz)
constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;   // Whether visualization is enabled

// ========== MOTOR SYSTEM ==========
constexpr size_t DEFAULT_MOTOR_COMMAND_TYPES = 6;      // Number of basic motor commands
constexpr float DEFAULT_MOTOR_ACTION_THRESHOLD = 0.5f;   // Activity threshold for motor output

// ========== AGENT PARAMETERS ==========
constexpr float DEFAULT_AGENT_REWARD_MODULATION_STRENGTH = 1.0f; // Agent reward modulation strength
constexpr float DEFAULT_AGENT_EXPLORATION_FACTOR = 0.5f; // Agent exploration factor

// ========== PERFORMANCE OPTIMIZATION ==========
constexpr bool DEFAULT_ENABLE_PERFORMANCE_OPTIMIZATIONS = true; // Enable performance optimizations
constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 1000;       // Size of memory pools
constexpr size_t DEFAULT_EVENT_QUEUE_SIZE = 10000;      // Size of event queues

// ========== EXPERIMENT METRICS ==========
constexpr float DEFAULT_METRIC_DECAY_FACTOR = 0.99f;     // Decay factor for metrics
constexpr size_t DEFAULT_METRICS_HISTORY_SIZE = 1000;   // Size of metrics history buffers

} // namespace nlm
