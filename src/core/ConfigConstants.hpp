// NLM Configuration Constants
// 
// This file defines constants for configuration keys and default values used throughout NLM.
// These constants provide a single source of truth for configuration parameters and
// make it easier to maintain and modify configuration options.

#pragma once

namespace nlm {

// Brain configuration constants
constexpr const char* CONFIG_BRAIN_NEURON_COUNT = "brain.neuron_count";
constexpr const char* CONFIG_BRAIN_REGION_COUNT = "brain.region_count";
constexpr const char* CONFIG_BRAIN_SYNAPSE_DENSITY = "brain.synapse_density";
constexpr const char* CONFIG_BRAIN_CONNECTION_PROBABILITY = "brain.connection_probability";
constexpr const char* CONFIG_BRAIN_INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
constexpr const char* CONFIG_BRAIN_INITIAL_WEIGHT_STD = "brain.initial_weight_std";
constexpr const char* CONFIG_BRAIN_V_THRESH = "brain.v_thresh";
constexpr const char* CONFIG_BRAIN_V_REST = "brain.v_rest";
constexpr const char* CONFIG_BRAIN_V_RESET = "brain.v_reset";
constexpr const char* CONFIG_BRAIN_TAU_MEM = "brain.tau_mem";
constexpr const char* CONFIG_BRAIN_TAU_REF = "brain.tau_ref";

// Plasticity configuration constants
constexpr const char* CONFIG_PLASTICITY_STDP_ENABLE = "plasticity.stdp.enable";
constexpr const char* CONFIG_PLASTICITY_STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
constexpr const char* CONFIG_PLASTICITY_STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
constexpr const char* CONFIG_PLASTICITY_STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
constexpr const char* CONFIG_PLASTICITY_HEBBIAN_ENABLE = "plasticity.hebbian.enable";
constexpr const char* CONFIG_PLASTICITY_STRUCTURAL_ENABLE = "plasticity.structural.enable";

// Configuration keys for structural plasticity
constexpr const char* CONFIG_SYNAPSIS_GENESIS_RATE = "synaptogenesis_rate";
constexpr const char* CONFIG_PRUNING_RATE = "pruning_rate";

// Neuromodulation configuration constants
constexpr const char* CONFIG_NEUROMOD_DOPAMINE_SCALE = "neuromod.dopamine.scale";
constexpr const char* CONFIG_NEUROMOD_CURIOUSITY_ENABLE = "neuromod.curiosity.enable";
constexpr const char* CONFIG_NEUROMOD_NOVELTY_ENABLE = "neuromod.novelty.enable";

// Simulation configuration constants
constexpr const char* CONFIG_SIMULATION_TIMESTEP = "simulation.timestep";
constexpr const char* CONFIG_SIMULATION_SEED = "simulation.seed";
constexpr const char* CONFIG_REPLAY_INTERVAL = "replay_interval";
constexpr const char* CONFIG_CONSOLIDATION_INTERVAL = "consolidation_interval";
constexpr const char* CONFIG_DEVELOPMENT_UPDATE_INTERVAL = "development.update_interval";
constexpr const char* CONFIG_CHECKPOINT_DIR = "checkpoint.dir";
constexpr const char* CONFIG_CHECKPOINT_MAX_FILES = "checkpoint.max_files";
constexpr const char* CONFIG_CHECKPOINT_COMPRESSION = "checkpoint.compression";

// Memory configuration constants
constexpr const char* CONFIG_WORKING_MEMORY_CAPACITY = "memory.working_memory.capacity";
constexpr const char* CONFIG_EPISODIC_MEMORY_MAX_EPISODES = "memory.episodic_memory.max_episodes";
constexpr const char* CONFIG_ASSOCIATIVE_MEMORY_PATTERN_COUNT = "memory.associative_memory.pattern_count";
constexpr const char* CONFIG_ATTENTION_INHIBITION_STRENGTH = "attention.inhibition_strength";
constexpr const char* CONFIG_ATTENTION_EXCITATION_STRENGTH = "attention.excitation_strength";

// Cognition configuration constants
constexpr const char* CONFIG_PLANNING_DEPTH = "cognition.planning.depth";
constexpr const char* CONFIG_CONCEPT_FORMATION_THRESHOLD = "cognition.concept_formation.threshold";
constexpr const char* CONFIG_PREDICTION_WINDOW_SIZE = "cognition.prediction.window_size";

// Environment configuration constants
constexpr const char* CONFIG_ENVIRONMENT_WIDTH = "environment.width";
constexpr const char* CONFIG_ENVIRONMENT_HEIGHT = "environment.height";
constexpr const char* CONFIG_ENVIRONMENT_VISION_WIDTH = "environment.vision_width";
constexpr const char* CONFIG_ENVIRONMENT_VISION_HEIGHT = "environment.vision_height";
constexpr const char* CONFIG_ENVIRONMENT_VISION_CHANNELS = "environment.vision_channels";
constexpr const char* CONFIG_ENVIRONMENT_MAX_ENERGY = "environment.max_energy";
constexpr const char* CONFIG_ENVIRONMENT_ENERGY_DECAY_RATE = "environment.energy_decay_rate";

// Agent configuration constants
constexpr const char* CONFIG_AGENT_START_X = "agent.start.x";
constexpr const char* CONFIG_AGENT_START_Y = "agent.start.y";
constexpr const char* CONFIG_AGENT_MOTOR_SPEED = "agent.motor_speed";
constexpr const char* CONFIG_AGENT_TURN_SPEED = "agent.turn_speed";

// Experiment configuration constants
constexpr const char* CONFIG_EXPERIMENT_NAME = "experiment.name";
constexpr const char* CONFIG_EXPERIMENT_DESCRIPTION = "experiment.description";
constexpr const char* CONFIG_EXPERIMENT_DURATION = "experiment.duration";
constexpr const char* CONFIG_EXPERIMENT_GOAL_REWARD = "experiment.goal_reward";
constexpr const char* CONFIG_EXPERIMENT_FAILURE_PENALTY = "experiment.failure_penalty";

// Debug and logging configuration constants
constexpr const char* CONFIG_LOG_LEVEL = "debug.log_level";
constexpr const char* CONFIG_LOG_FILE = "debug.log_file";
constexpr const char* CONFIG_LOG_CONSOLE_COLORS = "debug.log_console_colors";
constexpr const char* CONFIG_PERFORMANCE_PROFILE = "debug.performance_profile";

// Statistical configuration constants
constexpr const char* CONFIG_STATISTICAL_SAMPLE_SIZE = "statistical.sample_size";
constexpr const char* CONFIG_STATISTICAL_CONFIDENCE_LEVEL = "statistical.confidence_level";
constexpr const char* CONFIG_STATISTICAL_SIGNIFICANCE_LEVEL = "statistical.significance_level";

} // namespace nlm
