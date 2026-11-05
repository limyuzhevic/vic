// nlm/constants.hpp - Centralized constants for all magic numbers and configuration values
#pragma once

namespace nlm {
namespace constants {

// Simulation and timing
const double DEFAULT_TIMESTEP = 0.001;      // Default simulation timestep (1 ms)
const double BRAIN_STEP_INTERVAL = 0.001;   // Brain simulation step interval
const double WORLD_UPDATE_INTERVAL = 0.1;   // World update interval (100 ms per action)
const double DEFAULT_SIMULATION_DURATION = 1000.0;  // Default simulation time (1 second)

// Brain configuration
const size_t DEFAULT_NEURON_COUNT = 500;              // Default number of neurons
const size_t MIN_NEURON_COUNT = 10;                   // Minimum neurons for meaningful computation
const size_t MAX_NEURON_COUNT = 10000;                // Maximum neurons for memory/performance constraints
const size_t DEFAULT_REGION_COUNT = 1;                // Default number of neural regions
const float DEFAULT_CONNECTION_PROBABILITY = 0.15f;   // Probability of creating synapse between neurons
const float MIN_CONNECTION_PROBABILITY = 0.01f;       // Minimum connection probability
const float MAX_CONNECTION_PROBABILITY = 1.0f;        // Maximum connection probability

// Neuron parameters
const float DEFAULT_THRESHOLD = 1.0f;                // Default membrane potential threshold for spiking
const float DEFAULT_REST_POTENTIAL = 0.0f;           // Default resting membrane potential
const float DEFAULT_MEMBRANE_RESISTANCE = 1.0f;       // Default membrane resistance
const float DEFAULT_CAPACITANCE = 1.0f;               // Default membrane capacitance
const float DEFAULT_DECAY_RATE = 0.04f;              // Default membrane decay rate
const float LIF_MIN_THRESHOLD = 0.1f;                // Minimum viable threshold
const float LIF_MAX_THRESHOLD = 10.0f;               // Maximum threshold before instability

// Synaptic parameters
const float DEFAULT_SYNAPSE_WEIGHT = 0.5f;           // Default synaptic weight
const float MIN_SYNAPSE_WEIGHT = 0.0f;               // Minimum synaptic weight
const float MAX_SYNAPSE_WEIGHT = 1.0f;               // Maximum synaptic weight
const float DEFAULT_SYNAPSE_DELAY = 1.0f;             // Default synaptic transmission delay (ms)
const float MIN_SYNAPSE_DELAY = 0.0f;                // Minimum synaptic delay
const float MAX_SYNAPSE_DELAY = 100.0f;              // Maximum synaptic delay

// Input current parameters
const float DEFAULT_INPUT_INJECTION = 30.0f;          // Default current injection for testing
const float DEFAULT_SENSORY_INPUT_MAX = 60.0f;        // Maximum sensory input current
const float MIN_INPUT_INJECTION = 0.0f;              // Minimum input current
const float MAX_INPUT_INJECTION = 200.0f;            // Maximum input current (prevent seizure)

// Plasticity parameters
const float DEFAULT_STDP_LTP_WEIGHT = 0.02f;         // STDP long-term potentiation weight
const float DEFAULT_STDP_LTD_WEIGHT = 0.015f;        // STDP long-term depression weight
const float MIN_STDP_WEIGHT = 0.0001f;              // Minimum STDP weight change
const float MAX_STDP_WEIGHT = 0.1f;                  // Maximum STDP weight change
const double DEFAULT_STDP_TIME_CONST = 20.0f;         // STDP time constant (ms)
const double MIN_STDP_TIME_CONST = 1.0f;             // Minimum STDP time constant
const double MAX_STDP_TIME_CONST = 1000.0f;         // Maximum STDP time constant

const float DEFAULT_HBP_WEIGHT = 0.01f;              // Hebbian plasticity weight
const float MIN_HBP_WEIGHT = 0.0001f;               // Minimum Hebbian weight change
const float MAX_HBP_WEIGHT = 0.05f;                  // Maximum Hebbian weight change

const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;   // Structural plasticity - synapse creation rate
const float DEFAULT_PRUNING_RATE = 0.00001f;         // Structural plasticity - synapse removal rate
const float MIN_PLASTICITY_RATE = 0.000001f;         // Minimum plasticity rate
const float MAX_PLASTICITY_RATE = 0.01f;             // Maximum plasticity rate

// Neuromodulation parameters
const float DEFAULT_DOPAMINE_EFFECT = 1.0f;          // Default dopamine effect on plasticity
const float DEFAULT_NOVITY_THRESHOLD = 0.1f;         // Threshold for novelty detection
const float DEFAULT_CURiosity_BOOST = 2.0f;          // Curiosity modulation boost factor
const float MIN_MODULATION_LEVEL = 0.0f;             // Minimum neuromodulation level
const float MAX_MODULATION_LEVEL = 5.0f;             // Maximum neuromodulation level

// Memory system parameters
const size_t DEFAULT_WORKING_MEMORY_SIZE = 100;      // Maximum working memory traces
const size_t MIN_WORKING_MEMORY_SIZE = 10;           // Minimum working memory size
const size_t MAX_WORKING_MEMORY_SIZE = 1000;         // Maximum working memory size
const float DEFAULT_MEMORY_DECAY_RATE = 0.1f;        // Working memory trace decay rate
const size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000; // Maximum episodic episodes
const float DEFAULT_EPISODIC_REPLAY_PROB = 0.1f;     // Probability of episodic memory replay

// Development system parameters
const float DEFAULT_CRITICAL_PERIOD = 100.0f;         // Length of critical period (steps)
const float DEFAULT_MATURATION_RATE = 0.001f;        // Developmental maturation rate
const float DEFAULT_PLASTICITY_REDUCTION = 0.1f;    // Plasticity reduction during development
const float MIN_DEVELOPMENT_RATE = 0.0001f;          // Minimum development rate
const float MAX_DEVELOPMENT_RATE = 0.1f;             // Maximum development rate

// Cognitive and planning parameters
const float DEFAULT_PLANNING_HORIZON = 10;           // Default planning horizon (steps)
const float MIN_PLANNING_HORIZON = 1;               // Minimum planning horizon
const float MAX_PLANNING_HORIZON = 100;              // Maximum planning horizon
const float DEFAULT_ATTENTION_SELECTIVITY = 0.5f;    // Attention selectivity threshold
const float MIN_ATTENTION_SELECTIVITY = 0.1f;        // Minimum attention selectivity
const float MAX_ATTENTION_SELECTIVITY = 1.0f;        // Maximum attention selectivity

// World and environment parameters
const float DEFAULT_ENVIRONMENT_WIDTH = 20.0f;        // Default environment width (units)
const float DEFAULT_ENVIRONMENT_HEIGHT = 20.0f;       // Default environment height (units)
const float DEFAULT_AGENT_SPEED = 3.0f;              // Default agent movement speed
const float DEFAULT_TURN_SPEED = 2.0f;               // Default agent turn speed
const float DEFAULT_AGENT_ENERGY_MAX = 100.0f;        // Maximum agent energy
const float DEFAULT_ENERGY_DECAY_RATE = 0.01f;        // Energy decay rate per step
const float MIN_ENERGY = 0.0f;                       // Minimum energy before death

// Reward and motivation parameters
const float DEFAULT_REWARD_RANGE = 10.0f;             // Typical reward magnitude range
const float DEFAULT_REWARD_THRESHOLD = 0.0f;         // Reward threshold for motivation
const float MIN_REWARD = -10.0f;                     // Minimum possible reward
const float MAX_REWARD = 10.0f;                      // Maximum possible reward

// Observation and sensory parameters
const size_t DEFAULT_VISION_WIDTH = 16;              // Default vision field width
const size_t DEFAULT_VISION_HEIGHT = 16;             // Default vision field height
const size_t DEFAULT_TOUCH_POINTS = 8;               // Default touch sensor points
const float DEFAULT_SIGHT_RANGE = 10.0f;             // Default visual detection range
const float DEFAULT_SIGHT_ANGLE = 90.0f;             // Default field of view angle (degrees)

// Logging and debugging parameters
const size_t MAX_LOG_MESSAGE_LENGTH = 2048;          // Maximum log message length
const size_t MAX_CONSOLE_LOG_LINES = 1000;           // Maximum console log lines
const float LOG_FLUSH_INTERVAL = 0.1f;               // Log flush interval (seconds)

// Experiment and testing parameters
const size_t DEFAULT_EXPERIMENT_TRIALS = 50;         // Default number of experiment trials
const size_t MIN_EXPERIMENT_TRIALS = 1;              // Minimum trials
const size_t MAX_EXPERIMENT_TRIALS = 10000;          // Maximum trials
const float DEFAULT_EXPERIMENT_TIMEOUT = 300.0f;     // Experiment timeout (seconds)
const float DEFAULT_BENCHMARK_DURATION = 60.0f;      // Benchmark duration (seconds)

// Performance and resource parameters
const size_t MIN_SPINE_HISTORY = 0;                  // Minimum spike history per synapse
const size_t MAX_SPINE_HISTORY = 1000;               // Maximum spike history per synapse
const size_t DEFAULT_POOL_SIZE = 1024;               // Default memory pool size
const size_t MIN_POOL_SIZE = 64;                      // Minimum pool size
const size_t MAX_POOL_SIZE = 65536;                   // Maximum pool size

// Randomness and seed parameters
const int64_t DEFAULT_RANDOM_SEED = 42;              // Default random seed
const uint32_t MAX_RANDOM_UINT = 1000000;            // Maximum random unsigned int
const uint32_t MIN_RANDOM_UINT = 0;                  // Minimum random unsigned int

// Numerical precision parameters
const float DEFAULT_FLOAT_PRECISION = 0.001f;        // Default floating point precision
const double DEFAULT_DOUBLE_PRECISION = 0.0001;      // Default double precision
const float EPSILON = 1e-6f;                          // Machine epsilon for comparisons
const float FLOAT_EQ_TOLERANCE = 0.001f;             // Float equality tolerance
const double DOUBLE_EQ_TOLERANCE = 0.000001;         // Double equality tolerance

// System limits
const size_t MAX_COHORT_SIZE = 100;                  // Maximum concurrent brain instances
const double MAX_WALL_CLOCK_TIME = 3600.0;            // Maximum runtime (1 hour)
const size_t MAX_FILE_SIZE = 100 * 1024 * 1024;       // Maximum config file size (100 MB)
const size_t MAX_CONFIG_ENTRIES = 10000;             // Maximum configuration entries

// Quality of life constants
const char COMMENT_MARKER = '#';                     // Configuration comment marker
const char KEY_VALUE_SEPARATOR = '=';                // Configuration key-value separator
const char WHITESPACE[] = " \t\n\r";                 // Whitespace characters for parsing

// Success and failure indicators
const int SUCCESS = 0;                               // Success indicator
const int FAILURE = 1;                               // Failure indicator
const int ERROR_EXIT_CODE = 255;                      // Error exit code

}  // namespace constants
}  // namespace nlm
