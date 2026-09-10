#pragma once

namespace nlm {

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

// Brain.hpp constants
constexpr size_t BRAIN_MEMORY_SIZE = 1024;

// Development stages
constexpr double DEVELOPMENT_STAGE_0_INITIAL = 60.0;
constexpr double DEVELOPMENT_STAGE_1_CRITICAL_PERIOD = 300.0;
constexpr double DEVELOPMENT_STAGE_2_MATURATION = 900.0;
constexpr double DEVELOPMENT_STAGE_3_ADULT = 0.0;

// Development system constants
constexpr double DEVELOPMENT_COEFFICIENT_1 = 0.8;
constexpr double DEVELOPMENT_COEFFICIENT_2 = 1.0;

// Plasticity system constants
constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
constexpr float PLASTICITY_FACTOR_INITIAL = 0.5f;
constexpr float PLASTICITY_FACTOR_ADVANCED = 0.05f;

// Action command constants for advanced users
constexpr float ACTION_STRENGTH_THRESHOLD = 0.5f;
constexpr float ACTION_STRENGTH_DEFAULT = 0.75f;
constexpr size_t ACTION_SEARCH_NEIGHBORS = 5;

// Reward constants
constexpr float REWARD_BASELINE = 0.0f;
constexpr float REWARD_STANDARD = 1.0f;
constexpr float REWARD_CRITICAL = 2.0f;
constexpr float REWARD_NEGATIVE = -1.0f;

// Environment interaction constants
constexpr size_t ENVIRONMENT_LIMBO_VISUAL = 16;
constexpr size_t ENVIRONMENT_LIMBO_TOUCH = 8;
constexpr size_t ENVIRONMENT_LIMBO_INTERNAL = 4;
constexpr size_t ENVIRONMENT_LIMBO_PROPRIOCEPTION = 6;

// Reward prediction constants
constexpr double REWARD_PREDICTION_DECAY = 0.99;
constexpr double REWARD_PREDICTION_ACCURACY = 0.01;

// Simulation constants
constexpr double SIMULATION_TOLERANCE = 1e-6;
constexpr size_t SIMULATION_MAX_STEPS = 1000000;
constexpr double SIMULATION_TIMESTEP_MIN = 1e-6;
constexpr double SIMULATION_TIMESTEP_MAX = 0.01;

// Observation constants
constexpr size_t OBSERVATION_LIMBO_DEFAULT = 256;
constexpr size_t OBSERVATION_LIMBO_MAX = 1000;
constexpr size_t OBSERVATION_LIMBO_MIN = 1;

// Neural prediction constants
constexpr float PREDICTION_CONFIDENCE_MIN = 0.0f;
constexpr float PREDICTION_CONFIDENCE_MAX = 1.0f;
constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;
constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;

// Concept formation constants
constexpr float CONCEPT_FORMATION_THRESHOLD = 0.7f;
constexpr float CONCEPT_FORMATION_SIMILARITY = 0.8f;
constexpr size_t CONCEPT_FORMATION_MAX_CONCEPTS = 100;
constexpr size_t CONCEPT_FORMATION_MIN_CONCEPTS = 1;

// Attention system constants
constexpr size_t ATTENTION_FOCUS_MAX = 5;
constexpr size_t ATTENTION_FOCUS_MIN = 1;
constexpr float ATTENTION_FOCUS_THRESHOLD = 0.5f;
constexpr float ATTENTION_FOCUS_DEFAULT = 0.75f;

// Planning system constants
constexpr size_t PLANNING_ACTION_SEQUENCE_MAX = 10;
constexpr size_t PLANNING_ACTION_SEQUENCE_MIN = 1;
constexpr float PLANNING_SEQUENCE_QUALITY_THRESHOLD = 0.5f;
constexpr float PLANNING_SEQUENCE_QUALITY_DEFAULT = 0.7f;

// Curiosity system constants
constexpr float CURIOSITY_BASELINE = 0.0f;
constexpr float CURIOSITY_MAX = 1.0f;
constexpr float CURIOSITY_THRESHOLD = 0.5f;
constexpr float CURIOSITY_DECAY = 0.99f;
constexpr float CURIOSITY_NOVELTY_FACTOR = 2.0f;
constexpr float CURIOSITY_PREDICTION_ERROR_FACTOR = 0.5f;

// Novelty detection constants
constexpr float NOVELTY_THRESHOLD = 0.5f;
constexpr float NOVELTY_DECAY = 0.99f;
constexpr float NOVELTY_WEIGHT = 1.0f;

// Neuromodulation system constants
constexpr float NEUROMODULATION_DOPAMINE_POSITIVE = 1.0f;
constexpr float NEUROMODULATION_DOPAMINE_NEGATIVE = -1.0f;
constexpr float NEUROMODULATION_NOVELTY_THRESHOLD = 0.5f;
constexpr float NEUROMODULATION_NOVELTY_POSITIVE = 1.0f;
constexpr float NEUROMODULATION_NOVELTY_NEGATIVE = -0.5f;
constexpr float NEUROMODULATION_CURIOUS_THRESHOLD = 0.5f;
constexpr float NEUROMODULATION_CURIOUS_POSITIVE = 1.0f;
constexpr float NEUROMODULATION_CURIOUS_NEGATIVE = -0.5f;
constexpr float NEUROMODULATION_PREDICTION_ERROR_THRESHOLD = 0.5f;
constexpr float NEUROMODULATION_PREDICTION_ERROR_POSITIVE = 1.0f;
constexpr float NEUROMODULATION_PREDICTION_ERROR_NEGATIVE = -0.5f;

// Memory system constants
constexpr size_t MEMORY_CAPACITY = 10000;
constexpr size_t MEMORY_ACTIVE = 100;
constexpr size_t MEMORY_RECENT = 50;
constexpr size_t MEMORY_ASSOCIATIVE = 500;
constexpr float MEMORY_DECAY = 0.99f;
constexpr float MEMORY_STRENGTH = 1.0f;
constexpr float MEMORY_THRESHOLD = 0.5f;

// Working memory constants
constexpr size_t WORKING_MEMORY_CAPACITY = 50;
constexpr size_t WORKING_MEMORY_WINDOW = 20;
constexpr float WORKING_MEMORY_DECAY = 0.95f;
constexpr float WORKING_MEMORY_ACTIVATION_THRESHOLD = 0.1f;
constexpr float WORKING_MEMORY_ACTIVATION_DEFAULT = 0.5f;

// Episodic memory constants
constexpr size_t EPISODIC_MEMORY_CAPACITY = 1000;
constexpr size_t EPISODIC_MEMORY_REPLAY = 10;
constexpr float EPISODIC_MEMORY_STRENGTH = 1.0f;
constexpr float EPISODIC_MEMORY_DECAY = 0.98f;
constexpr float EPISODIC_MEMORY_THRESHOLD = 0.5f;

// Semantic memory constants
constexpr size_t SEMANTIC_MEMORY_CAPACITY = 5000;
constexpr size_t SEMANTIC_MEMORY_ASSOCIATIONS = 100;
constexpr float SEMANTIC_MEMORY_STRENGTH = 1.0f;
constexpr float SEMANTIC_MEMORY_DECAY = 0.97f;
constexpr float SEMANTIC_MEMORY_THRESHOLD = 0.3f;

// Procedural memory constants
constexpr size_t PROCEDURAL_MEMORY_CAPACITY = 1000;
constexpr size_t PROCEDURAL_MEMORY_SEQUENCE_LENGTH = 10;
constexpr float PROCEDURAL_MEMORY_STRENGTH = 1.0f;
constexpr float PROCEDURAL_MEMORY_DECAY = 0.96f;
constexpr float PROCEDURAL_MEMORY_THRESHOLD = 0.4f;

// Associative memory constants
constexpr size_t ASSOCIATIVE_MEMORY_CAPACITY = 5000;
constexpr size_t ASSOCIATIVE_MEMORY_PATTERNS = 100;
constexpr float ASSOCIATIVE_MEMORY_STRENGTH = 1.0f;
constexpr float ASSOCIATIVE_MEMORY_DECAY = 0.98f;
constexpr float ASSOCIATIVE_MEMORY_THRESHOLD = 0.6f;

// Performance constants
constexpr size_t PERFORMANCE_BUFFER_SIZE = 1024;
constexpr size_t PERFORMANCE_FRAMEWORK = 64;
constexpr size_t PERFORMANCE_ITERATIONS = 1000;
constexpr float PERFORMANCE_MIN_TIME = 1e-6;
constexpr float PERFORMANCE_MAX_TIME = 1.0f;
constexpr size_t PERFORMANCE_MEMORY_POOL = 1000000;
constexpr size_t PERFORMANCE_EVENT_QUEUE = 10000;

// Checkpoint system constants
constexpr size_t CHECKPOINT_VERSION = 1;
constexpr size_t CHECKPOINT_MAGIC = 0x4E4C4D00;  // 'NLM\0'
constexpr size_t CHECKPOINT_SIZE = 4096;
constexpr float CHECKPOINT_COMPRESSION_LEVEL = 6.0f;
constexpr float CHECKPOINT_ENCRYPTION_KEY = 0xCAFEBABE;

// Visualization constants
constexpr float VISUALIZATION_MIN_COORD = -10.0f;
constexpr float VISUALIZATION_MAX_COORD = 10.0f;
constexpr size_t VISUALIZATION_MIN_SIZE = 1;
constexpr size_t VISUALIZATION_MAX_SIZE = 100;
constexpr float VISUALIZATION_MIN_OPACITY = 0.1f;
constexpr float VISUALIZATION_MAX_OPACITY = 1.0f;
constexpr size_t VISUALIZATION_BUFFER_SIZE = 1024;

