#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

#include <cstdint>

namespace nlm {
namespace Constants {

// Simulation Constants
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;  // seconds
constexpr double DEFAULT_SIMULATION_DURATION = 1000.0;  // seconds
constexpr size_t DEFAULT_MAX_STEPS = 10000;
constexpr double DEFAULT_WORLD_TIME_STEP = 0.001;  // seconds

// Brain Structure Constants
constexpr size_t DEFAULT_NEURON_COUNT = 500;
constexpr size_t DEFAULT_REGION_COUNT = 1;
constexpr size_t DEFAULT_POPULATION_PER_REGION = 100;
constexpr size_t DEFAULT_MAX_SYNAPSES_PER_NEURON = 100;
constexpr size_t DEFAULT_MAX_CONCURRENT_SPIKES = 1000;
constexpr size_t MAX_SPIKE_HISTORY = 100;  // Length of spike history per neuron

// Connectivity Constants
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
constexpr size_t MIN_SYNAPSE_CONNECTIONS = 1;
constexpr size_t MAX_SYNAPSE_CONNECTIONS = 100;
constexpr float SYNAPSE_WEIGHT_RANGE_MIN = -1.0f;
constexpr float SYNAPSE_WEIGHT_RANGE_MAX = 1.0f;
constexpr float DEFAULT_INITIAL_WEIGHT = 0.5f;

// Neuron Dynamics Constants
constexpr float LIF_MEMBRANE_CAPACITANCE = 1.0f;  // nF
constexpr float LIF_TIME_CONSTANT = 20.0f;  // ms
constexpr float LIF_RESTING_POTENTIAL = -70.0f;  // mV
constexpr float LIF_THRESHOLD_POTENTIAL = -55.0f;  // mV
constexpr float LIF_RESET_POTENTIAL = -70.0f;  // mV
constexpr float LIF_LEAK_CONDUCTANCE = 0.1f;  // 1/ms
constexpr float DEFAULT_CURRENT_INJECTION = 50.0f;  // arbitrary units
constexpr float MIN_CURRENT_INJECTION = -20.0f;  // arbitrary units
constexpr float MAX_CURRENT_INJECTION = 200.0f;  // arbitrary units

// Plasticity Constants
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;  // ms
constexpr float DEFAULT_STDP_WINDOW = 50.0f;  // ms
constexpr float DEFAULT_HBOBBIAN_LEARNING_RATE = 0.01f;
constexpr float DEFAULT_HBOBBIAN_THRESHOLD = 0.0f;
constexpr float MIN_SYNAPSE_WEIGHT = 0.0f;
constexpr float MAX_SYNAPSE_WEIGHT = 1.0f;
constexpr float WEIGHT_PERTURBATION_THRESHOLD = 0.01f;

// Structural Plasticity Constants
constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
constexpr float DEFAULT_MATURATION_RATE = 0.0005f;
constexpr size_t DEFAULT_MAX_SYNAPSES_PER_NEURON = 200;
constexpr float SYNAPSE_WEIGHT_THRESHOLD_FOR_PRUNING = 0.1f;

// Memory Constants
constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 100;
constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000;
constexpr float WORKING_MEMORY_DECAY_RATE = 0.95f;
constexpr float EPISODIC_MEMORY_DECAY_RATE = 0.99f;
constexpr size_t EPISODIC_REPLAY_INTERVAL = 100;  // steps
constexpr size_t MEMORY_CONSOLIDATION_INTERVAL = 1000;  // steps
constexpr size_t MAX_EPISODIC_REPLAYS = 10;

// Neuromodulation Constants
constexpr float DEFAULT_DOPAMINE_PEAK = 1.0f;
constexpr float DEFAULT_DOPAMINE_DECAY = 0.9f;
constexpr float DEFAULT_CURIOUSITY_LEVEL = 0.5f;
constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.1f;
constexpr float DEFAULT_NOVELTY_DECAY = 0.95f;
constexpr float DEFAULT_REWARD_SCALE = 1.0f;
constexpr float DEFAULT_REWARD_DECAY = 0.9f;
constexpr float DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.5f;
constexpr float DEFAULT_PREDICTION_ERROR_DECAY = 0.9f;

// Cognitive Constants
constexpr size_t DEFAULT_PLANNER_HORIZON = 10;
constexpr float DEFAULT_PLANNING_COST = 0.1f;
constexpr float DEFAULT_CONCEPT_FORMATION_THRESHOLD = 0.7f;
constexpr float DEFAULT_CONCEPT_FORMATION_LEARNING_RATE = 0.05f;
constexpr size_t DEFAULT_ATTENTION_WINDOW_SIZE = 5;
constexpr float DEFAULT_ATTENTION_FOCUS_GAIN = 2.0f;
constexpr float DEFAULT_ATTENTION_DISTRACTION_RESISTANCE = 0.5f;

// Development Constants
constexpr float DEFAULT_DEVELOPMENT_STEP_SIZE = 0.001f;
constexpr float DEFAULT_DEVELOPMENT_PLATENCY_FACTOR = 1.5f;
constexpr float DEFAULT_DEVELOPMENT_PLASTICITY_DECREASE = 0.8f;
constexpr float DEFAULT_DEVELOPMENT_MATURATION_ACCELERATION = 1.2f;
constexpr float DEVELOPMENT_STAGE_DURATION_FACTOR = 2.0f;

// Motor Constants
constexpr float DEFAULT_ACTION_DURATION = 0.1f;
constexpr size_t MAX_ACTION_QUEUE_SIZE = 10;
constexpr float MOTOR_COMMAND_STRENGTH_SCALE = 1.0f;
constexpr float MOTOR_COMMAND_DECAY = 0.9f;

// Sensory Constants
constexpr size_t DEFAULT_VISION_WIDTH = 16;
constexpr size_t DEFAULT_VISION_HEIGHT = 16;
constexpr float DEFAULT_AUDIO_SAMPLE_RATE = 1000.0f;  // Hz
constexpr size_t DEFAULT_AUDIO_WINDOW_SIZE = 100;
constexpr float DEFAULT_PROPRIOCEPTION_RANGE = 1.0f;
constexpr size_t DEFAULT_INTERNAL_SIGNAL_HISTORY = 100;

// Reward and Evaluation Constants
constexpr float DEFAULT_REWARD_RANGE_MIN = -1.0f;
constexpr float DEFAULT_REWARD_RANGE_MAX = 1.0f;
constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.9f;
constexpr float DEFAULT_PUNISHMENT_STRENGTH = 1.0f;
constexpr float DEFAULT_REWARD_DELAY = 0.1f;
constexpr float DEFAULT_SURPRISE_THRESHOLD = 0.5f;

// Experiment Constants
constexpr size_t DEFAULT_EXPERIMENT_NUM_TRIALS = 100;
constexpr float DEFAULT_EXPERIMENT_REWARD_THRESHOLD = 0.5f;
constexpr size_t DEFAULT_EXPERIMENT_MAX_STEPS_PER_TRIAL = 1000;
constexpr size_t DEFAULT_EXPERIMENT_EPISODE_LIMIT = 100;
constexpr float DEFAULT_EXPERIMENT_NOVELTY_THRESHOLD = 0.1f;

// Checkpoint Constants
constexpr const char* DEFAULT_CHECKPOINT_PATH = "/tmp/nlm_checkpoint.bin";
constexpr const char* DEFAULT_CHECKPOINT_DIR = "/tmp";
constexpr float CHECKPOINT_COMPRESSION_LEVEL = 6;
constexpr size_t MAX_CHECKPOINT_HISTORY = 10;

// Visualization Constants
constexpr size_t DEFAULT_VISUALIZATION_WIDTH = 800;
constexpr size_t DEFAULT_VISUALIZATION_HEIGHT = 600;
constexpr float DEFAULT_VISUALIZATION_REFRESH_RATE = 30.0f;  // Hz
constexpr float VISUALIZATION_ALPHA_BLEND = 0.7f;

// Logging Constants
constexpr const char* DEFAULT_LOG_FILE = "nlm.log";
constexpr size_t MAX_LOG_FILE_SIZE = 10 * 1024 * 1024;  // 10MB
constexpr size_t MAX_LOG_ENTRIES = 1000000;
constexpr const char* LOG_TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";

// Performance Constants
constexpr size_t SIMD_VECTOR_SIZE = 8;  // elements for NEON/SSE
constexpr size_t PERFORMANCE_COUNTER_RESOLUTION = 1000;  // Hz
constexpr float DEFAULT_PERFORMANCE_TARGET_FPS = 30.0f;
constexpr size_t PERFORMANCE_STATS_BUFFER_SIZE = 100;

// Configuration Constants
constexpr size_t MAX_CONFIG_VALUE_LENGTH = 1024;
constexpr size_t MAX_CONFIG_PATH_LENGTH = 256;
constexpr const char* CONFIG_FILE_EXTENSION = ".cfg";
constexpr const char* CONFIG_FILE_HEADER = "# NLM Configuration File\n";
constexpr const char* CONFIG_SECTION_PREFIX = "[";
constexpr const char* CONFIG_SECTION_SUFFIX = "]";
constexpr const char* CONFIG_KEY_VALUE_SEPARATOR = "=";
constexpr const char* CONFIG_COMMENT_PREFIX = "#";

// File and Directory Constants
constexpr const char* CONFIG_DIRECTORY = "configs";
constexpr const char* DEFAULT_CONFIG_FILENAME = "default.cfg";
constexpr const char* EXPERIMENTS_DIRECTORY = "experiments";
constexpr const char* DOC_DIRECTORY = "docs";
constexpr const char* BUILD_DIRECTORY = "build";
constexpr const char* BINARY_DIRECTORY = "bin";
constexpr const char* LIBRARY_DIRECTORY = "lib";

// Network and Multi-threading Constants
constexpr size_t DEFAULT_THREAD_POOL_SIZE = 4;
constexpr size_t MAX_CONCURRENT_EXPERIMENTS = 8;
constexpr double CONNECTION_TIMEOUT = 30.0;  // seconds
constexpr size_t MAX_RETRY_ATTEMPTS = 3;

// Random Number Generation Constants
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
constexpr uint64_t MAX_RANDOM_SEED = 0xFFFFFFFFFFFFFFFF;
constexpr uint64_t MIN_RANDOM_SEED = 0;

// Bounds and Limits
constexpr double EPSILON = 1e-6;  // Small value for floating point comparisons
constexpr double PI = 3.14159265358979323846;
constexpr double TWO_PI = 2.0 * PI;
constexpr size_t MAX_CHILDREN_PER_NEURON = 50;
constexpr size_t MAX_PARENTS_PER_NEURON = 50;
constexpr size_t MAX_NEURONS_PER_REGION = 1000;
constexpr size_t MAX_REGIONS_PER_BRAIN = 100;
constexpr size_t MAX_POPULATIONS_PER_REGION = 10;

// Error Codes
enum class ErrorCode {
    SUCCESS = 0,
    GENERAL_ERROR = 1,
    INVALID_CONFIG = 2,
    FILE_NOT_FOUND = 3,
    MEMORY_ALLOCATION_FAILED = 4,
    INVALID_PARAMETERS = 5,
    SIMULATION_FAILED = 6,
    CHECKPOINT_FAILED = 7,
    NETWORK_ERROR = 8,
    TIMEOUT_ERROR = 9,
    OUT_OF_MEMORY = 10
};

} // namespace Constants
} // namespace nlm

#endif // NLM_CONSTANTS_H
