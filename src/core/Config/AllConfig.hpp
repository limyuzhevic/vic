// NLM Configuration File
// Centralized configuration for file paths, limits, and system parameters
// This file defines all hardcoded values that should be configurable

#pragma once

#include <string>
#include <vector>

namespace nlm {
namespace config {

// File system paths and directories
struct Paths {
    // Default configuration file path
    static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    
    // Checkpoint and save file paths
    static constexpr const char* CHECKPOINT_DIRECTORY = "checkpoints";
    static constexpr const char* CHECKPOINT_TEMPLATE = "checkpoints/nlm_checkpoint_{step}.nlm";
    static constexpr const char* BRAIN_STATE_TEMPLATE = "states/brain_{id}.nlm";
    static constexpr const char* EPISODIC_MEMORY_TEMPLATE = "memory/episodes/episode_{id}.nlm";
    
    // Log file paths
    static constexpr const char* LOG_FILE = "logs/nlm.log";
    static constexpr const char* ERROR_LOG_FILE = "logs/nlm_errors.log";
    
    // Temporary file paths
    static constexpr const char* TEMP_DIRECTORY = "/tmp/nlm";
    static constexpr const char* TEMP_CHECKPOINT = "/tmp/nlm_checkpoint_test.bin";
    
    // Database and storage paths
    static constexpr const char* DATABASE_DIRECTORY = "data";
    static constexpr const char* SEMANTIC_MEMORY_DB = "data/semantic_memory.db";
    static constexpr const char* PROCEDURAL_MEMORY_DB = "data/procedural_memory.db";
    
    // Build and output paths
    static constexpr const char* BUILD_DIRECTORY = "build";
    static constexpr const char* OUTPUT_DIRECTORY = "output";
    
    // Documentation paths
    static constexpr const char* DOCUMENTATION_DIR = "docs";
    static constexpr const char* API_DOCS_DIR = "docs/api";
};

// System limits and constraints
struct Limits {
    // Network architecture limits
    static constexpr size_t MIN_NEURONS = 10;
    static constexpr size_t MAX_NEURONS = 100000;  // 100k neurons
    static constexpr size_t DEFAULT_NEURON_COUNT = 500;
    static constexpr size_t PHASE_6_DEMO_NEURON_COUNT = 500;
    
    // Region and population limits
    static constexpr size_t MIN_REGIONS = 1;
    static constexpr size_t MAX_REGIONS = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr size_t MIN_POPULATIONS_PER_REGION = 1;
    static constexpr size_t MAX_POPULATIONS_PER_REGION = 100;
    
    // Synapse limits
    static constexpr size_t MIN_SYNAPSES_PER_NEURON = 1;
    static constexpr size_t MAX_SYNAPSES_PER_NEURON = 10000;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    static constexpr float MIN_CONNECTION_PROBABILITY = 0.001f;
    static constexpr float MAX_CONNECTION_PROBABILITY = 0.5f;
    
    // Memory system limits
    static constexpr size_t MAX_SPIKING = 1000000;  // 1M spikes per session
    static constexpr size_t MAX_SPIKE_HISTORY = 10000;
    static constexpr size_t MAX_EPISODIC_EPISODES = 10000;
    static constexpr size_t MAX_SEMANTIC_CONCEPTS = 10000;
    static constexpr size_t MAX_PROCEDURAL_SKILLS = 1000;
    static constexpr size_t MAX_ASSOCIATIVE_PATTERNS = 50000;
    
    // Neuromodulation limits
    static constexpr float MIN_NEUROMODULATION_LEVEL = 0.0f;
    static constexpr float MAX_NEUROMODULATION_LEVEL = 1.0f;
    static constexpr float DEFAULT_NEUROMODULATION_LEVEL = 0.1f;
    static constexpr float NEUROMODULATION_DECAY_RATE = 0.01f;
    
    // Developmental limits
    static constexpr size_t MIN_DEVELOPMENT_STEPS = 100;
    static constexpr size_t MAX_DEVELOPMENT_STEPS = 100000;
    static constexpr float MIN_MATURATION_RATE = 0.0001f;
    static constexpr float MAX_MATURATION_RATE = 0.1f;
    
