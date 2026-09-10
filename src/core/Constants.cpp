// Constants implementation file
// Provides implementations for the constants defined in Constants.hpp

#include "Constants.hpp"

namespace nlm {

// Constants implementation

// Learning Experiment Constants
constexpr size_t LEARNING_EXPERIMENT_SPIKE_THRESHOLD = 10;
constexpr float LEARNING_WEIGHT_STRENGTH_THRESHOLD = 0.01f;
constexpr float LEARNING_WEIGHT_CHANGE_THRESHOLD = 0.001f;

// Basic Connectivity Test Constants
constexpr float CONNECTIVITY_INJECT_CURRENT = 50.0f;
constexpr SimulationStep CONNECTIVITY_STEP_COUNT = 50;
constexpr SimulationStep CONNECTIVITY_INJECTED_NEURONS = 10;

// Plasticity Experiment Constants
constexpr SimulationStep PLASTICITY_STEPS = 1000;
constexpr float PLASTICITY_INPUT_CURRENT = 30.0f;
constexpr size_t PLASTICITY_INPUTS_PER_STEP = 20;
constexpr float PLASTICITY_LOG_EVERY_N_STEPS = 100;

// STDP Verification Constants
constexpr size_t STDP_SYNAPSE_TEST_COUNT = 5;
constexpr size_t STDP_SPIKE_PAIR_COUNT = 50;
constexpr float STDP_POTENTIATION_THRESHOLD = 0.001f;
constexpr float STDP_CORRELATED_CURRENT = 60.0f;

// AgentBrain Constants
constexpr size_t SENSE_NEUTRAL_SIZE = 256;  // 16x16 vision
constexpr size_t TOUCH_INPUT_SIZE = 8;
constexpr size_t INTERNAL_INPUT_SIZE = 4;
constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6;
constexpr size_t MOTOR_OUTPUT_SIZE = 6;

constexpr float SENSE_NOVELTY_DECAY = 0.99f;
constexpr float SENSE_CURRENT_SCALE_FACTOR = 5.0f;
constexpr float SENSE_TOUCH_SCALE_FACTOR = 8.0f;
constexpr float SENSE_INTERNAL_SCALE_FACTOR = 5.0f;
constexpr float SENSE_PROPRIOCEPTION_SCALE_FACTOR = 3.0f;

constexpr float CURIOUS_EXPLORATION_THRESHOLD = 0.3f;
constexpr float MAX_CURIOUS_LEVEL = 1.0f;
constexpr float HIGH_CURIOUS_THRESHOLD = 0.5f;
constexpr float EXPLORATION_CHANCE_MULTIPLIER = 0.3f;
constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;

constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
constexpr float MOTOR_CURRENT_SUM_DIVISOR = 1.0f;

// Development Constants
constexpr double DEVELOPMENT_INITIAL_PLACILITY_MULTIPLIER = 1.0;
constexpr double DEVELOPMENT_CRITICAL_PLACILITY_MULTIPLIER = 0.8;
constexpr double DEVELOPMENT_MATURATION_PLACILITY_MULTIPLIER = 0.5;
constexpr double DEVELOPMENT_ADULT_PLACILITY_MULTIPLIER = 0.2;

constexpr double DEVELOPMENT_INITIAL_TIME = 60.0;
constexpr double DEVELOPMENT_CRITICAL_TIME = 300.0;
constexpr double DEVELOPMENT_MATURATION_TIME = 900.0;

constexpr float DEVELOPMENT_INITIAL_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float DEVELOPMENT_CRITICAL_SYNAPTOGENESIS_RATE = 0.00008f;
constexpr float DEVELOPMENT_MATURATION_SYNAPTOGENESIS_RATE = 0.00005f;
constexpr float DEVELOPMENT_ADULT_SYNAPTOGENESIS_RATE = 0.00002f;

constexpr float DEVELOPMENT_INITIAL_PRUNING_RATE = 0.00001f;
constexpr float DEVELOPMENT_CRITICAL_PRUNING_RATE = 0.000012f;
constexpr float DEVELOPMENT_MATURATION_PRUNING_RATE = 0.000015f;
constexpr float DEVELOPMENT_ADULT_PRUNING_RATE = 0.00002f;

// Neuromodulation Constants
constexpr float NEUROMODULATION_DOPAMINE_CLAMP_MIN = -1.0f;
constexpr float NEUROMODULATION_DOPAMINE_CLAMP_MAX = 1.0f;
constexpr float NEUROMODULATION_ELIGIBILITY_DECAY = 0.1f;
constexpr float NEUROMODULATION_PREDICTION_ERROR_WINDOW = 0.95f;
constexpr float NEUROMODULATION_PREDICTION_UPDATE = 0.05f;

// STDP parameters
constexpr float STDP_DEFAULT_LTP_WEIGHT = 0.01f;
constexpr float STDP_DEFAULT_LTD_WEIGHT = 0.012f;

// Goal constants for more advanced users
constexpr int GOAL_DISTANCE_TO_REWARD = 5;
constexpr int GOAL_INTERACTION_SUCCESS = 3;
constexpr int GOAL_ENVIRONMENT_EXPLORATION = 2;

// Performance constants
constexpr size_t PERFORMANCE_BUFFER_SIZE = 1024;
constexpr size_t PERFORMANCE_FRAMEWORK = 64;
constexpr size_t PERFORMANCE_ITERATIONS = 1000;
constexpr float PERFORMANCE_MIN_TIME = 1e-6;
constexpr float PERFORMANCE_MAX_TIME = 1.0f;

// Checkpoint constants
constexpr size_t CHECKPOINT_VERSION = 1;
constexpr size_t CHECKPOINT_MAGIC = 0x4E4C4D00;  // 'NLM\0'
constexpr size_t CHECKPOINT_SIZE = 4096;

// Visualization constants
constexpr float VISUALIZATION_MIN_COORD = -10.0f;
constexpr float VISUALIZATION_MAX_COORD = 10.0f;
constexpr size_t VISUALIZATION_MIN_SIZE = 1;
constexpr size_t VISUALIZATION_MAX_SIZE = 100;

} // namespace nlm
