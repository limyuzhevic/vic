#pragma once

#include <cstdint>
#include <string>

namespace nlm {

// Simulation and Time Constants
constexpr double SIMULATION_TIMESTEP = 0.001;          // 1 ms simulation timestep
constexpr double DEFAULT_TIME_STEP = 0.001;            // Default timestep for simulation
constexpr double DEVELOPMENT_MIN_AGE = 60.0;           // Developmental stage: 60s (Initial)
constexpr double DEVELOPMENT_CRITICAL_PERIOD_END = 300.0;  // Developmental stage: 300s (CriticalPeriod)
constexpr double DEVELOPMENT_MATURATION_END = 900.0;  // Developmental stage: 900s (Maturation)
constexpr double DEVELOPMENT_ADULT_AGE = 900.0;        // Developmental stage: >900s (Adult)

// Neural Computation Constants
constexpr double LIF_RESTING_POTENTIAL = -70.0f;      // Resting potential (mV)
constexpr double LIF_THRESHOLD_POTENTIAL = -55.0f;    // Firing threshold (mV)
constexpr double LIF_RESET_POTENTIAL = -70.0f;        // Reset potential (mV)
constexpr double LIF_LEAK_CONDUCTANCE = 10.0f;         // Leak conductance (nS)
constexpr double DEFAULT_REFLECTORY_PERIOD = 5;       // Refractory period (steps)
constexpr double DEFAULT_ADAPTATION_STRENGTH = 0.05f; // For spike-frequency adaptation

// Plasticity Constants
constexpr float STDP_LTP_WEIGHT = 0.02f;              // STDP LTP weight factor
constexpr float STDP_LTD_WEIGHT = 0.015f;             // STDP LTD weight factor
constexpr float STDP_TIME_CONSTANT = 20.0f;           // STDP time constant (ms)
constexpr float HEBBIAN_LEARNING_RATE = 0.001f;        // Hebbian learning rate
constexpr float REWARD_MODULATION_SCALE = 1.0f;       // Reward modulation scale

// Structural Plasticity Constants
constexpr float SYNAPTOGENESIS_RATE = 0.0001f;       // Rate of new synapse formation
constexpr float PRUNING_RATE = 0.00001f;              // Rate of synapse elimination
constexpr float SYNAPSE_WEIGHT_MIN = 0.0f;            // Minimum synaptic weight
constexpr float SYNAPSE_WEIGHT_MAX = 1.0f;            // Maximum synaptic weight
constexpr float SYNAPSE_STRENGTH_THRESHOLD = 0.01f;   // Threshold for synapse strengthening

// Memory Constants
constexpr size_t WORKING_MEMORY_CAPACITY = 100;       // Working memory capacity (items)
constexpr size_t EPISODIC_MEMORY_LIMIT = 1000;        // Episodic memory limit (entries)
constexpr float EPISODIC_REPLAY_INTERVAL = 10.0;      // Replay interval (seconds)

// Neuromodulation Constants
constexpr float CURIOSITY_THRESHOLD = 0.3f;           // Minimum novelty to trigger curiosity
constexpr float CURIOSITY_EXPLORATION_FACTOR = 0.3f;  // Curiosity exploration probability scale
constexpr float NOVELTY_DECAY_RATE = 0.99f;           // Novelty decay per timestep
constexpr float NOVELTY_SENSITIVITY = 1.0f;            // Novelty detection sensitivity
constexpr float PREDICTION_ERROR_CLAMP_MIN = -1.0f;   // Min prediction error
constexpr float PREDICTION_ERROR_CLAMP_MAX = 1.0f;    // Max prediction error

// Agent Configuration Constants
constexpr size_t DEFAULT_NEURON_COUNT = 500;          // Default number of neurons
constexpr float DEFAULT_SYNAPSE_DENSITY = 0.05f;      // Default synapse density
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f; // Default connection probability
constexpr int DEFAULT_RANDOM_SEED = 42;               // Default random seed
constexpr float ENERGY_DECAY_RATE = 0.01f;            // Energy decay rate per timestep
constexpr float MAX_ENERGY = 100.0f;                   // Maximum agent energy

// Motor and Action Constants
constexpr int NUM_MOTOR_ACTIONS = 6;                  // Number of basic motor actions
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;      // Threshold for meaningful activity
constexpr float EXPLORATION_PROBABILITY_BASE = 0.0f;  // Base exploration probability
constexpr float EXPLORATION_PROBABILITY_MAX = 0.3f;   // Maximum exploration probability

// Vision and Sensory Constants
constexpr size_t VISION_DEFAULT_WIDTH = 8;            // Default vision width
constexpr size_t VISION_DEFAULT_HEIGHT = 8;           // Default vision height
constexpr size_t VISION_DEFAULT_CHANNELS = 3;         // Default vision channels (RGB)
constexpr size_t TOUCH_DEFAULT_SENSORS = 8;           // Default touch sensor count
constexpr size_t INTERNAL_DEFAULT_SIGNALS = 4;       // Default internal signal count
constexpr size_t PROPRIOCEPTION_DEFAULT_SENSORS = 6;  // Default proprioception sensor count

// Logging and Debugging Constants
constexpr const char* DEFAULT_LOG_LEVEL = "INFO";     // Default log level
constexpr bool ENABLE_DEBUG_LOGGING = false;          // Enable debug logging
constexpr size_t MAX_LOG_ENTRIES = 1000;              // Maximum log entries

// Test and Experiment Constants
constexpr size_t BASIC_CONNECTIVITY_STEPS = 50;       // Steps for basic connectivity test
constexpr size_t PLASTICITY_EXPERIMENT_STEPS = 1000;  // Steps for plasticity experiment
constexpr size_t STDP_VERIFICATION_TRIALS = 50;       // Trials for STDP verification
constexpr float STDP_POTENTIATION_THRESHOLD = 0.01f;  // STDP potentiation threshold
constexpr float STDP_DEPRESSION_THRESHOLD = -0.01f;   // STDP depression threshold

// Configuration Keys (for string constants)
namespace ConfigKeys {
    constexpr const char* BRAIN_NEURON_COUNT = "brain.neuron_count";
    constexpr const char* BRAIN_SYNAPSE_DENSITY = "brain.synapse_density";
    constexpr const char* BRAIN_CONNECTION_PROBABILITY = "brain.connection_probability";
    constexpr const char* BRAIN_INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
    constexpr const char* BRAIN_INITIAL_WEIGHT_STD = "brain.initial_weight_std";
    constexpr const char* BRAIN_V_THRESH = "brain.v_thresh";
    constexpr const char* BRAIN_V_REST = "brain.v_rest";
    constexpr const char* BRAIN_V_RESET = "brain.v_reset";
    constexpr const char* BRAIN_TAU_MEM = "brain.tau_mem";
    constexpr const char* BRAIN_TAU_REF = "brain.tau_ref";
    