    // Simulation limits
    static constexpr double MIN_TIMESTEP = 1e-6;   // 1 microsecond
    static constexpr double MAX_TIMESTEP = 1.0;    // 1 second
    static constexpr double DEFAULT_TIMESTEP = 0.001;  // 1 millisecond
    static constexpr size_t MAX_SIMULATION_STEPS = 1000000;
    static constexpr double MAX_WALL_CLOCK_TIME = 300.0;  // 5 minutes
    
    // Performance limits
    static constexpr size_t MAX_PARALLEL_SIMULATIONS = 4;
    static constexpr size_t MAX_THREADS = 32;
    static constexpr size_t MAX_MEMORY_POOL_SIZE = 100 * 1024 * 1024;  // 100 MB
    static constexpr size_t MAX_CACHE_SIZE = 10 * 1024 * 1024;         // 10 MB
    
    // Experiment limits
    static constexpr size_t MAX_EXPERIMENT_STEPS = 100000;
    static constexpr float MIN_REWARD = -100.0f;
    static constexpr float MAX_REWARD = 100.0f;
    static constexpr float DEFAULT_REWARD = 0.0f;
    
    // Logging limits
    static constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;
    static constexpr size_t MAX_LOG_ENTRIES = 1000000;
    static constexpr size_t LOG_FLUSH_INTERVAL_MS = 100;
    
    // Python binding limits
    static constexpr size_t MAX_PYTHON_SAMPLES_FOR_VALIDATION = 1000;
    static constexpr size_t MAX_PYTHON_ARRAY_DIMENSION = 3;
    static constexpr size_t PYTHON_MAX_STRING_LENGTH = 10000;
    
    // File size limits
    static constexpr size_t MAX_CHECKPOINT_SIZE_MB = 100;
    static constexpr size_t MAX_MEMORY_FILE_SIZE_MB = 1000;
    static constexpr size_t MAX_DATABASE_SIZE_MB = 5000;
};

// File I/O configuration
struct FileIO {
    // Checkpoint and save configuration
    static constexpr bool ENABLE_CHECKPOINTING = true;
    static constexpr bool ENABLE_COMPRESSION = true;
    static constexpr float CHECKPOINT_COMPRESSION_RATIO = 0.5f;  // ~50% compression
    static constexpr bool VERIFY_CHECKPOINT_HASH = true;
    static constexpr size_t CHECKPOINT_VERSION = 1;
    
    // Memory persistence configuration
    static constexpr bool ENABLE_EPISODIC_STORAGE = true;
    static constexpr bool ENABLE_SEMANTIC_STORAGE = true;
    static constexpr bool ENABLE_PROCEDURAL_STORAGE = true;
    static constexpr size_t EPISODIC_STORAGE_BATCH_SIZE = 100;
    static constexpr size_t SEMANTIC_STORAGE_BATCH_SIZE = 1000;
    static constexpr size_t PROCEDURAL_STORAGE_BATCH_SIZE = 100;
    
    // Replay configuration
    static constexpr bool ENABLE_REPLAY = true;
    static constexpr size_t REPLAY_MAX_ENTRIES = 10000;
    static constexpr bool ENABLE_REPLAY_COMPRESSION = true;
    
    // Database configuration
    static constexpr bool ENABLE_DATABASE = true;
    static constexpr size_t DATABASE_CONNECTION_POOL_SIZE = 10;
    static constexpr size_t DATABASE_QUERY_TIMEOUT_MS = 5000;
    static constexpr bool DATABASE_ENABLE_PERSISTENCE = true;
    
