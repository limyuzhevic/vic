#ifndef NLM_CONSTANTS_HPP
#define NLM_CONSTANTS_HPP

#include <string>

namespace nlm {

// Physical constants for LIF neuron model
namespace NeuronConstants {
    // Membrane capacitance (microfarads)
    const float MEMBRANE_CAPACITANCE = 1.0f;
    
    // Time constants (milliseconds)
    const float TIME_CONSTANT = 20.0f;           // Membrane time constant
    const float REFRACTORY_TIME = 2.0f;         // Refractory period
    const float ADAPTATION_TIME_CONSTANT = 100.0f; // Spike adaptation time constant
    
    // Voltage parameters (millivolts)
    const float V_REST = -70.0f;                 // Resting potential
    const float V_RESET = -65.0f;                // Reset potential
    const float V_THRESHOLD = -50.0f;            // Spike threshold
    const float V_SPIKE_PEAK = 40.0f;            // Maximum spike voltage
    
    // Current and conductance
    const float MAX_INPUT_CURRENT = 100.0f;      // Maximum injected current
    const float SYNAPTIC_CONDUCTANCE = 0.1f;      // Synaptic conductance
    
    // Spike detection parameters
    const float SPIKE_DETECTION_THRESHOLD = 15.0f; // Membrane potential above which spike is detected
    const float SPIKE_HISTORY_MAX_SIZE = 100;     // Maximum number of spikes to track
}

// STDP (Spike-Timing-Dependent Plasticity) parameters
namespace STDPConstants {
    // Weight change parameters
    const float DEFAULT_STDP_LTP_WEIGHT = 0.01f;  // Long-term potentiation weight
    const float DEFAULT_STDP_LTD_WEIGHT = 0.012f; // Long-term depression weight
    const float DEFAULT_STDP_TIME_CONSTANT = 20.0f; // STDP time constant
    
    // Eligibility trace parameters
    const float ELIGIBILITY_TRACE_DECAY = 0.1f;    // Decay rate for eligibility traces
    const float ELIGIBILITY_TRACE_THRESHOLD = 0.001f; // Minimum eligibility trace value
    
    // Learning rate modifiers
    const float PLASTICITY_MODIFIER_MIN = 0.1f;    // Minimum plasticity modifier
    const float PLASTICITY_MODIFIER_MAX = 2.0f;   // Maximum plasticity modifier
}

// Development system parameters
namespace DevelopmentConstants {
    // Synaptogenesis (formation of new synapses)
    const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f; // Rate of new synapse formation
    const float DEVELOPMENT_SYNAPTOGENESIS_MULTIPLIER = 1.0f; // Multiplier for early development
    
    // Pruning (removal of synapses)
    const float DEFAULT_PRUNING_RATE = 0.00001f;   // Rate of synapse removal
    const float DEVELOPMENT_PRUNING_MULTIPLIER = 2.0f; // Multiplier for early development
    
    // Developmental stages
    const float CRITICAL_PERIOD_DURATION = 60.0f;  // Duration of critical period (simulation steps)
    const float MATURATION_DURATION = 300.0f;     // Duration of maturation phase (simulation steps)
    const float ADULT_DURATION = 900.0f;         // Duration of adult phase (simulation steps)
    
    // Age-based plasticity modulation
    const float INITIAL_PLASTICITY = 1.0f;        // Plasticity during initial stage
    const float CRITICAL_PERIOD_PLASTICITY = 0.8f; // Plasticity during critical period
    const float MATURATION_PLASTICITY = 0.5f;     // Plasticity during maturation
    const float ADULT_PLASTICITY = 0.2f;         // Plasticity during adult stage
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    // Dopamine system
    const float DOPAMINE_BASELINE = 0.0f;         // Baseline dopamine level
    const float DOPAMINE_RANGE = 2.0f;            // Range of dopamine values (-1 to +1)
    const float DOSE_RESPONSE_CURVE = 0.5f;      // Dopamine dose-response curve steepness
    
    // Curiosity system
    const float DEFAULT_NOVELTY_SENSITIVITY = 2.0f; // Sensitivity to novelty
    const float DEFAULT_NOVELTY_DECAY = 0.99f;   // Novelty decay rate
    const float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f; // Weight of prediction error in curiosity
    
    // Learning rate modifiers based on neuromodulators
    const float REWARD_MODULATION_GAIN = 1.0f;   // Gain of reward modulation
    const float NOVELTY_EXPLORATION_THRESHOLD = 0.3f; // Novelty threshold for exploration
    const float PREDICTION_ERROR_THRESHOLD = 0.1f; // Prediction error threshold
}

// Memory system constants
namespace MemoryConstants {
    // Working memory
    const size_t DEFAULT_WORKING_MEMORY_CAPACITY = 50; // Default capacity
    const float WORKING_MEMORY_DECAY_RATE = 0.01f;    // Decay rate
    const float WORKING_MEMORY_RECALL_THRESHOLD = 0.5f; // Threshold for recall
    
    // Episodic memory
    const size_t DEFAULT_MAX_EPISODES = 1000;     // Maximum number of episodes
    const float EPISODIC_MEMORATION_CONSOLIDATION_RATE = 0.1f; // Rate of memory consolidation
    const float EPISODIC_REPLAY_INTERVAL = 100.0f; // Interval between replay events
    const float EPISODIC_CONSOLIDATION_INTERVAL = 1000.0f; // Interval for consolidation
    
    // Associative memory
    const float ASSOCIATIVE_CONNECTION_THRESHOLD = 0.5f; // Threshold for association
    const float ASSOCIATIVE_LEARNING_RATE = 0.1f;       // Learning rate
}

// Prediction system constants
namespace PredictionConstants {
    // Prediction parameters
    const float PREDICTION_ERROR_THRESHOLD = 0.1f;     // Threshold for significant prediction error
    const float PREDICTION_UPDATE_RATE = 0.1f;        // Rate of prediction updates
    const float PREDICTION_DECAY_RATE = 0.95f;       // Rate of prediction decay
    
    // Forward model parameters
    const float FORWARD_MODEL_NOISE_LEVEL = 0.01f;   // Noise level in forward model
    const float FORWARD_MODEL_WEIGHT = 0.5f;        // Weight of forward model predictions
    const float BACKWARD_MODEL_WEIGHT = 0.5f;       // Weight of prediction errors
}

// Cognition system constants
namespace CognitionConstants {
    // Planning
    const size_t DEFAULT_PLANNING_DEPTH = 5;        // Default planning depth
    const float PLANNING_TIME_CONSTRAINT = 0.1f;     // Time constraint for planning
    const float PLANNING_EXPLORATION_FACTOR = 0.1f; // Factor for exploration in planning
    
    // Concept formation
    const size_t DEFAULT_CONCEPT_FORMATION_WINDOW = 100; // Window size for concept formation
    const float CONCEPT_FORMATION_LEARNING_RATE = 0.01f; // Learning rate
    const float CONCEPT_STRENGTH_THRESHOLD = 0.7f;     // Threshold for concept strength
    
    // Attention
    const float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f; // Strength of inhibition in attention
    const float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f; // Strength of excitation in attention
    const float ATTENTION_FOCUS_DURATION = 10.0f;    // Duration of focused attention
    const float ATTENTION_SWITCH_COST = 0.1f;        // Cost of switching attention
}

// Environmental and world constants
namespace WorldConstants {
    // World geometry
    const float DEFAULT_WORLD_WIDTH = 20.0f;         // Default world width
    const float DEFAULT_WORLD_HEIGHT = 20.0f;        // Default world height
    const float DEFAULT_AGENT_START_X = 10.0f;      // Default agent start X position
    const float DEFAULT_AGENT_START_Y = 10.0f;      // Default agent start Y position
    
    // Vision
    const size_t DEFAULT_VISION_WIDTH = 8;          // Default vision width
    const size_t DEFAULT_VISION_HEIGHT = 8;         // Default vision height
    const float VISION_PER_PIXEL_RANGE = 1.0f;      // Range of vision per pixel
    const float VISION_DYNAMIC_RANGE = 1.0f;        // Dynamic range of vision
    
    // Agent body
    const float DEFAULT_AGENT_MAX_ENERGY = 100.0f;  // Default maximum energy
    const float DEFAULT_AGENT_ENERGY_DECAY_RATE = 0.001f; // Default energy decay rate
    const float DEFAULT_AGENT_HEALTH_INITIAL = 1.0f;   // Initial health
    const float DEFAULT_AGENT_AGE_INCREMENT = 1.0f;    // Age increment per step
    
