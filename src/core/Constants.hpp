// Constants.hpp - Centralized configuration for NLM simulation framework

#pragma once

// ========== SIMULATION CORE CONSTANTS ==========
namespace nlm {

namespace Simulation {
    // Time stepping
    constexpr double TIMESTEP = 0.001;  // Simulation timestep in seconds
    constexpr double DEFAULT_TIMESTEP = 0.001;
    
    // Random seed
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    
    // Simulation limits
    constexpr SimulationStep MAX_STEPS = 1000000;  // Maximum simulation steps
    constexpr Timestamp MAX_SIMULATION_TIME = 1000.0;  // Maximum simulation time in seconds
}

// ========== MEMORY SYSTEM CONSTANTS ==========
namespace Memory {
    // Working memory
    constexpr size_t WORKING_MEMORY_CAPACITY_FACTOR = 10;  // Capacity as fraction of total neurons
    constexpr float WORKING_MEMORY_DECAY = 0.95f;  // Decay factor per timestep
    constexpr float WORKING_MEMORY_THRESHOLD = 0.5f;  // Activation threshold for memory storage
    
    // Episodic memory
    constexpr size_t MAX_EPISODES = 1000;  // Maximum number of stored episodes
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;  // Store episode every N steps
    constexpr float EPISODE_REWARD_DECAY = 0.9f;  // Reward decay for episodic memory
    
    // Associative memory
    constexpr float ASSOCIATIVE_MEMORY_DECAY = 0.9f;  // Decay factor for associations
    constexpr size_t MAX_ASSOCIATIONS = 5000;  // Maximum number of associations
}

// ========== PREDICTION SYSTEM CONSTANTS ==========
namespace Prediction {
    constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;  // Threshold for significant prediction errors
    constexpr float PREDICTION_LEARNING_RATE = 0.01f;  // Learning rate for predictions
    constexpr size_t PREDICTION_HISTORY_SIZE = 100;  // Number of past predictions to store
}

// ========== COGNITION SYSTEM CONSTANTS ==========
namespace Cognition {
    // Neural planner
    constexpr size_t PLANNING_DEPTH = 5;  // Planning horizon depth
    constexpr float EXPLORATION_FACTOR = 0.1f;  // Exploration vs exploitation balance
    
    // Concept formation
    constexpr size_t CONCEPT_FORMATION_WINDOW = 100;  // Window for concept discovery
    constexpr float CONCEPT_THRESHOLD = 0.7f;  // Confidence threshold for concept formation
    
    // Attention system
    constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;  // Inhibition strength for attention
    constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;  // Excitation strength for attention
}

// ========== DEVELOPMENT SYSTEM CONSTANTS ==========
namespace Development {
    // Developmental stages
    constexpr double INITIAL_STAGE_DURATION = 60.0;  // Initial stage duration in seconds
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;  // Critical period duration in seconds
    constexpr double MATURATION_DURATION = 600.0;  // Maturation duration in seconds
    
    // Plasticity modulation
    constexpr float INITIAL_PLASTICITY = 1.0f;  // High plasticity in initial stage
    constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;  // Reduced plasticity in critical period
    constexpr float MATURATION_PLASTICITY = 0.5f;  // Further reduced plasticity during maturation
    constexpr float ADULT_PLASTICITY = 0.2f;  // Low plasticity in adult stage
}

// ========== NEUROMODULATION CONSTANTS ==========
namespace Neuromodulation {
    // Dopamine system
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;  // Minimum dopamine level
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;  // Maximum dopamine level
    constexpr float DOPAMINE_PLASTICITY_FACTOR_BASE = 0.5f;  // Base plasticity factor for dopamine
    constexpr float DOPAMINE_EXCITABILITY_FACTOR = 0.5f;  // Excitability modulation factor
    
    // Curiosity system
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;  // Weight of novelty in curiosity
    constexpr float CURIOSITY_ERROR_WEIGHT = 0.5f;  // Weight of prediction error in curiosity
    constexpr float CURIOSITY_MAX_LEVEL = 1.0f;  // Maximum curiosity level
    constexpr float CURIOSITY_THRESHOLD = 0.3f;  // Threshold for curiosity-driven exploration
    constexpr float CURIOSITY_EXPLORATION_FACTOR = 0.3f;  // Maximum exploration probability
    
    // Novelty system
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;  // Decay factor for novelty
    constexpr float NOVELTY_SENSITIVITY = 1.0f;  // Sensitivity to changes in input
}

// ========== PLASTICITY SYSTEM CONSTANTS ==========
namespace Plasticity {
    // STDP (Spike-Timing-Dependent Plasticity)
    constexpr float STDP_LTP_DEFAULT_WEIGHT = 0.01f;  // Default Long-Term Potentiation weight
    constexpr float STDP_LTD_DEFAULT_WEIGHT = 0.012f;  // Default Long-Term Depression weight
    constexpr float STDP_TIME_WINDOW = 20.0f;  // STDP time window in milliseconds
    constexpr float STDP_LTP_RATE_MODULATION = 1.0f;  // LTP rate modulation factor
    constexpr float STDP_LTD_RATE_MODULATION = 1.0f;  // LTD rate modulation factor
    
    // Hebbian learning
    constexpr float HEBBIAN_LEARNING_RATE = 0.001f;  // Hebbian learning rate
    constexpr float HEBBIAN_CORRELATION_THRESHOLD = 0.5f;  // Threshold for Hebbian updates
    
