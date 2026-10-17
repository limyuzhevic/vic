#pragma once

namespace nlm {

// Sensory input dimensions (configurable via AgentBrain::getSensoryInputSize)
constexpr size_t DEFAULT_VISION_WIDTH = 16;
constexpr size_t DEFAULT_VISION_HEIGHT = 16;
constexpr size_t DEFAULT_VISION_DIMENSIONS = DEFAULT_VISION_WIDTH * DEFAULT_VISION_HEIGHT; // 256
constexpr size_t DEFAULT_TOUCH_DIMENSIONS = 8;
constexpr size_t DEFAULT_INTERNAL_DIMENSIONS = 4;
constexpr size_t DEFAULT_PROPRIOCEPTION_DIMENSIONS = 6;

// Motor output dimensions (configurable via AgentBrain::getMotorOutputSize)
constexpr size_t DEFAULT_MOTOR_DIMENSIONS = 6; // One motor neuron per action

// Sensory scaling factors for input transduction
constexpr float VISION_INPUT_SCALE = 5.0f;          // Scale vision pixel intensity to neural current
constexpr float TOUCH_INPUT_SCALE = 8.0f;           // Scale touch collision signal to neural current
constexpr float INTERNAL_INPUT_CENTER = 2.0f;        // Center internal signal around zero
constexpr float INTERNAL_INPUT_SCALE = 5.0f;         // Scale internal signal to neural current
constexpr float PROPRIOCEPTION_INPUT_CENTER = 2.0f;  // Center proprioception around zero
constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f;   // Scale proprioception to neural current

// Neuromodulation and learning constants
constexpr float DEFAULT_NEGATIVE_REWARD = -1000.0f; // Large negative penalty for failure
constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;    // Default neuromodulation level
constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;       // Novelty decay rate per timestep
constexpr float DEFAULT_PREDICTION_ERROR_CLAMP = 1.0f; // Clamp prediction error to [-1, 1]
constexpr float DEFAULT_DOPAMINE_CLAMP = 1.0f;        // Clamp dopamine signal to [-1, 1]

// Curiosity and exploration constants
constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;   // Maximum random exploration probability
constexpr float DEFAULT_MIN_CURIOUSITY_THRESHOLD = 0.3f; // Minimum curiosity for exploration
constexpr float DEFAULT_MAX_CURIOUSITY_THRESHOLD = 0.5f; // Maximum curiosity for exploration

// Activity and detection thresholds
constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.5f;   // Minimum neural activity to trigger action
constexpr float DEFAULT_SPIKE_THRESHOLD = 0.0f;       // Membrane potential threshold for spiking
constexpr float DEFAULT_RESTING_POTENTIAL = -0.65f;   // Default resting membrane potential (normalized)
constexpr float DEFAULT_RESET_POTENTIAL = 100.0f;     // Maximum membrane potential before reset

// Developmental constants
constexpr double DEVELOPMENT_INITIAL_STAGE = 0.0;     // Age (seconds) when development starts
constexpr double DEVELOPMENT_CRITICAL_PERIOD_END = 60.0;  // End of critical period
constexpr double DEVELOPMENT_MATURATION_END = 300.0;    // End of maturation phase
constexpr double DEVELOPMENT_ADULT_START = 900.0;      // Start of adulthood

// Plasticity constants
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;     // Spike-timing dependent plasticity LTP weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;    // STDP LTD weight
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;  // STDP time constant in ms
constexpr float DEFAULT_PLACITY_MODIFIER_RANGE = 2.0f; // Range of plasticity modifier effects
constexpr float DEFAULT_ELIGIBILITY_TRACE_DECAY = 0.1f; // Decay rate for eligibility traces

// Reward modulation constants
constexpr float DEFAULT_REWARD_MOVING_AVERAGE_ALPHA = 0.05f; // Weight for new rewards in moving average
constexpr float DEFAULT_PLACITY_FACTOR_BASE = 0.5f;      // Base plasticity factor
constexpr float DEFAULT_PLACITY_FACTOR_RANGE = 2.0f;      // Range of plasticity factor effects

// Structural plasticity constants
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;    // Rate of new synapse formation
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // Rate of synapse elimination

// Configuration defaults
constexpr size_t DEFAULT_RANDOM_SEED = 42;              // Default random seed for reproducibility
constexpr size_t DEFAULT_NEURON_COUNT = 1000;           // Default number of neurons
constexpr size_t DEFAULT_REGION_COUNT = 2;              // Default number of brain regions
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;  // Default connection probability
constexpr float DEFAULT_PLASTICITY_LEARNING_RATE = 0.01f; // Default plasticity learning rate
constexpr float DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f; // Development synaptogenesis rate
constexpr float DEFAULT_DEVELOPMENT_PRUNING_RATE = 0.0001f;   // Development pruning rate

// Simulation constants
constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;   // Default simulation timestep in seconds
constexpr size_t DEFAULT_MAX_SIMULATION_STEPS = 10000;   // Maximum simulation steps
constexpr double DEFAULT_SIMULATION_TIME_LIMIT = 0.0;     // Simulation time limit (0 = unlimited)

// Visualization constants
constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;    // Default visualization state
constexpr float DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0f; // Frames per second for visualization

// World constants
constexpr size_t DEFAULT_ENVIRONMENT_WIDTH = 10;        // Default world width
constexpr size_t DEFAULT_ENVIRONMENT_HEIGHT = 10;       // Default world height
constexpr size_t DEFAULT_VISION_WIDTH = 16;              // Default vision sensor width
constexpr size_t DEFAULT_VISION_HEIGHT = 16;             // Default vision sensor height
constexpr float DEFAULT_MAX_ENERGY = 100.0f;              // Maximum agent energy
constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;       // Energy decay per timestep

// Motor action constants
constexpr float DEFAULT_MOVE_SPEED = 3.0f;              // Base movement speed
constexpr float DEFAULT_TURN_SPEED = 2.0f;              // Base turning speed
constexpr float DEFAULT_MAX_SPEED = 5.0f;               // Maximum movement speed
constexpr float DEFAULT_MIN_SPEED = 0.1f;               // Minimum movement speed
constexpr float DEFAULT_MAX_TURN_RATE = 0.5f;           // Maximum turning rate (radians per timestep)
constexpr float DEFAULT_MIN_TURN_RATE = 0.01f;          // Minimum turning rate (radians per timestep)

// Reward system constants
constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f; // Discount factor for future rewards
constexpr float DEFAULT_REWARD_MAX = 1000.0f;           // Maximum positive reward
constexpr float DEFAULT_REWARD_MIN = -1000.0f;          // Maximum negative reward

// Error handling constants
constexpr float DEFAULT_INVALID_REWARD = -1000.0f;      // Sentinel value for invalid rewards
constexpr float DEFAULT_INVALID_CURRENT = -1000.0f;    // Sentinel value for invalid currents
constexpr float DEFAULT_INVALID_WEIGHT = -1000.0f;      // Sentinel value for invalid weights

// Logging and debugging
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log"; // Default log filename
constexpr const char* DEFAULT_LOG_LEVEL = "INFO";      // Default log level
constexpr bool DEFAULT_LOG_TO_FILE = false;            // Default logging to file setting

} // namespace nlm
