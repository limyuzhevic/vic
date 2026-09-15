// NLM (熙然) - Neural Learning Machine
// Global constants for the NLM project
// Defines commonly used magic numbers and default values

#pragma once

namespace nlm {

// Default configuration values
namespace ConfigDefaults {
    // Random number generation
    constexpr int64_t RANDOM_SEED = 42;
    
    // Simulation parameters
    constexpr double SIMULATION_TIMESTEP = 0.001;        // 1ms timesteps
    constexpr size_t MAX_SIMULATION_STEPS = 10000;
    constexpr double SIMULATION_TIME_LIMIT = 0.0;
    
    // Neural system parameters
    constexpr size_t DEFAULT_NEURON_COUNT = 500;
    constexpr size_t MIN_NEURON_COUNT = 100;
    constexpr size_t MAX_NEURON_COUNT = 10000;
    
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr size_t MIN_REGION_COUNT = 1;
    constexpr size_t MAX_REGION_COUNT = 10;
    
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    constexpr float MIN_CONNECTION_PROBABILITY = 0.01f;
    constexpr float MAX_CONNECTION_PROBABILITY = 0.5f;
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float MIN_STDP_WEIGHT = 0.001f;
    constexpr float MAX_STDP_WEIGHT = 0.1f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float MIN_STDP_TIME_CONSTANT = 1.0f;
    constexpr float MAX_STDP_TIME_CONSTANT = 100.0f;
    
    // Structural plasticity parameters
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr float MIN_RATE = 0.000001f;
    constexpr float MAX_RATE = 0.01f;
    constexpr float DEFAULT_MIN_WEIGHT_THRESHOLD = 0.05f;
    constexpr float MIN_WEIGHT_THRESHOLD = 0.01f;
    constexpr float MAX_WEIGHT_THRESHOLD = 0.5f;
    constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.001f;
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.0001f;
    constexpr float MAX_ACTIVITY_THRESHOLD = 0.01f;
    constexpr size_t DEFAULT_MAX_SYNAPSES_PER_NEURON = 100;
    constexpr size_t MIN_MAX_SYNAPSES = 10;
    constexpr size_t MAX_MAX_SYNAPSES = 10000;
    constexpr size_t DEFAULT_MAX_TOTAL_SYNAPSES = 1000000;
    
    // Development parameters
    constexpr float DEFAULT_DEVELOPMENTAL_SYNAPTOGENESIS_RATE = 0.001f;
    constexpr float DEFAULT_DEVELOPMENTAL_PRUNING_RATE = 0.0001f;
    constexpr size_t DEFAULT_CRITICAL_PERIOD_STEPS = 1000;
    constexpr float DEFAULT_MATURATION_FACTOR = 0.9f;
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;
    constexpr float MIN_DOPAMINE = 0.0f;
    constexpr float MAX_DOPAMINE = 1.0f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.05f;
    constexpr float DEFAULT_CURIOUSITY_STRENGTH = 0.1f;
    
    // Reward parameters
    constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;
    constexpr float MIN_REWARD_DISCOUNT = 0.9f;
    constexpr float MAX_REWARD_DISCOUNT = 0.999f;
    constexpr float DEFAULT_REWARD_SCALE = 1.0f;
    
    // Environment parameters
    constexpr size_t DEFAULT_ENVIRONMENT_WIDTH = 20;
    constexpr size_t DEFAULT_ENVIRONMENT_HEIGHT = 20;
    constexpr size_t MIN_ENVIRONMENT_SIZE = 5;
    constexpr size_t MAX_ENVIRONMENT_SIZE = 100;
    
    // Memory system parameters
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    constexpr size_t MIN_WORKING_MEMORY_CAPACITY = 10;
    constexpr size_t MAX_WORKING_MEMORY_CAPACITY = 1000;
    constexpr size_t DEFAULT_WORKING_MEMORY_DECAY_RATE = 100;
    
    constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;
    constexpr size_t MIN_EPISODIC_MEMORY = 10;
    constexpr size_t MAX_EPISODIC_MEMORY = 10000;
    
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_CAPACITY = 5000;
    constexpr size_t MIN_ASSOCIATIVE_MEMORY = 100;
    constexpr size_t MAX_ASSOCIATIVE_MEMORY = 100000;
    
    // Cognition system parameters
    constexpr size_t DEFAULT_PLANNER_SEQUENCE_LENGTH = 10;
    constexpr float DEFAULT_PLANNER_REWARD_THRESHOLD = 0.5f;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_PATTERN_LENGTH = 20;
    constexpr float DEFAULT_CONCEPT_FORMATION_ACTIVATION_THRESHOLD = 0.7f;
    
    // Attention parameters
    constexpr size_t DEFAULT_ATTENTION_WINDOW_SIZE = 20;
    constexpr float DEFAULT_ATTENTION_FOCUS_STRENGTH = 0.8f;
    constexpr float DEFAULT_ATTENTION_DECAY_RATE = 0.95f;
    
