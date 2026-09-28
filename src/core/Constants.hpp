#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <limits>

namespace nlm {

// ==============================================
// WORLD/SIMULATION PARAMETERS
// ==============================================

// World dimensions
static constexpr float WORLD_WIDTH = 20.0f;           // meters
static constexpr float WORLD_HEIGHT = 20.0f;          // meters

// Vision parameters
static constexpr size_t WORLD_VISION_WIDTH = 16;      // pixels
static constexpr size_t WORLD_VISION_HEIGHT = 16;     // pixels

// Energy parameters
static constexpr float MAX_ENERGY = 100.0f;            // unitless (max agent energy)
static constexpr float ENERGY_DECAY_RATE = 0.01f;     // per timestep
static constexpr float RESOURCE_ENERGY_GAIN = 30.0f;  // energy from resource

// Random seed
static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;   // reproducible initialization

// RNG linear congruential generator constants
static constexpr uint64_t LCG_A = 6364136223846793005ULL;
static constexpr uint64_t LCG_C = 1442695040888963407ULL;

// ==============================================
// AGENT/MOTOR PARAMETERS
// ==============================================

// Motor actions and thresholds
static constexpr size_t MOTOR_ACTION_COUNT = 8;       // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
static constexpr float ACTION_COOLDOWN = 0.1f;        // seconds between actions
static constexpr float MOVE_SPEED = 3.0f;              // units per timestep
static constexpr float TURN_SPEED = 2.0f;              // radians per timestep

// Movement costs and rewards
static constexpr float MOVEMENT_COST = 0.01f;          // reward penalty for moving
static constexpr float BACKWARD_MOVEMENT_COST = 0.02f; // higher cost for backward
static constexpr float TURNING_COST = 0.005f;         // reward penalty for turning
static constexpr float WALL_COLLISION_COST = 0.05f;   // reward penalty for wall collision

// ==============================================
// SENSORY PARAMETERS
// ==============================================

// Sensory input sizes
static constexpr size_t SENSORY_VISION_INPUT_SIZE = 256;    // 16x16 vision grid
static constexpr size_t SENSORY_TOUCH_INPUT_SIZE = 8;       // proximity sensors
static constexpr size_t SENSORY_INTERNAL_INPUT_SIZE = 4;    // internal state sensors
static constexpr size_t SENSORY_PROPrioception_INPUT_SIZE = 6; // proprioceptive sensors

static constexpr size_t SENSORY_INPUT_COUNT = SENSORY_VISION_INPUT_SIZE + 
                                           SENSORY_TOUCH_INPUT_SIZE + 
                                           SENSORY_INTERNAL_INPUT_SIZE + 
                                           SENSORY_PROPrioception_INPUT_SIZE;

// Sensory distribution
static constexpr size_t SENSORY_VISION_NEURON_COUNT = SENSORY_VISION_INPUT_SIZE;
static constexpr size_t SENSORY_TOUCH_NEURON_COUNT = SENSORY_TOUCH_INPUT_SIZE;
static constexpr size_t SENSORY_INTERNAL_NEURON_COUNT = SENSORY_INTERNAL_INPUT_SIZE;
static constexpr size_t SENSORY_PROPrioception_NEURON_COUNT = SENSORY_PROPrioception_INPUT_SIZE;

// Sensory parameters
static constexpr float SENSORY_NOVELTY_DECAY = 0.99f;      // decay rate for novelty

// ==============================================
// NEURAL/NEURON PARAMETERS
// ==============================================

// Neuron biophysical parameters
static constexpr float MEMBRANE_CAPACITANCE = 1.0f;        // nF, typical for cortical neurons
static constexpr float TIME_CONSTANT = 20.0f;             // ms, membrane time constant
static constexpr float LEAK_CONDUCTANCE = 10.0f;           // nS, ion channel conductance

// Synapse parameters
static constexpr float SYNAPTIC_WEIGHT_SCALE = 10.0f;      // scale factor for synaptic current
static constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;       // lower bound for synaptic weight
static constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;        // upper bound for synaptic weight

// Refractory period parameters
static constexpr uint32_t MIN_REFRACTORY_STEPS = 2;       // minimum refractory period
static constexpr uint32_t MAX_REFRACTORY_STEPS = 10;      // maximum refractory period

// Spike history and limits
static constexpr size_t MAX_SPIKE_HISTORY = 100;          // number of spikes to store

// Neural adaptation
static constexpr float ADAPTATION_STRENGTH = 0.05f;       // spike-frequency adaptation strength
static constexpr float ADAPTATION_TIME_CONSTANT = 50.0f;  // adaptation decay time constant

// Membrane potential thresholds
static constexpr float THRESHOLD_OFFSET = -55.0f;          // base spike threshold
static constexpr float RESTING_POTENTIAL = -70.0f;        // resting membrane potential
static constexpr float RESET_POTENTIAL = -70.0f;         // potential after spike
static constexpr float THRESHOLD_NOISE_RANGE = 10.0f;     // variability in threshold
static constexpr float RESTING_POTENTIAL_NOISE_RANGE = 10.0f; // variability in resting potential
static constexpr float RESET_POTENTIAL_NOISE_RANGE = 5.0f;  // variability in reset potential

// Reversal potentials
static constexpr float REVERSAL_POTENTIAL_EXCITATORY = 0.0f;     // excitatory reversal potential
static constexpr float REVERSAL_POTENTIAL_INHIBITORY = -80.0f;   // inhibitory reversal potential

// Dendrite and spine parameters
static constexpr float DENDRITE_LENGTH = 100.0f;         // microns, typical dendrite length
static constexpr float SPINE_DENSITY = 1.0f;             // spines per micron
static constexpr float RECEPTOR_PROBABILITY = 0.3f;      // probability of receptor presence

// Membrane voltage constraints
static constexpr float MEMBRANE_MIN_VOLTAGE = -100.0f;    // absolute minimum voltage
static constexpr float MEMBRANE_MAX_VOLTAGE = 50.0f;     // absolute maximum voltage

// ==============================================
// SIMULATION TIME PARAMETERS
// ==============================================

// Time step and frequency
static constexpr double TIMESTEP_DURATION = 0.001;      // seconds (1ms)
static constexpr double SIMULATION_FREQUENCY = 1000.0;  // Hz (1 / TIMESTEP_DURATION)

// Integration parameters
static constexpr double INTEGRATION_SCALAR = 1000.0;    // for converting dV/dt to voltage

// ==============================================
// PLASTICITY PARAMETERS
// ==============================================

// STDP parameters
static constexpr float STDP_LTP_WEIGHT = 0.01f;         // long-term potentiation weight
static constexpr float STDP_LTD_WEIGHT = 0.012f;        // long-term depression weight
static constexpr float STDP_TIME_CONSTANT = 20.0f;      // STDP time window (ms)

// Structural plasticity
static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;    // rate of new synapse formation
static constexpr float PRUNING_RATE = 0.00001f;         // rate of synapse elimination

// ==============================================
// MEMORY PARAMETERS
// ==============================================

// Working memory
static constexpr size_t WORKING_MEMORY_CAPACITY_DIVISOR = 10; // working memory capacity relative to neuron count

// Episodic memory
static constexpr size_t MAX_EPISODIC_EPISODES = 1000;   // maximum number of episodes to store

// Associative memory
static constexpr size_t ASSOCIATIVE_MEMORY_SIZE = 100;  // capacity of associative memory

// ==============================================
// COGNITION PARAMETERS
// ==============================================

// Planning
static constexpr size_t PLANNING_DEPTH = 5;              // depth of planning horizon

// Attention
static constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;  // strength of attentional inhibition
static constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;   // strength of attentional excitation

// ==============================================
// NEUROMODULATION PARAMETERS
// ==============================================

// Neuromodulator parameters
static constexpr float NOVELTY_DECAY_RATE = 0.9f;        // novelty decay rate
static constexpr float REWARD_SCALE_FACTOR = 1.5f;       // scaling factor for reward

// ==============================================
// DEVELOPMENT PARAMETERS
// ==============================================

// Developmental stages
static constexpr double DEVELOPMENTAL_STAGE_INITIAL = 0.0;     // seconds
static constexpr double DEVELOPMENTAL_STAGE_CRITICAL_PERIOD = 100.0; // seconds
static constexpr double DEVELOPMENTAL_STAGE_MATURATION = 300.0;   // seconds
static constexpr double DEVELOPMENTAL_STAGE_ADULT = 500.0;       // seconds
static constexpr double DEVELOPMENTAL_STAGE_AGING = 800.0;       // seconds

// ==============================================
// EXPERIMENT PARAMETERS
// ==============================================

// Experiment configuration
static constexpr size_t EXPERIMENT_EPISODE_DURATION = 200;    // steps per episode
static constexpr double EXPERIMENT_TOTAL_DURATION = 1000.0;    // total duration for phase 3 experiment

// ==============================================
// ADDITIONAL CONSTANTS FOR SPECIFIC SYSTEMS
// ==============================================

// Position and boundary constraints
static constexpr float AGENT_POSITION_PADDING = 2.0f;     // safe margin from world boundaries
static constexpr float OBJECT_POSITION_PADDING = 1.0f;    // safe margin for object placement

// Resource parameters
static constexpr float RESOURCE_ENERGY_VALUE = 30.0f;      // energy provided by resource
static constexpr float RESOURCE_ATTRACTION_FACTOR = 0.4f;  // how much resource attracts agent

// Movement and interaction parameters
static constexpr float INTERACTION_RANGE = 1.0f;          // range for agent-object interaction
static constexpr float RAY_COUNT = 8.0f;                  // number of vision rays

// Vision parameters
static constexpr float VISION_FIELD_OF_VIEW = M_PI / 2.0f; // 90 degree FOV
static constexpr float VISION_DEFAULT_VALUE = 1.0f;       // default vision value (far/empty)

// Sensory parameters
static constexpr float SENSORY_TOUCH_DECAY = 1.5f;        // decay factor for touch sensing
static constexpr float SENSORY_INTERNAL_MOVEMENT_FACTOR = 1.0f; // movement state factor
static constexpr float SENSORY_INTERNAL_TURNING_FACTOR = 1.0f;  // turning state factor

// ==============================================
// MOTOR PARAMETERS
// ==============================================

// Motor system parameters
static constexpr size_t MOTOR_COMMAND_COUNT = 8;          // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
static constexpr size_t MOTOR_COMMAND_LOOK_LEFT_INDEX = 4;
static constexpr size_t MOTOR_COMMAND_LOOK_RIGHT_INDEX = 5;
static constexpr size_t MOTOR_COMMAND_INTERACT_INDEX = 6;
static constexpr size_t MOTOR_COMMAND_WAIT_INDEX = 7;

// ==============================================
// CONFIG/PARAMETER MAPPING
// ==============================================

// Configuration parameter names (for config file lookup)
namespace Config {
    static constexpr const char* RANDOM_SEED = "random_seed";
    static constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    static constexpr const char* NEURON_COUNT = "neuron_count";
    static constexpr const char* REGION_COUNT = "region_count";
    static constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    static constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    static constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    static constexpr const char* STDP_TAU = "stdp_tau";
    static constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    static constexpr const char* PRUNING_RATE = "pruning_rate";
    static constexpr const char* CHECKPOINT_DIR = "checkpoint_dir";
    static constexpr const char* REPLAY_INTERVAL = "replay_interval";
    static constexpr const char* CONSOLIDATION_INTERVAL = "consolidation_interval";
}

} // namespace nlm

