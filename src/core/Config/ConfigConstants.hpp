// Configuration constants for NLM (Neural Learning Machine)
// Centralized configuration parameters to avoid duplication and improve maintainability

#ifndef NLM_CONFIG_CONSTANTS
#define NLM_CONFIG_CONSTANTS

namespace nlm {
namespace config {

// Simulation constants
const double DEFAULT_SIMULATION_TIMESTEP = 0.001;           // Default simulation timestep in seconds
const double DEFAULT_WORLD_WIDTH = 16.0;                     // Default world width
const double DEFAULT_WORLD_HEIGHT = 16.0;                    // Default world height
const double DEFAULT_WORLD_VISION_WIDTH = 8.0;               // Default vision width
const double DEFAULT_WORLD_VISION_HEIGHT = 8.0;              // Default vision height

// Brain constants
const int64_t DEFAULT_NEURON_COUNT = 500;                    // Default number of neurons
const int64_t DEFAULT_REGION_COUNT = 1;                      // Default number of regions
const float DEFAULT_CONNECTION_PROBABILITY = 0.15f;          // Default connection probability

// Random seed
const int64_t DEFAULT_RANDOM_SEED = 42;                      // Default random seed

// STDP (Spiking Temporal Difference Learning) parameters
const float DEFAULT_STDP_LTP_WEIGHT = 0.01f;                  // Default STDP LTP weight
const float DEFAULT_STDP_LTD_WEIGHT = 0.012f;                 // Default STDP LTD weight
const float DEFAULT_STDP_TAU = 20.0f;                         // Default STDP time constant
const float DEFAULT_STDP_LTP_WEIGHT_PHASE2 = 0.02f;           // Phase 2 STDP LTP weight (main.cpp)
const float DEFAULT_STDP_LTD_WEIGHT_PHASE2 = 0.015f;          // Phase 2 STDP LTD weight (main.cpp)

// Structural plasticity parameters
const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;            // Default synaptogenesis rate
const float DEFAULT_PRUNING_RATE = 0.00001f;                 // Default pruning rate

// Memory system constants
const size_t DEFAULT_WORKING_MEMORY_CAPACITY = 50;            // Default working memory capacity
const size_t DEFAULT_MAX_EPISODES = 1000;                     // Default maximum episodic memory episodes

// Neuromodulation constants
const float DEFAULT_DOPAMINE_PEAK = 1.0f;                    // Default dopamine peak level
const float DEFAULT_DOPAMINE_BASELINE = 0.0f;                // Default dopamine baseline
const float DEFAULT_DOPAMINE_DECAY_RATE = 0.1f;              // Default dopamine decay rate
const float DEFAULT_DOPAMINE_RELEASE_RATE = 1.0f;            // Default dopamine release rate

const float DEFAULT_NOVITY_THRESHOLD = 0.3f;                 // Default novelty detection threshold
const float DEFAULT_NOVITY_DECAY_RATE = 0.1f;                // Default novelty decay rate

const float DEFAULT_CURIOUSITY_NOVELTY_WEIGHT = 0.5f;        // Default curiosity weight for novelty
const float DEFAULT_CURIOUSITY_PREDICTION_ERROR_WEIGHT = 0.5f; // Default curiosity weight for prediction error
const float DEFAULT_CURIOUSITY_DECAY_RATE = 0.05f;           // Default curiosity decay rate

const float DEFAULT_PREDICTION_ERROR_DECAY = 0.05f;          // Default prediction error decay rate

// Development system constants
const double DEFAULT_DEVELOPMENT_STAGE_TRANSITION_INTERVAL = 60.0;  // ~1 minute between stages
const float DEFAULT_INITIAL_PLASTICITY_MODIFIER = 1.0f;        // Initial plasticity modifier
const float DEFAULT_CRITICAL_PERIOD_PLASTICITY_MODIFIER = 0.8f;    // Critical period plasticity modifier
const float DEFAULT_MATURATION_PLASTICITY_MODIFIER = 0.5f;     // Maturation plasticity modifier
const float DEFAULT_ADULT_PLASTICITY_MODIFIER = 0.2f;          // Adult plasticity modifier

// Memory integration constants
const size_t DEFAULT_REPLAY_INTERVAL = 100;                  // Replay every 100 steps
const size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;           // Consolidate every 1000 steps

// Learning and plasticity constants
const float DEFAULT_ELIGIBILITY_TRACE_DECAY = 0.1f;         // Default eligibility trace decay
const float DEFAULT_SYNAPTIC_WEIGHT_CHANGE_CLAMP = 1.0f;     // Maximum weight change per step
const float DEFAULT_SYNAPTIC_WEIGHT_MIN = 0.0f;              // Minimum synaptic weight
const float DEFAULT_SYNAPTIC_WEIGHT_MAX = 1.0f;              // Maximum synaptic weight

// Action selection constants
const float DEFAULT_MOTOR_ACTIVITY_THRESHOLD = 0.5f;        // Minimum activity to trigger motor action
const float DEFAULT_EXPLORATION_CHANCE = 0.3f;              // Maximum exploration chance from curiosity
const float DEFAULT_NOVELTY_BASED_EXPLORATION_THRESHOLD = 0.3f; // Novelty threshold for exploration

// Logging and debugging constants
const size_t LOG_STATUS_INTERVAL = 1000;                     // Log status every 1000 steps
const size_t EXPERIMENT_STATUS_INTERVAL = 1000;              // Experiment status every 1000 steps

// Performance constants
const size_t PERFORMANCE_CHECKPOINT_INTERVAL = 100;         // Checkpoint every 100 steps
const size_t PERFORMANCE_STATISTICS_INTERVAL = 100;        // Collect stats every 100 steps

// File and I/O constants
const std::string DEFAULT_CHECKPOINT_FILENAME = "/tmp/nlm_checkpoint.bin"; // Default checkpoint filename
const std::string DEFAULT_CONFIG_FILENAME = "configs/default.cfg";      // Default config filename
const std::string DEFAULT_LOG_FILENAME = "nlm.log";                      // Default log filename

} // namespace config
} // namespace nlm

#endif // NLM_CONFIG_CONSTANTS
