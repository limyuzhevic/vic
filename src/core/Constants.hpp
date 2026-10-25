// NLM (熙然) Neural Learning Machine - Configuration Constants
// 
// This file contains named constants to replace magic numbers throughout the codebase,
// improving code readability, maintainability, and consistency.

#pragma once

namespace nlm {

// ============================================================================
// NEURON DYNAMICS CONSTANTS
// ============================================================================

/**
 * @brief Resting membrane potential (mV) - Default equilibrium state
 * 
 * Typical resting potential for biological neurons: -65 to -70 mV
 * Set to -70 mV as baseline resting state
 */
const float NEURON_RESTING_POTENTIAL = -70.0f;

/**
 * @brief Firing threshold (mV) - Spike initiation point
 * 
 * Typical neuronal threshold: -55 to -50 mV
 * Set to -55.0f for reliable spike generation
 */
const float NEURON_FIRING_THRESHOLD = -55.0f;

/**
 * @brief Post-spike reset potential (mV) - Voltage after spike
 * 
 * After firing, neuron resets to a value below resting
 * Set to -70.0f to match resting potential for simplicity
 */
const float NEURON_RESET_POTENTIAL = -70.0f;

/**
 * @brief Membrane leak conductance (nS) - Natural decay rate
 * 
 * Determines how quickly membrane potential decays toward resting
 * Typical range: 1-20 nS
 */
const float NEURON_LEAK_CONDUCTANCE = 10.0f;

/**
 * @brief Default refractory period (simulation steps) - Minimum interspike interval
 * 
 * Refractory period prevents immediate re-firing after a spike
 * Typical biological refractory: 1-5 ms
 * Set to 5 simulation steps for robustness
 */
const uint32_t NEURON_REFECTORY_PERIOD = 5;

/**
 * @brief Adaptation variable scaling factor - Spike-frequency adaptation
 * 
 * Controls how much spikes affect neuron excitability over time
 * Used for implementing adaptive firing rates
 */
const float NEURON_ADAPTATION_SCALE = 0.1f;

/**
 * @brief Minimum spike history for plasticity - Prevent spurious updates
 * 
 * Requires minimum number of spikes before synaptic changes can occur
 * Helps filter out noise and ensure stable learning
 */
const size_t PLASTICITY_MIN_SPIKE_COUNT = 1;

// ============================================================================
// PLASTICITY RULE CONSTANTS
// ============================================================================

/**
 * @brief STDP LTP (Long-Term Potentiation) weight factor - Positive synaptic change
 * 
 * When pre fires before post, strengthen connection
 * Typical biological A+: 0.01-0.05
 */
const float STDP_LTP_WEIGHT = 0.01f;

/**
 * @brief STDP LTD (Long-Term Depression) weight factor - Negative synaptic change
 * 
 * When post fires before pre, weaken connection
 * Biological A- is typically slightly larger than A+
 */
const float STDP_LTD_WEIGHT = 0.012f;

/**
 * @brief STDP time constant (ms) - Decay window for spike timing
 * 
 * Controls how quickly spike-time influence decays with time difference
 * Typical biological τ: 10-100 ms
 */
const float STDP_TIME_CONSTANT = 20.0f;

/**
 * @brief STDP exponential decay cutoff threshold - Numerical stability
 * 
 * Minimum value for exponential calculations to avoid numerical issues
 * Prevents overflow/underflow in exp() calculations
 */
const float STDP_EXP_CUTOFF = 1e-6f;

/**
 * @brief STDP eligibility trace decay rate - Temporal credit assignment
 * 
 * How quickly synaptic eligibility decays when not reinforced
 * Needed for reward-modulated learning
 */
const float STDP_ELIGIBILITY_DECAY = 0.999f;

/**
 * @brief Hebbian learning rate - Strength of co-activity learning
 * 
 * Controls how much correlated activity affects synaptic weights
 * Typical range: 0.001-0.1
 */
const float HEBBIAN_LEARNING_RATE = 0.01f;

/**
 * @brief Hebbian correlation window (ms) - Time window for correlation detection
 * 
 * Spikes within this window are considered correlated
 * Typical biological correlation windows: 20-100 ms
 */
const float HEBBIAN_CORRELATION_WINDOW = 100.0f;

/**
 * @brief Synaptic weight minimum bound - Lower safety limit
 * 
 * Absolute minimum synaptic weight to prevent runaway negative values
 * Typical biological range: [-1.0, 1.0] in normalized units
 */
const float SYNAPTIC_WEIGHT_MIN = -1.0f;

/**
 * @brief Synaptic weight maximum bound - Upper safety limit
 * 
 * Absolute maximum synaptic weight to prevent runaway positive values
 */
const float SYNAPTIC_WEIGHT_MAX = 1.0f;

/**
 * @brief Synapse efficacy scaling factor - Modulation of weight changes
 * 
 * Scales STDP weight changes based on current synapse efficacy
 * Used for implementing multiplicative normalization
 */
const float SYNAPTIC_EFFICACY_SCALE = 1.0f;

// ============================================================================
// STRUCTURAL PLASTICITY CONSTANTS
// ============================================================================

/**
 * @brief Synaptogenesis rate - Probability of new synapse formation per step
 * 
 * Controls how frequently new synapses are created through structural plasticity
 * Typical rates: 0.0001-0.001 (very low for stability)
 */
const float STRUCTURAL_SYNAPTOGENESIS_RATE = 0.0001f;

/**
 * @brief Pruning rate - Probability of synapse elimination per step
 * 
 * Controls how frequently unused or weak synapses are removed
 * Typical rates: 0.00001-0.0001
 */
const float STRUCTURAL_PRUNING_RATE = 0.00001f;

/**
 * @brief Minimum weight threshold - Synapses below this are weak
 * 
 * Synapses with weight below this threshold are more likely to be pruned
 * Used in structural plasticity decision making
 */
const float STRUCTURAL_MIN_WEIGHT_THRESHOLD = 0.05f;

/**
 * @brief Activity threshold - Minimum activity for synapse retention
 * 
 * Synapses receiving little activity are considered unused
 * Used for activity-dependent pruning
 */
const float STRUCTURAL_ACTIVITY_THRESHOLD = 0.001f;

/**
 * @brief Maximum synapses per neuron - Structural constraint
 * 
 * Limits outgoing connections from single neuron to prevent explosion
 * Typical biological constraints: 1000-10000 connections per neuron
 */
const size_t STRUCTURAL_MAX_SYNAPSES_PER_NEURON = 100;

/**
 * @brief Maximum total synapses - Global structural constraint
 * 
 * Upper bound on total number of synapses in the network
 * Prevents unbounded growth and resource exhaustion
 */
const size_t STRUCTURAL_MAX_TOTAL_SYNAPSES = 1000000;

/**
 * @brief Minimum activity for synaptogenesis - Activity-dependent formation
 * 
 * Neurons must be sufficiently active to form new connections
 * Ensures structural plasticity focuses on active circuits
 */
const float STRUCTURAL_MIN_ACTIVITY_FOR_CREATION = 0.1f;

/**
 * @brief Initial synaptic weight for new connections - Baseline strength
 * 
 * Default weight for newly created synapses
 * Set between 0.1 and 0.2 to ensure initial functional connections
 */
const float STRUCTURAL_INITIAL_WEIGHT = 0.1f;

/**
 * @brief Weight variation for new synapses - Randomization range
 * 
 * Adds randomness to initial weights of new synapses
 * Ensures diversity in newly formed connections
 */
const float STRUCTURAL_WEIGHT_VARIATION = 0.1f;

// ============================================================================
// NEUROMODULATION CONSTANTS
// ============================================================================

/**
 * @brief Dopamine baseline level - Default neuromodulator concentration
 * 
 * Starting level for dopamine system
 * Biological baseline varies by brain region and behavioral state
 */
const float NEUROMODULATION_DOPAMINE_BASELINE = 0.1f;

/**
 * @brief Dopamine plasticity modulation factor - Learning rate scaling
 * 
 * How much dopamine affects synaptic plasticity rates
 * Higher values = stronger dopamine influence on learning
 */
const float NEUROMODULATION_DOPAMINE_PLASTICITY_FACTOR = 1.0f;

/**
 * @brief Dopamine excitability modulation factor - Neural excitability scaling
 * 
 * How much dopamine affects neural excitability
 * Higher values = stronger dopamine influence on firing
 */
const float NEUROMODULATION_DOPAMINE_EXCITABILITY_FACTOR = 0.5f;

/**
 * @brief Curiosity baseline exploration rate - Default exploration tendency
 * 
 * Controls how often the system explores novel stimuli
 * Higher values = more exploratory behavior
 */
const float NEUROMODULATION_CURIOSITY_BASELINE = 0.5f;

/**
 * @brief Novelty detection sensitivity - Response to new patterns
 * 
 * Threshold for what constitutes a novel stimulus
 * Lower values = more sensitive to novelty
 */
const float NEUROMODULATION_NOVELTY_SENSITIVITY = 0.5f;

/**
 * @brief Prediction error discount factor - Temporal discounting
 * 
 * How quickly prediction errors decay over time
 * Used in reward prediction error calculations
 */
const float NEUROMODULATION_PREDICTION_ERROR_DECAY = 0.95f;

// ============================================================================
// SIMULATION TIMING CONSTANTS
// ============================================================================

/**
 * @brief Default simulation timestep (seconds) - Integration step size
 * 
 * Time step for numerical integration of differential equations
 * Smaller values = more accurate but slower simulation
 * Typical biological timescales: 0.1-1.0 ms
 */
const double SIMULATION_TIMESTEP = 0.001;

/**
 * @brief Maximum stable timestep - Numerical stability limit
 * 
 * Upper bound for timestep to ensure numerical stability
 * Exceeding this may cause simulation instability
 */
const double SIMULATION_MAX_TIMESTEP = 0.01;

/**
 * @brief Time window for spike history - How long to remember spikes
 * 
 * Duration of spike history used for plasticity calculations
 * Longer windows allow more complex temporal patterns
 * Typical range: 100-1000 ms
 */
const Timestamp SPIKE_HISTORY_WINDOW = 100.0;

/**
 * @brief Minimum time difference for STDP - Temporal resolution
 * 
 * Minimum distinguishable time difference for STDP calculations
 * Set to ensure numerical precision in timing calculations
 */
const float STDP_MIN_TIME_DIFFERENCE = 1e-6f;

// ============================================================================
// MEMORY SYSTEM CONSTANTS
// ============================================================================

/**
 * @brief Working memory capacity factor - Information storage limit
 * 
 * Proportional factor for working memory capacity based on neuron count
 * Typical working memory: 7±2 items, scales with network size
 */
const float MEMORY_WORKING_CAPACITY_FACTOR = 0.1f;

/**
 * @brief Episodic memory episode storage - Number of experiences stored
 * 
 * Maximum number of episodes that can be stored
 * Longer simulations need larger memory
 */
const size_t MEMORY_MAX_EPISODES = 1000;

/**
 * @brief Memory consolidation threshold - Important episode retention
 * 
 * Probability or threshold for consolidating episodes into long-term storage
 * Higher values = more aggressive consolidation
 */
const float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;

/**
 * @brief Working memory decay rate - Information loss over time
 * 
 * How quickly working memory traces decay without reinforcement
 * Higher values = faster decay
 */
const float MEMORY_WORKING_DECAY_RATE = 0.99f;

/**
 * @brief Episodic replay interval - How often to replay memories
 * 
 * Simulation steps between memory replay events
 * Used for strengthening and consolidating memories
 */
const size_t MEMORY_REPLAY_INTERVAL = 100;

// ============================================================================
// DEVELOPMENT CONSTANTS
// ============================================================================

/**
 * @brief Developmental stage multiplier - Plasticity rate scaling
 * 
 * How each developmental stage affects plasticity rates
 * Initial > Maturation > Adult > Aging
 */
const float DEVELOPMENT_STAGE_MULTIPLIERS[] = {1.0f, 0.8f, 0.5f, 0.2f, 0.1f};

/**
 * @brief Developmental stage count - Number of developmental stages
 * 
 * Total stages in the developmental timeline
 * Should match values in DevelopmentalStage enum
 */
const size_t DEVELOPMENT_STAGE_COUNT = 5;

/**
 * @brief Development rate scaling - Overall developmental progression
 * 
 * How quickly the system develops through stages
 * Higher values = faster development
 */
const float DEVELOPMENT_RATE_SCALE = 0.001f;

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

/**
 * @brief Configuration random seed - Default random number generator seed
 * 
 * Default seed for reproducible simulations
 * Can be overridden by command line or file
 */
const int64_t CONFIG_DEFAULT_RANDOM_SEED = 42;

/**
 * @brief Configuration simulation timestep - Default simulation step
 * 
 * Default time step for simulation
 * Can be overridden by command line arguments
 */
const double CONFIG_DEFAULT_SIMULATION_TIMESTEP = 0.001;

/**
 * @brief Configuration neuron count - Default network size
 * 
 * Default number of neurons for simulations
 * Set smaller for testing, larger for production runs
 */
const int64_t CONFIG_DEFAULT_NEURON_COUNT = 500;

/**
 * @brief Configuration region count - Default number of brain regions
 * 
 * Default number of neural regions for organization
 * 1 = single region, more = distributed processing
 */
const int64_t CONFIG_DEFAULT_REGION_COUNT = 1;

/**
 * @brief Configuration connection probability - Random connectivity
 * 
 * Probability of connection between neuron pairs
 * Lower values = sparse connectivity, higher = dense
 */
const float CONFIG_DEFAULT_CONNECTION_PROBABILITY = 0.15f;

/**
 * @brief Configuration synaptogenesis rate - Default structural plasticity
 * 
 * Default rate for creating new synapses
 * Can be overridden by specific configuration files
 */
const float CONFIG_DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;

/**
 * @brief Configuration pruning rate - Default synaptic elimination
 * 
 * Default rate for removing weak synapses
 */
const float CONFIG_DEFAULT_PRUNING_RATE = 0.00001f;

/**
 * @brief Configuration synaptic weight range - Default initial weights
 * 
 * Range for initial synaptic weights (-value, +value)
 * Used for network initialization
 */
const float CONFIG_DEFAULT_SYNAPTIC_WEIGHT_RANGE = 0.2f;

/**
 * @brief Configuration checkpoint interval - Automatic save frequency
 * 
 * Number of simulation steps between automatic checkpoints
 * Longer intervals save disk space, shorter intervals provide more safety
 */
const size_t CONFIG_DEFAULT_CHECKPOINT_INTERVAL = 10000;

/**
 * @brief Configuration maximum checkpoints - Storage limit
 * 
 * Maximum number of checkpoints to retain
 * Older checkpoints are automatically deleted
 */
const size_t CONFIG_MAX_CHECKPOINTS = 10000;

// ============================================================================
// SIMULATION METRICS
// ============================================================================

/**
 * @brief Average firing rate for silent network - Baseline activity
 * 
 * Expected firing rate when network is not driven by input
 * Used for detecting abnormal activity
 */
const float SIMULATION_SILENT_RATE = 0.1f;

/**
 * @brief Average firing rate for stimulated network - Active behavior
 * 
 * Expected firing rate when network receives input
 * Used for validating network responsiveness
 */
const float SIMULATION_STIMULATED_RATE = 5.0f;

/**
 * @brief Minimum stable weight change - Convergence criterion
 * 
 * Minimum absolute weight change considered meaningful
 * Used for determining learning completion
 */
const float SIMULATION_MIN_WEIGHT_CHANGE = 0.001f;

/**
 * @brief Maximum synaptic efficacy - Upper bound for modulation
 * 
 * Maximum value for synaptic efficacy calculations
 * Used for preventing overflow in multiplicative modulation
 */
const float SIMULATION_MAX_EFFICACY = 10.0f;

// ============================================================================
// NUMERICAL STABILITY CONSTANTS
// ============================================================================

/**
 * @brief Machine epsilon for float - Smallest representable difference
 * 
 * Used for numerical comparisons and avoiding division by zero
 * Important for robust numerical computations
 */
const float NUMERICAL_EPSILON = 1e-6f;

/**
 * @brief Maximum exponential argument - Prevent overflow
 * 
 * Upper bound for exponential calculations
 * Used in STDP and other exponential functions
 */
const float NUMERICAL_MAX_EXP = 20.0f;

/**
 * @brief Minimum exponential argument - Prevent underflow
 * 
 * Lower bound for exponential calculations
 * Used in STDP and other exponential functions
 */
const float NUMERICAL_MIN_EXP = -20.0f;

// ============================================================================
// PROGRESSION AND DEVELOPMENT
// ============================================================================

/**
 * @brief Development stage transition threshold - State change criterion
 * 
 * How much development progress is needed to transition stages
 * Used in developmental system progression
 */
const float DEVELOPMENT_TRANSITION_THRESHOLD = 1.0f;

/**
 * @brief Structural plasticity update frequency - How often to update
 * 
 * Simulation steps between structural plasticity updates
 * Less frequent updates improve performance
 */
const size_t STRUCTURAL_PLASTICITY_UPDATE_INTERVAL = 100;

/**
 * @brief Memory consolidation update frequency - How often to consolidate
 * 
 * Simulation steps between memory consolidation events
 * Used for long-term memory management
 */
const size_t MEMORY_CONSOLIDATION_INTERVAL = 1000;

/**
 * @brief Replay update frequency - How often to replay memories
 * 
 * Simulation steps between memory replay events
 * Used for strengthening and consolidating episodic memories
 */
const size_t MEMORY_REPLAY_INTERVAL = 100;

} // namespace nlm

