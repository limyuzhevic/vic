#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

namespace nlm {
namespace constants {

// Neural membrane potentials
static constexpr float NEURON_RESTING_POTENTIAL = -70.0f;    // mV, typical resting potential
static constexpr float NEURON_THRESHOLD = -55.0f;            // mV, typical action potential threshold
static constexpr float NEURON_RESET_POTENTIAL = -70.0f;     // mV, after-spike reset potential
static constexpr float NEURON_CLAMP_MIN = -100.0f;           // mV, lower bound for membrane potential
static constexpr float NEURON_CLAMP_MAX = 50.0f;             // mV, upper bound for membrane potential

// STDP plasticity parameters
static constexpr float STDP_LTP_WEIGHT = 0.01f;              // Learning rate for LTP
static constexpr float STDP_LTD_WEIGHT = 0.012f;             // Learning rate for LTD
static constexpr float STDP_TIME_CONSTANT = 20.0f;          // Time constant for STDP decay (ms)
static constexpr float STDP_ELIGIBILITY_THRESHOLD = 0.001f; // Minimum eligibility trace magnitude

// Hebbian plasticity parameters
static constexpr float HEBBIAN_LEARNING_RATE = 0.01f;        // Hebbian learning rate
static constexpr float HEBBIAN_MAX_WEIGHT = 1.0f;            // Maximum synaptic weight
static constexpr float HEBBIAN_MIN_WEIGHT = -1.0f;           // Minimum synaptic weight

// Plasticity learning thresholds
static constexpr float PLASTICITY_LEARNING_THRESHOLD = 0.001f; // Minimum change to consider learning
static constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;      // Minimum weight delta to classify as significant

// Dopamine neuromodulation parameters
static constexpr float DOPAMINE_BASELINE = 0.1f;             // Baseline dopamine level
static constexpr float DOPAMINE_MIN = -1.0f;                 // Minimum dopamine level
static constexpr float DOPAMINE_MAX = 1.0f;                  // Maximum dopamine level

// Development/plasticity rates
static constexpr float SYNAPTOGENESIS_RATE = 0.001f;         // Rate of new synapse formation
static constexpr float PRUNING_RATE = 0.0001f;               // Rate of synapse pruning
static constexpr float SYNAPTIC_MIN_WEIGHT = 0.05f;          // Minimum weight for functional synapse
static constexpr float SYNAPTIC_ACTIVITY_THRESHOLD = 0.001f; // Minimum activity for synapse retention

// Memory and learning parameters
static constexpr float WORKING_MEMORY_DECAY_RATE = 0.01f;    // Memory decay rate
static constexpr float EPISODIC_MEMORY_MAX_EPISODES = 1000.0f; // Maximum stored episodes
static constexpr float CONCEPT_FORMATION_THRESHOLD = 0.75f;   // Concept formation confidence threshold

// Environmental and world parameters
static constexpr float ENVIRONMENT_WALL_MARGIN = 0.01f;      // Distance from wall boundaries
static constexpr float MOVEMENT_COST = 0.01f;                // Cost per movement action
static constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;       // Discount factor for future rewards

// Development stages
static constexpr float DEVELOPMENT_HIGH_PLasticity = 1.0f;   // High plasticity period
static constexpr float DEVELOPMENT_MODERATE_PLASTICITY = 0.8f; // Moderate plasticity period
static constexpr float DEVELOPMENT_LOW_PLASTICITY = 0.5f;    // Low plasticity period
static constexpr float DEVELOPMENT_ADULT_PLASTICITY = 0.2f;  // Adult stable period

// Adaptation and homeostasis
static constexpr float ADAPTATION_COEFFICIENT = 0.01f;       // Spike-frequency adaptation coefficient
static constexpr float ADAPTATION_DECAY_RATE = 0.95f;        // Adaptation variable decay
static constexpr float HOMEOSTASIS_COEFFICIENT = 0.98f;      // Homeostatic synaptic scaling

// Simulation parameters
static constexpr float SIMULATION_DEFAULT_TIMESTEP = 0.001f;  // Default simulation timestep (ms)
static constexpr float SIMULATION_REFractory_MIN = 2.0f;      // Minimum refractory period (ms)
static constexpr float SIMULATION_REFractory_MAX = 10.0f;     // Maximum refractory period (ms)

// Experimental thresholds
static constexpr float LEARNING_DETECTION_THRESHOLD = 0.001f; // Minimum change to detect learning
static constexpr float EXPLORATION_NOISE_LEVEL = 0.1f;       // Noise level for exploration

// Reward and value parameters
static constexpr float REWARD_SMALL_POSITIVE = 0.1f;         // Small positive reward
static constexpr float REWARD_SMALL_NEGATIVE = -0.01f;       // Small negative penalty
static constexpr float REWARD_MOVEMENT_COST = -0.05f;        // Movement cost
static constexpr float REWARD_WALL_HIT_COST = -0.1f;         // Cost for hitting walls

// Visualization parameters
static constexpr float VISUALIZATION_UPDATE_RATE = 30.0f;    // Frame rate for visualization (Hz)
static constexpr float NEURON_VISUALIZATION_SIZE = 5.0f;      // Size of neuron visualization
static constexpr float SYNAPSE_VISUALIZATION_WIDTH = 1.0f;    // Width of synapse visualization

// Checkpoint system
static constexpr uint64_t CHECKPOINT_MAGIC_NUMBER = 0x4B504D4E5843434EULL; // NLMCCP magic number

} // namespace constants
} // namespace nlm

#endif // NLM_CONSTANTS_H
