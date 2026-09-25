// Central constants namespace for all hardcoded values
// All constants are documented with biological/experimental basis where possible
// Provides normalized dimensionless values for cross-platform reproducibility
// Supports both single-precision (float) and double-precision calculations

// Biological inspiration: Constants derived from experimental measurements
// across various neural systems, scaled to dimensionless [0,1] range
// for computational efficiency and numerical stability

namespace nlm {

namespace Constants {

// Time constants (in milliseconds)
// These define the temporal dynamics of neural processes
// Values based on experimental measurements across species and brain regions

constexpr double DEFAULT_TIMESTEP_MS = 1.0;      // Standard simulation timestep
// Rationale: 1ms provides good temporal resolution while maintaining
// computational efficiency. Matches typical neural event frequencies.
// Mathematically: Δt should be << τ_m (membrane time constant ≈ 20ms)

constexpr double TIME_CONSTANT_MS = 20.0;        // Membrane time constant for LIF neurons
// Biological basis: Typical membrane time constant for cortical neurons
// Mathematical formulation: τ_m = R * C, where R=membrane resistance, C=capacitance
// Computationally: τ_m determines integration time for LIF dynamics

constexpr double REFRACTORY_PERIOD_MS = 5.0;     // Default refractory period (2-10ms range)
// Biological basis: Absolute refractory period prevents action potential
// overlap and maintains spike timing accuracy
// Performance: Hardcoded for efficiency, configurable per neuron type

constexpr double STDP_TIME_WINDOW_MS = 20.0;     // STDP exponential window parameter
// Biological inspiration: Typical spike-timing windows for STDP
// Computational: Defines integration window for spike history
// Complexity: O(1) lookup for spike history management

constexpr double NOVELTY_DECAY_MS = 100.0;       // Novelty decay timescale
// Biological basis: Decay of novelty signals in cortical areas
// Mathematical: Exponential decay: novelty(t) = novelty(0) * e^(-t/τ_n)
// Performance: Long timescale allows accumulation of persistent novelty signals

constexpr double CURIOSITY_DECAY_MS = 1000.0;   // Curiosity decay timescale
// Biological basis: Longer persistence of curiosity signals compared to novelty
// Computational rationale: Curiosity integrates over broader temporal windows
// Numerically stable: Larger timescale for long-term signal integration

// Weight and activity constants (dimensionless, normalized)
// All values normalized to [-1,1] or [0,1] range for stability
// Reduces numerical precision issues across platforms

constexpr double DEFAULT_LEARNING_RATE = 0.01;   // Standard Hebbian learning rate
// Biological inspiration: Synaptic modification threshold for Hebbian learning
// Mathematically: Δw ∝ x·y where x,y are pre/post synaptic activities
// Performance: Small learning rate ensures stability for online learning

constexpr double STDP_LTP_WEIGHT = 0.01;         // Potentiation amplitude (A+)
// Biological basis: Magnitude of long-term potentiation in STDP
// Computational: Scaling factor for weight changes in STDP
// Stability: Weight bounded by MIN/MAX_WEIGHT constraints

constexpr double STDP_LTD_WEIGHT = 0.012;        // Depression amplitude (A-)
// Biological inspiration: Slightly larger LTD than LTP (commonly observed)
// Computational trade-off: Ensures balanced synaptic dynamics
// Mathematically: LTD > LTP to prevent runaway excitation

constexpr double MIN_WEIGHT = -1.0;              // Minimum synaptic weight
// Theoretical lower bound: Maximum inhibitory strength
// Biological basis: Minimum synaptic efficacy observed in experiments
// Implementation: Hardcoded for performance, enables efficient clamping

constexpr double MAX_WEIGHT = 1.0;               // Maximum synaptic weight
// Theoretical upper bound: Maximum excitatory strength
// Biological basis: Saturation level for synaptic efficacy
// Computational: Symmetric range with MIN_WEIGHT for easy normalization

constexpr double MIN_LEARNING_RATE = 0.0;        // Non-negative learning
// Biological basis: Learning rates cannot be negative
// Implementation: Clamp learning rates to this range for stability
// Performance: Enables early termination checks for zero learning rates

constexpr double MAX_LEARNING_RATE = 1.0;        // Upper bound for learning rate
// Biological inspiration: Maximum reasonable learning speed
// Computational rationale: Prevents unstable learning dynamics
// Performance: Hardcoded bound for efficient clamping operations

constexpr double MIN_EFFICIENCY = 0.0;           // Minimum synaptic efficacy
// Biological lower bound: Minimum possible synaptic transmission
// Implementation: Enables efficient efficacy clamping
// Performance: Hardcoded for O(1) clamping operations

constexpr double MAX_EFFICIENCY = 2.0;           // Maximum synaptic efficacy
// Biological upper bound: Maximum synaptic amplification
// Computational: Allows for short-term plasticity effects
// Performance: Hardcoded for efficient clamping operations

constexpr double NOVELTY_THRESHOLD = 0.3;        // Novelty detection threshold
// Biological basis: Typical novelty detection sensitivity
// Computational: Determines when an experience is considered novel
// Mathematical: Threshold for correlation difference calculations
// Performance: Hardcoded for efficient threshold comparisons

// Neuron membrane potential constants (in millivolts)
// All values based on experimental measurements of real neurons
// Enable realistic spiking dynamics and threshold behavior

constexpr double RESTING_POTENTIAL_MV = -70.0;   // Typical resting potential
// Biological basis: Nernst potential for potassium ions
// Mathematical foundation: V_rest = E_K (equilibrium potential)
// Computational: Starting point for membrane potential integration

constexpr double THRESHOLD_MV = -55.0;           // Firing threshold
// Biological basis: Voltage-dependent sodium channel activation
// Mathematical: Point where dV/dt becomes positive and self-sustaining
// Computational: Critical value for spike initiation logic

constexpr double RESET_POTENTIAL_MV = -70.0;    // Post-spike reset value
// Biological basis: Fast sodium channel inactivation + potassium activation
// Computational: Returns neuron to resting state after spike
// Performance: Rapid reset enables high firing rates

constexpr double MIN_POTENTIAL_MV = -100.0;     // Safety lower bound
// Biological lower limit: Minimum possible membrane potential
// Computational safety: Prevents numerical instability
// Implementation: Ensures potential remains in valid range

constexpr double MAX_POTENTIAL_MV = 50.0;       // Safety upper bound
// Biological upper limit: Maximum depolarization before damage
// Computational safety: Prevents overflow and instability
// Implementation: Ensures numerical stability across all operations

// Synaptic conductance constants
// Basis: Typical conductance values for excitatory and inhibitory synapses
// Enable realistic postsynaptic potential (PSP) calculations

constexpr double DEFAULT_LEAK_CONDUCTANCE_NS = 10.0;  // Membrane leak conductance
// Biological basis: Potassium leak channels maintain resting potential
// Mathematical: Determines exponential decay time constant
// Performance: Hardcoded for efficient membrane potential updates

constexpr double EXCITATORY_INPUT_SCALE = 5.0;        // Vision input scaling factor
// Biological inspiration: Excitatory synaptic strength for sensory inputs
// Computational: Converts sensory input to membrane potential changes
// Performance: Hardcoded scaling for efficient input processing

constexpr double TOUCH_INPUT_SCALE = 8.0;             // Touch input scaling factor
// Biological basis: Stronger response to tactile stimuli
// Computational: Amplifies touch-related neural activity
// Performance: Hardcoded for efficient sensory transduction

constexpr double INTERNAL_INPUT_SCALE = 5.0;          // Internal signal scaling factor
// Biological inspiration: Internal brain signal strength
// Computational: Modulates internal state processing
// Performance: Hardcoded for efficient internal communication

constexpr double PROPRIOCEPTION_SCALE = 3.0;          // Proprioception scaling factor
// Biological basis: Proprioceptive signal characteristics
// Computational: Converts body position to neural activity
// Performance: Hardcoded for efficient sensorimotor integration

// Short-term plasticity (STP) parameters
// Biological basis: Short-term synaptic dynamics observed in experiments
// Computational implementation: Enables realistic short-term synaptic behavior

constexpr double STP_FACILITATION_TAU_MS = 100.0;    // Facilitation time constant
// Biological inspiration: Facilitation buildup time constant
// Mathematical: τ_f = time constant for facilitation recovery
// Computational: O(1) calculation for facilitation dynamics

constexpr double STP_DEPRESSION_TAU_MS = 200.0;      // Depression time constant
// Biological basis: Depression decay time constant
// Mathematical: τ_d = time constant for depression recovery
// Computational: O(1) calculation for depression dynamics

constexpr double STP_U_MAX = 1.0;                     // Max utilization factor
// Biological basis: Maximum synaptic resource availability
// Mathematical: u ∈ [0,u_max] where u is utilization
// Implementation: Normalized to [0,1] range for efficiency

// Reward modulation constants
// Biological inspiration: Neuromodulatory signal characteristics
// Computational implementation: Scales plasticity based on reward prediction

constexpr double DEFAULT_NOVELTY_DECAY = 0.99;        // Novelty decay factor per timestep
// Biological basis: Exponential decay of novelty signals
// Mathematical: novelty(t) = novelty(t-1) * decay_rate
// Performance: Precomputed for O(1) decay operations

constexpr double REWARD_PREDICTION_DECAY = 0.95;     // Expected reward decay factor
// Biological inspiration: Temporal difference learning discount factor
// Mathematical: δ = γ * V(s') - V(s) where γ = discount factor
// Computational: Enables reward prediction error computation

constexpr double REWARD_CLAMP_MIN = -1.0;             // Minimum dopamine level
// Biological lower bound: Minimum possible dopamine concentration
// Implementation: Ensures stable neuromodulation signals
// Performance: Hardcoded for efficient clamping operations

constexpr double REWARD_CLAMP_MAX = 1.0;              // Maximum dopamine level
// Biological upper bound: Maximum possible dopamine concentration
// Implementation: Ensures bounded neuromodulation signals
// Performance: Hardcoded for efficient clamping operations

constexpr double PLASTICITY_RANGE_MIN = 0.1;          // Minimum plasticity factor
// Biological basis: Minimum modulation effect on synaptic change
// Computational: Ensures non-zero minimum plasticity for exploration
// Performance: Precomputed for O(1) plasticity factor calculation

constexpr double PLASTICITY_RANGE_MAX = 2.0;           // Maximum plasticity factor
// Biological inspiration: Maximum amplification of plasticity
// Computational: Enables significant plasticity effects under strong signals
// Performance: Precomputed for O(1) plasticity factor calculation

// Developmental stage constants (in simulation timesteps)
// Biological basis: Developmental timeline across lifespan
// Computational implementation: Enables staged developmental changes

constexpr double INITIAL_STAGE_MS = 60.0;             // ~1 minute
// Biological inspiration: Critical period for synaptic formation
// Computational: Early developmental stage with high plasticity
// Implementation: Stage-based plasticity modulation

constexpr double CRITICAL_PERIOD_MS = 300.0;          // ~5 minutes
// Biological basis: Critical period for sensory processing development
// Computational: Enhanced learning during developmental window
// Implementation: Stage-based plasticity modulation with higher rates

constexpr double MATURATION_MS = 900.0;               // ~15 minutes
// Biological inspiration: Maturation period for cognitive development
// Computational: Transition from high to moderate plasticity
// Implementation: Stage-based plasticity modulation with decreasing rates

// Synaptogenesis and pruning constants
// Biological basis: Activity-dependent synaptic growth and elimination
// Computational implementation: Enables realistic developmental dynamics

constexpr double SYNAPTOGENESIS_RATE_BASE = 0.0001;   // Base rate for new synapse formation
// Biological inspiration: Synaptogenesis rate in early development
// Mathematical: Poisson process with rate λ for new synapse formation
// Performance: Precomputed for O(1) synaptogenesis probability

constexpr double PRUNING_RATE_BASE = 0.00001;         // Base rate for synapse elimination
// Biological basis: Synaptic pruning rate during development
// Mathematical: Exponential decay with rate μ for existing synapses
// Performance: Precomputed for O(1) pruning probability

// Spike history and correlation constants
// Biological basis: Temporal integration of spike patterns
// Computational implementation: Enables spike timing-dependent plasticity

constexpr size_t MAX_SPIKE_HISTORY = 100;             // Maximum number of recent spikes to track
// Biological inspiration: Neural integration time window for STDP
// Computational: Limits memory usage while maintaining sufficient history
// Performance: Fixed-size circular buffer for O(1) access

constexpr double CORRELATION_WINDOW_MS = 100.0;       // Time window for spike correlation
// Biological basis: Correlation window for spike timing-dependent plasticity
// Mathematical: Kernel function κ(t-t') for STDP learning rule
// Implementation: Precomputed for O(1) correlation calculations

constexpr double EPSILON_FOR_NORMALS = 1e-6;          // Small value for numerical stability
// Mathematical purpose: Prevents division by zero in normalization
// Computational implementation: Safe value for floating-point comparisons
// Performance: Precomputed for O(1) stability checks

// Motor and sensory mapping constants
// Biological basis: Neural population organization for sensorimotor processing
// Computational implementation: Efficient mapping between senses and actions

constexpr size_t VISION_WIDTH = 16;                   // Vision grid width
// Biological inspiration: Retina-inspired visual processing
// Computational: Defines 2D visual field resolution
// Performance: Hardcoded for efficient memory allocation

constexpr size_t VISION_HEIGHT = 16;                  // Vision grid height
// Biological basis: Visual field partitioning
// Computational: Completes vision grid definition
// Performance: Combined with VISION_WIDTH for total pixel count

constexpr size_t NUM_VISION_NEURONS = VISION_WIDTH * VISION_HEIGHT;  // 256
// Mathematical: Total visual processing units
// Performance: Precomputed for efficient array sizing
// Implementation: Enables fixed-size vision arrays for cache efficiency

constexpr size_t NUM_TOUCH_NEURONS = 8;                // Touch input neurons
// Biological inspiration: Tactile receptor distribution
// Computational: Number of distinct touch modalities
// Performance: Hardcoded for efficient sensory processing

constexpr size_t NUM_INTERNAL_NEURONS = 4;             // Internal signal neurons
// Biological basis: Internal brain state representation
// Computational: Number of internal processing units
// Performance: Hardcoded for efficient state management

constexpr size_t NUM_PROPRIOCEPTION_NEURONS = 6;      // Proprioception neurons
// Biological inspiration: Body position sensing
// Computational: Number of proprioceptive channels
// Performance: Hardcoded for efficient sensorimotor integration

constexpr size_t NUM_MOTOR_NEURONS = 6;                // One per action type
// Biological basis: Motor neuron pool organization
// Computational: Maps actions to neural populations
// Performance: Hardcoded for efficient action selection

constexpr size_t NUM_MOTOR_ACTIONS = 6;                // Motor command types
// Biological inspiration: Behavioral action categorization
// Computational: Total number of possible motor outputs
// Performance: Hardcoded for efficient action encoding

// Motor command enumeration mapping
// Biological basis: Action selection in behavioral systems
// Computational implementation: Maps neural activity to behaviors

constexpr size_t MOTOR_COMMAND_COUNT = 8;              // Total motor actions including lookahead
// Biological inspiration: Extended action space with anticipation
// Computational: Enables proactive motor planning
// Performance: Precomputed for efficient action indexing

// Random initialization ranges
// Biological basis: Natural variability in neural parameters
// Computational implementation: Enables diverse initial conditions
// Performance: Precomputed for efficient random initialization

constexpr double NEURON_WEIGHT_INITIAL_MIN = 0.1;      // Minimum initial neuron weight
// Biological inspiration: Natural synaptic weight distribution
// Mathematical: Uniform distribution for initial weights
// Performance: Precomputed for efficient random weight generation

constexpr double NEURON_WEIGHT_INITIAL_MAX = 0.4;     // Maximum initial neuron weight
// Biological basis: Range of initial synaptic strengths
// Computational: Enables appropriate starting conditions
// Performance: Precomputed for efficient weight initialization

constexpr double INHIBITORY_WEIGHT_INITIAL_MIN = -0.4; // Minimum inhibitory weight
// Biological inspiration: Range of inhibitory synaptic strengths
// Mathematical: Negative weights for inhibitory synapses
// Performance: Precomputed for efficient inhibitory weight initialization

constexpr double INHIBITORY_WEIGHT_INITIAL_MAX = -0.1; // Maximum inhibitory weight
// Biological basis: Range of inhibitory synaptic strengths
// Computational: Less negative than minimum for appropriate inhibition
// Performance: Precomputed for efficient inhibitory weight initialization

constexpr double OTHER_WEIGHT_INITIAL_MIN = -0.1;      // Minimum other synapse weight
// Biological inspiration: Non-excitatory, non-inhibitory synapses
// Computational: Modulatory and specialized synapse types
// Performance: Precomputed for efficient specialized synapse initialization

constexpr double OTHER_WEIGHT_INITIAL_MAX = 0.1;       // Maximum other synapse weight
// Biological basis: Range of modulatory synapse strengths
// Computational: Small positive and negative values for modulation
// Performance: Precomputed for efficient modulatory synapse initialization

// Adaptation and scaling constants
// Biological inspiration: Neural adaptation and gain control mechanisms
// Computational implementation: Enables realistic neural dynamics
// Performance: Precomputed for efficient adaptation calculations

constexpr double ADAPTATION_INCREMENT = 1.0;           // Spike-frequency adaptation increment
// Biological basis: Spike-frequency adaptation in real neurons
// Mathematical: A(t+1) = A(t) + increment * spike(t)
// Implementation: Tracks adaptation variable for each neuron

constexpr double ADAPTATION_DECAY = 0.95;             // Adaptation variable decay rate
// Biological inspiration: Adaptation variable recovery between spikes
// Mathematical: A(t) = A(t-1) * decay_rate
// Performance: Precomputed for efficient adaptation updates

constexpr double ADAPTATION_EFFECT = 0.01;            // Adaptation effect on membrane potential
// Biological basis: Adaptation current's effect on membrane potential
// Mathematical: V_rest' = V_rest + effect * adaptation
// Implementation: Enables spike-frequency adaptation

// Activity and firing constants
// Biological basis: Neural activity statistics and firing patterns
// Computational implementation: Enables realistic activity levels
// Performance: Precomputed for efficient activity calculations

constexpr double Firing_RATE_SCALE = 1.0;              // Firing rate scaling factor
// Biological inspiration: Normalization of firing rates
// Mathematical: scales firing rates to [0,1] range
// Implementation: Enables consistent activity measurements

constexpr double ACTIVITY_THRESHOLD = 0.5;            // Minimum activity for motor commands
// Biological basis: Activation threshold for motor commands
// Computational: Determines when motor action is triggered
// Performance: Hardcoded for efficient activity threshold checking

constexpr double EXPLORATION_CHANCE_MAX = 0.3;        // Maximum exploration probability
// Biological inspiration: Exploration-exploitation balance
// Mathematical: p_explore = curiosity_level * exploration_chance_max
// Implementation: Enables curiosity-driven exploration

// Logging and debugging constants
// Computational implementation: Memory and performance tuning
// Performance: Precomputed for efficient memory allocation

constexpr size_t LOG_MESSAGE_BUFFER_SIZE = 1024;      // Buffer size for log messages
// Performance: Preallocated buffer for efficient logging
// Memory: Fixed size prevents memory allocation overhead
// Implementation: Circular buffer for log rotation

constexpr int MAX_ERROR_CODE = 1000;                   // Maximum error code value
// Implementation: Range for error classification
// Performance: Fixed maximum for efficient error handling
// Implementation: Enum-based error code system

// Configuration defaults
// Computational implementation: Default system configuration
// Performance: Hardcoded defaults for initialization efficiency

constexpr const char* CONFIG_FILE_NAME = "config.json";  // Default configuration filename
// Implementation: File-based configuration system
// Performance: Standard format for cross-platform compatibility
// Documentation: Clear default for configuration file management

constexpr const char* LOG_FILE_NAME = "nlm.log";        // Default log filename
// Implementation: File-based logging system
// Performance: Standard format for log analysis
// Documentation: Clear default for logging file management

} // namespace Constants

} // namespace nlm
