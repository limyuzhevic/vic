// Configuration Constants for NLM
// Standardized values throughout the codebase
// Makes it easier to configure and use the NLM brain

#pragma once

namespace nlm {
namespace ConfigConstants {

// Simulation parameters
constexpr double DEFAULT_TIMESTEP = 0.001;          // seconds (1ms)
constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;     // steps
constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // steps

// Neural parameters
constexpr size_t DEFAULT_NEURON_COUNT = 1000;
constexpr size_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
constexpr float DEFAULT_SYNAPT Genesis_RATE = 0.0001f;
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// Plasticity parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
constexpr float DEFAULT_STDP_TAU = 20.0f;

// Memory parameters
constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;

// Development parameters
constexpr size_t DEFAULT_DEVELOPMENT_MIN_AGE = 0;
constexpr size_t DEFAULT_DEVELOPMENT_MAX_AGE = 1000;
constexpr float DEFAULT_DEVELOPMENT_MATURATION_RATE = 0.1f;

// Neuromodulation parameters
constexpr float DEFAULT_DOPAMINE_BASE_LEVEL = 0.1f;
constexpr float DEFAULT_CURIOUSITY_BASE_LEVEL = 0.05f;
constexpr float DEFAULT_NOVELTY_SENSITIVITY = 0.2f;

// Development stage durations
constexpr size_t DEVELOPMENT_INITIAL_STEPS = 100;
constexpr size_t DEVELOPMENT_CRITICAL_PERIOD_STEPS = 500;
constexpr size_t DEVELOPMENT_MATURATION_STEPS = 1000;

// Boundary limits
constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;
constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;
constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;

// Checkpoints
constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10;
constexpr size_t DEFAULT_MAX_CHECKPOINT_SIZE = 1000000; // bytes

// World parameters
constexpr int DEFAULT_WORLD_WIDTH = 16;
constexpr int DEFAULT_WORLD_HEIGHT = 16;

// Experiment parameters
constexpr uint64_t DEFAULT_EXPERIMENT_MAX_STEPS = 10000;
constexpr float DEFAULT_EXPERIMENT_CONVERGENCE_THRESHOLD = 0.01f;

} // namespace ConfigConstants
} // namespace nlm