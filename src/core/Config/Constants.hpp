#pragma once

// NLM Configuration Constants
// This file contains all configurable parameters to eliminate magic numbers
// and provide better maintainability and documentation

namespace nlm {
namespace config {

// Simulation constants
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;  // seconds
constexpr double DEFAULT_SIMULATION_STEP_MULTIPLIER = 1.0;  // scaling factor
constexpr size_t DEFAULT_MAX_SIMULATION_STEPS = 10000;

// Brain constants  
constexpr size_t DEFAULT_NEURON_COUNT = 1000;
constexpr size_t DEFAULT_MIN_NEURON_COUNT = 100;
constexpr size_t DEFAULT_MAX_NEURON_COUNT = 10000;
constexpr size_t DEFAULT_REGION_COUNT = 2;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
constexpr float MIN_CONNECTION_PROBABILITY = 0.0f;
constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;

// Plasticity constants
constexpr float DEFAULT_PLASTICITY_LEARNING_RATE = 0.01f;
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
constexpr float MIN_STDP_WEIGHT = 0.0f;
constexpr float MAX_STDP_WEIGHT = 0.5f;

// Development constants
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.001f;
constexpr float DEFAULT_PRUNING_RATE = 0.0001f;
constexpr float MIN_DEVELOPMENT_RATE = 0.0f;
constexpr float MAX_DEVELOPMENT_RATE = 1.0f;

// Neuromodulation constants
constexpr float DEFAULT_DOPAMINE_BASELINE = 0.0f;
constexpr float NEUROMODULATION_BASELINE_MIN = -1.0f;
constexpr float NEUROMODULATION_BASELINE_MAX = 1.0f;

// Reward constants
constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;
constexpr float MIN_REWARD_DISCOUNT = 0.0f;
constexpr float MAX_REWARD_DISCOUNT = 1.0f;

// Environment constants
constexpr const char* DEFAULT_ENVIRONMENT_NAME = "GridWorld";
constexpr size_t DEFAULT_ENVIRONMENT_WIDTH = 10;
constexpr size_t DEFAULT_ENVIRONMENT_HEIGHT = 10;
constexpr size_t MIN_ENVIRONMENT_SIZE = 1;
constexpr size_t MAX_ENVIRONMENT_SIZE = 1000;

// Visualization constants
constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;
constexpr double DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0;  // Hz
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log";

// Random constants
constexpr int64_t DEFAULT_RANDOM_SEED = 42;
constexpr int64_t MIN_RANDOM_SEED = 0;
constexpr int64_t MAX_RANDOM_SEED = 2147483647;

// Testing constants (for experiments)
constexpr float DEFAULT_EXPERIMENT_INPUT_STRENGTH = 50.0f;
constexpr float DEFAULT_PLASTICITY_EXPERIMENT_STRENGTH = 30.0f;
constexpr float DEFAULT_STDP_TEST_STRENGTH = 60.0f;
constexpr size_t DEFAULT_PLASTICITY_EXPERIMENT_STEPS = 1000;
constexpr size_t DEFAULT_STDP_TEST_TRIALS = 50;
constexpr size_t DEFAULT_CONNECTIVITY_TEST_STEPS = 50;
constexpr size_t MIN_TEST_STEPS = 1;
constexpr size_t MAX_TEST_STEPS = 100000;

// Logging constants
constexpr const char* DEFAULT_LOG_LEVEL = "INFO";
constexpr bool DEFAULT_LOG_TO_FILE = false;
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 4096;
constexpr size_t MAX_LOG_BUFFER_SIZE = 1024 * 1024;  // 1MB

// Neural constants
constexpr float DEFAULT_MEMBRANE_POTENTIAL_THRESHOLD = 20.0f;
constexpr float DEFAULT_MEMBRANE_POTENTIAL_RESTING = -70.0f;
constexpr float DEFAULT_MEMBRANE_TIME_CONSTANT = 10.0f;
constexpr float MIN_MEMBRANE_THRESHOLD = -100.0f;
constexpr float MAX_MEMBRANE_THRESHOLD = 100.0f;
constexpr float DEFAULT_SYNAPTIC_WEIGHT_MIN = -1.0f;
constexpr float DEFAULT_SYNAPTIC_WEIGHT_MAX = 1.0f;

// Performance constants
constexpr size_t MAX_SIMULTANEOUS_SYNAPSES = 100000;
constexpr size_t MAX_SIMULTANEOUS_NEURONS = 50000;
constexpr double MAX_SIMULATION_TIME_LIMIT = 3600.0;  // 1 hour in seconds

} // namespace config
} // namespace nlm