// NLM Constants and Magic Numbers
// Provides named constants for commonly used values to improve code readability
// and maintainability.

#pragma once

namespace nlm {
namespace constants {

// Time and Simulation Constants
constexpr double DEFAULT_TIMESTEP = 0.001;           // Default simulation timestep (s)
constexpr double EPISODE_TIMESTEP = 0.01;            // Episode timestep for world updates (s)
constexpr double LONG_TERM_DEVELOPMENT_STEP = 0.1;   // Development update step

// Neural Network Size Constants
constexpr int DEFAULT_NEURON_COUNT = 500;             // Default number of neurons for basic tests
constexpr int MIN_NEURON_COUNT = 10;                  // Minimum viable neuron count
constexpr int MAX_NEURON_COUNT = 10000;               // Maximum neuron count for simulations
constexpr int DEFAULT_REGION_COUNT = 1;               // Default number of brain regions
constexpr int DEFAULT_EPISODE_STEPS = 200;            // Default steps per episode
constexpr int MIN_EPISODE_STEPS = 10;                 // Minimum episode steps
constexpr int MAX_EPISODE_STEPS = 10000;              // Maximum episode steps

// Activity and Counting Constants
constexpr int ACTIVITY_THRESHOLD = 50;               // Current injection for neuron activation
constexpr int CONNECTION_TEST_CURRENT = 50;          // Current used in connectivity tests
constexpr int LTP_TEST_CURRENT = 60;                 // Current used in STDP LTP tests
constexpr int DEFAULT_SPIKE_COUNT_HISTORY = 1000;     // Default history size for spike tracking
constexpr int DEFAULT_NUM_ACTIVE_NEIGHBORS = 10;     // Number of active neighbors to track
constexpr float EPSILON_FLOAT = 0.001f;              // Small value for float comparisons
constexpr double EPSILON_DOUBLE = 1e-6;              // Small value for double comparisons

// Plasticity Constants
constexpr float DEFAULT_HEBBIAN_RATE = 0.01f;        // Default Hebbian learning rate
constexpr float DEFAULT_STDP_LTP = 0.02f;            // Default STDP LTP weight
constexpr float DEFAULT_STDP_LTD = 0.015f;           // Default STDP LTD weight
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;  // STDP time constant
constexpr float SYNAPTOGENESIS_RATE = 0.0001f;       // Structural plasticity rate
constexpr float PRUNING_RATE = 0.00001f;             // Structural pruning rate

// Weight and Threshold Constants
constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;     // Threshold for significant weight change
constexpr float WEIGHT_CHANGE_SMALL = 0.001f;        // Threshold for small weight change
constexpr float ACTIVATION_THRESHOLD = 10.0f;        // Neuron activation threshold
constexpr float DEFAULT_INJECTION_CURRENT = 30.0f;   // Default current injection for learning
constexpr float STRONG_INJECTION_CURRENT = 50.0f;   // Strong current for stimulation
constexpr float HIGH_NOISE_LEVEL = 5.0f;              // High noise level for exploration

// Memory and Learning Constants
constexpr size_t MEMORY_PATTERN_SIZE = 100;          // Default pattern size for memory
constexpr float MEMORY_ASSOCIATION_THRESHOLD = 0.5f; // Threshold for memory association
constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;   // Threshold for prediction error detection
constexpr float NOVELTY_THRESHOLD = 0.3f;             // Threshold for novelty detection
constexpr float CURIOSITY_THRESHOLD = 0.2f;           // Threshold for curiosity activation

// Neuromodulation Constants
constexpr float DOPAMINE_BASELINE = 0.0f;            // Baseline dopamine level
constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;     // Reward discount factor for RL
constexpr float NEUROMODULATION_MAX_LEVEL = 1.0f;    // Maximum neuromodulation level

// World and Environment Constants
constexpr float DEFAULT_WORLD_SIZE = 20.0f;          // Default world dimension
constexpr float DEFAULT_VISION_RANGE = 0.5f;         // Default vision range
constexpr float ENERGY_DECAY_RATE = 0.01f;           // Energy decay per timestep
constexpr float MAX_ENERGY = 100.0f;                  // Maximum energy capacity
constexpr float HEALTH_DECAY_RATE = 0.001f;          // Health decay rate

// Logging and Debug Constants
constexpr int LOG_HISTORY_LIMIT = 1000;              // Maximum log history entries
constexpr int METRICS_UPDATE_INTERVAL = 100;         // Interval for metrics updates
constexpr int PROGRESS_REPORT_INTERVAL = 1000;      // Progress reporting interval
constexpr int VERIFICATION_TRIALS = 50;              // Number of trials for verification tests

// Configuration Constants
constexpr int MAX_CONFIG_ITEMS = 100;                // Maximum number of config entries
constexpr int CONFIG_VALUE_BUFFER_SIZE = 256;        // Buffer size for config values
constexpr char CONFIG_FILE_EXTENSION[] = ".cfg";     // Default config file extension
constexpr int DEFAULT_SEED = 42;                     // Default random seed

// Logging verbosity levels (if needed)
constexpr int LOG_LEVEL_TRACE = 0;                   // Detailed tracing
constexpr int LOG_LEVEL_DEBUG = 1;                   // Debug information
constexpr int LOG_LEVEL_INFO = 2;                    // Information messages
constexpr int LOG_LEVEL_WARNING = 3;                 // Warning messages
constexpr int LOG_LEVEL_ERROR = 4;                   // Error messages

// Development Stages
constexpr int DEVELOPMENT_INITIAL = 0;              // Initial development stage
constexpr int DEVELOPMENT_CRITICAL = 1;              // Critical period
constexpr int DEVELOPMENT_MATURATION = 2;            // Maturation stage
constexpr int DEVELOPMENT_ADULT = 3;                 // Adult stage
constexpr int DEVELOPMENT_AGING = 4;                 // Aging stage

// Action and Motor Command Constants
constexpr int MOTOR_COMMAND_IDLE_STEPS = 10;         // Steps for idle motor commands
constexpr float ACTION_ACCURACY = 0.95f;             // Expected action accuracy
constexpr float ACTION_DURATION = 0.1f;              // Standard action duration

// Experiment and Episode Constants
constexpr int MAX_EPISODES = 1000;                   // Maximum number of episodes
constexpr int MAX_EXPERIMENT_RUNS = 100;              // Maximum experiment runs
constexpr float EXPERIMENT_TIMEOUT = 300.0f;         // Experiment timeout in seconds

// Visualization Constants
constexpr float VISUALIZATION_SCALE = 1.0f;          // Visualization scaling factor
constexpr int VISUALIZATION_UPDATE_RATE = 30;        // Frames per second for visualization
constexpr float MIN_VISUALIZATION_UPDATE = 0.001f;   // Minimum visualization update interval

// Performance and Optimization Constants
constexpr int OPTIMAL_THREAD_COUNT = 4;              // Optimal number of threads
constexpr size_t MEMORY_POOL_SIZE = 1024;            // Size of memory pools
constexpr double SIMD_ALIGNMENT = 32.0;              // SIMD alignment requirement

// Error and Tolerance Constants
constexpr float FLOAT_TOLERANCE = 1e-5f;             // Float comparison tolerance
constexpr double DOUBLE_TOLERANCE = 1e-8;           // Double comparison tolerance
constexpr int MAX_RETRIES = 3;                       // Maximum retry attempts
constexpr float RECOVERY_THRESHOLD = 0.01f;          // Recovery threshold

// Numerical Constants
constexpr float PI = 3.141592653589793f;              // Pi
constexpr float HALF_PI = 1.5707963267948966f;       // Pi/2
constexpr float SQRT_2 = 1.414213562373095f;         // Square root of 2
constexpr float E = 2.718281828459045f;               // Euler's number

// Version and Build Constants
constexpr int VERSION_MAJOR = 0;                     // Major version
constexpr int VERSION_MINOR = 1;                     // Minor version
constexpr int VERSION_PATCH = 0;                     // Patch version
constexpr char BUILD_DATE[] = __DATE__;               // Build date
constexpr char BUILD_TIME[] = __TIME__;               // Build time

} // namespace constants
} // namespace nlm
