// Core configuration and constants for NLM neural simulation framework
// Provides named constants to replace magic numbers throughout the codebase

#pragma once

#include <cstdint>
#include <limits>

namespace nlm {

// Configuration version
constexpr const char* CONFIG_VERSION = "0.1.0";

// Simulation constants
constexpr double DEFAULT_TIMESTEP = 0.001;  // seconds
constexpr double MAX_TIMESTEP = 0.1;        // Maximum simulation timestep
constexpr double MIN_TIMESTEP = 0.00001;     // Minimum simulation timestep
constexpr SimulationStep DEFAULT_MAX_STEPS = 100000;  // Maximum simulation steps

// Neuron constants
constexpr size_t DEFAULT_NEURON_COUNT = 1000;      // Default number of neurons
constexpr size_t MIN_NEURON_COUNT = 10;             // Minimum neurons
constexpr size_t MAX_NEURON_COUNT = 10000;          // Maximum neurons
constexpr MembranePotential MAX_MEMBRANE_POTENTIAL = 50.0f;  // mV
constexpr MembranePotential MIN_MEMBRANE_POTENTIAL = -100.0f; // mV
constexpr MembranePotential DEFAULT_RESTING_POTENTIAL = -65.0f; // mV
constexpr MembranePotential DEFAULT_THRESHOLD = -50.0f; // mV
constexpr float DEFAULT_LEAK_CONDUCTANCE = 0.1f;    // nS
constexpr float DEFAULT_REFACTORY_PERIOD = 2.0f;    // ms
constexpr MembranePotential DEFAULT_RESET_POTENTIAL = -70.0f; // mV

// Synapse constants
constexpr float DEFAULT_SYNAPSE_WEIGHT = 0.5f;     // Initial synaptic weight
constexpr float MAX_SYNAPSE_WEIGHT = 10.0f;        // Maximum synaptic weight
constexpr float MIN_SYNAPSE_WEIGHT = -10.0f;       // Minimum synaptic weight
constexpr Delay DEFAULT_SYNAPSE_DELAY = 1;         // Default synaptic delay (steps)
constexpr size_t MAX_SYNAPSE_HISTORY = 100;        // Maximum spike history length
constexpr float DEFAULT_ELEVATORY_THRESHOLD = 0.5f; // Synaptic eligibility trace threshold

// Population constants
constexpr size_t MIN_POPULATION_SIZE = 1;          // Minimum neurons per population
constexpr float EXCITATORY_RATIO = 0.5f;           // Ratio of excitatory neurons
constexpr float INTERNAL_RATIO = 0.5f;             // Ratio of internal neurons
constexpr float MOTOR_RATIO = 0.5f;                // Ratio of motor neurons
constexpr float SENSORY_RATIO = 0.5f;              // Ratio of sensory neurons

// Network connectivity constants
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f; // Probability of synapse formation
constexpr float MIN_CONNECTION_PROBABILITY = 0.0f;     // Minimum connection probability
constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;     // Maximum connection probability
constexpr float DEFAULT_SYNAPSE_WEIGHT_STD = 0.2f;      // Weight standard deviation

// Plasticity constants
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;     // STDP LTP weight change
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;    // STDP LTD weight change
constexpr float DEFAULT_STDP_TAU = 20.0f;            // STDP trace decay time
constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f; // Hebbian learning rate
constexpr float DEFAULT_STRUCTUREL_RATE = 0.0001f;   // Structural plasticity rate
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;     // Synapse pruning rate

// Neuromodulation constants
constexpr float DEFAULT_DOPAMINE_MAX = 1.0f;         // Maximum dopamine level
constexpr float DEFAULT_DOPAMINE_MIN = -1.0f;        // Minimum dopamine level
constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;    // Novelty detection threshold
constexpr float DEFAULT_NOVELTY_DECAY = 0.95f;       // Novelty decay rate
constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f; // Curiosity threshold
constexpr float DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.5f; // Prediction error threshold

// Memory system constants
constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;     // Working memory capacity
constexpr size_t DEFAULT_WORKING_MEMORY_DECAY_STEPS = 1000; // Working memory decay steps
constexpr size_t DEFAULT_EPISODIC_MAX_EPISODES = 1000;     // Maximum episodic episodes
constexpr float DEFAULT_EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f; // Memory consolidation threshold
constexpr size_t DEFAULT_EPISODIC_REPLAY_INTERVAL = 100;      // Replay interval
constexpr size_t DEFAULT_EPISODIC_CONSOLIDATION_INTERVAL = 1000; // Consolidation interval
constexpr float DEFAULT_ASSOCIATIVE_STRENGTH = 1.0f;        // Default association strength
constexpr float MIN_ASSOCIATIVE_STRENGTH = 0.0f;           // Minimum association strength
constexpr float MAX_ASSOCIATIVE_STRENGTH = 10.0f;          // Maximum association strength

// Cognitive system constants
constexpr size_t DEFAULT_PLANNING_DEPTH = 3;                // Planning lookahead depth
constexpr float DEFAULT_PLANNING_CONFIDENCE = 0.5f;         // Default planning confidence
constexpr size_t DEFAULT_CONCEPT_FORMATION_THRESHOLD = 20;   // Concept formation threshold
constexpr float DEFAULT_CONCEPT_STABILITY_THRESHOLD = 0.8f;  // Concept stability threshold
constexpr size_t DEFAULT_CONCEPT_STABILITY_WINDOW = 20;     // Stability window size
constexpr size_t DEFAULT_ATTENTION_INHIBITION_STRENGTH = 50; // Attention inhibition strength
constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f; // Attention excitation strength

// Development system constants
constexpr SimulationStep DEFAULT_DEVELOPMENT_UPDATE_INTERVAL = 1000; // Development update interval
constexpr float DEFAULT_DEVELOPMENT_PLATIVITY_MODIFIER = 1.0f;        // Developmental plasticity modifier
constexpr float DEFAULT_CRITICAL_PERIOD_MULTIPLIER = 2.0f;           // Critical period plasticity
constexpr float DEFAULT_MATURATION_MODIFIER = 0.5f;                 // Maturation plasticity modifier
constexpr float DEFAULT_AGING_DECLINE_RATE = 0.01f;                 // Age-related decline rate

// Checkpoint system constants
constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";  // Default checkpoint directory
constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10;                   // Maximum stored checkpoints
constexpr size_t DEFAULT_CHECKPOINT_INTERVAL = 1000;            // Checkpoint saving interval
constexpr CompressionLevel DEFAULT_COMPRESSION_LEVEL = CompressionLevel::Balanced; // Default compression

// Performance constants
constexpr size_t MAX_SIMULTANEOUS_SPIKES = 1000;                // Maximum simultaneous spikes
constexpr size_t MAX_PENDING_SPIKE_EVENTS = 1000;             // Maximum pending spike events
constexpr float MAX_SPIKE_RATE = 1000.0f;                      // Maximum firing rate (Hz)
constexpr float MIN_SPIKE_RATE = 0.0f;                        // Minimum firing rate (Hz)

// Error handling constants
constexpr int DEFAULT_ERROR_CODE = -1;                        // Default error code
constexpr const char* ERROR_CATEGORY_GENERAL = "general";     // General error category
constexpr const char* ERROR_CATEGORY_MEMORY = "memory";       // Memory error category
constexpr const char* ERROR_CATEGORY_VALIDATION = "validation"; // Validation error category
constexpr const char* ERROR_CATEGORY_IO = "io";                // IO error category

// Logging constants
constexpr const char* LOG_DEFAULT_FILE = "nlm.log";           // Default log file
constexpr size_t LOG_MAX_SIZE = 10 * 1024 * 1024;             // Maximum log size (10MB)
constexpr const char* LOG_ROTATION_SUFFIX = ".%Y%m%d_%H%M%S";  // Log rotation suffix
constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::Info;       // Default log level

// Random number generation constants
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;                  // Default random seed
constexpr uint64_t MIN_RANDOM_SEED = 0;                       // Minimum random seed
constexpr uint64_t MAX_RANDOM_SEED = UINT64_MAX;              // Maximum random seed

// Visualization constants
constexpr size_t DEFAULT_VISUALIZATION_WIDTH = 800;           // Default visualization width
constexpr size_t DEFAULT_VISUALIZATION_HEIGHT = 600;          // Default visualization height
constexpr float DEFAULT_ZOOM_LEVEL = 1.0f;                    // Default zoom level
constexpr const char* DEFAULT_COLOR_SCHEME = "default";      // Default color scheme

// Experimental constants
constexpr const char* DEFAULT_EXPERIMENT_DIR = "./experiments"; // Default experiment directory
constexpr const char* DEFAULT_EXPERIMENT_RESULTS_FILE = "results.json"; // Default results file
constexpr double DEFAULT_EXPERIMENT_TIMEOUT = 300.0;         // Default experiment timeout (seconds)

// Mathematical constants
constexpr float PI = 3.141592653589793f;                      // Pi
constexpr float E = 2.718281828459045f;                      // Euler's number
constexpr float SQRT_2 = 1.414213562373095f;                  // Square root of 2
constexpr float EULER_MASCHERONI = 0.577215664901532f;        // Euler–Mascheroni constant

// Statistical constants
constexpr float EPSILON = 0.000001f;                         // Small value for floating-point comparisons
constexpr float EPSILON_SQUARED = 0.000001f * 0.000001f;     // Epsilon squared
constexpr float LARGE_VALUE = 1e10f;                         // Large value for comparisons
constexpr float SMALL_VALUE = 1e-10f;                        // Small value for comparisons

// File I/O constants
constexpr size_t DEFAULT_BUFFER_SIZE = 4096;                 // Default buffer size
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg"; // Default config file
constexpr const char* DEFAULT_RESULT_FILE = "results.txt";   // Default results file
constexpr const char* DEFAULT_CHECKPOINT_FILE = "checkpoint.nlp"; // Default checkpoint file

// Network constants
constexpr float DEFAULT_BANDWIDTH = 1000.0f;                 // Default network bandwidth (Mbps)
constexpr float DEFAULT_LATENCY = 0.001f;                   // Default network latency (seconds)
constexpr size_t DEFAULT_PACKET_SIZE = 1500;                 // Default packet size (bytes)

// Simulation optimization constants
constexpr size_t SIMD_ALIGNMENT = 32;                        // SIMD alignment (bytes)
constexpr size_t CACHE_LINE_SIZE = 64;                       // Cache line size (bytes)
constexpr size_t MAX_THREADS = 16;                          // Maximum threads for parallel processing
constexpr double THREAD_BALANCE_THRESHOLD = 0.8;             // Thread balance threshold

// System limits
constexpr size_t MAX_FILE_DESCRIPTOR = 1024;                // Maximum file descriptors
constexpr size_t MAX_MEMORY_LIMIT = 4 * 1024 * 1024 * 1024; // Maximum memory (4GB)
constexpr size_t MAX_CPU_TIME = 3600;                        // Maximum CPU time (seconds)

// Debug and testing constants
constexpr bool ENABLE_DEBUG_OUTPUT = true;                  // Enable debug output
constexpr bool ENABLE_PERFORMANCE_PROFILING = true;         // Enable performance profiling
constexpr bool ENABLE_UNIT_TESTS = true;                     // Enable unit tests
constexpr const char* TEST_RESULTS_DIR = "./test_results";   // Test results directory

// Compatibility constants
constexpr const char* COMPATIBILITY_VERSION = "0.1.0";      // Compatibility version
constexpr const char* COMPATIBILITY_FEATURE_FLAGS = "all";   // Compatibility feature flags

} // namespace nlm