    // Visualization parameters
    constexpr size_t DEFAULT_VISUALIZATION_UPDATE_RATE = 30; // Hz
    constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;
    
    // Logging parameters
    constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 1024;
    constexpr size_t MAX_LOG_MESSAGES_PER_SECOND = 1000;
    
    // Performance parameters
    constexpr size_t DEFAULT_BENCHMARK_STEPS = 1000;
    constexpr size_t DEFAULT_BENCHMARK_REPEATS = 3;
    constexpr size_t DEFAULT_BENCHMARK_WARMUP = 100;
    constexpr double DEFAULT_BENCHMARK_DURATION = 30.0;
    
    // Checkpointing parameters
    constexpr char DEFAULT_CHECKPOINT_FILE_EXTENSION[] = ".brn";
    constexpr size_t DEFAULT_CHECKPOINT_SAVE_INTERVAL = 100;
    constexpr float DEFAULT_CHECKPOINT_COMPRESSION_LEVEL = 0.8f;
}

// Simulation and processing limits
namespace Limits {
    // Neuron properties
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    constexpr float RESTING_POTENTIAL = -65.0f;
    constexpr float SPIKE_THRESHOLD = 20.0f;
    constexpr float REFRACTORY_PERIOD = 5.0f; // ms
    
    // Synapse properties
    constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;
    constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;
    constexpr float EPSILON_WEIGHT_CHANGE = 0.0001f;
    constexpr float MAX_DELAY_STEPS = 1000;
    
    // Time and frequency
    constexpr float MAX_SIMULATION_FPS = 1000.0f;  // Hz
    constexpr float MIN_SIMULATION_DT = 0.0001f;   // 0.1ms
    constexpr float MAX_SIMULATION_DT = 0.1f;      // 100ms
    constexpr size_t MAX_SPIKE_HISTORY_SIZE = 10000;
    
    // Memory and storage
    constexpr size_t MAX_MEMORY_USAGE_MB = 1024;
    constexpr size_t DEFAULT_STACK_SIZE = 1024 * 1024; // 1MB
    constexpr size_t MAX_FILE_PATH_LENGTH = 1024;
    
    // Numerical precision
    constexpr float FLOATING_POINT_EPSILON = 1e-6f;
    constexpr double DOUBLE_EPSILON = 1e-9;
    constexpr float PI = 3.141592653589793f;
}

// Timing constants (in milliseconds)
namespace Timing {
    constexpr Timestamp SIMULATION_DT = 0.001;  // 1ms
    constexpr Timestamp CRITICAL_PERIOD_DURATION = 1000.0;  // ms
    constexpr Timestamp MATURATION_DURATION = 5000.0;       // ms
    constexpr Timestamp EPISODE_REPLAY_INTERVAL = 100.0;    // steps
    constexpr Timestamp EPISODE_CONSOLIDATION_INTERVAL = 1000.0; // steps
    constexpr Timestamp CHECKPOINT_INTERVAL = 100.0;        // steps
    constexpr Timestamp VISUALIZATION_UPDATE_INTERVAL = 33.33; // ~30Hz
    constexpr Timestamp BENCHMARK_WARMUP_DURATION = 100.0;   // ms
    constexpr Timestamp BENCHMARK_RUN_DURATION = 1000.0;     // ms
}

// Counting and indexing constants
namespace Counts {
    constexpr SimulationStep DEFAULT_EPISODE_LENGTH = 1000;
    constexpr size_t MIN_PLANNING_HORIZON = 5;
    constexpr size_t MAX_PLANNING_HORIZON = 100;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_WINDOW = 50;
    constexpr size_t DEFAULT_ATTENTION_SELECTION_SIZE = 10;
    constexpr size_t DEFAULT_BENCHMARK_STEP_COUNT = 1000;
    constexpr size_t DEFAULT_BENCHMARK_REPEAT_COUNT = 3;
    constexpr size_t DEFAULT_BENCHMARK_WARMUP_COUNT = 100;
    constexpr size_t DEFAULT_VISUALIZATION_UPDATE_RATE = 30;
    constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 1024;
    constexpr size_t MAX_LOG_ENTRIES = 10000;
}

// Special sentinel values
namespace Sentinels {
    constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
    constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
    constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
    constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);
    
    constexpr SimulationStep INVALID_STEP = UINT64_MAX;
    constexpr Timestamp INVALID_TIMESTAMP = std::numeric_limits<double>::max();
    
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    constexpr const char* DEFAULT_CHECKPOINT_FILE = "checkpoint.brn";
    constexpr const char* DEFAULT_LOG_FILE = "nlm.log";
}

