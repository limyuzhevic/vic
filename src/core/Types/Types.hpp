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

// Simulation constants
constexpr TimestepDuration SIMULATION_TIMESTEP = 0.001;          // 1 ms timestep
constexpr TimestepDuration PRECISION_THRESHOLD = 0.001f;          // Precision threshold for comparisons
constexpr Timestamp MAX_SIMULATION_TIME = 3600.0;                  // Maximum simulation time (1 hour)

// Neural constants
constexpr SynapticWeight DEFAULT_CURRENT_INJECTION = 50.0f;       // Default current for activation (60.0f in some places)
constexpr SynapticWeight STRONG_CURRENT_INJECTION = 60.0f;        // Strong current injection for spikes
constexpr SynapticWeight WEAK_CURRENT_INJECTION = 30.0f;          // Weak current injection for sub-threshold activation
constexpr SynapticWeight REWARD_WEIGHT = 0.5f;                     // Reward modulation weight
constexpr SynapticWeight NEUROMODULATION_GAIN = 1.0f;              // Neuromodulation gain factor

// Plasticity constants
constexpr SynapticWeight STDP_LTP_WEIGHT = 0.02f;                  // STDP long-term potentiation weight
constexpr SynapticWeight STDP_LTD_WEIGHT = 0.015f;                 // STDP long-term depression weight
constexpr SynapticWeight STDP_TIME_CONSTANT = 20.0f;               // STDP time constant (tau)
constexpr SynapticWeight HEBBIAN_LEARNING_RATE = 0.01f;             // Hebbian learning rate
constexpr SynapticWeight STRUCTURE_PRUNING_THRESHOLD = 0.05f;       // Structural pruning threshold
constexpr SynapticWeight ACTIVITY_THRESHOLD = 0.001f;              // Activity threshold for plasticity

// Development constants
constexpr float DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f;         // Synaptogenesis rate
constexpr float DEVELOPMENT_PRUNING_RATE = 0.0001f;               // Pruning rate
constexpr float DEVELOPMENT_CRITICAL_PERIOD = 0.01f;              // Critical period coefficient

// Environment constants
constexpr uint32_t DEFAULT_ENVIRONMENT_WIDTH = 10;                 // Default world width
constexpr uint32_t DEFAULT_ENVIRONMENT_HEIGHT = 10;                // Default world height

// Sensory constants
constexpr NeuronIndex VISION_INPUT_SIZE = 256;                     // Vision input neurons (16x16 grid)
constexpr NeuronIndex TOUCH_INPUT_SIZE = 8;                       // Touch input neurons
constexpr NeuronIndex INTERNAL_INPUT_SIZE = 4;                    // Internal state neurons
constexpr NeuronIndex PROPRIOCEPTION_INPUT_SIZE = 6;               // Proprioception neurons
constexpr NeuronIndex TOTAL_SENSORY_INPUT_SIZE = VISION_INPUT_SIZE + TOUCH_INPUT_SIZE + 
                                                 INTERNAL_INPUT_SIZE + PROPRIOCEPTION_INPUT_SIZE;

// Motor constants
constexpr NeuronIndex MOTOR_OUTPUT_SIZE = 6;                       // Motor output neurons (forward, backward, turn left, turn right, interact, wait)

// Scaling constants
constexpr float VISION_SCALE_FACTOR = 5.0f;                        // Vision signal scaling
constexpr float TOUCH_SCALE_FACTOR = 8.0f;                        // Touch signal scaling
constexpr float INTERNAL_SCALE_FACTOR = 5.0f;                     // Internal signal scaling
constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;                // Proprioception signal scaling

// Curiosity and novelty constants
constexpr float NOVELTY_DECAY_RATE = 0.99f;                        // Novelty decay rate
constexpr float CURIOSITY_WEIGHT_NOVELTY = 2.0f;                   // Curiosity weight for novelty
constexpr float CURIOSITY_WEIGHT_PREDICTION_ERROR = 0.5f;          // Curiosity weight for prediction error
constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;                        // Maximum curiosity level

// Reward and motivation constants
constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;                    // Reward discount factor
constexpr SynapticWeight DOSE_BASELINE = 0.1f;                      // Baseline dopamine level
constexpr float REWARD_THRESHOLD = 0.01f;                          // Reward threshold for learning