// ============================================================================
// Convenience macros for constants
// ============================================================================

#define NLM_NEURON_THRESHOLD nlm::NEURON_FIRING_THRESHOLD
#define NLM_NEURON_RESTING nlm::NEURON_RESTING_POTENTIAL
#define NLM_STDP_LTP_WEIGHT nlm::STDP_LTP_WEIGHT
#define NLM_STDP_LTD_WEIGHT nlm::STDP_LTD_WEIGHT
#define NLM_STDP_TIME_CONSTANT nlm::STDP_TIME_CONSTANT
#define NLM_HEBBIAN_LEARNING_RATE nlm::HEBBIAN_LEARNING_RATE
#define NLM_SYNAPTIC_WEIGHT_MIN nlm::SYNAPTIC_WEIGHT_MIN
#define NLM_SYNAPTIC_WEIGHT_MAX nlm::SYNAPTIC_WEIGHT_MAX
#define NLM_SYNAPTIC_EFFICACY_SCALE nlm::SYNAPTIC_EFFICACY_SCALE
#define NLM_CONFIG_DEFAULT_NEURON_COUNT nlm::CONFIG_DEFAULT_NEURON_COUNT
#define NLM_CONFIG_DEFAULT_SIMULATION_TIMESTEP nlm::CONFIG_DEFAULT_SIMULATION_TIMESTEP
#define NLM_NUMERICAL_EPSILON nlm::NUMERICAL_EPSILON

#endif // NLM_CONSTANTS_HPP
