// NLM System Constants
// Centralized configuration for magic numbers and hardcoded values
// This improves maintainability and makes parameters easily configurable

#ifndef NLM_SYSTEM_CONSTANTS_HPP
#define NLM_SYSTEM_CONSTANTS_HPP

namespace nlm {

// ============================================================================
// NEURAL DYNAMICS CONSTANTS
// ============================================================================

// Neuron membrane and electrical properties
constexpr float NEURON_TIME_CONSTANT = 20.0f;           // Membrane time constant (ms)
constexpr float NEURON_MEMBRANE_CAPACITANCE = 1.0f;     // Membrane capacitance (nF)
constexpr float RESTING_POTENTIAL = -65.0f;            // Resting membrane potential (mV)
constexpr float THRESHOLD_POTENTIAL = -50.0f;          // Spike threshold (mV)
constexpr float REFractory_PERIOD = 5.0f;              // Refractory period (ms)
constexpr float SYNAPTIC_DELAY = 1.0f;                 // Synaptic transmission delay (ms)

// ============================================================================
// SENSORY PROCESSING CONSTANTS
// ============================================================================

// Vision processing
constexpr size_t VISION_WIDTH = 16;                    // Vision input grid width
constexpr size_t VISION_HEIGHT = 16;                   // Vision input grid height
constexpr size_t VISION_CHANNELS = 3;                  // Vision channels (RGB)
constexpr size_t VISION_TOTAL_PIXELS = VISION_WIDTH * VISION_HEIGHT;
constexpr float VISION_SCALE_FACTOR = 5.0f;            // Vision intensity scaling

// Touch processing
constexpr size_t TOUCH_TOTAL_SENSORS = 8;              // Number of touch sensors
constexpr float TOUCH_SCALE_FACTOR = 8.0f;             // Touch signal scaling

// Internal signals
constexpr size_t INTERNAL_SIGNALS_COUNT = 4;           // Number of internal signals
constexpr float INTERNAL_SCALE_FACTOR = 5.0f;          // Internal signal scaling

// Proprioception
constexpr size_t PROPRIOCEPTION_COUNT = 6;             // Number of proprioceptive sensors
constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;    // Proprioception scaling

// General sensory normalization
constexpr float SENSORY_CENTER_OFFSET = 1.0f;           // For centering signals from [0,1] to [-1,1]
constexpr float SENSORY_MAX_RANGE = 2.0f;               // Full range for normalization

// ============================================================================
// MOTOR SYSTEM CONSTANTS
// ============================================================================

// Motor action groups
constexpr size_t MOTOR_ACTION_GROUPS = 6;              // Number of motor actions (Forward, Backward, Left, Right, Interact, Wait)
constexpr size_t MOTOR_OUTPUT_SIZE = MOTOR_ACTION_GROUPS;

// Action distribution
constexpr float MOTOR_NEuron_DISTRIBUTION_MODULO = MOTOR_ACTION_GROUPS;

// Activity thresholds and scaling
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;       // Minimum activity for motor activation
constexpr float MOTOR_MAX_ACTIVITY = 10.0f;            // Maximum expected membrane potential deviation

// ============================================================================
// NEUROMODULATION CONSTANTS
// ============================================================================

// Dopamine and reward processing
constexpr float DOPAMINE_MIN = -1.0f;                  // Minimum dopamine level
constexpr float DOPAMINE_MAX = 1.0f;                   // Maximum dopamine level
constexpr float REWARD_PREDICTION_ALPHA = 0.95f;       // EMA smoothing factor for expected reward
constexpr float REWARD_PREDICTION_BETA = 0.05f;       // EMA complementary factor
constexpr float PREDICTION_ERROR_WEIGHT = 1.0f;         // Weight of prediction error in neuromodulation

// Curiosity and exploration
constexpr float CURIOSITY_VISION_WEIGHT = 2.0f;         // Weight of novelty in curiosity calculation
constexpr float CURIOSITY_PREDICTION_WEIGHT = 0.5f;    // Weight of prediction error in curiosity
constexpr float CURIOSITY_MAX_LEVEL = 1.0f;             // Maximum curiosity level
constexpr float CURIOSITY_THRESHOLD_EXPLORATION = 0.3f; // Curiosity level threshold for exploration
constexpr float CURIOSITY_THRESHOLD_HIGH = 0.5f;       // High curiosity threshold for random exploration
constexpr float EXPLORATION_MAX_CHANCE = 0.3f;         // Maximum exploration probability
constexpr float NOVELTY_DECAY_RATE = 0.99f;            // Novelty decay per timestep

// ============================================================================
// PLASTICITY CONSTANTS
// ============================================================================

// Synaptic plasticity
constexpr float STDP_LTP_BASE = 0.01f;                 // Base Long-Term Potentiation weight
constexpr float STDP_LTD_BASE = 0.012f;                // Base Long-Term Depression weight
constexpr float PLpASTICITY_BASE_MODIFIER = 0.5f;      // Base plasticity modifier
constexpr float PLpASTICITY_MIN_MODIFIER = 0.1f;       // Minimum plasticity modifier
constexpr float PLpASTICITY_MAX_MODIFIER = 2.0f;       // Maximum plasticity modifier
constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;  // Minimum eligibility trace for plasticity
constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;        // Eligibility trace decay rate

// Structural plasticity
constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;    // Base synaptogenesis rate
constexpr float PRUNING_BASE_RATE = 0.00001f;         // Base pruning rate
constexpr float SYNAPTOGENESIS_MODIFIER = 1.0f;        // Synaptogenesis modifier factor
constexpr float PRUNING_MODIFIER = 2.0f;               // Pruning modifier factor

// Hebbian plasticity
constexpr float HEBBIAN_STRENGTH = 0.01f;              // Hebbian learning rate
constexpr float HEBBIAN_DECAY = 0.995f;                // Hebbian weight decay

// ============================================================================
// DEVELOPMENTAL CONSTANTS
// ============================================================================

// Developmental stages
constexpr double DEVELOPMENTAL_STAGE_1_END = 60.0;    // ~1 minute - Initial stage
constexpr double DEVELOPMENTAL_STAGE_2_END = 300.0;   // ~5 minutes - Critical period
constexpr double DEVELOPMENTAL_STAGE_3_END = 900.0;   // ~15 minutes - Maturation stage
// Adult stage continues beyond this

// Developmental stage modifiers
constexpr float DEVELOPMENTAL_STAGE_1_MODIFIER = 1.0f;  // High plasticity
constexpr float DEVELOPMENTAL_STAGE_2_MODIFIER = 0.8f;  // Moderate plasticity
constexpr float DEVELOPMENTAL_STAGE_3_MODIFIER = 0.5f;  // Lower plasticity
constexpr float DEVELOPMENTAL_STAGE_ADULT_MODIFIER = 0.2f; // Stable adult

// ============================================================================
// SIMULATION CONSTANTS
// ============================================================================

// Time and simulation
constexpr double SIMULATION_TIMESTEP = 0.001;         // Simulation timestep (seconds)
constexpr double SIMULATION_STEP_SIZE = 1.0;          // Brain step size (arbitrary units)

// State management
constexpr size_t MAX_PREVIOUS_VISION_SIZE = 256;      // Maximum vision history storage
constexpr float INITIAL_PLASITY_MODIFIER = 1.0f;      // Initial plasticity value
constexpr float INITIAL_DOPAMINE = 0.0f;              // Initial dopamine level
constexpr float INITIAL_NOVELTY = 0.0f;               // Initial novelty level
constexpr float INITIAL_CURIOUSITY = 0.0f;            // Initial curiosity level
constexpr float INITIAL_PREDICTION_ERROR = 0.0f;      // Initial prediction error

// ============================================================================
// WORLD AND ENVIRONMENT CONSTANTS
// ============================================================================

// World configuration
constexpr size_t DEFAULT_WORLD_WIDTH = 10;            // Default world width
constexpr size_t DEFAULT_WORLD_HEIGHT = 10;           // Default world height
constexpr size_t DEFAULT_VISION_WIDTH = 8;            // Default vision width
constexpr size_t DEFAULT_VISION_HEIGHT = 8;           // Default vision height
constexpr float DEFAULT_MAX_ENERGY = 100.0f;           // Default maximum energy
constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;    // Default energy decay rate
constexpr float DEFAULT_SIMULATION_TIME = 0.0f;       // Initial simulation time

// ============================================================================
// ERROR HANDLING CONSTANTS
// ============================================================================

// Validation thresholds
constexpr float NULL_POINTER_CHECK_ENABLED = true;   // Enable null pointer checks in debug
constexpr float BOUNDS_CHECK_ENABLED = true;          // Enable bounds checking in debug
constexpr float ASSERTION_ENABLED = true;             // Enable assertions for debugging

// ============================================================================
// PERFORMANCE OPTIMIZATION CONSTANTS
// ============================================================================

// Memory and caching
constexpr size_t VECTOR_RESERVE_FACTOR = 2;           // Vector reserve factor for pre-allocation
constexpr float CACHE_LINE_SIZE = 64.0f;              // CPU cache line size for optimization
constexpr size_t SIMD_ALIGNMENT = 32;                 // SIMD alignment requirement

// ============================================================================
// LOGGING AND DEBUGGING CONSTANTS
// ============================================================================

// Logging thresholds and levels
constexpr int LOG_LEVEL_DEBUG = 0;                    // Debug log level
constexpr int LOG_LEVEL_INFO = 1;                     // Info log level
constexpr int LOG_LEVEL_WARNING = 2;                  // Warning log level
constexpr int LOG_LEVEL_ERROR = 3;                    // Error log level
constexpr size_t LOG_MESSAGE_MAX_LENGTH = 1024;      // Maximum log message length

} // namespace nlm

#endif // NLM_SYSTEM_CONSTANTS_HPP

// ============================================================================
// USAGE EXAMPLES:
// ============================================================================

// Old way (magic numbers):
//   float current = vision[i] * 5.0f;
//   MotorCommand bestActivity = 0.5f;

// New way (named constants):
//   float current = vision[i] * nlm::VISION_SCALE_FACTOR;
//   MotorCommand bestActivity = nlm::MOTOR_ACTIVITY_THRESHOLD;

// ============================================================================
// CONFIGURATION OVERRIDE:
// ============================================================================

// These constants can be overridden in configuration files by defining them before
// including this header, or by creating specialized configuration headers for
// different simulation regimes (research, production, debugging, etc.)
