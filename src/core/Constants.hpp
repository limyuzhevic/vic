#pragma once

// NLM Constants File
// Centralized constants for all magic numbers used throughout the codebase
// This improves code readability, maintainability, and consistency

namespace nlm {

// Simulation and Time Constants
namespace Time {
    constexpr double DEFAULT_TIMESTEP = 0.001;        // Simulation timestep in seconds
    constexpr double DEFAULT_TOTAL_DURATION = 10.0;    // Default total simulation duration in seconds
    constexpr double MAX_TIME_STEP = 0.01;            // Maximum allowed timestep
    constexpr double MIN_TIME_STEP = 0.0001;          // Minimum allowed timestep
    constexpr double NANOS_PER_SECOND = 1e9;          // Nanoseconds in one second
    constexpr double MICROS_PER_SECOND = 1e6;         // Microseconds in one second
}

// Neuron and Neural Dynamics Constants
namespace Neuron {
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // Membrane capacitance in nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;       // Time constant in ms
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;   // Resting potential in mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;           // Firing threshold in mV
    constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;     // Reset potential in mV
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;      // Leak conductance in nS
    constexpr float DEFAULT_MAX_MEMBRANE_POTENTIAL = 50.0f;  // Maximum membrane potential
    constexpr float DEFAULT_MIN_MEMBRANE_POTENTIAL = -100.0f; // Minimum membrane potential
    constexpr float DEFAULT_SPIKE_FREQUENCY_ADAPTATION = 1.0f; // Initial adaptation variable
    constexpr float ADAPTATION_DECAY_RATE = 0.95f;         // Decay rate for adaptation
    constexpr float ADAPTATION_EFFECT_PER_SPIKE = 1.0f;    // Adaptation contribution per spike
    constexpr float ADAPTATION_EFFECT = 0.01f;             // Overall adaptation effect
    constexpr float SPIKE_FREQUENCY_ADAPTATION_GAIN = 0.1f; // Gain for spike-frequency adaptation
    constexpr float DEFAULT_REFRACTORY_PERIOD = 5;         // Default refractory period in steps
    constexpr float MAX_REFRACTORY_PERIOD = 20.0f;         // Maximum refractory period
    constexpr float MIN_REFRACTORY_PERIOD = 2.0f;          // Minimum refractory period
    constexpr float DEFAULT_MAX_SPIKE_HISTORY = 100;       // Maximum spike history entries
    constexpr float SPIKE_THRESHOLD_COMPARISON_TOLERANCE = 1e-6f; // Tolerance for spike detection
    constexpr float CURRENT_INJECTION_FACTOR = 10.0f;      // Factor for converting current to voltage
}

// Synapse and Connectivity Constants
namespace Synapse {
    constexpr float DEFAULT_WEIGHT = 0.2f;                // Default synaptic weight
    constexpr float MAX_WEIGHT = 1.0f;                    // Maximum synaptic weight
    constexpr float MIN_WEIGHT = -1.0f;                   // Minimum synaptic weight
    constexpr float DEFAULT_DELAY = 1;                    // Default synaptic delay in steps
    constexpr float MAX_DELAY = 10;                       // Maximum synaptic delay
    constexpr float MIN_DELAY = 0;                        // Minimum synaptic delay
    constexpr float EXCITATORY_WEIGHT_THRESHOLD = 0.0f;   // Weight threshold for excitatory synapses
    constexpr float INHIBITORY_WEIGHT_THRESHOLD = 0.0f;   // Weight threshold for inhibitory synapses
    constexpr float SYNAPTIC_EFFICIENCY_DEFAULT = 1.0f;    // Default synaptic efficacy
    constexpr float SYNAPTIC_EFFICIENCY_MIN = 0.0f;       // Minimum synaptic efficacy
    constexpr float SYNAPTIC_EFFICIENCY_MAX = 2.0f;       // Maximum synaptic efficacy
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.9f;       // Decay rate for eligibility traces
    constexpr float ELIGIBILITY_TRACE_MIN = -1.0f;       // Minimum eligibility trace
    constexpr float ELIGIBILITY_TRACE_MAX = 1.0f;        // Maximum eligibility trace
    constexpr float DEFAULT_SYNAPTIC_CONDUCTANCE = 0.5f;  // Default synaptic conductance
}

// Plasticity Constants
namespace Plasticity {
    // STDP Parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;     // STDP LTP weight (A+)
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;    // STDP LTD weight (A-)
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;  // STDP time constant (tau)
    constexpr float STDP_TIME_CONSTANT_MIN = 1.0f;       // Minimum STDP time constant
    constexpr float STDP_TIME_CONSTANT_MAX = 100.0f;     // Maximum STDP time constant
    constexpr float STDP_WEIGHT_MIN = 0.0f;              // Minimum STDP weight
    constexpr float STDP_WEIGHT_MAX = 1.0f;              // Maximum STDP weight
    constexpr float STDP_POTENTIATION_THRESHOLD = 0.01f; // Threshold for considering potentiation
    constexpr float STDP_DEPRESSION_THRESHOLD = -0.01f; // Threshold for considering depression
    constexpr float STDP_MAX_DELTA_PER_PAIR = 0.05f;      // Maximum weight change per spike pair
    constexpr float STDP_MIN_DELTA_PER_PAIR = -0.05f;    // Minimum weight change per spike pair
    