    // Log configuration
    static constexpr bool ENABLE_FILE_LOGGING = true;
    static constexpr bool ENABLE_CONSOLE_LOGGING = true;
    static constexpr bool ENABLE_ERROR_LOGGING = true;
    static constexpr size_t LOG_MAX_FILE_SIZE_MB = 100;
    static constexpr size_t LOG_ROTATION_COUNT = 10;
};

// Runtime behavior configuration
struct Runtime {
    // Development behavior
    static constexpr bool ENABLE_DEVELOPMENT = true;
    static constexpr bool ENABLE_STRUCTURAL_PLASTICITY = true;
    static constexpr bool ENABLE_REWARD_MODULATION = true;
    static constexpr bool ENABLE_CURIOUSY = true;  // Note: Typo for curiosity
    static constexpr bool ENABLE_PREDICTION = true;
    static constexpr bool ENABLE_PLANNING = true;
    static constexpr bool ENABLE_CONCEPT_FORMATION = true;
    static constexpr bool ENABLE_ATTENTION = true;
    
    // Plasticity behavior
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TIME_CONST = 20.0f;
    static constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Memory consolidation
    static constexpr bool ENABLE_MEMORY_CONSOLIDATION = true;
    static constexpr size_t CONSOLIDATION_INTERVAL_STEPS = 1000;
    static constexpr float CONSOLIDATION_STRENGTH = 0.9f;
    
    // Learning parameters
    static constexpr float LEARNING_RATE = 0.01f;
    static constexpr float EXPLORATION_RATE = 0.1f;
    static constexpr float EXTRACTION_THRESHOLD = 0.05f;
    static constexpr size_t MAX_EXPLORATION_STEPS = 1000;
    
    // Performance tuning
    static constexpr bool ENABLE_OPTIMIZATION = true;
    static constexpr bool ENABLE_VECTORIZATION = true;
    static constexpr bool ENABLE_SIMD = true;
    static constexpr bool ENABLE_MULTITHREADING = false;  // Currently disabled due to complexity
    static constexpr size_t SIMD_WIDTH = 8;  // AVX-256 width
};

// Command line interface configuration
struct CLI {
    // Argument parsing configuration
    static constexpr const char* PROGRAM_NAME = "nlm";
    static constexpr const char* VERSION = "2.0.0";
    static constexpr const char* AUTHOR = "NLM Development Team";
    static constexpr const char* COPYRIGHT = "Copyright (c) 2024-2026 NLM Project";
    static constexpr const char* DESCRIPTION = "Neural Learning Machine - Advanced Neural Simulation Framework";
    
    // Help and documentation
    static constexpr const char* HELP_FILE = "docs/cli_help.txt";
    static constexpr bool SHOW_ADVANCED_HELP = true;
    static constexpr bool SHOW_EXAMPLES = true;
    static constexpr bool SHOW_CONFIG_SUMMARY = true;
    
    // Argument limits
    static constexpr size_t MAX_ARGS = 100;
    static constexpr size_t MAX_ARG_LENGTH = 100;
    static constexpr size_t MAX_CONFIG_VALUE_LENGTH = 1000;
    
    // Configuration file formats
    static constexpr const char* DEFAULT_CONFIG_FORMAT = "json";
    static constexpr const char* SUPPORTED_CONFIG_FORMATS[] = {"json", "yaml", "ini"};
    
    // Output configuration
    static constexpr bool ENABLE_COLOR_OUTPUT = true;
    static constexpr bool ENABLE_PROGRESS_BARS = true;
    static constexpr size_t PROGRESS_BAR_WIDTH = 50;
    static constexpr bool ENABLE_TIMING_INFO = true;
    static constexpr bool ENABLE_MEMORY_INFO = true;
};

// Experimental configuration
struct Experimental {
    // Phase-specific configurations
    struct Phase2 {
        static constexpr size_t NEURON_COUNT = 500;
        static constexpr size_t MIN_NEURONS = 100;
        static constexpr size_t MAX_NEURONS = 5000;
        static constexpr size_t TEST_STEPS = 50;
        static constexpr size_t PLASTICITY_STEPS = 1000;
        static constexpr size_t STDP_STEPS = 1000;
        static constexpr float CURRENT_INJECTION = 50.0f;
        static constexpr float STDP_CURRENT = 60.0f;
        static constexpr double TIMESTEP = 0.001;
    };
    
