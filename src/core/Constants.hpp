// Constants and configuration values for NLM
// Centralized configuration to eliminate magic numbers and improve maintainability

#pragma once

#include <string>
#include "core/Types/Types.hpp"

namespace nlm {
namespace constants {

// Simulation parameters
constexpr double DEFAULT_TIMESTEP = 0.001;  // 1ms timestep
constexpr double DEFAULT_SIMULATION_TIME = 1000.0;  // Default simulation duration
constexpr double PI = 3.14159265358979323846;

// Neuron parameters
constexpr float DEFAULT_REFRACTORY_PERIOD = 9.0f;  // Steps
constexpr float DEFAULT_THRESHOLD = -55.0f;  // mV
constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;  // mV
constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;  // nS
constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms

// Vision parameters
constexpr size_t DEFAULT_VISION_WIDTH = 16;  // Vision rays
constexpr size_t DEFAULT_VISION_HEIGHT = 16;
constexpr float VISION_EMPTY_VALUE = 1.0f;  // Represents empty vision
constexpr float VISION_OBJECT_THRESHOLD = 0.5f;  // Resource detection
constexpr float VISION_HAZARD_THRESHOLD = 0.35f;  // Hazard detection
constexpr float VISION_BRIGHTNESS_THRESHOLD = 0.3f;  // Bright approach

// Touch/proximity parameters
constexpr size_t DEFAULT_TOUCH_SENSOR_COUNT = 8;
constexpr float TOUCH_EMPTY_VALUE = 0.0f;  // No touch
constexpr float TOUCH_THRESHOLD = 0.05f;  // Interaction range
constexpr float FORWARD_TOUCH_SENSORS[] = {0, 1, 7};  // Forward cone sensors

// Internal signals
constexpr size_t DEFAULT_INTERNAL_SIGNAL_COUNT = 4;

// Proprioception parameters
constexpr size_t DEFAULT_PROPRIOCEPTOR_COUNT = 6;

// Audio parameters
constexpr size_t DEFAULT_AUDIO_SIGNAL_COUNT = 0;  // Currently disabled

// Motor parameters
constexpr size_t DEFAULT_MOTOR_GROUP_COUNT = 6;  // forward, backward, turn left/right, interact, wait
constexpr float MOTOR_WAIT_GROUP_BIAS = 0.0f;  // Wait group has no bias

// Plasticity parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_HOPPB_COUPLING_STRENGTH = 0.001f;

// Neuromodulation parameters
constexpr float DEFAULT_DOPAMINE_SCALE = 1.0f;
constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;  // Habituation decay rate
constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;  // For curiosity activation
constexpr float DEFAULT_PREDICTION_ERROR_DECAY = 0.05f;  // Learning rate for predictions

// Development parameters
constexpr double DEVELOPMENT_INITIAL_STAGE = 60.0;  // ~1 minute
constexpr double DEVELOPMENT_CRITICAL_PERIOD_END = 300.0;  // ~5 minutes
constexpr double DEVELOPMENT_MATURATION_END = 900.0;  // ~15 minutes
constexpr float DEVELOPMENT_PLATONICITY_INITIAL = 1.0f;
constexpr float DEVELOPMENT_PLATONICITY_ADULT = 0.2f;

// Memory parameters
constexpr size_t MAX_SPIKE_HISTORY = 100;
constexpr size_t DEFAULT_WORKING_MEMORY_SIZE = 100;
constexpr size_t DEFAULT_EPISODIC_MEMORY_SIZE = 1000;
constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SIZE = 500;

// Concept formation parameters
constexpr float CONCEPT_FORMATION_THRESHOLD = 0.01f;  // Minimum pattern strength
constexpr float CONCEPT_REWARD_THRESHOLD = 0.1f;  // For concept biasing
constexpr float CONCEPT_NEGATIVE_THRESHOLD = -0.1f;  // For hazard concepts

// Social learning parameters
constexpr float SOCIAL_SIMILARITY_THRESHOLD = 0.8f;  // For imitation
constexpr size_t MAX_SOCIAL_OBSERVATIONS = 100;

// Text encoding parameters
constexpr size_t TEXT_ENCODER_NEIGHBOR_COUNT = 4;  // Number of text neurons driven
constexpr size_t TEXT_ENCODER_TOTAL_NEURONS = 15;  // Available text neurons
constexpr size_t TEXT_START_INDEX = 16;  // First text neuron index

// Logging parameters
constexpr size_t MAX_LOG_HISTORY = 1000;
constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 4096;

// Performance parameters
constexpr size_t MIN_PERFORMANCE_SAMPLE_SIZE = 100;
constexpr double PERFORMANCE_CONFIDENCE_INTERVAL = 0.95;

// Configuration file defaults
constexpr const char* DEFAULT_CONFIG_FILE = "nlm_config.json";
constexpr const char* DEFAULT_CHECKPOINT_FILE = "nlm_checkpoint.bin";

// String constants
constexpr const char* STRING_NONE = "none";
constexpr const char* STRING_EMPTY = "";

// Error messages
constexpr const char* ERROR_INVALID_PARAMETER = "Invalid parameter value";
constexpr const char* ERROR_INITIALIZATION_FAILED = "Initialization failed";
constexpr const char* ERROR_CONFIG_NOT_FOUND = "Configuration file not found";
constexpr const char* ERROR_INVALID_OPERATION = "Invalid operation for current state";

// Success messages
constexpr const char* SUCCESS_INITIALIZED = "Successfully initialized";
constexpr const char* SUCCESS_COMPLETED = "Successfully completed";
constexpr const char* SUCCESS_SAVED = "Successfully saved";

// Debug flags (can be enabled via configuration)
constexpr bool DEBUG_MODE_DEFAULT = false;
constexpr bool VERBOSE_LOGGING_DEFAULT = false;
constexpr bool PERFORMANCE_LOGGING_DEFAULT = false;

// Conversion factors
constexpr float MILLIVOLT_TO_VOLT = 0.001f;
constexpr float SECONDS_TO_MILLISECONDS = 1000.0f;
constexpr float HERTZ_TO_KILOHERTZ = 0.001f;
constexpr float PERCENT_TO_DECIMAL = 0.01f;

// Safety limits
constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;  // Upper limit for voltage
constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // Lower limit for voltage
constexpr float MAX_FIRING_RATE = 1000.0f;  // Maximum firing rate in Hz
constexpr float MIN_FIRING_RATE = 0.0f;  // Minimum firing rate

// Experimental parameters (for research use)
constexpr size_t EXPERIMENT_MAX_STEPS = 10000;
constexpr float EXPERIMENT_DEFAULT_REWARD = 10.0f;
constexpr float EXPERIMENT_DEFAULT_PUNISHMENT = -15.0f;
constexpr float EXPERIMENT_NOVELTY_SENSITIVITY = 0.05f;

// Scaling factors (empirically determined)
constexpr float DRIVE_SCALING_FACTOR = 0.85f;  // For motor group activity
constexpr float NOVELTY_SENSITIVITY_FACTOR = 2.0f;  // For curiosity calculation
constexpr float REWARD_SCALING_FACTOR = 10.0f;  // For reward prediction error
constexpr float BIAS_MULTIPLIER = 5.0f;  // For learned preference effects

// Time-related constants (in simulation steps)
constexpr size_t TURN_DIVERSITY_THRESHOLD = 25;  // Steps without turning
constexpr size_t REPETITION_FATIGUE_THRESHOLD = 3;  // Consecutive same actions
constexpr size_t MOMENTUM_BIAS_DURATION = 1;  // Steps for reversal penalty

// Window sizes for calculations (in simulation steps)
constexpr size_t NOVELTY_PREDICTION_WINDOW = 20;  // How long to remember predictions
constexpr size_t CONSOLIDATION_WINDOW = 100;  // Memory consolidation window
constexpr size_t REPLAY_INTERVAL = 50;  // Memory replay interval

// Numerical precision limits
constexpr float EPSILON = 0.000001f;  // For floating-point comparisons
constexpr float FLOAT_COMPARE_THRESHOLD = 1e-5f;  // For equality checks

// System resource limits
constexpr size_t MAX_CONCURRENT_THREADS = 8;
constexpr size_t MAX_MEMORY_POOL_SIZE = 1000000;  // For memory pool optimization
constexpr size_t MAX_EVENT_QUEUE_SIZE = 10000;  // For event-driven simulation

// Visualization parameters
constexpr float VISUALIZATION_SCALE = 1.0f;  // For brain visualization
constexpr float VISUALIZATION_OPACITY = 0.8f;  // For overlay visualization
constexpr size_t VISUALIZATION_MAX_FRAMES = 1000;  // For animation

// Output format specifications
constexpr const char* OUTPUT_FORMAT_TEXT = "text";
constexpr const char* OUTPUT_FORMAT_JSON = "json";
constexpr const char* OUTPUT_FORMAT_CSV = "csv";
constexpr const char* OUTPUT_FORMAT_PNG = "png";
constexpr const char* OUTPUT_FORMAT_HTML = "html";

} // namespace constants
} // namespace nlm