    // Hebbian Parameters
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f; // Hebbian learning rate
    constexpr float HEBBIAN_LEARNING_RATE_MIN = 0.0f;       // Minimum Hebbian learning rate
    constexpr float HEBBIAN_LEARNING_RATE_MAX = 1.0f;       // Maximum Hebbian learning rate
    constexpr float HEBBIAN_WEIGHT_CHANGE_FACTOR = 0.1f;    // Factor for Hebbian weight changes
    constexpr float HEBBIAN_MAX_WEIGHT_CHANGE = 0.1f;      // Maximum Hebbian weight change
    
    // Structural Plasticity Parameters
    constexpr float DEFAULT_SYNAPTAGENESIS_RATE = 0.0001f; // Synaptogenesis rate
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;        // Pruning rate
    constexpr float STRUCTURAL_PLASTICITY_MIN_RATE = 0.0f; // Minimum plasticity rate
    constexpr float STRUCTURAL_PLASTICITY_MAX_RATE = 0.01f; // Maximum plasticity rate
    constexpr float STRUCTURAL_PLASTICITY_MIN_WEIGHT = 0.01f; // Minimum weight for pruning consideration
    constexpr float STRUCTURAL_PLASTICITY_MAX_WEIGHT = 0.5f;  // Maximum weight for synaptogenesis consideration
    constexpr float DEVELOPMENT_PLASTICITY_MODIFIER_INITIAL = 1.0f; // Initial plasticity modifier
    constexpr float DEVELOPMENT_PLASTICITY_MODIFIER_CRITICAL = 0.8f; // Critical period modifier
    constexpr float DEVELOPMENT_PLASTICITY_MODIFIER_MATURATION = 0.5f; // Maturation modifier
    constexpr float DEVELOPMENT_PLASTICITY_MODIFIER_ADULT = 0.2f; // Adult modifier
    