    // Structural plasticity
    constexpr float SYNAPTAGENESIS_RATE_INITIAL = 0.0001f;  // Initial synaptogenesis rate
    constexpr float PRUNING_RATE_INITIAL = 0.00001f;  // Initial pruning rate
    constexpr float SYNAPTAGENESIS_RATE_ADULT = 0.00002f;  // Adult synaptogenesis rate
    constexpr float PRUNING_RATE_ADULT = 0.000005f;  // Adult pruning rate
}

// ========== NEURAL COMPUTATION CONSTANTS ==========
namespace Neural {
    // Neuron properties
    constexpr float NEURON_THRESHOLD = 20.0f;  // Firing threshold in mV
    constexpr float NEURON_RESET_POTENTIAL = -70.0f;  // Reset potential in mV
    constexpr float NEURON_RESTING_POTENTIAL = -65.0f;  // Resting potential in mV
    constexpr float NEURON_LEAK_CONDUCTANCE = 0.1f;  // Membrane leak conductance
    constexpr float NEURON_REFECTORY_PERIOD = 5.0f;  // Refractory period in ms
    
    // Synaptic properties
    constexpr float SYNAPSE_DELAY = 1;  // Default synaptic delay in timesteps
    constexpr float EXCITATORY_WEIGHT_MIN = 0.0f;  // Minimum excitatory weight
    constexpr float EXCITATORY_WEIGHT_MAX = 2.0f;  // Maximum excitatory weight
    constexpr float INHIBITORY_WEIGHT_MIN = -2.0f;  // Minimum inhibitory weight
    constexpr float INHIBITORY_WEIGHT_MAX = 0.0f;  // Maximum inhibitory weight
    constexpr float SYNAPSE_ELIGIBILITY_TRACE_DECAY = 0.1f;  // Eligibility trace decay factor
    constexpr float SYNAPSE_ELIGIBILITY_TRACE_THRESHOLD = 0.001f;  // Eligibility trace threshold
    
    // Neural population sizes
    constexpr size_t MOTOR_NEURON_GROUPS = 6;  // Number of motor neuron groups (forward, backward, turn left/right, interact, wait)
    constexpr size_t SENSORY_NEURON_GROUPS = 4;  // Number of sensory neuron groups (vision, touch, internal, proprioception)
    constexpr size_t VISION_NEURONS = 256;  // Number of vision sensory neurons (16x16 grid)
    constexpr size_t TOUCH_NEURONS = 8;  // Number of touch sensory neurons
    constexpr size_t INTERNAL_NEURONS = 4;  // Number of internal sensory neurons
    constexpr size_t PROPRIOCEPTION_NEURONS = 6;  // Number of proprioception sensory neurons
    
    // Action system
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;  // Threshold for motor activity detection
    constexpr float CURIOUS_EXPLORATION_THRESHOLD = 0.5f;  // Threshold for curiosity-driven exploration
    constexpr float RANDOM_EXPLORATION_MAX = 7;  // Maximum random exploration choice (0-7)
}

// ========== ACTION AND REWARD CONSTANTS ==========
namespace Action {
    constexpr float ACTION_MOVEMENT_ENERGY_COST = 0.01f;  // Energy cost for movement actions
    constexpr float ACTION_INTERACTION_REWARD = 1.0f;  // Reward for successful interaction
    constexpr float ACTION_MOVEMENT_REWARD = 0.1f;  // Reward for successful movement
    constexpr float ACTION_WAITTING_COST = 0.001f;  // Small cost for waiting
    constexpr float ACTION_FAILURE_PENALTY = -0.5f;  // Penalty for failed actions
    constexpr float ACTION_STEP_PENALTY = -0.01f;  // Penalty for each step taken
}

// ========== CHECKPOINT CONSTANTS ==========
namespace Checkpoint {
    constexpr size_t MAX_CHECKPOINTS = 10000;  // Maximum number of checkpoints
    constexpr size_t CHECKPOINT_INTERVAL = 1000;  // Checkpoint saving interval
    constexpr CompressionLevel DEFAULT_COMPRESSION = CompressionLevel::Balanced;  // Default compression level
    constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";  // Default checkpoint directory
}

// ========== EXPERIMENT CONSTANTS ==========
namespace Experiment {
    constexpr size_t EXPERIMENT_MAX_STEPS = 10000;  // Maximum steps per experiment
    constexpr double EXPERIMENT_SIMULATION_TIME = 60.0;  // Simulation duration in seconds
    constexpr float EXPERIMENT_REWARD_THRESHOLD = 0.5f;  // Reward threshold for success
    constexpr size_t EXPERIMENT_REPLAY_EPISODES = 3;  // Number of episodes to replay
}

// ========== LOGGING CONSTANTS ==========
namespace Logging {
    constexpr const char* LOG_LEVEL_TRACE = "TRACE";  // Trace level logging
    constexpr const char* LOG_LEVEL_DEBUG = "DEBUG";  // Debug level logging
    constexpr const char* LOG_LEVEL_INFO = "INFO";  // Info level logging
    constexpr const char* LOG_LEVEL_WARN = "WARN";  // Warning level logging
    constexpr const char* LOG_LEVEL_ERROR = "ERROR";  // Error level logging
    constexpr const char* LOG_LEVEL_FATAL = "FATAL";  // Fatal level logging
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;  // Maximum log message length
}

} // namespace nlm
