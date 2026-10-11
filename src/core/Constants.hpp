// NLM Constants and Configuration
// Centralized constants and configuration values for the NLM framework

#pragma once

namespace nlm {
namespace constants {

// ==============================================================================
// Simulation Constants
// ==============================================================================

// Time and Step Constants
static constexpr double SIMULATION_TIMESTEP = 0.001;       // Default simulation timestep (seconds)
static constexpr double MIN_TIMESTEP = 1e-6;               // Minimum allowed timestep
static constexpr double MAX_TIMESTEP = 0.1;                // Maximum allowed timestep
static constexpr SimulationStep MAX_SIMULATION_STEPS = 1000000; // Maximum simulation steps

// Neuron and Network Constants
static constexpr int64_t DEFAULT_NEURON_COUNT = 500;        // Default number of neurons
static constexpr int64_t MIN_NEURON_COUNT = 1;               // Minimum neurons
static constexpr int64_t MAX_NEURON_COUNT = 100000;         // Maximum neurons
static constexpr int64_t DEFAULT_REGION_COUNT = 1;          // Default number of regions
static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f; // Default connection probability

// ==============================================================================
// Plasticity Constants
// ==============================================================================

// STDP Constants
static constexpr float STDP_LTP_WEIGHT = 0.02f;             // STDP long-term potentiation weight
static constexpr float STDP_LTD_WEIGHT = 0.015f;            // STDP long-term depression weight
static constexpr float STDP_MIN_WEIGHT = 0.0f;              // Minimum STDP weight
static constexpr float STDP_MAX_WEIGHT = 1.0f;              // Maximum STDP weight
static constexpr float STDP_TIME_CONSTANT = 20.0f;         // STDP time constant (tau)
static constexpr float STDP_MIN_TIME_CONSTANT = 1.0f;       // Minimum STDP time constant
static constexpr float STDP_MAX_TIME_CONSTANT = 100.0f;     // Maximum STDP time constant

// Structural Plasticity Constants
static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;       // Synaptogenesis rate
static constexpr float PRUNING_RATE = 0.00001f;            // Pruning rate
static constexpr float MAX_STRUCTUREAL_CHANGE = 1.0f;      // Maximum structural plasticity change

// Hebbian Constants
static constexpr float HEBBIAN_LEARNING_RATE = 0.01f;       // Hebbian learning rate
static constexpr float HEBBIAN_MAX_WEIGHT = 1.0f;            // Maximum Hebbian weight
static constexpr float HEBBIAN_MIN_WEIGHT = 0.0f;           // Minimum Hebbian weight

// ==============================================================================
// Test and Experiment Constants
// ==============================================================================

// Basic Connectivity Test Constants
static constexpr size_t BASIC_TEST_NEURON_INJECTION_COUNT = 10; // Neurons to inject current into
static constexpr float BASIC_TEST_INJECT_CURRENT = 50.0f;      // Current injection for basic test
static constexpr SimulationStep BASIC_TEST_MAX_STEPS = 50;    // Maximum steps for basic test

// Plasticity Experiment Constants
static constexpr size_t PLASTICITY_TEST_STEP_COUNT = 1000;    // Steps for plasticity test
static constexpr size_t PLASTICITY_TEST_INPUT_COUNT = 20;     // Input neurons for plasticity test
static constexpr float PLASTICITY_TEST_INJECT_CURRENT = 30.0f; // Current injection for plasticity test

// STDP Verification Constants
static constexpr size_t STDP_TEST_SYNAPSE_COUNT = 5;          // Synapses to test for STDP
static constexpr size_t STDP_TEST_MAX_TRIALS = 50;            // Maximum STDP test trials
static constexpr float STDP_TEST_CURRENT = 60.0f;            // Current injection for STDP test
static constexpr float STDP_LTP_THRESHOLD = 0.001f;          // LTP threshold
static constexpr float STDP_LTD_THRESHOLD = -0.001f;         // LTD threshold

// ==============================================================================
// Memory System Constants
// ==============================================================================

// Working Memory Constants
static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000; // Default working memory capacity
static constexpr float WORKING_MEMORY_FORGETTING_RATE = 0.01f;   // Working memory forgetting rate
static constexpr float WORKING_MEMORY_STRENGTH_DECAY = 0.995f;   // Memory strength decay per step

// Episodic Memory Constants
static constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 5000; // Default episodic memory capacity
static constexpr float EPISODIC_MEMORY_RECALL_THRESHOLD = 0.3f;   // Recall threshold
static constexpr float EPISODIC_MEMORY_FORGETTING_RATE = 0.001f;  // Episodic memory forgetting rate

// Associative Memory Constants
static constexpr float ASSOCIATIVE_STRENGTH_THRESHOLD = 0.3f;   // Associative strength threshold
static constexpr size_t ASSOCIATIVE_MAX_PATTERNS = 100;         // Maximum number of patterns

// ==============================================================================
// Development Constants
// ==============================================================================

// Developmental Constants
static constexpr float CRITICAL_PERIOD_DURATION = 100.0f;       // Critical period duration (steps)
static constexpr float DEVELOPMENT_PLASTICITY_BOOST = 2.0f;   // Plasticity boost during development
static constexpr float DEVELOPMENT_SYNAPTogenesis_BOOST = 1.5f; // Synaptogenesis boost during development
static constexpr float DEVELOPMENT_PRUNING_BOOST = 1.5f;      // Pruning boost during development
static constexpr float DEVELOPMENT_MIN_EXCITABILITY = 0.5f;    // Minimum neural excitability
static constexpr float DEVELOPMENT_MAX_EXCITABILITY = 2.0f;    // Maximum neural excitability

// ==============================================================================
// Neuromodulation Constants
// ==============================================================================

// Reward Modulation Constants
static constexpr float REWARD_SENSITIVITY = 1.0f;               // Reward sensitivity
static constexpr float REWARD_MAX_EFFECT = 1.0f;               // Maximum reward effect
static constexpr float REWARD_MIN_EFFECT = -1.0f;              // Minimum reward effect
static constexpr float REWARD_DECAY_RATE = 0.95f;              // Reward signal decay rate

// Curiosity Constants
static constexpr float CURIOSITY_SENSITIVITY = 1.0f;            // Curiosity sensitivity
static constexpr float CURIOSITY_MAX_DRIVE = 1.0f;              // Maximum curiosity drive
static constexpr float CURIOSITY_NOVELTY_THRESHOLD = 0.3f;      // Novelty detection threshold

// Novelty Detection Constants
static constexpr float NOVELTY_SENSITIVITY = 1.0f;              // Novelty sensitivity
static constexpr float NOVELTY_MAX_DETECTION = 1.0f;            // Maximum novelty detection
static constexpr float NOVELTY_DECAY_RATE = 0.9f;               // Novelty decay rate

// ==============================================================================
// Prediction and Cognition Constants
// ==============================================================================

// Prediction System Constants
static constexpr float PREDICTION_ERROR_THRESHOLD = 0.8f;       // Prediction error threshold
static constexpr float PREDICTION_MAX_ERROR = 1.0f;             // Maximum prediction error
static constexpr float PREDICTION_MIN_ERROR = 0.0f;             // Minimum prediction error
static constexpr float PREDICTION_LEARNING_RATE = 0.1f;         // Prediction learning rate

// Planning Constants
static constexpr float PLANNING_CONFIDENCE_THRESHOLD = 0.7f;    // Planning confidence threshold
static constexpr float PLANNING_MAX_CONFIDENCE = 1.0f;           // Maximum planning confidence
static constexpr float PLANNING_MIN_CONFIDENCE = 0.0f;          // Minimum planning confidence
static constexpr size_t PLANNING_MAX_SEQUENCE_LENGTH = 10;     // Maximum planning sequence length

// Attention Constants
static constexpr float ATTENTION_SELECTIVITY_THRESHOLD = 0.3f;  // Attention selectivity threshold
static constexpr float ATTENTION_DISTRECTION_RESISTANCE = 0.5f; // Distraction resistance threshold

// ==============================================================================
// Environment and Behavior Constants
// ==============================================================================

// Environment Constants
static constexpr int64_t DEFAULT_ENVIRONMENT_WIDTH = 20;     // Default environment width
static constexpr int64_t DEFAULT_ENVIRONMENT_HEIGHT = 20;    // Default environment height
static constexpr float ENVIRONMENT_CELL_SIZE = 1.0f;          // Environment cell size

// Action Constants
static constexpr ActionType DEFAULT_ACTION = ActionType::MoveForward; // Default action
static constexpr float ACTION_EXECUTION_TIME = 0.1f;          // Action execution time

// ==============================================================================
// Logging and Output Constants
// ==============================================================================

// Logging Constants
static constexpr int DEBUG_LOG_LEVEL = 0;                     // Debug log level
static constexpr int INFO_LOG_LEVEL = 1;                     // Info log level
static constexpr int WARNING_LOG_LEVEL = 2;                  // Warning log level
static constexpr int ERROR_LOG_LEVEL = 3;                    // Error log level
static constexpr int CRITICAL_LOG_LEVEL = 4;                 // Critical log level

// Output Constants
static constexpr const char* DEFAULT_RESULTS_FILE = "results.csv"; // Default results file
static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg"; // Default config file
static constexpr const char* DEFAULT_LOG_FILE = "nlm.log";   // Default log file
static constexpr size_t MAX_LOG_ENTRIES = 100000;             // Maximum log entries

// ==============================================================================
// Visualization Constants
// ==============================================================================

// Visualization Constants
static constexpr size_t VISUALIZATION_DATA_POINTS = 1000;    // Number of data points for visualization
static constexpr float VISUALIZATION_SCALE_FACTOR = 1.0f;     // Visualization scale factor
static constexpr double VISUALIZATION_UPDATE_INTERVAL = 0.1; // Visualization update interval

// ==============================================================================
// Performance Constants
// ==============================================================================

// Performance Constants
static constexpr double BENCHMARK_MIN_TIME = 0.001;           // Minimum benchmark time
static constexpr double BENCHMARK_MAX_TIME = 1000.0;         // Maximum benchmark time
static constexpr size_t BENCHMARK_MIN_NEURONS = 10;           // Minimum benchmark neurons
static constexpr size_t BENCHMARK_MAX_NEURONS = 10000;        // Maximum benchmark neurons
static constexpr size_t BENCHMARK_MIN_STEPS = 10;             // Minimum benchmark steps
static constexpr size_t BENCHMARK_MAX_STEPS = 100000;         // Maximum benchmark steps

// ==============================================================================
// System and Resource Constants
// ==============================================================================

// System Constants
static constexpr size_t MAX_THREADS = 8;                     // Maximum threads for parallel processing
static constexpr size_t MAX_MEMORY_POOL_SIZE = 1024 * 1024 * 10; // Maximum memory pool size (10MB)
static constexpr double MAX_CPU_USAGE = 0.95;                 // Maximum CPU usage threshold
static constexpr double MAX_MEMORY_USAGE = 0.90;             // Maximum memory usage threshold

// ==============================================================================
// Experimental Constants
// ==============================================================================

// Experimental Constants
static constexpr size_t PHASE3_EXPERIMENT_DEFAULT_EPISODES = 5;   // Phase 3 experiment default episodes
static constexpr size_t PHASE3_EXPERIMENT_DEFAULT_STEPS_PER_EPISODE = 200; // Phase 3 experiment default steps
static constexpr size_t PHASE4_EXPERIMENT_DEFAULT_EPISODES = 3;   // Phase 4 experiment default episodes
static constexpr size_t PHASE4_EXPERIMENT_DEFAULT_STEPS_PER_EPISODE = 200; // Phase 4 experiment default steps
static constexpr size_t PHASE5_EXPERIMENT_DEFAULT_EPISODES = 10;  // Phase 5 experiment default episodes
static constexpr size_t PHASE5_EXPERIMENT_DEFAULT_STEPS_PER_EPISODE = 100; // Phase 5 experiment default steps

// ==============================================================================
// Mathematical and Statistical Constants
// ==============================================================================

// Mathematical Constants
static constexpr float MATH_EPSILON = 1e-6f;                  // Machine epsilon for float
static constexpr double MATH_DOUBLE_EPSILON = 1e-12;         // Machine epsilon for double
static constexpr float MATH_PI = 3.141592653589793f;          // Pi
static constexpr float MATH_2PI = 6.283185307179586f;         // 2 * Pi
static constexpr float MATH_HALF_PI = 1.5707963267948966f;    // Pi / 2

// Statistical Constants
static constexpr float STAT_MEAN_DEFAULT = 0.0f;              // Default mean value
static constexpr float STAT_STDDEV_DEFAULT = 1.0f;            // Default standard deviation
static constexpr float STAT_MIN_VALUE = -1e6f;                // Minimum statistical value
static constexpr float STAT_MAX_VALUE = 1e6f;                 // Maximum statistical value

// ==============================================================================
// Configuration Schema Constants
// ==============================================================================

// Schema Constants
static constexpr const char* CONFIG_SCHEMA_NAME = "NLM Configuration"; // Configuration schema name
static constexpr const char* CONFIG_SCHEMA_VERSION = "2.0.0";      // Configuration schema version
static constexpr const char* CONFIG_SCHEMA_DESCRIPTION = "Neural Learning Machine Configuration Schema"; // Schema description

// Field validation constants
static constexpr size_t CONFIG_MIN_FIELD_LENGTH = 1;           // Minimum field name length
static constexpr size_t CONFIG_MAX_FIELD_LENGTH = 100;        // Maximum field name length
static constexpr size_t CONFIG_MAX_DESCRIPTION_LENGTH = 500;  // Maximum field description length

// ==============================================================================
// File and Path Constants
// ==============================================================================

// File constants
static constexpr const char* DEFAULT_BACKUP_EXTENSION = ".bak";      // Default backup file extension
static constexpr const char* DEFAULT_ARCHIVE_EXTENSION = ".zip";     // Default archive file extension
static constexpr const char* DEFAULT_CHECKPOINT_EXTENSION = ".chk";   // Default checkpoint file extension

// Path constants
static constexpr const char* DEFAULT_CONFIG_DIRECTORY = "configs";      // Default config directory
static constexpr const char* DEFAULT_LOG_DIRECTORY = "logs";           // Default log directory
static constexpr const char* DEFAULT_DATA_DIRECTORY = "data";         // Default data directory
static constexpr const char* DEFAULT_OUTPUT_DIRECTORY = "output";      // Default output directory

// ==============================================================================
// Debug and Development Constants
// ==============================================================================

// Debug constants
static constexpr bool DEBUG_ENABLED = false;                  // Debug mode enabled
static constexpr size_t DEBUG_MAX_STACK_TRACE = 100;           // Maximum stack trace entries
static constexpr double DEBUG_BREAKPOINT_THRESHOLD = 1e6;       // Debug breakpoint threshold

// Development constants
static constexpr const char* DEV_MODE_ENVIRONMENT = "development"; // Development environment
static constexpr const char* PROD_MODE_ENVIRONMENT = "production";  // Production environment
static constexpr const char* TEST_MODE_ENVIRONMENT = "test";        // Test environment

// ==============================================================================
// Copyright and Legal Constants
// ==============================================================================

// Copyright constants
static constexpr const char* COPYRIGHT_HOLDER = "NLM Authors";      // Copyright holder
static constexpr const char* LICENSE_TYPE = "MIT";                  // License type
static constexpr const char* PROJECT_NAME = "NLM — 熙然";          // Project name
static constexpr const char* PROJECT_VERSION = "0.1.0";           // Project version

// End of namespace
} // namespace constants
} // namespace nlm