#endif // CONSTANTS_HPP

// Sensory type count (vision, touch, internal, proprioception)
static constexpr size_t SENSORY_TYPE_COUNT = 4;  // Number of sensory neuron groups


// Developmental age thresholds (in seconds)
static constexpr double DEVELOPMENTAL_AGE_MILITARY_SERVICE = 300.0;  // ~5 minutes
static constexpr double DEVELOPMENTAL_AGE_ADULTHOOD = 900.0;          // ~15 minutes

// Plasticity parameters
static constexpr float STDP_LTP_WEIGHT = 0.01f;  // long-term potentiation weight


// Neuron parameter ranges for initialization
static constexpr float MEMBRANE_CAPACITANCE_MIN = 0.0f;
static constexpr float MEMBRANE_CAPACITANCE_MAX = 10.0f;
static constexpr float TIME_CONSTANT_MIN = 0.0f;
static constexpr float TIME_CONSTANT_MAX = 100.0f;
static constexpr float LEAK_CONDUCTANCE_MIN = 0.0f;
static constexpr float LEAK_CONDUCTANCE_MAX = 100.0f;


// Brain system parameters
static constexpr size_t CONSOLIDATION_INTERVAL_DEFAULT = 1000;      // steps
static constexpr size_t MAX_EPISODIC_EPISODES = 1000;              // episodes
static constexpr size_t CHECKPOINT_MANAGER_SIZE = 10000;            // checkpoints
static constexpr size_t CHECKPOINT_MANAGER_INTERVAL = 5;            // checkpoint intervals
static constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000;        // steps
static constexpr size_t NEURON_COUNT_DEFAULT = 1000;                // neurons
static constexpr float SYNAPTIC_CURRENT_SCALE_FACTOR = 10.0f;        // for synaptic current
static constexpr float DEVELOPMENT_THRESHOLD_5 = 5.0f;              // voltage threshold
static constexpr float DEVELOPMENT_DENOMINATOR_20 = 20.0f;         // developmental scaling
static constexpr float PLASTICITY_ADJUSTMENT_0001 = 0.001f;          // plasticity adjustment
static constexpr float SPINE_PRUNING_BASE = 0.00001f;               // base pruning rate


// Brain configuration defaults
static constexpr double SIMULATION_TIMESTEP_DEFAULT = 0.001;    // default simulation timestep
static constexpr double DEVELOPMENT_TIME_SCALE = 1000.0;      // scale for development timing

