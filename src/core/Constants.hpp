#pragma once

namespace nlm {
namespace constants {

// Experiment Configuration Constants
constexpr int DEFAULT_RANDOM_SEED = 42;
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
constexpr int64_t DEFAULT_NEURON_COUNT = 500;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;

// Plasticity Parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
constexpr float DEFAULT_LEARNING_RATE = 0.01f;

// Learning Thresholds
constexpr float DEFAULT_LEARNING_THRESHOLD = 0.01f;
constexpr float DEFAULT_SPIKE_THRESHOLD = 1.0f;
constexpr float DEFAULT_WEIGHT_CHANGE_THRESHOLD = 0.01f;
constexpr float DEFAULT_WEIGHT_DIFFERENCE_THRESHOLD = 0.001f;

// Experiment Parameters
constexpr float DEFAULT_INPUT_STRENGTH = 50.0f;
constexpr int DEFAULT_OUTPUT_NEURONS = 10;
constexpr int DEFAULT_BASIC_CONNECTIVITY_STEPS = 50;
constexpr int DEFAULT_PLASTICITY_EXPERIMENT_STEPS = 1000;
constexpr int DEFAULT_STDP_VERIFICATION_TRIALS = 50;
constexpr int DEFAULT_PLASTICITY_INPUT_STIMULI = 20;
constexpr float DEFAULT_PLASTICITY_CURRENT = 30.0f;
constexpr float DEFAULT_STDP_CORRELATED_CURRENT = 60.0f;

// Progress Reporting
constexpr int DEFAULT_PROGRESS_REPORT_INTERVAL = 100;
constexpr int MIN_NEURON_COUNT_FOR_VALID_EXPERIMENTS = 10;
constexpr float MAX_REASONABLE_SIMULATION_TIMESTEP = 0.1f;
constexpr float MIN_REASONABLE_SIMULATION_TIMESTEP = 1e-6f;

// Logging Constants
constexpr int LOG_LEVEL_ERROR = 0;
constexpr int LOG_LEVEL_WARNING = 1;
constexpr int LOG_LEVEL_INFO = 2;
constexpr int LOG_LEVEL_DEBUG = 3;
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log";
constexpr size_t DEFAULT_LOG_BUFFER_SIZE = 4096;

// Experiment Validation
constexpr float MIN_STDP_WEIGHT_CHANGE = 0.001f;
constexpr float MAX_WEIGHT_VARIATION_PERCENT = 0.5f;
constexpr int MAX_CONCURRENT_EXPERIMENTS = 3;

// System Limits
constexpr int MAX_MEMORY_MB = 512;
constexpr double MAX_SIMULATION_TIME_SECONDS = 300.0; // 5 minutes

} // namespace constants
} // namespace nlm