    constexpr const char* PLASTICITY_STDP_ENABLE = "plasticity.stdp.enable";
    constexpr const char* PLASTICITY_STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    constexpr const char* PLASTICITY_STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
    constexpr const char* PLASTICITY_STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
    
    constexpr const char* PLASTICITY_HEBBIAN_ENABLE = "plasticity.hebbian.enable";
    constexpr const char* PLASTICITY_STRUCTURAL_ENABLE = "plasticity.structural.enable";
    
    constexpr const char* NEUROMOD_DOPAMINE_SCALE = "neuromod.dopamine.scale";
    constexpr const char* NEUROMOD_CURiosity_ENABLE = "neuromod.curiosity.enable";
    constexpr const char* NEUROMOD_NOVELTY_ENABLE = "neuromod.novelty.enable";
    
    constexpr const char* RANDOM_SEED = "random_seed";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    
    constexpr const char* AGENT_START_X = "agent.start_x";
    constexpr const char* AGENT_START_Y = "agent.start_y";
    constexpr const char* AGENT_MAX_ENERGY = "agent.max_energy";
    constexpr const char* AGENT_ENERGY_DECAY = "agent.energy_decay_rate";
    
    constexpr const char* WORLD_WIDTH = "world.width";
    constexpr const char* WORLD_HEIGHT = "world.height";
    constexpr const char* WORLD_VISION_WIDTH = "world.vision_width";
    constexpr const char* WORLD_VISION_HEIGHT = "world.vision_height";
    
    constexpr const char* ENABLED_SUBSYSTEMS = "enabled_systems";
}

// Action Names (for convenience)
namespace ActionNames {
    constexpr const char* MOVE_FORWARD = "move_forward";
    constexpr const char* MOVE_BACKWARD = "move_backward";
    constexpr const char* TURN_LEFT = "turn_left";
    constexpr const char* TURN_RIGHT = "turn_right";
    constexpr const char* LOOK_LEFT = "look_left";
    constexpr const char* LOOK_RIGHT = "look_right";
    constexpr const char* INTERACT = "interact";
    constexpr const char* WAIT = "wait";
    constexpr const char* EAT = "eat";
    constexpr const char* DRINK = "drink";
    constexpr const char* REST = "rest";
}

// Neuron Type Names
namespace NeuronTypeNames {
    constexpr const char* EXCITATORY = "excitatory";
    constexpr const char* INHIBITORY = "inhibitory";
    constexpr const char* MODULATORY = "modulatory";
    constexpr const char* SENSORY = "sensory";
    constexpr const char* MOTOR = "motor";
    constexpr const char* INTERNAL = "internal";
}

// Synapse Type Names
namespace SynapseTypeNames {
    constexpr const char* EXCITATORY = "excitatory";
    constexpr const char* INHIBITORY = "inhibitory";
    constexpr const char* MODULATORY = "modulatory";
    constexpr const char* ELECTRICAL = "electrical";
    constexpr const char* GAP_JUNCTION = "gap_junction";
}

// Default Configuration Values
namespace DefaultConfig {
    // Brain parameters
    constexpr int64_t NEURON_COUNT = 500;
    constexpr float SYNAPSE_DENSITY = 0.1f;
    constexpr float CONNECTION_PROBABILITY = 0.05f;
    constexpr float INITIAL_WEIGHT_MEAN = 0.5f;
    constexpr float INITIAL_WEIGHT_STD = 0.1f;
    constexpr float V_THRESH = -50.0f;
    constexpr float V_REST = -70.0f;
    constexpr float V_RESET = -75.0f;
    constexpr float TAU_MEM = 20.0f;
    constexpr float TAU_REF = 2.0f;
    
    // Plasticity parameters
    constexpr bool STDP_ENABLE = true;
    constexpr float STDP_LEARNING_RATE = 0.001f;
    constexpr float STDP_TAU_PLUS = 20.0f;
    constexpr float STDP_TAU_MINUS = 20.0f;
    constexpr bool HEBBIAN_ENABLE = true;
    constexpr bool STRUCTURAL_ENABLE = true;
    
    // Neuromodulation parameters
    constexpr float DOPAMINE_SCALE = 1.0f;
    constexpr bool CURIOSITY_ENABLE = true;
    constexpr bool NOVELTY_ENABLE = true;
    
    // Simulation parameters
    constexpr int64_t RANDOM_SEED = 42;
    constexpr double SIMULATION_TIMESTEP = 0.001;
    constexpr int64_t REGION_COUNT = 1;
}

} // namespace nlm