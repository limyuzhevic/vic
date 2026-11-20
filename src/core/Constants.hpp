#pragma once

namespace nlm {
namespace Constants {

// Configuration constants
constexpr int64_t DEFAULT_RANDOM_SEED = 42;
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
constexpr int64_t DEFAULT_NEURON_COUNT = 1000;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;

// Plasticity constants
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
constexpr float DEFAULT_STDP_MIN_WEIGHT = -1.0f;
constexpr float DEFAULT_STDP_MAX_WEIGHT = 1.0f;

constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
constexpr float DEFAULT_HEBBIAN_MAX_WEIGHT = 1.0f;
constexpr float DEFAULT_HEBBIAN_MIN_WEIGHT = -1.0f;

constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// Neuron constants
constexpr float NEURON_RESTING_POTENTIAL = -70.0f;
constexpr float NEURON_THRESHOLD = -55.0f;
constexpr float NEURON_RESET_POTENTIAL = -75.0f;
constexpr float NEURON_LEAK_CONDUCTANCE = 10.0f;
constexpr float NEURON_MIN_EXCITATORY_WEIGHT = 0.01f;
constexpr float NEURON_MAX_INHIBITORY_WEIGHT = -0.01f;

// Refractory periods
constexpr uint32_t DEFAULT_REFACTORY_PERIOD = 9;
constexpr uint32_t CRITICAL_PERIOD_STEPS = 60;
constexpr uint32_t MATURATION_PERIOD_STEPS = 300;
constexpr uint32_t ADULT_DEVELOPMENT_STEPS = 600;

// Memory constants
constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
constexpr size_t DEFAULT_MAX_EPISODIC_EPISODES = 1000;

// Logging constants
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;

// Performance constants
constexpr float EXCITATORY_WEIGHT_CAP = 3.0f;

// Time constants (ms)
constexpr double DEVELOPMENT_TIME_INITIAL = 60.0;
constexpr double DEVELOPMENT_TIME_CRITICAL_PERIOD = 300.0;
constexpr double DEVELOPMENT_TIME_MATURATION = 600.0;

} // namespace Constants
} // namespace nlm