    struct Phase3 {
        static constexpr size_t NEURON_COUNT = 1000;
        static constexpr size_t WORLD_WIDTH = 50;
        static constexpr size_t WORLD_HEIGHT = 50;
        static constexpr size_t MAX_AGENTS = 10;
        static constexpr double UPDATE_INTERVAL = 0.1;
    };
    
    struct Phase4 {
        static constexpr size_t NEURON_COUNT = 2000;
        static constexpr size_t CONCEPT_BANK_SIZE = 500;
        static constexpr size_t PLANNER_DEPTH = 5;
        static constexpr float PLANNING_HORIZON = 10.0f;
        static constexpr size_t ATTENTION_POOL_SIZE = 50;
    };
    
    struct Phase5 {
        static constexpr size_t NEURON_COUNT = 3000;
        static constexpr size_t EPISODIC_CAPACITY = 1000;
        static constexpr size_t SEMANTIC_CAPACITY = 1000;
        static constexpr size_t PROCEDURAL_CAPACITY = 500;
        static constexpr float REWARD_PREDICTION_HORIZON = 20.0f;
        static constexpr float INTRINSIC_REWARD_STRENGTH = 0.5f;
    };
    
    struct Phase6 {
        static constexpr size_t NEURON_COUNT = 5000;
        static constexpr size_t MAX_EXPERIMENT_STEPS = 5000;
        static constexpr bool ENABLE_DEVELOPMENT = true;
        static constexpr bool ENABLE_REWARD_MODULATION = true;
        static constexpr bool ENABLE_PREDICTION = true;
        static constexpr bool ENABLE_PLANNING = true;
        static constexpr bool ENABLE_CONCEPT_FORMATION = true;
        static constexpr bool ENABLE_ATTENTION = true;
        static constexpr float DEVELOPMENT_PROGRESSION_RATE = 0.01f;
        static constexpr float NEUROMODULATION_INTENSITY = 0.8f;
        static constexpr float PLASTICITY_STRENGTH = 1.0f;
    };
    
    // General experimental parameters
    static constexpr size_t MIN_EXPERIMENT_DURATION = 100;
    static constexpr size_t MAX_EXPERIMENT_DURATION = 100000;
    static constexpr float REWARD_DISCOUNT_FACTOR = 0.95f;
    static constexpr float EXPLORATION_EXPLOITATION_TRADEOFF = 0.5f;
    static constexpr bool ENABLE_EXPERIMENT_TELEMETRY = true;
    static constexpr const char* EXPERIMENT_RESULTS_DIR = "results";
    static constexpr const char* EXPERIMENT_DATA_DIR = "data/experiments";
};

// Debug and testing configuration
struct Debug {
    // Logging levels
    static constexpr const char* LOG_LEVEL = "INFO";
    static constexpr bool ENABLE_DEBUG_LOGGING = false;
    static constexpr bool ENABLE_PERFORMANCE_PROFILING = false;
    static constexpr bool ENABLE_MEMORY_PROFILING = false;
    static constexpr bool ENABLE_THREAD_PROFILING = false;
    
    // Validation settings
    static constexpr bool ENABLE_INPUT_VALIDATION = true;
    static constexpr bool ENABLE_RANGE_CHECKING = true;
    static constexpr bool ENABLE_TYPE_CHECKING = true;
    static constexpr bool ENABLE_STATE_VALIDATION = true;
    
    // Test configuration
    static constexpr bool ENABLE_UNIT_TESTS = true;
    static constexpr bool ENABLE_INTEGRATION_TESTS = true;
    static constexpr bool ENABLE_PERFORMANCE_TESTS = false;
    static constexpr bool ENABLE_STRESS_TESTS = false;
    static constexpr const char* TEST_RESULTS_DIR = "test_results";
    
    // Memory debugging
    static constexpr bool ENABLE_MEMORY_LEAK_DETECTION = false;
    static constexpr size_t MEMORY_LEAK_CHECK_INTERVAL_MS = 60000;
    static constexpr size_t MEMORY_LEAK_THRESHOLD_MB = 100;
};

// Python-specific configuration
struct Python {
    // Binding configuration
    static constexpr bool ENABLE_PYTHON_BINDINGS = true;
    static constexpr bool ENABLE_PYTHON_EXCEPTIONS = true;
    static constexpr bool ENABLE_PYTHON_DOCUMENTATION = true;
    static constexpr bool ENABLE_PYTHON_TYPE_HINTS = true;
    static constexpr bool ENABLE_PYTHON_VALIDATION = true;
    