    // General Plasticity Parameters
    constexpr float DEFAULT_LEARNING_RATE = 0.01f;       // Default learning rate
    constexpr float PLASTICITY_LEARNING_RATE_MIN = 0.0f; // Minimum learning rate
    constexpr float PLASTICITY_LEARNING_RATE_MAX = 1.0f; // Maximum learning rate
    constexpr float PLASTICITY_THRESHOLD = 0.001f;        // Threshold for considering significant change
    constexpr float PLASTICITY_EPSILON = 1e-6f;           // Small value for numerical stability
    constexpr float PLASTICITY_CONVERGENCE_TOLERANCE = 1e-4f; // Tolerance for convergence checks
}

// Network and Architecture Constants
namespace Network {
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;  // Default connection probability
    constexpr float CONNECTION_PROBABILITY_MIN = 0.0f;       // Minimum connection probability
    constexpr float CONNECTION_PROBABILITY_MAX = 1.0f;       // Maximum connection probability
    constexpr float DEFAULT_NEURON_COUNT = 500;               // Default number of neurons
    constexpr float NEURON_COUNT_MIN = 10;                    // Minimum neuron count
    constexpr float NEURON_COUNT_MAX = 10000;                 // Maximum neuron count
    constexpr float DEFAULT_REGION_COUNT = 1;                // Default number of regions
    constexpr float REGION_COUNT_MIN = 1;                    // Minimum region count
    constexpr float REGION_COUNT_MAX = 10;                   // Maximum region count
    constexpr float DEFAULT_NEURONS_PER_REGION = 500;         // Default neurons per region
    constexpr float NEURONS_PER_REGION_MIN = 10;              // Minimum neurons per region
    constexpr float NEURONS_PER_REGION_MAX = 2000;            // Maximum neurons per region
    constexpr float DEFAULT_SENSORY_NEURONS_RATIO = 0.25f;   // Ratio of sensory neurons
    constexpr float DEFAULT_MOTOR_NEURONS_RATIO = 0.25f;     // Ratio of motor neurons
    constexpr float DEFAULT_INTERNAL_NEURONS_RATIO = 0.5f;   // Ratio of internal neurons
    constexpr float SYNAPSE_PER_NEURON_MIN = 1;               // Minimum synapses per neuron
    constexpr float SYNAPSE_PER_NEURON_MAX = 100;             // Maximum synapses per neuron
    constexpr float EXCITATORY_INHIBITORY_RATIO_DEFAULT = 1.0f; // Default E/I ratio
    constexpr float EXCITATORY_INHIBITORY_RATIO_MIN = 0.1f;   // Minimum E/I ratio
    constexpr float EXCITATORY_INHIBITORY_RATIO_MAX = 10.0f;  // Maximum E/I ratio
}

// Testing and Experiment Constants
namespace Testing {
    constexpr size_t BASIC_TEST_STEPS = 50;                   // Number of steps in basic connectivity test
    constexpr size_t PLASTICITY_TEST_STEPS = 500;             // Number of steps in plasticity test
    constexpr size_t STDP_TEST_STEPS = 20;                   // Number of steps in STDP test
    constexpr size_t MAX_TEST_STEPS = 10000;                 // Maximum steps in any test
    constexpr float BASIC_TEST_CURRENT = 50.0f;             // Current injection in basic test
    constexpr float PLASTICITY_TEST_CURRENT = 30.0f;         // Current injection in plasticity test
    constexpr float STDP_TEST_CURRENT = 60.0f;              // Current injection in STDP test
    constexpr float STDP_TRIALS = 20;                       // Number of STDP trials
    constexpr size_t MAX_TEST_NEURONS = 1000;               // Maximum neurons for testing
    constexpr size_t MAX_TEST_SYNAPSES = 10000;             // Maximum synapses for testing
    constexpr float PLOTTING_FREQUENCY = 0.1f;              // Frequency for plotting updates
    constexpr float METRICS_COLLECTION_INTERVAL = 10.0f;   // Interval for collecting metrics
    constexpr float BENCHMARK_DURATION = 100.0f;           // Duration for benchmarking
}

// Memory and Storage Constants
namespace Memory {
    constexpr size_t WORKING_MEMORY_CAPACITY_DEFAULT = 50;   // Default working memory capacity
    constexpr size_t WORKING_MEMORY_CAPACITY_MIN = 1;        // Minimum working memory capacity
    constexpr size_t WORKING_MEMORY_CAPACITY_MAX = 10000;    // Maximum working memory capacity
    constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;   // Maximum episodic memory episodes
    constexpr size_t EPISODIC_MEMORY_MAX_EPISODE_SIZE = 1000; // Maximum size of an episode
    constexpr size_t ASSOCIATIVE_MEMORY_MAX_PATTERNS = 1000; // Maximum associative memory patterns
    constexpr float EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f;  // Consolidation threshold for episodic memory
    constexpr float EPISODIC_CONSOLIDATION_MIN_ACTIVATION = 0.1f; // Minimum activation for consolidation
    constexpr size_t EPISODE_REPLAY_INTERVAL = 100;          // Steps between episode replay
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;         // Steps between episode storage
    constexpr size_t MEMORY_CONSOLIDATION_INTERVAL = 1000;  // Steps between consolidation
    constexpr float MEMORY_LEAK_RATE = 0.99f;               // Rate of memory decay
    constexpr float MEMORY_COMPETITION_FACTOR = 0.5f;       // Factor for memory competition
}

// Neuromodulation Constants
namespace Neuromodulation {
    constexpr float DOPAMINE_BASELINE = 0.0f;                // Baseline dopamine level
    constexpr float DOPAMINE_MAX_LEVEL = 1.0f;              // Maximum dopamine level
    constexpr float DOPAMINE_MIN_LEVEL = 0.0f;              // Minimum dopamine level
    constexpr float DOPAMINE_PLASTICITY_FACTOR_MAX = 2.0f; // Maximum plasticity modulation by dopamine
    constexpr float DOPAMINE_EXCITABILITY_FACTOR = 0.5f;   // Factor for dopamine effect on excitability
    constexpr float CURIOUSITY_BASELINE = 0.0f;             // Baseline curiosity level
    constexpr float CURIOUSY_MAX_LEVEL = 1.0f;             // Maximum curiosity level
    constexpr float NOVELTY_BASELINE = 0.0f;               // Baseline novelty level
    constexpr float NOVELTY_THRESHOLD = 0.1f;               // Threshold for novelty detection
    constexpr float NOVELTY_SENSITIVITY = 0.5f;             // Sensitivity to novelty
    constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;         // Weight for prediction error signal
    constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;        // Discount factor for future rewards
    constexpr float NOVELTY_DETECTION_SENSITIVITY = 0.2f;   // Sensitivity for novelty detection
    constexpr float EXPLORATION_EXCITATION = 0.3f;         // Excitation from exploration
    constexpr float EXPLOITATION_INHIBITION = 0.3f;       // Inhibition from exploitation
    constexpr float NEUROMODULATION_TRANSITION_RATE = 0.1f; // Rate of neuromodulation transitions
}

// Attention and Cognition Constants
namespace Cognition {
    constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;    // Strength of attentional inhibition
    constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;    // Strength of attentional excitation
    constexpr float ATTENTION_THRESHOLD = 0.1f;              // Threshold for attentional selection
    constexpr float PLANNER_PLANNING_DEPTH = 5;              // Default planning depth
    constexpr float PLANNER_EXPLORATION_FACTOR = 0.3f;       // Factor for planning exploration
    constexpr float PLANNER_EXPLOITATION_FACTOR = 0.7f;      // Factor for planning exploitation
    constexpr float CONCEPT_FORMATION_THRESHOLD = 0.5f;      // Threshold for concept formation
    constexpr float CONCEPT_FORMATION_SIMILARITY = 0.8f;     // Similarity threshold for concepts
    constexpr float CONCEPT_FORMATION_MAX_CONCEPTS = 100;   // Maximum number of concepts
    constexpr float ATTENTION_FOCUS_TIME = 10.0f;           // Time to maintain focus
    constexpr float ATTENTION_SWITCH_COST = 0.1f;           // Cost of switching attention
    constexpr float COGNITION_UPDATE_INTERVAL = 1.0f;       // Interval for cognition updates
    constexpr float COGNITION_NOISE_LEVEL = 0.01f;          // Noise level for cognitive processes
}

// Development Constants
namespace Development {
    constexpr float SYNAPTAGENESIS_RATE_DEFAULT = 0.001f;   // Default synaptogenesis rate
    constexpr float PRUNING_RATE_DEFAULT = 0.0001f;        // Default pruning rate
    constexpr float MATURATION_RATE_DEFAULT = 0.01f;       // Default maturation rate
    constexpr float DEVELOPMENT_RATE_MIN = 0.0f;           // Minimum development rate
    constexpr float DEVELOPMENT_RATE_MAX = 0.1f;           // Maximum development rate
    constexpr float CRITICAL_PERIOD_START = 10.0f;         // Start of critical period
    constexpr float CRITICAL_PERIOD_END = 50.0f;           // End of critical period
    constexpr float DEVELOPMENT_STAGE_INITIAL = 0.0f;     // Initial developmental stage
    constexpr float DEVELOPMENT_STAGE_MATURATION = 50.0f; // Maturation developmental stage
    constexpr float DEVELOPMENT_STAGE_ADULT = 100.0f;     // Adult developmental stage
    constexpr float DEVELOPMENT_EFFECT_ON_PLASTICITY = 0.5f; // Effect of development on plasticity
    constexpr float DEVELOPMENT_EFFECT_ON_FUNCTION = 0.3f; // Effect of development on function
}

// Visualization and Output Constants
namespace Visualization {
    constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;   // Default visualization setting
    constexpr float DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0f; // Default visualization update rate
    constexpr float VISUALIZATION_UPDATE_RATE_MIN = 1.0f;   // Minimum update rate
    constexpr float VISUALIZATION_UPDATE_RATE_MAX = 60.0f;  // Maximum update rate
    constexpr float DEFAULT_PLOT_HEIGHT = 400.0f;          // Default plot height
    constexpr float DEFAULT_PLOT_WIDTH = 600.0f;           // Default plot width
    constexpr float PLOT_POINT_SIZE = 2.0f;                // Size of plot points
    constexpr float PLOT_LINE_WIDTH = 1.0f;                // Width of plot lines
    constexpr float PLOT_AXIS_LABEL_PADDING = 10.0f;       // Padding for axis labels
    constexpr float PLOT_TITLE_PADDING = 20.0f;            // Padding for plot titles
    constexpr float VISUALIZATION_COLOR_MAP_MIN = 0.0f;    // Minimum visualization color
    constexpr float VISUALIZATION_COLOR_MAP_MAX = 1.0f;    // Maximum visualization color
}

// Checkpoint and Serialization Constants
namespace Checkpoint {
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints"; // Default checkpoint directory
    constexpr float CHECKPOINT_INTERVAL = 100.0f;           // Interval for checkpoint saving
    constexpr float CHECKPOINT_MAX_SIZE = 10000.0f;         // Maximum checkpoint file size (KB)
    constexpr float CHECKPOINT_COMPRESSION_LEVEL = 5.0f;   // Default compression level
    constexpr bool CHECKPOINT_COMPRESS_BY_DEFAULT = true;  // Compress checkpoints by default
    constexpr float CHECKPOINT_METADATA_VERSION = 1.0f;   // Checkpoint metadata version
    constexpr size_t CHECKPOINT_MAX_VERSIONS = 10;          // Maximum checkpoint versions to keep
    constexpr float CHECKPOINT_BACKUP_INTERVAL = 1000.0f;  // Interval for creating backups
}

// Random and Stochastic Constants
namespace Random {
    constexpr int64_t DEFAULT_RANDOM_SEED = 42;             // Default random seed
    constexpr int64_t RANDOM_SEED_MIN = 0;                 // Minimum random seed
    constexpr int64_t RANDOM_SEED_MAX = 2147483647;         // Maximum random seed
    constexpr float RANDOM_FLOAT_MIN = 0.0f;               // Minimum random float
    constexpr float RANDOM_FLOAT_MAX = 1.0f;               // Maximum random float
    constexpr float RANDOM_NORMAL_MEAN = 0.0f;             // Mean for normal distribution
    constexpr float RANDOM_NORMAL_STDDEV = 1.0f;           // Standard deviation for normal distribution
    constexpr float RANDOM_GAUSSIAN_MIN = -3.0f;           // Minimum for Gaussian distribution
    constexpr float RANDOM_GAUSSIAN_MAX = 3.0f;            // Maximum for Gaussian distribution
    constexpr float RANDOM_EXPONENTIAL_LAMBDA = 1.0f;      // Lambda for exponential distribution
    constexpr float RANDOM_UNIFORM_MIN = 0.0f;             // Minimum for uniform distribution
    constexpr float RANDOM_UNIFORM_MAX = 1.0f;             // Maximum for uniform distribution
}

// Performance and Optimization Constants
namespace Performance {
    constexpr size_t SIMD_VECTOR_SIZE = 4;                 // SIMD vector size (float)
    constexpr size_t CACHE_LINE_SIZE = 64;                 // Cache line size in bytes
    constexpr size_t THREAD_POOL_SIZE = 4;                 // Default thread pool size
    constexpr float PERFORMANCE_TARGET_FPS = 60.0f;      // Target frames per second
    constexpr float PERFORMANCE_TARGET_UPDATE_TIME = 0.016f; // Target update time in seconds
    constexpr float PERFORMANCE_MEMORY_POOL_SIZE = 1024;  // Size of memory pool
    constexpr float PERFORMANCE_EVENT_QUEUE_SIZE = 10000; // Size of event queue
    constexpr bool PERFORMANCE_OPTIMIZE_FOR_SPEED = true; // Optimize for speed vs memory tradeoff
    constexpr bool PERFORMANCE_OPTIMIZE_FOR_QUALITY = false; // Optimize for quality vs speed tradeoff
    constexpr float PERFORMANCE_MEMORY_ALIGNMENT = 16.0f;  // Memory alignment in bytes
    constexpr size_t PERFORMANCE_MAX_THREADS = 8;         // Maximum number of threads
    constexpr float PERFORMANCE_CPU_SATURATION_THRESHOLD = 0.8f; // CPU saturation threshold
    constexpr float PERFORMANCE_MEMORY_BANDWIDTH_THRESHOLD = 0.9f; // Memory bandwidth threshold
    constexpr size_t PERFORMANCE_L1_CACHE_SIZE = 32768;    // L1 cache size in bytes
    constexpr size_t PERFORMANCE_L2_CACHE_SIZE = 262144;  // L2 cache size in bytes
    constexpr size_t PERFORMANCE_L3_CACHE_SIZE = 8388608; // L3 cache size in bytes
}

// Error and Exception Constants
namespace Error {
    constexpr int ERROR_SUCCESS = 0;                       // Success code
    constexpr int ERROR_GENERIC = 1;                       // Generic error
    constexpr int ERROR_INVALID_ARGUMENT = 2;             // Invalid argument
    constexpr int ERROR_FILE_NOT_FOUND = 3;                // File not found
    constexpr int ERROR_MEMORY_ALLOCATION = 4;             // Memory allocation error
    constexpr int ERROR_INVALID_CONFIGURATION = 5;        // Invalid configuration
    constexpr int ERROR_SIMULATION_FAILED = 6;             // Simulation failed
    constexpr int ERROR_VALIDATION_FAILED = 7;             // Validation failed
    constexpr int ERROR_INITIALIZATION_FAILED = 8;        // Initialization failed
    constexpr int ERROR_LOAD_FAILED = 9;                   // Load failed
    constexpr int ERROR_SAVE_FAILED = 10;                  // Save failed
    constexpr int ERROR_SERIALIZATION = 11;                // Serialization error
    constexpr int ERROR_DESERIALIZATION = 12;              // Deserialization error
    constexpr int ERROR_CORRUPTION = 13;                   // Data corruption
    constexpr int ERROR_TIMEOUT = 14;                      // Timeout error
    constexpr int ERROR_PERMISSION_DENIED = 15;            // Permission denied
    constexpr int ERROR_OUT_OF_MEMORY = 16;                // Out of memory error
    constexpr int ERROR_FILE_ACCESS = 17;                  // File access error
    constexpr int ERROR_NETWORK = 18;                      // Network error
    constexpr int ERROR_HARDWARE = 19;                     // Hardware error
    constexpr int ERROR_SYSTEM = 20;                       // System error
    constexpr int MAX_ERROR_CODE = 100;                    // Maximum error code
    constexpr size_t ERROR_MESSAGE_MAX_LENGTH = 1024;     // Maximum error message length
    constexpr float ERROR_RETRY_DELAY = 1.0f;              // Delay before retry
    constexpr size_t ERROR_MAX_RETRIES = 3;               // Maximum number of retries
    constexpr float ERROR_CONSECUTIVE_FAILURES_THRESHOLD = 5.0f; // Threshold for consecutive failures
}

// Logging Constants
namespace Logging {
    constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log"; // Default log filename
    constexpr size_t DEFAULT_LOG_MAX_SIZE = 10485760;     // Default maximum log size (10MB)
    constexpr size_t LOG_MESSAGE_MAX_LENGTH = 2048;      // Maximum log message length
    constexpr float LOG_ROTATION_SIZE = 1048576.0f;       // Log rotation size (1MB)
    constexpr size_t LOG_ROTATION_COUNT = 5;              // Number of log rotations to keep
    constexpr const char* LOG_LEVEL_NAMES[] = {"OFF", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"}; // Log level names
    constexpr int LOG_LEVEL_OFF = 0;                      // Off log level
    constexpr int LOG_LEVEL_ERROR = 1;                    // Error log level
    constexpr int LOG_LEVEL_WARN = 2;                     // Warning log level
    constexpr int LOG_LEVEL_INFO = 3;                     // Info log level
    constexpr int LOG_LEVEL_DEBUG = 4;                    // Debug log level
    constexpr int LOG_LEVEL_TRACE = 5;                    // Trace log level
    constexpr const char* LOG_TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S"; // Log timestamp format
    constexpr bool LOG_INCLUDE_TIMESTAMP = true;          // Include timestamp in log messages
    constexpr bool LOG_INCLUDE_THREAD_ID = true;          // Include thread ID in log messages
    constexpr bool LOG_INCLUDE_FILE_INFO = true;          // Include file and line info in log messages
    constexpr size_t LOG_BUFFER_SIZE = 8192;              // Log buffer size
    constexpr float LOG_FLUSH_INTERVAL = 1.0f;            // Log flush interval in seconds
    constexpr bool LOG_FLUSH_IMMEDIATELY = false;         // Flush log immediately
    constexpr const char* LOG_COLOR_CODES[] = {"\033[0m", "\033[31m", "\033[33m", "\033[32m", "\033[36m", "\033[37m"}; // Log color codes
}

// Unit Conversion Constants
namespace Units {
    constexpr float MILLISECONDS_TO_SECONDS = 0.001f;     // Milliseconds to seconds
    constexpr float SECONDS_TO_MILLISECONDS = 1000.0f;    // Seconds to milliseconds
    constexpr float MICROSECONDS_TO_SECONDS = 1e-6f;      // Microseconds to seconds
    constexpr float SECONDS_TO_MICROSECONDS = 1e6f;       // Seconds to microseconds
    constexpr float NANoseconds_TO_SECONDS = 1e-9f;       // Nanoseconds to seconds
    constexpr float SECONDS_TO_NANOSECONDS = 1e9f;        // Seconds to nanoseconds
    constexpr float VOLTS_TO_MILLIVOLTS = 1000.0f;        // Volts to millivolts
    constexpr float MILLIVOLTS_TO_VOLTS = 0.001f;         // Millivolts to volts
    constexpr float HERTZ_TO_RADIANS_PER_SECOND = 2.0f * 3.141592653589793f; // Hz to rad/s
    constexpr float RADIANS_PER_SECOND_TO_HERTZ = 1.0f / (2.0f * 3.141592653589793f); // rad/s to Hz
    constexpr float NEWTONS_TO_UNIT_FORCE = 1.0f;          // Newtons to unit force
    constexpr float UNIT_FORCE_TO_NEWTONS = 1.0f;         // Unit force to Newtons
    constexpr float COUNTS_PER_SECOND_TO_HERTZ = 1.0f;   // Counts per second to Hz
    constexpr float KIBIBYTES_TO_BYTES = 1024.0f;         // Kibibytes to bytes
    constexpr float MEBIBYTES_TO_BYTES = 1048576.0f;      // Mebibytes to bytes
    constexpr float GIBIBYTES_TO_BYTES = 1073741824.0f;   // Gibibytes to bytes
    constexpr float BYTES_TO_KIBIBYTES = 1.0f / 1024.0f; // Bytes to kibibytes
    constexpr float BYTES_TO_MEBIBYTES = 1.0f / 1048576.0f; // Bytes to mebibytes
    constexpr float BYTES_TO_GIBIBYTES = 1.0f / 1073741824.0f; // Bytes to gibibytes
}

} // namespace nlm
