// NLM Constants
// 
// This file defines named constants for commonly used magic numbers in the NLM codebase.
// Using named constants improves code readability, maintainability, and makes it easier
// to adjust parameters during research and experimentation.

#pragma once

namespace nlm {

// Value constants
constexpr float ZERO_FLOAT = 0.0f;
constexpr float HALF_FLOAT = 0.5f;
constexpr float ONE_FLOAT = 1.0f;
constexpr float MINUS_ONE_FLOAT = -1.0f;

// Common learning rates and weights
constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.01f;
constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.012f;
constexpr float STDP_LEARNING_RATE_DEFAULT = 0.001f;
constexpr float HEBBIAN_LEARNING_RATE_DEFAULT = 0.1f;
constexpr float CONNECTION_PROBABILITY_DEFAULT = 0.1f;
constexpr float SYNAPSE_WEIGHT_INITIAL = 0.2f;
constexpr float SYNAPSE_WEIGHT_INITIAL_NEGATIVE = 0.1f;

// Plasticity parameters
constexpr float PLASTICITY_MODULATION_MAX = 2.0f;
constexpr float PLASTICITY_MODULATION_MIN = 0.1f;
constexpr float SYNAPSE_GROWTH_RATE_DEFAULT = 0.0001f;
constexpr float SYNAPSE_PRUNING_RATE_DEFAULT = 0.00001f;
constexpr float ACTIVITY_THRESHOLD_DEFAULT = 0.001f;
constexpr float ACTIVITY_THRESHOLD_MEDIUM = 0.01f;
constexpr float ACTIVITY_THRESHOLD_LOW = 0.1f;

// Neuromodulation parameters
constexpr float NEUROMODULATION_LEVEL_DEFAULT = 0.0f;
constexpr float NEUROMODULATION_PEAK_LEVEL = 1.0f;
constexpr float NEUROMODULATION_DECAY_RATE_DEFAULT = 0.1f;
constexpr float NEUROMODULATION_RELEASE_RATE_DEFAULT = 1.0f;
constexpr float NOVELTY_THRESHOLD_DEFAULT = 0.3f;
constexpr float NOVELTY_DECAY_RATE_DEFAULT = 0.1f;
constexpr float NOVELTY_LEVEL_DEFAULT = 0.0f;
constexpr float CURIOSITY_TARGET_LEVEL_DEFAULT = 0.5f;
constexpr float CURIOSITY_DECAY_RATE_DEFAULT = 0.1f;
constexpr float REWARD_SCALE_DEFAULT = 1.0f;

// Memory parameters
constexpr float MEMORY_ACTIVATION_THRESHOLD = 0.1f;
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;
constexpr float WORKING_MEMORY_CAPACITY_RATIO = 0.1f;
constexpr float EPISODIC_MEMORY_MAX_EPISODES = 1000.0f;
constexpr float ASSOCIATIVE_MEMORY_STRENGTH_RANGE_MIN = 0.0f;
constexpr float ASSOCIATIVE_MEMORY_STRENGTH_RANGE_MAX = 1.0f;

// Developmental parameters
constexpr float DEVELOPMENTAL_STAGE_TRANSITION_AGE = 60.0f;
constexpr float DEVELOPMENTAL_STAGE_2_TRANSITION_AGE = 300.0f;
constexpr float DEVELOPMENTAL_STAGE_3_TRANSITION_AGE = 900.0f;
constexpr float PLASTICITY_PLACEMENT_DURING_INITIAL = 1.0f;
constexpr float PLASTICITY_PLACEMENT_DURING_CRITICAL = 0.8f;
constexpr float PLASTICITY_PLACEMENT_DURING_MATURATION = 0.5f;
constexpr float PLASTICITY_PLACEMENT_DURING_ADULT = 0.2f;

// Simulation parameters
constexpr float SIMULATION_TIMESTEP_DEFAULT = 0.001f;
constexpr float SIMULATION_TAU_MEM_DEFAULT = 20.0f;
constexpr float SIMULATION_TAU_REF_DEFAULT = 2.0f;
constexpr float SIMULATION_REWARD_REPLAY_INTERVAL_DEFAULT = 100;
constexpr float SIMULATION_CONSOLIDATION_INTERVAL_DEFAULT = 1000;
constexpr float SIMULATION_DEVELOPMENT_UPDATE_INTERVAL = 1000;

// Movement and action parameters
constexpr float MOVEMENT_DELTA_DEFAULT = 0.1f;
constexpr float ROTATION_DELTA_DEFAULT = 0.1f;
constexpr float ACTION_STRENGTH_MULTIPLIER = 10.0f;
constexpr float PROPRIOCEPTION_SCALE = 2.0f;
constexpr float INTERNAL_SIGNALS_CENTER = 1.0f;

// Confidence and prediction parameters
constexpr float PREDICTION_CONFIDENCE_DEFAULT = 0.5f;
constexpr float PREDICTION_ERROR_CLIP_MIN = 0.0001f;
constexpr float PREDICTION_ERROR_CLIP_MAX = 10.0f;
constexpr float NOVELTY_THRESHOLD_SENSITIVITY = 0.001f;

// Normalization and scaling parameters
constexpr float NORMALIZATION_MIN_SCALE = 0.0001f;
constexpr float NORMALIZATION_MAX_SCALE = 10.0f;
constexpr float GAUSSIAN_KERNEL_SIGMA = 0.5f;
constexpr float EXPONENTIAL_DECAY_RATE = 0.9f;
constexpr float SMOOTHING_FACTOR = 0.1f;

// Exploration and curiosity
constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
constexpr float CURIOSITY_HIGH_LEVEL_THRESHOLD = 0.5f;
constexpr float CURIOSITY_EXPLORATION_CHANCE_MAX = 0.3f;
constexpr float NOVELTY_IMPORTANCE_WEIGHT = 2.0f;
constexpr float REWARD_IMPORTANCE_WEIGHT = 0.5f;

// Safety and clamping constants
constexpr float CLAMP_MIN_FLOAT = 0.0f;
constexpr float CLAMP_MAX_FLOAT = 1.0f;
constexpr float CLAMP_MIN_INT = 0;
constexpr float CLAMP_MAX_INT = 1000;
constexpr float EPSILON_FLOAT = 0.0001f;
constexpr float EPSILON_DOUBLE = 0.000001f;

// Statistical parameters
constexpr float STATISTICAL_SAMPLE_SIZE = 100;
constexpr float STATISTICAL_CONFIDENCE_LEVEL = 0.95f;
constexpr float STATISTICAL_SIGNIFICANCE_LEVEL = 0.05f;

// Time constants (in simulation steps)
constexpr SimulationStep TIME_SHORT = 10;
constexpr SimulationStep TIME_MEDIUM = 100;
constexpr SimulationStep TIME_LONG = 1000;
constexpr SimulationStep TIME_VERY_LONG = 10000;

// Memory constants
constexpr size_t MEMORY_POOL_DEFAULT_SIZE = 10000;
constexpr size_t EVENT_QUEUE_DEFAULT_SIZE = 1000;
constexpr size_t CHECKPOINT_BUFFER_SIZE = 5000;

// Visualization constants
constexpr int VISUALIZATION_RESOLUTION_DEFAULT = 256;
constexpr float VISUALIZATION_INTENSITY_MAX = 255.0f;
constexpr float VISUALIZATION_INTENSITY_MIN = 0.0f;

} // namespace nlm
