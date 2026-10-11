#pragma once

namespace nlm {

// Configuration parameter keys (centralized for consistency)
namespace ConfigKeys {
    // Basic simulation parameters
    const std::string SIMULATION_TIMESTEP = "simulation_timestep";
    const std::string RANDOM_SEED = "random_seed";
    const std::string NEURON_COUNT = "neuron_count";
    const std::string REGION_COUNT = "region_count";
    const std::string CONNECTION_PROBABILITY = "connection_probability";
    
    // Plasticity parameters
    const std::string STDP_LTP_WEIGHT = "stdp_ltp_weight";
    const std::string STDP_LTD_WEIGHT = "stdp_ltd_weight";
    const std::string STDP_TAU = "stdp_tau";
    const std::string SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    const std::string PRUNING_RATE = "pruning_rate";
    
    // Memory system parameters
    const std::string WORKING_MEMORY_CAPACITY = "working_memory_capacity";
    const std::string EPISODIC_MEMORY_MAX_EPISODES = "episodic_memory_max_episodes";
    const std::string EPISODIC_MEMORY_CONSOLIDATION_INTERVAL = "episodic_memory_consolidation_interval";
    const std::string EPISODIC_MEMORY_REPLAY_INTERVAL = "episodic_memory_replay_interval";
    
    // Cognitive parameters
    const std::string PLANNER_PLANNING_DEPTH = "planner_planning_depth";
    const std::string ATTENTION_INHIBITION_STRENGTH = "attention_inhibition_strength";
    const std::string ATTENTION_EXCITATION_STRENGTH = "attention_excitation_strength";
    
    // Neuromodulation parameters
    const std::string NOVELTY_DECAY_RATE = "novelty_decay_rate";
    const std::string REWARD_MODULATION_ENABLED = "reward_modulation_enabled";
    const std::string STRUCTURAL_PLASTICITY_ENABLED = "structural_plasticity_enabled";
    const std::string DEVELOPMENT_ENABLED = "development_enabled";
    const std::string CURIOSITY_ENABLED = "curiosity_enabled";
    
    // Development parameters
    const std::string DEVELOPMENT_INITIAL_AGE = "development_initial_age";
    const std::string DEVELOPMENT_CRITICAL_PERIOD_START = "development_critical_period_start";
    const std::string DEVELOPMENT_CRITICAL_PERIOD_END = "development_critical_period_end";
    
    // Checkpoint parameters
    const std::string CHECKPOINT_DIR = "checkpoint_dir";
    const std::string CHECKPOINT_MAX_SAVE_COUNT = "checkpoint_max_save_count";
    const std::string CHECKPOINT_MAX_FILE_SIZE = "checkpoint_max_file_size";
    const std::string CHECKPOINT_ENABLED = "checkpoint_enabled";
    
    // Agent parameters
    const std::string AGENT_REWARD_THRESHOLD = "agent_reward_threshold";
    const std::string AGENT_NOVELTY_THRESHOLD = "agent_novelty_threshold";
    const std::string AGENT_CURIOUSY_THRESHOLD = "agent_curiosity_threshold";
    
    // Experiment parameters
    const std::string EXPERIMENT_DURATION = "experiment_duration";
    const std::string EXPERIMENT_SEED = "experiment_seed";
}

// Default configuration values
namespace Defaults {
    // Basic simulation parameters
    const double SIMULATION_TIMESTEP = 0.001;
    const uint64_t RANDOM_SEED = 42;
    const size_t NEURON_COUNT = 1000;
    const size_t REGION_COUNT = 1;
    const float CONNECTION_PROBABILITY = 0.1f;
    
    // Plasticity parameters
    const float STDP_LTP_WEIGHT = 0.01f;
    const float STDP_LTD_WEIGHT = 0.012f;
    const float STDP_TAU = 20.0f;
    const float SYNAPTOGENESIS_RATE = 0.0001f;
    const float PRUNING_RATE = 0.00001f;
    
    // Memory system parameters
    const size_t WORKING_MEMORY_CAPACITY = 100;
    const size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;
    const size_t EPISODIC_MEMORY_CONSOLIDATION_INTERVAL = 1000;
    const size_t EPISODIC_MEMORY_REPLAY_INTERVAL = 100;
    
    // Cognitive parameters
    const size_t PLANNER_PLANNING_DEPTH = 5;
    const float ATTENTION_INHIBITION_STRENGTH = 0.5f;
    const float ATTENTION_EXCITATION_STRENGTH = 1.5f;
    
