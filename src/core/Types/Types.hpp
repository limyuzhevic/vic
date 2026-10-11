#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety
// Provides type-safe identifiers to prevent programmer errors when mixing different entity types
// Each identifier has bounds checking, comparison operators, and helper methods

// Neuron identifier - uniquely identifies a neuron in the simulation
// NeuronId replaces raw uint64_t with a typed identifier for better code safety and debugging
struct NeuronId {
    uint64_t value;
    
    // Default constructor creates an invalid (zero) neuron ID
    constexpr NeuronId() : value(0) {}
    
    // Constructor from uint64_t - explicit to prevent accidental conversions
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Equality comparison - allows neuron IDs in if statements and maps
    constexpr bool operator==(const NeuronId& other) const = default;
    
    // Inequality comparison - essential for container operations
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    // Ordering comparison - enables sorting and use as map keys
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    // Helper method to get the underlying index for array access
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

// Synapse identifier - uniquely identifies a synaptic connection between neurons
// SynapseId ensures we don't accidentally confuse different synapses in the neural network
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier - uniquely identifies a brain region containing multiple neural populations
// Regions group neurons with similar functional properties (e.g., sensory cortex, motor cortex)
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier - uniquely identifies a neural population within a brain region
// Populations contain neurons with similar response properties and connectivity patterns
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter - tracks discrete simulation steps for event-driven computation
// Using uint64_t prevents overflow in long-running simulations
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point) - continuous time for differential equations
// Timestamp represents absolute time in the simulation, measured in seconds
using Timestamp = double;

// Timestep duration in seconds - discrete time interval for numerical integration
// Used for adaptive time-stepping in neural dynamics simulations
using TimestepDuration = double;

// Index types for arrays - specialized unsigned integer types for array indexing
// Using distinct types helps prevent bugs when mixing different array dimensions
using NeuronIndex = uint64_t;      // Index into neuron arrays (1D)
using SynapseIndex = uint64_t;      // Index into synapse arrays (1D)
using PopulationIndex = uint64_t;   // Index into population arrays (1D)
using RegionIndex = uint64_t;       // Index into region arrays (1D)

// Synaptic weight type - represents the strength of synaptic connections
// Range: typically [-1.0, 1.0] where negative = inhibitory, positive = excitatory
// Stored as float for efficiency in neural computations
using SynapticWeight = float;

// Membrane potential type - represents the electrical potential across neuron membrane
// Range: typically [-100mV, 50mV] depending on neuron model
// Stored as float for precise voltage calculations
using MembranePotential = float;

// Neuron firing rate type - represents the rate at which a neuron fires spikes
// Measured in spikes per second (Hz)
// Stored as float for precise rate calculations
using FiringRate = float;

// Connection delay in simulation steps - represents transmission delay in synaptic connections
// Measured in discrete simulation steps, not physical time
// Allows modeling of conduction velocity and synaptic transmission delays
using Delay = uint32_t;

// Neuron type enumeration - defines functional categories of neurons
// Different neuron types have distinct physiological properties and connectivity patterns
enum class NeuronType : uint8_t {
    Excitatory,     // Neurons that increase postsynaptic firing probability (positive weight)
    Inhibitory,     // Neurons that decrease postsynaptic firing probability (negative weight)
    Modulatory,     // Neurons that affect other neurons' plasticity or excitability
    Sensory,        // Neurons that receive external sensory input
    Motor,          // Neurons that control motor output
    Internal        // Neurons that process internal state (homeostatic, regulatory)
};

// Synapse type enumeration - defines structural and functional synapse categories
// Different synapse types have distinct transmission properties and plasticity rules
enum class SynapseType : uint8_t {
    Excitatory,     // Fast, ionotropic synapses that depolarize postsynaptic neuron
    Inhibitory,     // Fast, ionotropic synapses that hyperpolarize postsynaptic neuron
    Modulatory,     // Slow, metabotropic synapses that affect plasticity or gene expression
    Electrical,     // Direct electrical coupling via gap junctions
    GapJunction     // Synonym for electrical synapses (historical note)
};

// Synaptic plasticity state flags - bitfield tracking which plasticity mechanisms are active
// Used to control which plasticity rules apply to a given synapse
struct PlasticityFlags {
    bool hebbian : 1;        // Enable Hebbian learning (co-activation dependent)
    bool stdp : 1;            // Enable Spike-Timing-Dependent Plasticity
    bool reward_modulated : 1; // Enable reward-modulated plasticity (e.g., dopamine)
    bool structural : 1;      // Enable structural plasticity (synaptic formation/removal)
    bool eligible : 1;        // Flag indicating synapse is eligible for plasticity
    
    // Default constructor creates a plasticity state with all mechanisms disabled
    // New synapses typically start as ineligible for plasticity
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental stage enumeration - tracks the age-based phase of neural development
// Different stages have distinct plasticity rates, connectivity patterns, and gene expression
enum class DevelopmentalStage : uint8_t {
    Initial,           // Embryonic/early developmental stage - high plasticity
    CriticalPeriod,    // Sensitive period for sensory/motor learning - maximal plasticity
    Maturation,        // Post-critical period refinement - guided plasticity
    Adult,             // Mature state - limited, fine-tuned plasticity
    Aging             // Age-related decline in plasticity and function
};

// Spike event structure - records neural spike timing for event-driven computation
// Used by SpikeSystem to propagate spikes and trigger plasticity events
struct SpikeEvent {
    NeuronId source_neuron;    // ID of the neuron that generated the spike
    Timestamp timestamp;       // Absolute time when spike occurred (seconds)
    SimulationStep step;       // Discrete simulation step when spike occurred
    
    // Default constructor creates an invalid (empty) spike event
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    // Parameterized constructor for creating spike events from simulation state
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event structure - represents a spike that will be delivered after a delay
// Used to model synaptic transmission delays and conduction velocity
struct DelayedSpikeEvent {
    NeuronId source_neuron;           // Origin neuron of the spike
    NeuronId destination_neuron;      // Target neuron receiving the spike
    SynapseId synapse_id;              // Identifier of the connecting synapse
    SynapticWeight weight;            // Synaptic weight at time of spike (frozen for transmission)
    SynapseType synapse_type;         // Type of synapse determining transmission properties
    Timestamp timestamp;              // When the spike originated
    Timestamp delivery_time;          // When the spike reaches destination (timestamp + delay)
    SimulationStep step;              // Current simulation step when spike occurred
    SimulationStep delivery_step;     // Step when spike should be delivered to postsynaptic neuron
    bool is_excitatory;               // Cached boolean for fast path in spike delivery
    
    // Default constructor creates an invalid (empty) delayed spike event
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    // Parameterized constructor for creating delayed spike events
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Action potential state enumeration - represents the current electrophysiological state of a neuron
// Different states determine how the neuron responds to inputs and generates outputs
enum class FiringState : uint8_t {
    Resting,     // Neuron at resting membrane potential, not firing
    Active,      // Neuron in spike generation mode, currently firing or about to fire
    Refractory,  // Neuron recovering from recent spike, unable to fire again
    Inhibited    // Neuron actively suppressed by inhibitory inputs
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm
