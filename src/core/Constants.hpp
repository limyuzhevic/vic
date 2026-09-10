// NLM Neural Constants
// Centralized configuration for all magic numbers and biological constants
// This file replaces hardcoded values throughout the codebase with named constants

#pragma once

// Neural constants
namespace nlm::constants {

    // Neuron membrane properties
    constexpr float NEURON_MEMBRANE_CAPACITANCE = 1.0f;
    constexpr float NEURON_TIME_CONSTANT = 20.0f;           // τ in LIF model
    constexpr float NEURON_RESTING_POTENTIAL = -70.0f;     // V_rest in mV
    constexpr float NEURON_THRESHOLD = -55.0f;              // V_threshold in mV
    constexpr float NEURON_RESET_POTENTIAL = -65.0f;       // V_reset in mV
    constexpr float NEURON_MAX_SPIKE_HISTORY = 100;          // Maximum spikes to track
    constexpr float NEURON_LEAK_CONDUCTANCE = 0.01f;        // Membrane conductance
    constexpr float NEURON_MEMBRANE_CLAMP_MIN = -100.0f;   // Minimum V_m clamp
    constexpr float NEURON_MEMBRANE_CLAMP_MAX = 50.0f;     // Maximum V_m clamp
    
    // Refractory period
    constexpr int NEURON_MIN_REFECTORY_PERIOD = 2;
    constexpr int NEURON_MAX_REFECTORY_PERIOD = 10;
    
    // Timestep and simulation
    constexpr double SIMULATION_TIMESTEP_DEFAULT = 0.001;  // Default dt in seconds
    constexpr float SIMULATION_TIMESTEP_FACTOR = 1000.0f;   // Factor for step conversion
    
    // Plasticity constants (STDP)
    constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.01f;       // Long-term potentiation weight
    constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.012f;      // Long-term depression weight
    constexpr float STDP_TAU_DEFAULT = 20.0f;              // STDP time constant
    constexpr float STDP_LTP_MIN_WEIGHT = 0.0001f;        // Minimum LTP effect
    constexpr float STDP_LTD_MIN_WEIGHT = 0.0001f;        // Minimum LTD effect
    
    // Structural plasticity
    constexpr float SYNAPTOGENESIS_RATE_DEFAULT = 0.0001f;   // Initial synaptogenesis rate
    constexpr float PRUNING_RATE_DEFAULT = 0.00001f;       // Initial pruning rate
    constexpr float STRUCTURAL_PLASTICITY_MODIFIER = 1.0f;   // Scaling factor
    
    // Memory system
    constexpr size_t WORKING_MEMORY_CAPACITY_DEFAULT = 1000;  // Default working memory capacity
    constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;      // Maximum stored episodes
    constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;      // Threshold for consolidation
    
    // Neuromodulation
    constexpr float DOPAMINE_MAX_LEVEL = 1.0f;                 // Maximum dopamine level
    constexpr float DOPAMINE_MIN_LEVEL = -1.0f;               // Minimum dopamine level
    constexpr float DOPAMINE_PLASTICITY_MIN = 0.1f;           // Minimum plasticity factor
    constexpr float DOPAMINE_PLASTICITY_MAX = 2.0f;           // Maximum plasticity factor
    constexpr float CURIOUSITY_THRESHOLD = 0.3f;              // Threshold for curiosity-based behavior
    constexpr float CURIOUSITY_EXPLORATION_MAX = 0.3f;        // Maximum exploration chance
    
    // Development stages
    constexpr double DEVELOPMENT_INITIAL_STAGE = 0.0;        // Developmental stage start (0-60s)
    constexpr double DEVELOPMENT_CRITICAL_START = 60.0;    // Critical period start (60-300s)
    constexpr double DEVELOPMENT_MATURATION_START = 300.0;  // Maturation start (300-900s)
    constexpr double DEVELOPMENT_ADULT_START = 900.0;       // Adult stage start (900+s)
    constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;      // High initial plasticity
    constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;     // Critical period plasticity
    constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;   // Maturation plasticity
    constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;        // Adult plasticity
    
    // Memory replay and consolidation
    constexpr size_t MEMORY_REPLAY_INTERVAL_DEFAULT = 100;   // Replay every 100 steps
    constexpr size_t MEMORY_CONSOLIDATION_INTERVAL_DEFAULT = 1000;  // Consolidate every 1000 steps
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;          // Store episode every 10 steps
    
    // Checkpoint system
    constexpr int CHECKPOINT_MAX_VERSIONS = 10;               // Maximum checkpoint versions to keep
    constexpr int CHECKPOINT_COMPRESSION_BALANCED = 5;       // Balanced compression level
    constexpr size_t CHECKPOINT_MIN_SIZE = 1000;             // Minimum checkpoint size
    
    // Motor system
    constexpr size_t MOTOR_OUTPUT_COUNT = 6;                  // Number of action types
    constexpr size_t MOTOR_GROUP_SIZE_DIVISOR = 6;            // Distribute motor neurons into 6 groups
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;          // Threshold for meaningful motor activity
    constexpr float MOTOR_ACTIVITY_SCALE = 10.0f;             // Scale factor for motor neuron activity
    
    // Sensory system
    constexpr size_t VISION_SIZE = 256;                       // Vision input size (16x16)
    constexpr size_t TOUCH_INPUT_COUNT = 8;                   // Touch sensor count
    constexpr size_t INTERNAL_INPUT_COUNT = 4;               // Internal sensor count
    constexpr size_t PROPRIOCEPTION_INPUT_COUNT = 6;         // Proprioception sensor count
    constexpr float SENSORY_NOVELTY_DECAY = 0.99f;            // Novelty decay factor
    
    // World and environment
    constexpr float WORLD_X_SIZE = 20.0f;                     // World width
    constexpr float WORLD_Y_SIZE = 20.0f;                     // World height
    constexpr float WORLD_VISION_WIDTH = 16;                  // Vision grid width
    constexpr float WORLD_VISION_HEIGHT = 16;                 // Vision grid height
    constexpr float WORLD_MAX_ENERGY = 100.0f;                // Maximum energy per object
    constexpr float WORLD_ENERGY_DECAY = 0.1f;               // Energy decay rate
    constexpr float WORLD_ENERGY_GAIN = 5.0f;                // Energy gain factor
    constexpr float WORLD_OBJECT_PLACEMENT_COUNT = 8;        // Number of objects to place
    constexpr float WORLD_HAZARD_COUNT = 3;                  // Number of hazards
    
    // Physics constants
    constexpr float PHYSICS_MOVEMENT_SPEED = 3.0f;            // Movement speed
    constexpr float PHYSICS_TURN_SPEED = 2.0f;                // Turning speed
    constexpr float PHYSICS_INTERACTION_RANGE = 1.0f;        // Interaction detection range
    constexpr float PHYSICS_ACTION_INTERVAL = 0.1f;          // Action execution interval
    constexpr float PHYSICS_SPEED_SCALING = 0.1f;            // Speed scaling factor
    
    // Perception constants
    constexpr int PERCEPTION_RAY_COUNT = 16;                 // Number of rays for vision
    constexpr double PERCEPTION_FIELD_OF_VIEW = 1.5708f;     // 90 degrees in radians (π/2)
    constexpr float PERCEPTION_MAX_RANGE = 8.0f;             // Maximum vision range
    constexpr float PERCEPTION_RAY_STEP = 0.1f;              // Ray casting step size
    constexpr size_t PERCEPTION_TOUCH_SENSORS = 8;          // Touch direction count
    constexpr float PERCEPTION_TOUCH_SENSITIVITY = 1.0f;    // Touch sensitivity factor
    
    // Connection and probability
    constexpr float CONNECTION_PROBABILITY_DEFAULT = 0.1f;   // Default connection probability
    constexpr float CONNECTION_WEIGHT_MIN = 0.0f;           // Minimum connection weight
    constexpr float CONNECTION_WEIGHT_MAX = 1.0f;           // Maximum connection weight
    constexpr int CONNECTION_DELAY_DEFAULT = 1;             // Default synaptic delay
    constexpr float CONNECTION_EXCITATORY_WEIGHT_SCALE = 10.0f; // Scale factor for excitatory input
    
    // Statistics and analysis
    constexpr float STAT_WEIGHT_CHANGE_THRESHOLD = 0.01f;    // Threshold for significant weight change
    constexpr float STAT_EXCITATION_INHIBITION_MIN = 0.0f;  // Minimum E/I ratio
    constexpr float STAT_FIRING_RATE_UPDATE_INTERVAL = 0.001f; // For firing rate calculations
    
    // Configuration defaults
    constexpr int CONFIG_RANDOM_SEED_DEFAULT = 42;           // Default random seed
    constexpr int CONFIG_NEURON_COUNT_DEFAULT = 1000;        // Default neuron count
    constexpr int CONFIG_REGION_COUNT_DEFAULT = 1;           // Default region count
    constexpr float CONFIG_CONNECTION_PROBABILITY_DEFAULT = 0.1f; // Default connection probability
    
    // Action types
    constexpr int ACTION_MOVE_FORWARD = 0;                   // Action index constants
    constexpr int ACTION_MOVE_BACKWARD = 1;
    constexpr int ACTION_TURN_LEFT = 2;
    constexpr int ACTION_TURN_RIGHT = 3;
    constexpr int ACTION_INTERACT = 4;
    constexpr int ACTION_WAIT = 5;
    constexpr int ACTION_LOOK_LEFT = 6;
    constexpr int ACTION_LOOK_RIGHT = 7;
    
    // Error and safety margins
    constexpr float SAFETY_EPSILON = 0.0001f;                // Small value for comparisons
    constexpr float NUMERICAL_EPSILON = 0.001f;              // Numerical tolerance for comparisons
    constexpr size_t MAX_ARRAY_SIZE = 100000;               // Maximum array size for safety
}