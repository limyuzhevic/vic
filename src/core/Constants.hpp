// Neural computation constants
// Contains biologically inspired default values for neuron parameters
// and simulation settings

#pragma once

namespace nlm {
namespace constants {

// Neuron membrane parameters (mV, nF, Hz)
constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;     // mV - typical resting potential
constexpr float DEFAULT_THRESHOLD = -55.0f;             // mV - typical firing threshold
constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;       // mV - reset after spike
constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;        // nS - leak conductance
constexpr float DEFAULT_SYNAPSE_CONDUCTANCE = 0.0f;      // nS - baseline synaptic conductance
constexpr float DEFAULT_FIRING_RATE = 0.0f;              // Hz - initial firing rate
constexpr float DEFAULT_ADAPTATION_INCREMENT = 1.0f;     // adaptation variable increment
constexpr float DEFAULT_LAST_SPIKE_TIME = -1.0f;         // timestamp indicator for no spike
constexpr int DEFAULT_REFECTORY_PERIOD = 5;              // simulation steps - default refractory period

// Biological variation ranges (mV)
constexpr float MEMBRANE_POTENTIAL_VARIATION = 3.0f;    // mV - ±3mV around resting
constexpr float THRESHOLD_VARIATION = 2.0f;              // mV - ±2mV around default
constexpr float RESET_POTENTIAL_VARIATION = 5.0f;       // mV - ±5mV above resting

// Refractory period range (ms simulation steps)
constexpr int REFECTORY_PERIOD_MIN = 2;                   // minimum refractory steps
constexpr int REFECTORY_PERIOD_MAX = 10;                  // maximum refractory steps

// Simulation parameters (seconds, Hz)
constexpr double DEFAULT_TIMESTEP = 0.001;               // s - 1ms simulation timestep
constexpr float DEFAULT_CURRENT_INJECTION = 50.0f;       // mV - strong excitatory input
constexpr float SENSORY_INPUT_SCALE = 10.0f;              // scaling factor for sensory input
constexpr float WORKING_MEMORY_THRESHOLD = 0.5f;         // normalized threshold for memory storage
constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;    // attention modulation parameter
constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;    // attention modulation parameter

// Plasticity parameters
constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;         // STDP long-term potentiation weight
constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;        // STDP long-term depression weight
constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;      // STDP trace decay constant
constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;   // structural plasticity rate
constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // structural plasticity rate

// Memory system parameters
constexpr size_t WORKING_MEMORY_CAPACITY_FACTOR = 10;    // capacity = neuron_count / this
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;    // maximum stored episodes
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;    // consolidation confidence threshold
constexpr size_t REPLAY_INTERVAL = 100;                   // steps between memory replays
constexpr size_t CONSOLIDATION_INTERVAL = 1000;          // steps between consolidation events

// Development parameters
constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000;    // steps between development updates
constexpr float INITIAL_PLASTICITY_MOD = 1.0f;            // high plasticity in initial stage
constexpr float CRITICAL_PERIOD_MOD = 0.8f;              // reduced plasticity
constexpr float MATURATION_MOD = 0.5f;                    // moderate plasticity
constexpr float ADULT_MOD = 0.2f;                         // stable plasticity

// Event system parameters
constexpr size_t MAX_SPIKE_HISTORY = 100;                 // maximum recorded spike events
constexpr float SPIKE_HISTORY_CLEANUP_THRESHOLD = 0.95f;  // history cleanup threshold
constexpr size_t DELAYED_SPIKE_MAX_QUEUE_SIZE = 1000;     // maximum pending delayed spikes

// Action system parameters
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.0f;         // firing threshold for action selection
constexpr float MOVE_FORWARD_EXCITATION = 1.0f;          // action selection weight

// Numerical tolerances
constexpr float WEIGHT_CHANGE_TOLERANCE = 0.001f;        // threshold for significant weight change
constexpr float SYNAPSE_WEIGHT_INCREMENT = 0.001f;       // small weight adjustment
constexpr float CONVERGENCE_TOLERANCE = 0.0001f;         // for learning convergence checks

// Logging and monitoring
constexpr int LOG_LEVEL_INFO = 1;                        // log level constant
constexpr int LOG_LEVEL_WARNING = 2;                     // log level constant
constexpr int LOG_LEVEL_ERROR = 3;                       // log level constant

// Configuration defaults
constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";  // default config file
constexpr const char* CHECKPOINT_DIR = "./checkpoints";            // default checkpoint directory
constexpr size_t MAX_CHECKPOINTS = 10000;                    // maximum checkpoints to retain
constexpr CompressionLevel DEFAULT_COMPRESSION = CompressionLevel::Balanced;  // default compression

} // namespace constants
} // namespace nlm