    // Neuromodulation parameters
    const float NOVELTY_DECAY_RATE = 0.99f;
    const bool REWARD_MODULATION_ENABLED = true;
    const bool STRUCTURAL_PLASTICITY_ENABLED = true;
    const bool DEVELOPMENT_ENABLED = true;
    const bool CURIOSITY_ENABLED = true;
    
    // Development parameters
    const double DEVELOPMENT_INITIAL_AGE = 0.0;
    const double DEVELOPMENT_CRITICAL_PERIOD_START = 60.0;
    const double DEVELOPMENT_CRITICAL_PERIOD_END = 300.0;
    
    // Checkpoint parameters
    const std::string CHECKPOINT_DIR = "./checkpoints";
    const size_t CHECKPOINT_MAX_SAVE_COUNT = 10000;
    const size_t CHECKPOINT_MAX_FILE_SIZE = 5 * 1024 * 1024;  // 5MB
    const bool CHECKPOINT_ENABLED = true;
    
    // Agent parameters
    const float AGENT_REWARD_THRESHOLD = 0.1f;
    const float AGENT_NOVELTY_THRESHOLD = 0.3f;
    const float AGENT_CURIOSITY_THRESHOLD = 0.5f;
    
    // Experiment parameters
    const double EXPERIMENT_DURATION = 60.0;
    const uint64_t EXPERIMENT_SEED = 42;
}

// Behavioral thresholds and constants
namespace Behavioral {
    // Activity thresholds
    const float MIN_ACTIVITY_FOR_ACTION = 0.5f;
    const float MIN_NEURON_FIRE_THRESHOLD = 20.0f;
    const float MAX_INPUT_CURRENT = 50.0f;
    
    // Plasticity thresholds
    const float LTP_THRESHOLD = 0.01f;
    const float LTD_THRESHOLD = 0.01f;
    const float STDP_TIME_WINDOW = 20.0f;  // ms
    
    // Learning parameters
    const float REWARD_PREDICTION_ERROR_DECAY = 0.95f;
    const float ELIGIBILITY_TRACE_DECAY = 0.1f;
    const float PLASTICITY_MODIFIER_DECAY = 0.98f;
    
    // Memory parameters
    const float MEMORY_DECAY_RATE = 0.9f;
    const float CONSOLIDATION_THRESHOLD = 0.8f;
    
    // Development parameters
    const float PLASTICITY_MODIFIER_INITIAL = 1.0f;
    const float PLASTICITY_MODIFIER_ADULT = 0.2f;
    const float DEVELOPMENT_AGE_SCALE = 0.01f;
    
    // Neuromodulation parameters
    const float DOPAMINE_CLAMP_MIN = -1.0f;
    const float DOPAMINE_CLAMP_MAX = 1.0f;
    const float NOVELTY_CLAMP_MIN = 0.0f;
    const float NOVELTY_CLAMP_MAX = 1.0f;
    const float CURIOSITY_CLAMP_MIN = 0.0f;
    const float CURIOSITY_CLAMP_MAX = 1.0f;
    
    // Spatial parameters (for world)
    const size_t WORLD_SIZE = 32;
    const float AGENT_MOVEMENT_SPEED = 1.0f;
    const float AGENT_ROTATION_SPEED = 0.1f;
    
    // Sensory processing
    const size_t VISION_INPUT_WIDTH = 16;
    const size_t VISION_INPUT_HEIGHT = 16;
    const size_t TOUCH_INPUT_COUNT = 8;
    const size_t INTERNAL_INPUT_COUNT = 4;
    const size_t PROPRIOCEPTION_INPUT_COUNT = 6;
    const size_t AUDIO_INPUT_COUNT = 0;
    
    // Motor output
    const size_t MOTOR_OUTPUT_COUNT = 6;
    
    // Time constants
    const double TIME_STEP_CONVERSION = 0.001;
    const double MINUTES_TO_STEPS = 60000.0;
    
    // Exploration parameters
    const float EXPLORATION_BASE_RATE = 0.1f;
    const float EXPLORATION_MAX_RATE = 0.3f;
    const size_t EXPLORATION_RANDOM_ACTIONS = 7;
}

// Error codes
namespace ErrorCodes {
    const int SUCCESS = 0;
    const int FAILURE = 1;
    const int CONFIG_LOAD_FAILED = 2;
    const int BRAIN_INITIALIZATION_FAILED = 3;
    const int INVALID_INPUT = 4;
    const int MEMORY_ALLOCATION_FAILED = 5;
    const int FILE_OPERATION_FAILED = 6;
    const int VALIDATION_FAILED = 7;
}

// Logging levels (if needed)
namespace LogLevels {
    const int DEBUG = 0;
    const int INFO = 1;
    const int WARNING = 2;
    const int ERROR = 3;
    const int CRITICAL = 4;
}

} // namespace nlm