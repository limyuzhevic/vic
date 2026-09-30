// Core constants for NLM - Neural Learning Machine
// 
// This file defines strongly-typed constants for the NLM project to:
// 1. Replace magic numbers throughout the codebase
// 2. Provide clear, named constants for common values
// 3. Enable easy configuration through the Config system
// 4. Improve code readability and maintainability
// 
// Constants are organized by category:
// - Physics/Neural constants
// - Time constants
// - Performance constants
// - Development constants
// - Configuration defaults

#pragma once

#include <cstdint>

namespace nlm {

// =============================================
// PHYSICS/NEURAL CONSTANTS
// =============================================

// Neural weight bounds (should match plasticity system limits)
constexpr float NEURAL_WEIGHT_MAX = 1.0f;
constexpr float NEURAL_WEIGHT_MIN = -1.0f;
constexpr float NEURAL_WEIGHT_DEFAULT = 0.0f;

// Neural membrane potential bounds
constexpr float MEMBRANE_POTENTIAL_RESTING = 0.0f;
constexpr float MEMBRANE_POTENTIAL_THRESHOLD = 0.5f;  // Firing threshold
constexpr float MEMBRANE_POTENTIAL_MAX = 1.0f;

// Neural current injection scaling
constexpr float CURRENT_INJECTION_SCALE_VISION = 5.0f;
constexpr float CURRENT_INJECTION_SCALE_TOUCH = 8.0f;
constexpr float CURRENT_INJECTION_SCALE_INTERNAL = 5.0f;
constexpr float CURRENT_INJECTION_SCALE_PROPRIOCEPTION = 3.0f;

// Synaptic transmission constants
constexpr float SYNAPTIC_RECOVERY_TIME_CONST = 20.0f;  // STDP time constant
constexpr float SYNAPTIC_ELLIGIBILITY_WINDOW = 10.0f;  // ms

// =============================================
// TIME CONSTANTS
// =============================================

// Default simulation timestep
constexpr double SIMULATION_TIMESTEP_DEFAULT = 0.001;  // 1ms
constexpr double SIMULATION_TIMESTEP_MIN = 0.0001;      // 0.1ms
constexpr double SIMULATION_TIMESTEP_MAX = 0.1;         // 100ms

// Development stage durations (in seconds)
constexpr double DEVELOPMENT_INITIAL_DURATION = 60.0;   // ~1 minute
constexpr double DEVELOPMENT_CRITICAL_PERIOD_DURATION = 300.0;  // ~5 minutes
constexpr double DEVELOPMENT_MATURATION_DURATION = 600.0;  // ~10 minutes
constexpr double DEVELOPMENT_ADULT_DURATION = 900.0;   // ~15 minutes
constexpr double DEVELOPMENT_AGING_DURATION = 1800.0;  // ~30 minutes

// Activity decay constants
constexpr float NOVELTY_DECAY_RATE = 0.99f;
constexpr float SENSORIAL_NOVELTY_DECAY = 0.995f;

// =============================================
// PLASTICITY CONSTANTS
// =============================================

// STDP (Spike-Timing-Dependent Plasticity) weights
constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.02f;    // Long-Term Potentiation
constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.015f;   // Long-Term Depression
constexpr float STDP_WEIGHT_MAX = 1.0f;
constexpr float STDP_WEIGHT_MIN = -1.0f;

// Hebbian plasticity
constexpr float HEBBIAN_LEARNING_RATE_DEFAULT = 0.01f;
constexpr float HEBBIAN_WEIGHT_MAX = 1.0f;
constexpr float HEBBIAN_WEIGHT_MIN = -1.0f;

// Reward-modulated plasticity
constexpr float REWARD_PREDICTION_ERROR_THRESHOLD = 0.5f;
constexpr float REWARD_DOPAMINE_CLAMP_MIN = -1.0f;
constexpr float REWARD_DOPAMINE_CLAMP_MAX = 1.0f;
constexpr float REWARD_DOPAMINE_DEFAULT = 0.0f;

// =============================================
// BEHAVIORAL/CEREBRAL CONSTANTS
// =============================================

// Curiosity/exploration constants
constexpr float CURIOSITY_ENABLED_DEFAULT = true;
constexpr float CURIOSITY_THRESHOLD_EXPLORATION = 0.3f;
constexpr float CURIOSITY_THRESHOLD_HIGH = 0.5f;
constexpr float CURIOSITY_EXPLORATION_CHANCE_MAX = 0.3f;  // 30%
constexpr float CURIOSITY_LEVEL_CLAMP_MIN = 0.0f;
constexpr float CURIOSITY_LEVEL_CLAMP_MAX = 1.0f;

// Novelty detection constants
constexpr float NOVELTY_SENSITIVITY = 1.0f;
constexpr float NOVELTY_THRESHOLD = 0.01f;  // Minimum change to register novelty

// Prediction error constants
constexpr float PREDICTION_ERROR_DEFAULT = 0.0f;
constexpr float PREDICTION_ERROR_CLAMP_MIN = -1.0f;
constexpr float PREDICTION_ERROR_CLAMP_MAX = 1.0f;

// =============================================
// MEMORY SYSTEM CONSTANTS
// =============================================

// Working memory constants
constexpr size_t WORKING_MEMORY_MAX_ENTRIES = 1000;
constexpr float WORKING_MEMORY_ACTIVITY_THRESHOLD = 0.1f;
constexpr double WORKING_MEMORY_DECAY_RATE = 0.95f;

// Episodic memory constants
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 10000;
constexpr float EPISODIC_REPLAY_PROBABILITY = 0.1f;  // 10% chance during rest
constexpr double EPISODIC_CONSOLIDATION_THRESHOLD = 0.8f;

// Associative memory constants
constexpr size_t ASSOCIATIVE_MEMORY_MAX_PATTERNS = 5000;
constexpr float ASSOCIATIVE_SIMILARITY_THRESHOLD = 0.7f;
constexpr float ASSOCIATIVE_STRENGTH_DEFAULT = 1.0f;

// =============================================
// DEVELOPMENT CONSTANTS
// =============================================

// Developmental plasticity modifiers
constexpr float PLASTICITY_INITIAL = 1.0f;      // High plasticity
constexpr float PLASTICITY_CRITICAL = 0.8f;   // Reduced plasticity
constexpr float PLASTICITY_MATURE = 0.5f;      // Low plasticity
constexpr float PLASTICITY_ADULT = 0.2f;       // Minimal plasticity
constexpr float PLASTICITY_AGING = 0.1f;       // Very low plasticity

// Structural plasticity rates
constexpr float SYNAPTONGENESIS_RATE_INITIAL = 0.0001f;
constexpr float SYNAPTONGENESIS_RATE_CRITICAL = 0.00005f;
constexpr float SYNAPTONGENESIS_RATE_MATURE = 0.00001f;
constexpr float SYNAPTONGENESIS_RATE_ADULT = 0.000005f;

constexpr float PRUNING_RATE_INITIAL = 0.00001f;
constexpr float PRUNING_RATE_CRITICAL = 0.000005f;
constexpr float PRUNING_RATE_MATURE = 0.0000025f;
constexpr float PRUNING_RATE_ADULT = 0.000001f;

// =============================================
// SENSORIMOTOR CONSTANTS
// =============================================

// Sensory processing constants
constexpr size_t SENSORY_VISION_SIZE = 16;     // 16x16 vision grid
constexpr size_t SENSORY_VISION_DIMS = 256;    // 16*16
constexpr size_t SENSORY_TOUCH_SIZE = 8;       // 8 touch sensors
constexpr size_t SENSORY_INTERNAL_SIZE = 4;    // 4 internal sensors
constexpr size_t SENSORY_PROPRIOCEPTION_SIZE = 6;  // 6 proprioception sensors

// Motor output constants
constexpr size_t MOTOR_ACTION_SIZE = 6;        // 6 possible actions
constexpr float MOTOR_OUTPUT_THRESHOLD = 0.5f;  // Minimum activity for action selection
constexpr float MOTOR_NOISE_LEVEL = 0.01f;     // Random noise in motor output

// Environment constants
constexpr int ENVIRONMENT_DEFAULT_WIDTH = 10;  // 10x10 grid world
constexpr int ENVIRONMENT_DEFAULT_HEIGHT = 10;
constexpr float ENVIRONMENT_GOAL_REWARD = 1.0f;
constexpr float ENVIRONMENT_STEP_COST = -0.01f;  // Small penalty per step
constexpr int ENVIRONMENT_MAX_STEPS = 1000;
constexpr float ENVIRONMENT_TIMEOUT = 0.0f;     // No timeout by default

// =============================================
// COGNITION CONSTANTS
// =============================================

// Neural planning constants
constexpr size_t NEURAL_PLANNER_HORIZON = 10;    // Planning horizon in steps
constexpr float PLANNER_EXPLORATION_FACTOR = 0.1f;
constexpr float PLANNER_EXPLOITATION_FACTOR = 0.9f;
constexpr float ACTION_VALUE_INITIAL = -1000.0f;  // Poor initial value
constexpr float ACTION_VALUE_GOAL_BONUS = 5.0f;

// Concept formation constants
constexpr size_t CONCEPT_MIN_INSTANCES = 3;     // Minimum instances to form concept
constexpr float CONCEPT_STABILITY_THRESHOLD = 0.7f;
constexpr float CONCEPT_SIMILARITY_THRESHOLD = 0.6f;
constexpr size_t CONCEPT_GRID_SIZE = 10;        // Spatial grid for concept formation
constexpr float CONCEPT_ACTIVATION_THRESHOLD = 0.5f;

// Attention constants
constexpr size_t ATTENTION_FOCUS_SIZE = 5;     // How many neurons can be attended
constexpr float ATTENTION_SELECTION_THRESHOLD = 0.3f;
constexpr float ATTENTION_SALIENCY_DECAY = 0.95f;

// =============================================
// PERFORMANCE CONSTANTS
// =============================================

// Performance monitoring
constexpr float PERFORMANCE_UPDATE_RATE = 30.0f;     // Hz
constexpr size_t PERFORMANCE_HISTORY_SIZE = 1000;   // Number of samples to keep
constexpr float PERFORMANCE_UTILIZATION_THRESHOLD = 0.8f;  // 80% CPU utilization

// Visualization constants
constexpr bool VISUALIZATION_ENABLED_DEFAULT = false;
constexpr float VISUALIZATION_UPDATE_RATE_DEFAULT = 30.0f;  // Hz
constexpr size_t VISUALIZATION_FRAME_BUFFER_SIZE = 100;  // Number of frames to buffer
constexpr float VISUALIZATION_SCALE_FACTOR = 1.0f;       // Neuron size scaling

// =============================================
// SYSTEM ADMINISTRATION CONSTANTS
// =============================================

// Logging
constexpr const char* LOG_LEVEL_DEBUG = "DEBUG";
constexpr const char* LOG_LEVEL_INFO = "INFO";
constexpr const char* LOG_LEVEL_WARNING = "WARNING";
constexpr const char* LOG_LEVEL_ERROR = "ERROR";
constexpr const char* LOG_FILENAME_DEFAULT = "nlm.log";
constexpr size_t LOG_MAX_FILE_SIZE = 10 * 1024 * 1024;  // 10MB
constexpr size_t LOG_MAX_MESSAGE_LENGTH = 1024;         // Characters

// Checkpointing
constexpr bool CHECKPOINT_ENABLED_DEFAULT = false;
constexpr const char* CHECKPOINT_FILENAME_DEFAULT = "nlm_checkpoint.bin";
constexpr double CHECKPOINT_SAVE_INTERVAL = 300.0;  // Save every 5 minutes
constexpr size_t CHECKPOINT_MAX_HISTORY = 100;      // Maximum checkpoints to keep

// Random number generation
constexpr uint64_t RNG_SEED_DEFAULT = 42;
constexpr uint64_t RNG_SEED_MIN = 0;
constexpr uint64_t RNG_SEED_MAX = UINT64_MAX;

// =============================================
// UTILITY CONSTANTS
// =============================================

// Mathematical constants
constexpr float MATH_EPSILON = 0.0001f;   // Small value for floating-point comparisons
constexpr float MATH_PI = 3.14159265358979323846f;
constexpr float MATH_2PI = 2.0f * MATH_PI;

// Array/vector operations
constexpr size_t ARRAY_RESIZE_GROWTH_FACTOR = 2;  // For dynamic arrays
constexpr size_t ARRAY_MAX_SIZE = 1000000;        // Prevent memory exhaustion

// Network/communication constants (if applicable)
constexpr size_t NETWORK_BUFFER_SIZE = 1024;       // Bytes
constexpr double NETWORK_TIMEOUT = 5.0;            // Seconds
constexpr int NETWORK_PORT_MIN = 1024;
constexpr int NETWORK_PORT_MAX = 65535;

// =============================================
// BACKWARD COMPATIBILITY MACROS
// =============================================

// Legacy constants for backward compatibility
// These can be mapped to the new constants for gradual migration

// Phase 2 constants (historical)
#define LEGACY_STDP_LTP_WEIGHT 0.01f
#define LEGACY_STDP_LTD_WEIGHT 0.012f
#define LEGACY_SIMULATION_TIMESTEP 0.001

// Phase 3 constants (historical)
#define LEGACY_DEVELOPMENT_Synaptogenesis_RATE 0.001f
#define LEGACY_DEVELOPMENT_PRUNING_RATE 0.00001f

// Neural population constants (historical)
#define LEGACY_NEURON_COUNT 1000
#define LEGACY_REGION_COUNT 2

// =============================================
// CONFIGURATION DEFAULTS
// =============================================

// These values should be used as defaults in the Config system
namespace Defaults {
    // Core simulation
    constexpr double simulation_timestep = SIMULATION_TIMESTEP_DEFAULT;
    constexpr uint64_t random_seed = RNG_SEED_DEFAULT;
    