    // Performance tuning
    static constexpr bool PYTHON_ENABLE_VECTORIZATION = true;
    static constexpr bool PYTHON_ENABLE_MULTITHREADING = false;
    static constexpr size_t PYTHON_MAX_THREADS = 4;
    static constexpr size_t PYTHON_MAX_MEMORY_MB = 1024;
    
    // Security and safety
    static constexpr bool PYTHON_SANITIZE_INPUTS = true;
    static constexpr size_t PYTHON_MAX_STRING_LENGTH = 10000;
    static constexpr size_t PYTHON_MAX_ARRAY_SIZE = 1000000;
    static constexpr float PYTHON_NUMERICAL_PRECISION = 1e-6f;
    
    // Integration
    static constexpr bool PYTHON_ENABLE_MATPLOTLIB = false;  // Optional dependency
    static constexpr bool PYTHON_ENABLE_NUMPY = true;
    static constexpr bool PYTHON_ENABLE_PANDAS = false;      // Optional dependency
    static constexpr bool PYTHON_ENABLE_JUPYTER = false;      // Optional dependency
};

// Build and deployment configuration
struct Build {
    // Compilation
    static constexpr const char* COMPILER = "g++";
    static constexpr const char* CXX_STANDARD = "c++20";
    static constexpr bool ENABLE_OPTIMIZATION = true;
    static constexpr bool ENABLE_DEBUG_SYMBOLS = false;
    static constexpr const char* OPTIMIZATION_LEVEL = "O3";
    
    // Linking
    static constexpr bool ENABLE_SHARED_LIBS = true;
    static constexpr bool ENABLE_STATIC_LIBS = true;
    static constexpr const char* LIBRARY_PREFIX = "lib";
    static constexpr const char* LIBRARY_SUFFIX = ".so";
    
    // Installation
    static constexpr const char* INSTALL_PREFIX = "/usr/local";
    static constexpr const char* CONFIG_INSTALL_DIR = "${prefix}/etc/nlm";
    static constexpr const char* LIB_INSTALL_DIR = "${prefix}/lib";
    static constexpr const char* INCLUDE_INSTALL_DIR = "${prefix}/include";
    static constexpr const char* BIN_INSTALL_DIR = "${prefix}/bin";
    static constexpr const char* PYTHON_INSTALL_DIR = "${prefix}/lib/python3.10/site-packages";
    
    // Packaging
    static constexpr const char* PACKAGE_NAME = "nlm";
    static constexpr const char* PACKAGE_VERSION = "2.0.0";
    static constexpr const char* PACKAGE_DESCRIPTION = "Neural Learning Machine Neural Simulation Framework";
};

// System configuration
namespace System {
    static constexpr const char* NAME = "NLM";
    static constexpr const char* VERSION = "2.0.0";
    static constexpr const char* AUTHOR = "NLM Development Team";
    static constexpr const char* DESCRIPTION = "Advanced Neural Learning Machine Framework";
    static constexpr const char* LICENSE = "MIT";
    static constexpr const char* WEBSITE = "https://github.com/nlm-project/nlm";
    static constexpr const char* DOCUMENTATION = "https://nlm.readthedocs.io";
}

// Helper functions for configuration
namespace utils {
    template<typename T>
    std::string toString(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    std::string getConfigPath(const std::string& filename) {
        return "configs/" + filename;
    }
    
    std::string getLogPath(const std::string& filename) {
        return "logs/" + filename;
    }
    
    std::string getCheckpointPath(const std::string& suffix = "") {
        if (suffix.empty()) {
            return Paths::CHECKPOINT_TEMPLATE;
        }
        return Paths::CHECKPOINT_TEMPLATE.substr(0, Paths::CHECKPOINT_TEMPLATE.find("{step}")) + suffix;
    }
}

} // namespace config
} // namespace nlm