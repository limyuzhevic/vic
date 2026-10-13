#pragma once

namespace nlm {

// Brain and neuron constants
constexpr float DEFAULT_MEMBRANE_POTENTIAL = -70.0f;
constexpr float DEFAULT_THRESHOLD = -55.0f;
constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
constexpr float LIF_SPIKE_THRESHOLD = -50.0f;
constexpr float LIF_RESET_POTENTIAL = -70.0f;
constexpr float LIF_TIME_CONSTANT = 20.0f;
constexpr float MAX_SPIKE_CURRENT = 100.0f;
constexpr float MIN_SPIKE_CURRENT = 1.0f;

// Synaptic weight constants
constexpr float MIN_SYNAPTIC_WEIGHT = 0.0f;
constexpr float MAX_SYNAPTIC_WEIGHT = 2.0f;
constexpr float DEFAULT_SYNAPTIC_WEIGHT = 1.0f;
constexpr float SYNAPTIC_WEIGHT_STEPS = 0.01f;
constexpr float SYNAPTIC_WEIGHT_CLAMP_MAX_DELTA = 0.1f;
constexpr float SYNAPTIC_ELIGIBILITY_THRESHOLD = 0.001f;
constexpr float SYNAPTIC_ELIGIBILITY_DECAY = 0.1f;
constexpr float SYNAPTIC_PLASTICITY_MODIFIER_BASE = 0.5f;
constexpr float SYNAPTIC_PLASTICITY_MODIFIER_FACTOR = 0.5f;
constexpr float SYNAPTIC_PLASTICITY_MIN = 0.1f;
constexpr float SYNAPTIC_PLASTICITY_MAX = 2.0f;

// STDP constants
constexpr float STDP_LTP_DEFAULT_WEIGHT = 0.02f;
constexpr float STDP_LTD_DEFAULT_WEIGHT = 0.015f;
constexpr float STDP_TIME_CONSTANT = 20.0f;
constexpr float STDP_MAX_LTP_DELTA = 0.1f;
constexpr float STDP_MAX_LTD_DELTA = 0.1f;
constexpr float STDP_STRENGTH_BASE = 0.5f;
constexpr float STDP_STRENGTH_RANGE = 0.5f;

// Hebbian constants
constexpr float HEBBIAN_LEARNING_RATE = 0.01f;
constexpr float HEBBIAN_MAX_DELTA = 0.05f;
constexpr float HEBBIAN_SYNAPSE_THRESHOLD = 0.1f;
constexpr float HEBBIAN_COEFFICIENT_BASE = 0.5f;
constexpr float HEBBIAN_COEFFICIENT_RANGE = 0.5f;

// Structural plasticity constants
constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
constexpr float PRUNING_BASE_RATE = 0.00001f;
constexpr float SYNAPTOGENESIS_FACTOR = 2.0f;
constexpr float SYNAPTOGENESIS_MAX_RATE = 0.1f;
constexpr float PRUNING_MAX_RATE = 0.01f;
constexpr float MIN_WEIGHT_THRESHOLD = 0.05f;
constexpr float ACTIVITY_THRESHOLD = 0.001f;
constexpr float SYNAPSE_CREATION_WEIGHT = 0.1f;
constexpr float SYNAPSE_CREATION_WEIGHT_RANGE = 0.2f;
constexpr float SYNAPSE_PRUNING_ACTIVITY_FACTOR = 0.5f;

// Development constants
constexpr float DEVELOPMENT_AGE_INITIAL = 60.0;     // ~1 minute
constexpr float DEVELOPMENT_AGE_CRITICAL = 300.0;  // ~5 minutes
constexpr float DEVELOPMENT_AGE_MATURATION = 900.0; // ~15 minutes
constexpr float PLASTICITY_INITIAL = 1.0f;
constexpr float PLASTICITY_CRITICAL = 0.8f;
constexpr float PLASTICITY_MATURATION = 0.5f;
constexpr float PLASTICITY_ADULT = 0.2f;
constexpr float PLASTICITY_MIN = 0.1f;
constexpr float PLASTICITY_MAX = 2.0f;

// Neuromodulation constants
constexpr float DOPAMINE_PREDICTION_ERROR_SCALE = 1.0f;
constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
constexpr float REWARD_EXP_AVG_WEIGHT = 0.95f;
constexpr float REWARD_EXP_UPDATE = 0.05f;
constexpr float NOVELTY_DECAY_RATE = 0.99f;
constexpr float NOVELTY_THRESHOLD = 0.3f;
constexpr float NOVELTY_WEIGHT = 2.0f;
constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
constexpr float HIGH_CURIOUSITY_THRESHOLD = 0.5f;
constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;
constexpr float EXPLORATION_MAX_CHANCE = 0.3f;

// Agent brain constants
constexpr float VISION_SCALE_FACTOR = 5.0f;
constexpr float TOUCH_SCALE_FACTOR = 8.0f;
constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
constexpr float INTERNAL_CENTER_FACTOR = 2.0f;
constexpr float INTERNAL_OFFSET = 1.0f;
constexpr float PROPRIOCEPTION_CENTER_FACTOR = 2.0f;
constexpr float PROPRIOCEPTION_OFFSET = 1.0f;
constexpr float MAX_WEIGHT_DELTA = 0.1f;
constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
constexpr float MAX_RANDOM_CHOICE = 7;

// Sensory percept constants
constexpr float VISION_DEFAULT_SIZE = 16.0f;
constexpr float TOUCH_DEFAULT_SIZE = 8.0f;
constexpr float INTERNAL_DEFAULT_SIZE = 4.0f;
constexpr float PROPRIOCEPTION_DEFAULT_SIZE = 6.0f;

// Motor command constants
constexpr size_t MOTOR_COMMAND_COUNT = 8;
constexpr size_t MOTOR_COMMAND_VARIATION_COUNT = 8;

// Random generation constants
constexpr float RANDOM_UNIFORM_MIN = 0.0f;
constexpr float RANDOM_UNIFORM_MAX = 1.0f;
constexpr int RANDOM_INT_MIN = 0;
constexpr int RANDOM_INT_MAX = 100;
constexpr float RANDOM_BERNOULLI_THRESHOLD = 0.5f;

// Configuration defaults
constexpr int DEFAULT_RANDOM_SEED = 42;
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
constexpr int64_t DEFAULT_NEURON_COUNT = 500;
constexpr int64_t DEFAULT_REGION_COUNT = 1;
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
constexpr float DEFAULT_SYNAPSES_PER_REGION = 100.0f;

// Experiment constants
constexpr float LEARNING_WEIGHT_CHANGE_THRESHOLD = 0.01f;
constexpr float LEARNING_STRENGTH_THRESHOLD = 0.5f;
constexpr float LEARNING_VALUE_MULTIPLIER = 0.01f;
constexpr float EXPERIMENT_TIME_STEP = 0.001f;
constexpr float METRICS_AVERAGING_WINDOW = 100.0f;
constexpr float METRICS_DECAY_RATE = 0.9f;

// Memory constants
constexpr float WORKING_MEMORY_DECAY_RATE = 0.95f;
constexpr float WORKING_MEMORY_MAX_SIZE = 100.0f;
constexpr float EPISODIC_MEMORY_MAX_CAPACITY = 1000.0f;
constexpr float ASSOCIATIVE_MEMORY_DECAY_RATE = 0.9f;

// Prediction constants
constexpr float PREDICTION_ERROR_DEFAULT = 0.5f;
constexpr float PREDICTION_ERROR_THRESHOLD = 0.001f;
constexpr float PREDICTION_ACCURACY_THRESHOLD = 0.5f;
constexpr float PREDICTION_NOISE_LEVEL = 0.1f;
constexpr float PREDICTION_EXPLORATION_FACTOR = 0.2f;

// Cognitive constants
constexpr float PLANNER_ACTION_THRESHOLD = 0.5f;
constexpr float CONCEPT_FORMATION_THRESHOLD = 0.3f;
constexpr float ATTENTION_FOCUS_THRESHOLD = 0.5f;
constexpr float ATTENTION_FOCUS_DECAY = 0.1f;
constexpr float CONCEPT_ACTIVATION_THRESHOLD = 0.5f;
constexpr float CONCEPT_ACTIVATION_RANGE = 0.5f;

// Environment constants
constexpr float WORLD_BOUNDS_DEFAULT = 100.0f;
constexpr float AGENT_DEFAULT_ENERGY = 1.0f;
constexpr float AGENT_DEFAULT_HEALTH = 1.0f;
constexpr float AGENT_MOVEMENT_SPEED = 2.0f;
constexpr float AGENT_TURN_SPEED = 2.0f;
constexpr float AGENT_INTERACTION_RANGE = 1.0f;
constexpr float AGENT_REWARD_MAGNITUDE = 10.0f;
constexpr float AGENT_PUNISHMENT_MAGNITUDE = -10.0f;
constexpr float ENVIRONMENT_DECAY_RATE = 0.01f;
constexpr float OBJECT_DEFAULT_INTERACTION_RADIUS = 0.3f;

// Logging constants
constexpr const char* DEFAULT_LOG_FILENAME = "nlm_simulation.log";
constexpr size_t LOG_BUFFER_SIZE = 1024;
constexpr size_t MAX_LOG_MESSAGE_SIZE = 4096;
constexpr size_t LOG_HISTORY_SIZE = 10000;

// Performance constants
constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t VECTORIZATION_ALIGNMENT = 32;
constexpr size_t THREAD_POOL_SIZE = 4;
constexpr size_t TASK_QUEUE_SIZE = 1000;
constexpr float TASK_SCHEDULING_THRESHOLD = 0.5f;
constexpr float PARALLEL_SPEEDUP_FACTOR = 2.0f;

// Debugging constants
constexpr bool DEBUG_MODE_ENABLED = false;
constexpr bool VERBOSE_LOGGING_ENABLED = false;
constexpr bool PERFORMANCE_PROFILING_ENABLED = false;
constexpr size_t DEBUG_BREAKPOINT_THRESHOLD = 1000;
constexpr float DEBUG_VALUE_THRESHOLD_HIGH = 1.0f;
constexpr float DEBUG_VALUE_THRESHOLD_LOW = -1.0f;

// Physics simulation constants
constexpr float GRAVITY_ACCELERATION = 9.8f;
constexpr float FRICTION_COEFFICIENT = 0.5f;
constexpr float DRAG_COEFFICIENT = 0.1f;
constexpr float RESTITUTION_COEFFICIENT = 0.7f;
constexpr float TIME_STEP_MIN = 0.0001f;
constexpr float TIME_STEP_MAX = 0.1f;
constexpr float SIMULATION_MAX_TIME = 1000.0f;

// Safety and robustness constants
constexpr float SAFETY_EPSILON = 0.0001f;
constexpr float SAFETY_MAX_INPUT = 1000.0f;
constexpr float SAFETY_MIN_INPUT = -1000.0f;
constexpr float SAFETY_MAX_OUTPUT = 1000.0f;
constexpr float SAFETY_MIN_OUTPUT = -1000.0f;
constexpr size_t SAFETY_MAX_ITERATIONS = 10000;
constexpr double SAFETY_TIMEOUT_SECONDS = 60.0;

} // namespace nlm