// Enumerated constants
namespace Enums {
    // Development stages
    constexpr size_t CRITICAL_PERIOD_START_STEP = 100;
    constexpr size_t CRITICAL_PERIOD_END_STEP = 1000;
    constexpr size_t MATURATION_START_STEP = 1000;
    
    // Memory system modes
    constexpr size_t WORKING_MEMORY_MODE_ACTIVE = 0;
    constexpr size_t WORKING_MEMORY_MODE_PASSIVE = 1;
    constexpr size_t WORKING_MEMORY_MODE_COMPETITIVE = 2;
    
    // Attention modes
    constexpr size_t ATTENTION_MODE_TOP_DOWN = 0;
    constexpr size_t ATTENTION_MODE_BOTTOM_UP = 1;
    constexpr size_t ATTENTION_MODE_BIAS = 2;
}

// Performance tuning constants
namespace Performance {
    constexpr size_t NEURON_POOL_SIZE = 1024;
    constexpr size_t SYNAPSE_POOL_SIZE = 10240;
    constexpr size_t EVENT_QUEUE_SIZE = 8192;
    constexpr size_t MEMORY_POOL_ALIGNMENT = 64;
    
    // SIMD optimization constants
    constexpr size_t SIMD_VECTOR_SIZE = 16;  // float vector size
    constexpr size_t CACHE_LINE_SIZE = 64;
    constexpr size_t L1_CACHE_SIZE = 32 * 1024;
    constexpr size_t L2_CACHE_SIZE = 256 * 1024;
    constexpr size_t L3_CACHE_SIZE = 8 * 1024 * 1024;
    
    // Parallel processing
    constexpr size_t MAX_THREADS = 16;
    constexpr size_t DEFAULT_THREADS = 1;
    constexpr size_t THREAD_POOL_SIZE = 8;
}

// File and directory constants
namespace Files {
    constexpr const char* CONFIG_EXTENSION = ".cfg";
    constexpr const char* CHECKPOINT_EXTENSION = ".brn";
    constexpr const char* LOG_EXTENSION = ".log";
    constexpr const char* HTML_EXTENSION = ".html";
    constexpr const char* JSON_EXTENSION = ".json";
    constexpr const char* CSV_EXTENSION = ".csv";
    
    constexpr const char* DEFAULT_CONFIG_DIR = "configs";
    constexpr const char* DEFAULT_DATA_DIR = "data";
    constexpr const char* DEFAULT_LOG_DIR = "logs";
    constexpr const char* DEFAULT_OUTPUT_DIR = "output";
    
    constexpr const char* README_FILENAME = "README.md";
    constexpr const char* ARCHITECTURE_FILENAME = "docs/ARCHITECTURE.md";
    constexpr const char* SCIENCE_FILENAME = "docs/SCIENCE.md";
    constexpr const char* ROADMAP_FILENAME = "docs/ROADMAP.md";
}

// Human-readable string constants
namespace Strings {
    constexpr const char* VERSION = "0.1.0";
    constexpr const char* PROJECT_NAME = "NLM";
    constexpr const char* PROJECT_DESCRIPTION = "Neural Learning Machine";
    constexpr const char* PHASE_6_NAME = "Final Integration";
    constexpr const char* AUTHOR_NAME = "Research Project";
    constexpr const char* LICENSE = "MIT";
    
    // Log message templates
    constexpr const char* LOG_LOADING_CONFIG = "Loading configuration from: %s";
    constexpr const char* LOG_SAVING_CONFIG = "Saving configuration to: %s";
    constexpr const char* LOG_INITIALIZING_BRAIN = "Initializing NLM Brain...";
    constexpr const char* LOG_BRAIN_INITIALIZED = "Brain initialized successfully";
    constexpr const char* LOG_BRAIN_FAILED_INIT = "Failed to initialize brain!";
    constexpr const char* LOG_SIMULATION_STEP = "Step %zu: %zu spikes, %zu active neurons";
    constexpr const char* LOG_BRAIN_SAVED = "Brain state saved to: %s";
    constexpr const char* LOG_BRAIN_LOADED = "Brain state loaded from: %s";
    
    // Error message templates
    constexpr const char* ERROR_FILE_NOT_FOUND = "File not found: %s";
    constexpr const char* ERROR_INVALID_CONFIG = "Invalid configuration in: %s";
    constexpr const char* ERROR_BRAIN_INIT_FAILED = "Brain initialization failed";
    constexpr const char* ERROR_SAVE_FAILED = "Failed to save to: %s";
    constexpr const char* ERROR_LOAD_FAILED = "Failed to load from: %s";
    
    // Benchmark result templates
    constexpr const char* BENCHMARK_RESULT_FORMAT = "%s: %.2f steps/s, %.3f avg step time, %.1f MB memory";
    constexpr const char* BENCHMARK_SUMMARY_FORMAT = "Benchmark completed in %.2f seconds";
}

} // namespace nlm
