#ifndef NLM_CONFIG_H
#define NLM_CONFIG_H

namespace nlm {

// Configuration file paths
namespace ConfigPaths {
    constexpr const char* DEFAULT_CONFIG_FILE = "config.json";
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    constexpr const char* DEFAULT_LOG_FILE = "nlm.log";
}

// Simulation configuration
namespace SimulationConfig {
    constexpr double DEFAULT_TIMESTEP = 0.001;  // 1ms
    constexpr double DEFAULT_TOTAL_STEPS = 10000.0;
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
}

// Memory configuration
namespace MemoryConfig {
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000;
    constexpr size_t DEFAULT_SEMINAL_MEMORY_CAPACITY = 10000;
    constexpr size_t DEFAULT_PROCEDURAL_MEMORY_CAPACITY = 500;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_CAPACITY = 5000;
}

// Neural network configuration
namespace NeuralConfig {
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr size_t DEFAULT_NEURON_ID_OFFSET = 1000;
    constexpr float DEFAULT_SYNAPTIC_WEIGHT_RANGE = 1.0f;
    constexpr float DEFAULT_LEARNING_RATE = 0.01f;
    constexpr float DEFAULT_DECAY_RATE = 0.1f;
}

// Reward system configuration
namespace RewardConfig {
    constexpr float DEFAULT_ENERGY_GOAL_MIN = 50.0f;
    constexpr float DEFAULT_ENERGY_GOAL_MAX = 90.0f;
    constexpr float DEFAULT_LOW_ENERGY_PENALTY = -2.0f;
    constexpr float DEFAULT_RECOVERY_BONUS = 1.0f;
    constexpr float DEFAULT_TIME_BONUS = 0.01f;
}

// Visualization configuration
namespace VisualizationConfig {
    constexpr bool DEFAULT_ENABLE_VISUALIZATION = false;
    constexpr const char* DEFAULT_VISUALIZATION_BACKEND = "headless";
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
}

// Logging configuration
namespace LoggingConfig {
    constexpr bool DEFAULT_ENABLE_LOGGING = true;
    constexpr const char* DEFAULT_LOG_LEVEL = "INFO";
    constexpr size_t DEFAULT_LOG_FILE_SIZE_LIMIT = 10 * 1024 * 1024;  // 10 MB
}

// Debug/Development configuration
namespace DebugConfig {
    constexpr bool DEFAULT_ENABLE_DEBUG_OUTPUT = false;
    constexpr bool DEFAULT_ENABLE_PERFORMANCE_PROFILING = false;
    constexpr size_t DEFAULT_DEBUG_OUTPUT_INTERVAL = 1000;
}

// Performance configuration
namespace PerformanceConfig {
    constexpr size_t DEFAULT_MAX_THREADS = 4;
    constexpr double DEFAULT_THREAD_TIMEOUT = 30.0;
    constexpr size_t DEFAULT_CACHE_SIZE = 1000;
}

// Experimental configuration
namespace ExperimentConfig {
    constexpr double DEFAULT_EXPERIMENT_DURATION = 1000.0;  // steps
    constexpr size_t DEFAULT_EXPERIMENT_SEED_OFFSET = 1000;
    constexpr bool DEFAULT_EXPERIMENT_REPRODUCIBLE = true;
}

} // namespace nlm

#endif // NLM_CONFIG_H