    // Objects
    const float DEFAULT_OBJECT_RADIUS = 0.5f;       // Default object radius
    const float RESOURCE_VALUE_MAX = 10.0f;         // Maximum resource value
    const float HAZARD_VALUE_MIN = -10.0f;          // Minimum hazard value
    const float OBJECT_INTERACTION_RANGE = 0.5f;    // Range for object interaction
    
    // Simulation
    const double DEFAULT_TIMESTEP = 0.001;          // Default simulation timestep
    const size_t DEFAULT_MAX_STEPS = 10000;         // Default maximum simulation steps
    const double DEFAULT_SIMULATION_TIME_LIMIT = 0.0; // Default simulation time limit
}

// Performance and optimization constants
namespace PerformanceConstants {
    // Checkpoint system
    const size_t DEFAULT_CHECKPOINT_INTERVAL = 10000; // Default checkpoint interval
    const size_t DEFAULT_CHECKPOINT_COUNT = 5;       // Default number of checkpoints
    const bool DEFAULT_CHECKPOINT_COMPRESSION = true; // Default compression
    
    // Simulation
    const size_t DEFAULT_SIMULATION_STEPS_PER_CHECKPOINT = 1000; // Steps per checkpoint
    const size_t DEFAULT_SIMULATION_CLEANUP_INTERVAL = 100;       // Cleanup interval
    
    // Memory management
    const size_t DEFAULT_MEMORY_POOL_SIZE = 1024;     // Default memory pool size
    const size_t DEFAULT_EVENT_QUEUE_SIZE = 10000;    // Default event queue size
    const size_t DEFAULT_OBJECT_POOL_SIZE = 1000;    // Default object pool size
}

// Configuration key constants
namespace ConfigKeys {
    // Random seed
    const std::string RANDOM_SEED = "random_seed";
    
    // Simulation parameters
    const std::string SIMULATION_TIMESTEP = "simulation_timestep";
    const std::string SIMULATION_STEPS = "simulation_steps";
    const std::string SIMULATION_TIME_LIMIT = "simulation_time_limit";
    
    // Brain configuration
    const std::string NEURON_COUNT = "neuron_count";
    const std::string REGION_COUNT = "region_count";
    const std::string CONNECTION_PROBABILITY = "connection_probability";
    
    // STDP configuration
    const std::string STDP_LTP_WEIGHT = "stdp_ltp_weight";
    const std::string STDP_LTD_WEIGHT = "stdp_ltd_weight";
    const std::string STDP_TIME_CONSTANT = "stdp_time_constant";
    
    // Development configuration
    const std::string SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    const std::string PRUNING_RATE = "pruning_rate";
    const std::string REPLAY_INTERVAL = "replay_interval";
    const std::string CONSOLIDATION_INTERVAL = "consolidation_interval";
    
    // Neuromodulation configuration
    const std::string DOPAMINE_BASELINE = "dopamine_baseline";
    const std::string REWARD_DISCOUNT_FACTOR = "reward_discount_factor";
    const std::string NOVELTY_SENSITIVITY = "novelty_sensitivity";
    const std::string PREDICTION_ERROR_WEIGHT = "prediction_error_weight";
    
    // Environment configuration
    const std::string ENVIRONMENT_NAME = "environment_name";
    const std::string ENVIRONMENT_WIDTH = "environment_width";
    const std::string ENVIRONMENT_HEIGHT = "environment_height";
    const std::string MAX_ENERGY = "max_energy";
    const std::string ENERGY_DECAY_RATE = "energy_decay_rate";
    const std::string AGENT_START_X = "agent_start_x";
    const std::string AGENT_START_Y = "agent_start_y";
    
    // Logging configuration
    const std::string LOG_LEVEL = "log_level";
    const std::string LOG_TO_FILE = "log_to_file";
    const std::string LOG_FILENAME = "log_filename";
    
    // Checkpoint configuration
    const std::string CHECKPOINT_DIR = "checkpoint_dir";
    const std::string CHECKPOINT_INTERVAL = "checkpoint_interval";
    const std::string CHECKPOINT_COUNT = "checkpoint_count";
    const std::string CHECKPOINT_COMPRESSION = "checkpoint_compression";
    
