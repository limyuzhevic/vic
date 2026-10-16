// NLM Constants
// Common constants used throughout the NLM codebase to avoid magic numbers
// and improve maintainability and readability

#pragma once

namespace nlm {

// Simulation parameters
constexpr double SIMULATION_TIMESTEP = 0.001;        // Time step in seconds (1ms)
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001; // Default timestep for simulation

// Memory system parameters
constexpr float DEFAULT_MEMORY_CAPACITY_FACTOR = 0.1f;  // Memory capacity as fraction of neuron count
constexpr size_t DEFAULT_MAX_EPISODES = 1000;           // Maximum episodes in episodic memory

// Replay and consolidation parameters
constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;       // Replay memory every 100 steps
constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidate memory every 1000 steps

// Plasticity parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;      // STDP Long-Term Potentiation weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;     // STDP Long-Term Depression weight
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;   // STDP time constant (tau)

// Neuromodulation parameters
constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;     // Baseline dopamine level
constexpr float DEFAULT_DOPAMINE_BASELINE_OLD = 0.0f; // Old dopamine baseline (kept for compatibility)

// Development parameters
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.001f;  // Synaptogenesis rate
constexpr float DEFAULT_PRUNING_RATE = 0.0001f;        // Pruning rate

// Motor parameters
constexpr float MOTOR_NEIGHBORHOOD_RADIUS = 1.0f;      // Radius for motor neuron activation

// Action selection parameters
constexpr float ACTION_SELECTION_THRESHOLD = 0.5f;    // Threshold for action selection
constexpr float ACTION_EXECUTION_THRESHOLD = 1.0f;   // Threshold for action execution

// Attention parameters
constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;  // Attention inhibition strength
constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;   // Attention excitation strength

// Planning parameters
constexpr int DEFAULT_PLANNING_DEPTH = 5;             // Default planning depth

// Numerical stability parameters
constexpr float EPSILON_FLOAT = 0.001f;                // Small value for numerical comparisons
constexpr float EPSILON_DOUBLE = 0.001;                // Small value for double comparisons
constexpr float HALF_PI = 1.57079632679489661923f;     // π/2 for trigonometric operations
constexpr float TWO_PI = 6.28318530717958647692f;       // 2π for trigonometric operations

// Performance and capacity parameters
constexpr size_t DEFAULT_CHECKPOINT_VERSION = 1;      // Default checkpoint version
constexpr float CHECKPOINT_COMPRESSION_BALANCED = 0.0f; // Compression level for balanced compression

// Visualization parameters
constexpr int DEFAULT_FRAME_RATE = 30;                // Default frame rate for visualization
constexpr float DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0f; // Default visualization update rate

// Reward and discounting parameters
constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;  // Reward discount factor

// Environment parameters
constexpr int DEFAULT_GRID_WORLD_WIDTH = 10;          // Default grid world width
constexpr int DEFAULT_GRID_WORLD_HEIGHT = 10;         // Default grid world height

// Logging parameters
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log"; // Default log filename
constexpr const char* LOG_FORMAT_DEFAULT = "[%datetime] [%level] %message"; // Default log format

// Configuration parameter names
constexpr const char* CONFIG_RANDOM_SEED = "random_seed";                    // Random seed parameter
constexpr const char* CONFIG_SIMULATION_TIMESTEP = "simulation_timestep";  // Simulation timestep parameter
constexpr const char* CONFIG_NEURON_COUNT = "neuron_count";                // Neuron count parameter
constexpr const char* CONFIG_REGION_COUNT = "region_count";                // Region count parameter
constexpr const char* CONFIG_CONNECTION_PROBABILITY = "connection_probability"; // Connection probability parameter
constexpr const char* CONFIG_SYNAPSIS_RATE = "synaptogenesis_rate";        // Synaptogenesis rate parameter
constexpr const char* CONFIG_PRUNING_RATE = "pruning_rate";                // Pruning rate parameter
constexpr const char* CONFIG_STDP_LTP_WEIGHT = "stdp_ltp_weight";        // STDP LTP weight parameter
constexpr const char* CONFIG_STDP_LTD_WEIGHT = "stdp_ltd_weight";       // STDP LTD weight parameter
constexpr const char* CONFIG_STDP_TIME_CONSTANT = "stdp_time_constant";   // STDP time constant parameter
constexpr const char* CONFIG_DOPAMINE_BASELINE = "dopamine_baseline";    // Dopamine baseline parameter
constexpr const char* CONFIG_REWARD_DISCOUNT_FACTOR = "reward_discount_factor"; // Reward discount factor parameter
constexpr const char* CONFIG_ENVIRONMENT_NAME = "environment_name";      // Environment name parameter
constexpr const char* CONFIG_ENVIRONMENT_WIDTH = "environment_width";    // Environment width parameter
constexpr const char* CONFIG_ENVIRONMENT_HEIGHT = "environment_height";   // Environment height parameter
constexpr const char* CONFIG_LOG_LEVEL = "log_level";                    // Log level parameter
constexpr const char* CONFIG_LOG_TO_FILE = "log_to_file";                // Log to file parameter
constexpr const char* CONFIG_LOG_FILENAME = "log_filename";              // Log filename parameter
constexpr const char* CONFIG_MAX_SIMULATION_STEPS = "max_simulation_steps"; // Max simulation steps parameter
constexpr const char* CONFIG_SIMULATION_TIME_LIMIT = "simulation_time_limit"; // Simulation time limit parameter
constexpr const char* CONFIG_VISUALIZATION_ENABLED = "visualization_enabled"; // Visualization enabled parameter
constexpr const char* CONFIG_VISUALIZATION_UPDATE_RATE = "visualization_update_rate"; // Visualization update rate parameter
constexpr const char* CONFIG_CHECKPOINT_DIR = "checkpoint_dir";          // Checkpoint directory parameter

} // namespace nlm