    // Brain architecture
    constexpr uint64_t neuron_count = 500;        // Reduced for testing
    constexpr uint64_t region_count = 1;
    constexpr float connection_probability = 0.1f;
    
    // Plasticity systems
    constexpr float stdp_ltp_weight = STDP_LTP_WEIGHT_DEFAULT;
    constexpr float stdp_ltd_weight = STDP_LTD_WEIGHT_DEFAULT;
    constexpr float stdp_time_constant = SYNAPTIC_RECOVERY_TIME_CONST;
    constexpr float hebbian_learning_rate = HEBBIAN_LEARNING_RATE_DEFAULT;
    
    // Development
    constexpr float synaptogenesis_rate = SYNAPTONGENESIS_RATE_INITIAL;
    constexpr float pruning_rate = PRUNING_RATE_INITIAL;
    
    // Neuromodulation
    constexpr float dopamine_baseline = REWARD_DOPAMINE_DEFAULT;
    constexpr float reward_discount_factor = 0.99f;
    
    // Environment
    constexpr const char* environment_name = "GridWorld";
    constexpr int environment_width = ENVIRONMENT_DEFAULT_WIDTH;
    constexpr int environment_height = ENVIRONMENT_DEFAULT_HEIGHT;
    
    // Logging
    constexpr const char* log_level = LOG_LEVEL_INFO;
    constexpr bool log_to_file = false;
    constexpr const char* log_filename = LOG_FILENAME_DEFAULT;
    
    // Performance
    constexpr bool visualization_enabled = VISUALIZATION_ENABLED_DEFAULT;
    constexpr float visualization_update_rate = VISUALIZATION_UPDATE_RATE_DEFAULT;
    
    // Simulation limits
    constexpr uint64_t max_simulation_steps = ENVIRONMENT_MAX_STEPS;
    constexpr double simulation_time_limit = ENVIRONMENT_TIMEOUT;
}

// =============================================
// INLINE HELPER FUNCTIONS
// =============================================

// Utility functions for working with constants

// Clamp a value between min and max
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

// Linear interpolation
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Exponential decay
inline float exponentialDecay(float value, float rate, double dt) {
    return value * std::pow(rate, dt);
}

// Normalize value to [0, 1] range given min and max
inline float normalize(float value, float min, float max) {
    if (max <= min) return 0.0f;
    return (value - min) / (max - min);
}

// Denormalize value from [0, 1] range given min and max
inline float denormalize(float value, float min, float max) {
    return min + value * (max - min);
}

// =============================================
// END OF CONSTANTS.HPP
// =============================================
