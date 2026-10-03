#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// Index types for arrays
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Synaptic weight type
using SynapticWeight = float;

// Membrane potential type
using MembranePotential = float;

// Neuron firing rate type
using FiringRate = float;

// Connection delay in simulation steps
using Delay = uint32_t;

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Constants for agent brain configuration and behavior
namespace AgentConstants {
    // Sensory input scaling factors (vision, touch, internal, proprioception)
    constexpr float VISION_SCALE = 5.0f;           // Vision intensity to current scaling
    constexpr float TOUCH_SCALE = 8.0f;           // Collision signal scaling
    constexpr float INTERNAL_CENTER = 2.0f;       // Internal signal center
    constexpr float INTERNAL_SCALE = 5.0f;        // Internal signal scaling
    constexpr float PROPRIOCEPT_CENTER = 2.0f;    // Proprioception signal center
    constexpr float PROPRIOCEPT_SCALE = 3.0f;     // Proprioception signal scaling
    constexpr float NOVELTY_NORMALIZATION = 1.0f; // Division factor for novelty calculation
    
    // Novelty detection parameters
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f; // Decay factor for novelty persistence
    
    // Motor action selection thresholds
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;      // Minimum activity to consider an action
    constexpr float CURIOSITY_THRESHOLD = 0.5f;           // Curiosity level for exploration
    constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;    // Maximum exploration probability
    constexpr int MAX_RANDOM_ACTIONS = 7;                 // Number of random action choices (0-6)
    
    // Reward modulation parameters
    constexpr float REWARD_EXPECTATION_DECAY = 0.95f;    // EMA decay factor for expected reward
    constexpr float REWARD_LEARNING_RATE = 0.05f;        // EMA learning rate for expected reward
    
    // Development stage parameters
    constexpr float DEVELOPMENT_STAGE_1_THRESHOLD = 60.0;   // Age threshold for Initial stage (~1 minute)
    constexpr float DEVELOPMENT_STAGE_2_THRESHOLD = 300.0;  // Age threshold for CriticalPeriod (~5 minutes)
    constexpr float DEVELOPMENT_STAGE_3_THRESHOLD = 900.0;  // Age threshold for Maturation (~15 minutes)
    
    // Plasticity modifiers for different developmental stages
    constexpr float PLASTICITY_INITIAL = 1.0f;     // High plasticity in initial stage
    constexpr float PLASTICITY_CRITICAL = 0.8f;   // Moderate plasticity in critical period
    constexpr float PLASTICITY_MATURE = 0.5f;     // Lower plasticity in maturation
    constexpr float PLASTICITY_ADULT = 0.2f;      // Low plasticity in adult stage
    
    // Structural plasticity base rates
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;  // Base synaptogenesis rate
    constexpr float PRUNING_BASE_RATE = 0.00001f;       // Base pruning rate
    constexpr float PLASTICITY_FOR_PRUNING = 2.0f;       // Plasticity factor for pruning rate calculation
    
    // Vision processing parameters (from AgentBrain.cpp)
    constexpr size_t VISION_INPUT_SIZE = 256;        // Vision input dimensionality (16x16)
    constexpr size_t TOUCH_INPUT_SIZE = 8;          // Touch input dimensionality
    constexpr size_t INTERNAL_INPUT_SIZE = 4;       // Internal signals dimensionality
    constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6; // Proprioception dimensionality
    
    // Motor output configuration (from AgentBrain.cpp)
    constexpr size_t MOTOR_OUTPUT_SIZE = 6;          // Number of motor actions (forward, backward, left, right, interact, wait)
    
    // Development parameters
    constexpr float DEVELOPMENT_MAX_AGE = 1800.0;   // Maximum developmental age (30 minutes)
    constexpr double DEVELOPMENT_TIMESTEP = 0.1;   // Development update timestep
} // namespace AgentConstants

// Integration constants
namespace IntegrationConstants {
    // Replay and consolidation intervals (from Brain.cpp)
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;      // Replay every 100 steps
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Consolidate every 1000 steps
    
    // Neural dynamics constants (from Brain.cpp)
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001; // Default simulation timestep
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;     // STDP LTP weight
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;    // STDP LTD weight
    constexpr float DEFAULT_STDP_TAU = 20.0f;            // STDP time constant
    
    // Structural plasticity constants (from Brain.cpp)
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // Default synaptogenesis rate
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;        // Default pruning rate
} // namespace IntegrationConstants

// Random seed and simulation parameters
namespace RandomConstants {
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;    // Default random seed
    constexpr uint64_t MIN_RANDOM_SEED = 0;        // Minimum random seed
    constexpr uint64_t MAX_RANDOM_SEED = UINT64_MAX; // Maximum random seed
} // namespace RandomConstants

// Memory system parameters
namespace MemoryConstants {
    constexpr size_t MAX_SPIKE_HISTORY = 100;      // Maximum spike history length per neuron
    constexpr size_t MAX_SPIKE_EVENTS = 10000;     // Maximum pending spike events
} // namespace MemoryConstants

// World simulation parameters
namespace WorldConstants {
    constexpr float DEFAULT_WORLD_WIDTH = 16.0f;   // Default world width
    constexpr float DEFAULT_WORLD_HEIGHT = 16.0f; // Default world height
    constexpr float DEFAULT_VISION_WIDTH = 8.0f;  // Default vision width
    constexpr float DEFAULT_VISION_HEIGHT = 8.0f; // Default vision height
    constexpr float DEFAULT_ENERGY_DECAY = 0.95f; // Default energy decay rate
    constexpr float MAX_ENERGY = 100.0f;          // Maximum agent energy
} // namespace WorldConstants

// Visualization and logging parameters
namespace LoggingConstants {
    constexpr size_t MAX_LOG_ENTRIES = 10000;     // Maximum log entries before rotation
    constexpr size_t MAX_LOG_MESSAGE_LENGTH = 2048; // Maximum log message length
} // namespace LoggingConstants

} // namespace nlm
