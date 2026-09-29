// NLM Project Constants
// Centralized definitions for magic numbers and configuration values
// This improves code maintainability and reduces hard-coded values

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace nlm {

// ============================================================================
// SIMULATION PARAMETERS
// ============================================================================

// Default simulation parameters
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;      // 1 ms timestep
constexpr uint64_t DEFAULT_RANDOM_SEED = 42;                // Deterministic by default
constexpr size_t DEFAULT_NEURON_COUNT = 500;               // Reduced for testing
constexpr size_t DEFAULT_REGION_COUNT = 1;                 // Single region by default
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;    // 15% connection rate
constexpr size_t MAX_SPIKE_HISTORY = 1000;                 // Circular buffer size

// ============================================================================
// MOTOR SYSTEM CONSTANTS
// ============================================================================

constexpr size_t MOTOR_GROUP_COUNT = 6;                     // Forward, Backward, Left, Right, Interact, Wait
constexpr size_t MOTOR_GROUP_SIZE = 10;                     // Average neurons per motor group
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;            // Minimum activity to trigger action
constexpr size_t MOTOR_CURiosity_EXPLORATION_CHANCE = 30;   // Maximum exploration percentage

// ============================================================================
// SENSORY SYSTEM CONSTANTS
// ============================================================================

constexpr size_t VISION_WIDTH = 16;                         // Vision grid dimension (16x16)
constexpr size_t VISION_HEIGHT = 16;
constexpr size_t VISION_SIZE = VISION_WIDTH * VISION_HEIGHT; // 256 vision neurons
constexpr size_t TOUCH_SENSORS = 8;                        // Touch input sensors
constexpr size_t INTERNAL_SIGNALS = 4;                      // Internal system signals
constexpr size_t PROPRIOCEPTION_SENSORS = 6;                // Proprioception sensors
constexpr float VISION_INPUT_SCALE = 5.0f;                  // Vision current scaling
constexpr float TOUCH_INPUT_SCALE = 8.0f;                   // Touch current scaling
constexpr float INTERNAL_INPUT_SCALE = 5.0f;                // Internal signal scaling
constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f;          // Proprioception scaling

// ============================================================================
// NEUROMODULATION CONSTANTS
// ============================================================================

constexpr float DOPAMINE_CLAMP_MIN = -1.0f;                  // Minimum dopamine level
constexpr float DOPAMINE_CLAMP_MAX = 1.0f;                   // Maximum dopamine level
constexpr float NOVELTY_DECAY_RATE = 0.99f;                 // Novelty decay per timestep
constexpr float CURIOcity_THRESHOLD = 0.3f;                  // Minimum curiosity for exploration
constexpr float HIGH_CURIOcity_LEVEL = 0.5f;                 // High curiosity triggers exploration
constexpr float PLATPICITY_MODIFIER_MIN = 0.1f;              // Minimum plasticity factor
constexpr float PLATPICITY_MODIFIER_MAX = 2.0f;              // Maximum plasticity factor
constexpr float NEUROMODULATION_BASE_FACTOR = 0.5f;         // Base neuromodulation effect

// ============================================================================
// PLASTICITY CONSTANTS
// ============================================================================

constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;           // Long-Term Potentiation weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;          // Long-Term Depression weight
constexpr float STDP_TIME_CONSTRAINT = 20.0f;               // STDP time window (ms)
constexpr float SYNAPTONGENESIS_RATE_INITIAL = 0.0001f;    // Initial synaptogenesis rate
constexpr float PRUNING_RATE_INITIAL = 0.00001f;           // Initial pruning rate
constexpr float HEBBIAN_LEARNING_RATE = 0.01f;              // Hebbian learning rate
constexpr float SYNAPSE_WEIGHT_MIN = 0.0f;                  // Minimum synapse weight
constexpr float SYNAPSE_WEIGHT_MAX = 2.0f;                  // Maximum synapse weight
constexpr float SYNAPSE_WEIGHT_CHANGE_THRESHOLD = 0.01f;    // Significant weight change threshold

// ============================================================================
// DEVELOPMENT CONSTANTS
// ============================================================================

constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;        // Initial development stage (ms)
constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;       // Critical period end (ms)
constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0;      // Maturation period end (ms)
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;         // High initial plasticity
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;        // Critical period plasticity
constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;      // Maturation plasticity
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;           // Adult stability
constexpr double DEVELOPMENT_UPDATE_INTERVAL = 0.001;     // Development update timestep

// ============================================================================
// EXPERIMENT CONSTANTS
// ============================================================================

constexpr uint64_t DEFAULT_EXPERIMENT_MAX_STEPS = 10000;    // Default experiment duration
constexpr size_t DEFAULT_EXPERIMENT_NEURON_COUNT = 1000;    // Larger for full experiments
constexpr size_t CHECKPOINT_VERSION = 1;                    // Checkpoint file format version
constexpr double CHECKPOINT_COMPATIBILITY_VERSION = 1.0;    // Backward compatibility
constexpr size_t REPLAY_MAX_EPISODES = 100;                  // Maximum episodes for replay
constexpr float REPLAY_INTERPOLATION_FACTOR = 0.5f;          // Replay memory weighting
constexpr double EPISODE_MAX_DURATION = 1000.0;              // Maximum episode length (ms)

// ============================================================================
// MEMORY SYSTEM CONSTANTS
// ============================================================================

constexpr size_t WORKING_MEMORY_MAX_TRACES = 1000;          // Maximum working memory traces
constexpr size_t WORKING_MEMORY_DEFAULT_TRACES = 100;       // Default working memory size
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 10000;       // Maximum stored episodes
constexpr size_t EPISODIC_MEMORY_DEFAULT_CAPACITY = 1000;    // Default episode capacity
constexpr float EPISODE_RELEVANCE_THRESHOLD = 0.7f;           // Episode importance threshold
constexpr double EPISODE_CONSOLIDATION_INTERVAL = 100.0;      // Episode consolidation period
constexpr float MEMORY_CONSOLIDATION_STRENGTH = 0.8f;        // Memory consolidation factor

// ============================================================================
// PERFORMANCE OPTIMIZATION CONSTANTS
// ============================================================================

constexpr size_t VECTOR_RESERVE_SIZE = 1024;                // Vector pre-allocation size
constexpr size_t MAP_RESERVE_SIZE = 512;                    // Map pre-allocation size
constexpr size_t POOL_ALLOCATION_SIZE = 4096;               // Memory pool chunk size
constexpr double PERFORMANCE_MEASUREMENT_INTERVAL = 100.0;  // Performance sampling interval
constexpr float PERFORMANCE_TARGET_FPS = 1000.0f;           // Target simulation frames per second
constexpr size_t THREAD_POOL_SIZE = 4;                      // Number of worker threads

// ============================================================================
// LOGGING CONSTANTS
// ============================================================================

constexpr size_t LOG_BUFFER_SIZE = 8192;                    // Log message buffer size
constexpr size_t LOG_HISTORY_SIZE = 1000;                   // Log history retention
constexpr size_t LOG_MAX_LINE_LENGTH = 1024;                // Maximum log line length
constexpr auto LOG_LEVEL_DEFAULT = LogLevel::Info;          // Default log level
constexpr double LOG_ROTATION_SIZE = 10.0;                  // Log file rotation size (MB)
constexpr size_t LOG_ROTATION_FILES = 5;                    // Number of rotated log files to keep

// ============================================================================
// VALIDATION CONSTANTS
// ============================================================================

constexpr size_t VALIDATION_BUFFER_SIZE = 1024;             // Validation buffer size
constexpr double VALIDATION_TIMEOUT = 1.0;                  // Validation timeout (seconds)
constexpr size_t VALIDATION_MAX_ERRORS = 10;                // Maximum validation errors before failure
constexpr double VALIDATION_RELAXED_FACTOR = 0.8;           // Validation relaxation factor
constexpr double VALIDATION_STRICT_FACTOR = 1.2;             // Validation strictness factor

// ============================================================================
 // UTILITY FUNCTIONS
 // ============================================================================

// Clamp function for float values
inline float clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

// Linear interpolation
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Normalize value to [0, 1] range
inline float normalize(float value, float min, float max) {
    if (max <= min) return 0.0f;
    return (value - min) / (max - min);
}

// Sigmoid function for neural computations
inline float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

// Tanh function for neural computations
inline float tanhf(float x) {
    return std::tanh(x);
}

// ============================================================================
 // TEMPLATE UTILITIES
 // ============================================================================

// Safe minimum/maximum for numeric types
template<typename T>
inline T safe_min(T a, T b) { return a < b ? a : b; }

template<typename T>
inline T safe_max(T a, T b) { return a > b ? a : b; }

// Clamped value
template<typename T>
inline T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

// Linear interpolation
template<typename T>
inline T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

} // namespace nlm