    // Feature flags
    const std::string ENABLE_REWARD_MODULATION = "enable_reward_modulation";
    const std::string ENABLE_CURIOUSITY = "enable_curiosity";
    const std::string ENABLE_DEVELOPMENT = "enable_development";
    const std::string ENABLE_STRUCTURAL_PLASTICITY = "enable_structural_plasticity";
    const std::string ENABLE_PREDICTION = "enable_prediction";
    const std::string ENABLE_PLANNING = "enable_planning";
    const std::string ENABLE_CONCEPT_FORMATION = "enable_concept_formation";
    const std::string ENABLE_ATTENTION = "enable_attention";
    const std::string ENABLE_VISUALIZATION = "visualization_enabled";
    
    // Performance settings
    const std::string PERFORMANCE_OPTIMIZATION_LEVEL = "performance_optimization_level";
    const std::string SIMD_ENABLED = "simd_enabled";
    const std::string PARALLEL_ENABLED = "parallel_enabled";
    const std::string MEMORY_POOL_ENABLED = "memory_pool_enabled";
}

// Error message constants
namespace ErrorMessages {
    const std::string INVALID_BRAIN_CONFIG = "Invalid brain configuration";
    const std::string BRAIN_INITIALIZATION_FAILED = "Brain initialization failed";
    const std::string INVALID_WORLD_CONFIGURATION = "Invalid world configuration";
    const std::string WORLD_UPDATE_FAILED = "World update failed";
    const std::string INVALID_AGENT_CONFIGURATION = "Invalid agent configuration";
    const std::string AGENT_INITIALIZATION_FAILED = "Agent initialization failed";
    const std::string SIMULATION_STEP_FAILED = "Simulation step failed";
    const std::string INVALID_CONFIGURATION_KEY = "Invalid configuration key";
    const std::string CONFIGURATION_FILE_NOT_FOUND = "Configuration file not found";
    const std::string CONFIGURATION_FILE_PARSE_ERROR = "Configuration file parse error";
    const std::string INSUFFICIENT_MEMORY = "Insufficient memory";
    const std::string INVALID_ARGUMENT = "Invalid argument";
    const std::string OPERATION_NOT_SUPPORTED = "Operation not supported";
    const std::string FILE_ACCESS_ERROR = "File access error";
    const std::string SIMULATION_TIMEOUT = "Simulation timeout";
    const std::string CHECKPOINT_SAVE_FAILED = "Checkpoint save failed";
    const std::string CHECKPOINT_LOAD_FAILED = "Checkpoint load failed";
}

// Log message constants
namespace LogMessages {
    const std::string BRAIN_INITIALIZATION_STARTED = "Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...";
    const std::string BRAIN_INITIALIZATION_COMPLETE = "NLM Brain initialization complete (Phase 6 - Integrated)";
    const std::string TOTAL_NEURONS = "Total neurons: ";
    const std::string TOTAL_SYNAPSES = "Total synapses: ";
    const std::string SENSORY_NEURONS = "Sensory neurons: ";
    const std::string MOTOR_NEURONS = "Motor neurons: ";
    const std::string AGENT_BRAIN_INITIALIZATION = "AgentBrain initialized with ";
    const std::string VISION_SENSORY_NEURONS = " vision sensory neurons, ";
    const std::string TOUCH_SENSORY_NEURONS = " touch sensory neurons, ";
    const std::string INTERNAL_SENSORY_NEURONS = " internal sensory neurons";
    const std::string SIMULATION_STARTED = "Starting simulation...";
    const std::string SIMULATION_STEP = "Step ";
    const std::string SIMULATION_COMPLETE = "Simulation complete";
    const std::string SIMULATION_FAILED = "Simulation failed: ";
    const std::string CONFIGURATION_LOADED = "Configuration loaded from ";
    const std::string CONFIGURATION_SAVED = "Configuration saved to ";
    const std::string CHECKPOINT_SAVED = "Checkpoint saved to ";
    const std::string CHECKPOINT_LOADED = "Checkpoint loaded from ";
}

} // namespace nlm

#endif // NLM_CONSTANTS_HPP