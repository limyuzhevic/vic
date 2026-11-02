#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

// =============================================================================
// FUNDAMENTAL TYPES AND IDENTIFIERS
// =============================================================================

/**
 * @brief Unique identifier for neurons in the neural network.
 * 
 * NeuronId provides strong typing for neuron references throughout the system.
 * Each neuron in the brain is assigned a unique NeuronId that persists across
 * simulation steps and checkpoint saves/loads. This prevents ambiguity in neuron
 * references and enables efficient O(1) lookup operations.
 */
struct NeuronId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates an invalid neuron identifier.
     * 
     * The default-constructed NeuronId has value 0, which is used as a sentinel
     * value for invalid or uninitialized neuron references.
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Explicit constructor from uint64_t.
     * 
     * @param v The unsigned integer value to use as the neuron identifier.
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Comparison operators for sorting and hashing
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    // Pre-increment and post-increment operators for iteration
    constexpr NeuronId& operator++() { ++value; return *this; }
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    /**
     * @brief Get the underlying numeric value of the identifier.
     * 
     * @return The numeric index value.
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Unique identifier for synapses connecting neurons.
 * 
 * SynapseId tracks individual synaptic connections between neurons. Each synapse
 * has a unique identifier that persists across simulation steps and allows
 * efficient access to synaptic properties like weight, delay, and plasticity flags.
 */
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    constexpr SynapseId& operator++() { ++value; return *this; }
    constexpr SynapseId operator++(int) { SynapseId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Unique identifier for brain regions.
 * 
 * RegionId distinguishes between different anatomical or functional regions
 * within the neural network. Regions group neurons and populations together,
 * enabling efficient spatial processing and inter-regional connectivity.
 */
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    constexpr RegionId& operator++() { ++value; return *this; }
    constexpr RegionId operator++(int) { RegionId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Unique identifier for neuron populations within regions.
 * 
 * PopulationId groups neurons with similar properties or functions within a region.
 * Populations enable organized neuron management and can share common characteristics
 * like type, plasticity rules, and computational roles.
 */
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    constexpr PopulationId& operator++() { ++value; return *this; }
    constexpr PopulationId operator++(int) { PopulationId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
};

// =============================================================================
// SIMULATION TIME AND TEMPO
// =============================================================================

/**
 * @brief Simulation step counter.
 * 
 * SimulationStep represents discrete time steps in the simulation. Each step
 * corresponds to a fixed timestep duration and advances the neural dynamics,
 * plasticity, memory systems, and environmental interactions.
 */
using SimulationStep = uint64_t;

/**
 * @brief Simulation time in seconds (floating point).
 * 
 * Timestamp represents continuous time in seconds with millisecond precision.
 * Used for spike timing, event scheduling, and temporal coordination between
 * different neural subsystems.
 */
using Timestamp = double;

/**
 * @brief Timestep duration in seconds.
 * 
 * TimestepDuration defines the fixed time interval between simulation steps.
 * Standard values are typically 0.001 seconds (1ms) for real-time brain simulation.
 */
using TimestepDuration = double;

// =============================================================================
// ARRAY INDEX TYPES
// =============================================================================

/**
 * @brief Index type for neuron arrays.
 * 
 * NeuronIndex is used for array indexing operations and iteration over neurons.
 * Provides a clear semantic meaning for array operations in neural regions.
 */
using NeuronIndex = uint64_t;

/**
 * @brief Index type for synapse arrays.
 * 
 * SynapseIndex is used for array indexing operations and iteration over synapses.
 * Essential for efficient synaptic connectivity management.
 */
using SynapseIndex = uint64_t;

/**
 * @brief Index type for population arrays.
 * 
 * PopulationIndex is used for array indexing operations and iteration over populations.
 * Enables efficient population-based processing and management.
 */
using PopulationIndex = uint64_t;

/**
 * @brief Index type for region arrays.
 * 
 * RegionIndex is used for array indexing operations and iteration over regions.
 * Facilitates efficient regional processing and inter-regional connectivity.
 */
using RegionIndex = uint64_t;

// =============================================================================
// SYNAPTIC AND NEURAL TYPES
// =============================================================================

/**
 * @brief Synaptic weight type.
 * 
 * SynapticWeight represents the strength of connections between neurons.
 * Positive values indicate excitatory connections, negative values indicate
 * inhibitory connections. Range typically [-1.0, 1.0] for stable dynamics.
 */
using SynapticWeight = float;

/**
 * @brief Membrane potential type.
 * 
 * MembranePotential represents the electrical potential across a neuron's
 * membrane, typically ranging from -70mV (resting) to +40mV (threshold).
 * Follows the Leaky Integrate-and-Fire neuron model dynamics.
 */
using MembranePotential = float;

/**
 * @brief Neuron firing rate type.
 * 
 * FiringRate represents the average number of spikes per second (Hz) over
 * a simulation period. Used for activity quantification and rate coding.
 */
using FiringRate = float;

/**
 * @brief Connection delay in simulation steps.
 * 
 * Delay represents the number of simulation steps required for a spike to
 * propagate from the presynaptic neuron to the postsynaptic neuron.
 * Maps to biological synaptic transmission delays.
 */
using Delay = uint32_t;

// =============================================================================
// ENUMERATIONS
// =============================================================================

/**
 * @brief Enumeration of neuron types in the neural network.
 * 
 * NeuronType categorizes neurons based on their functional role and
 * intrinsic properties. Each type has specific dynamics, connectivity patterns,
 * and plasticity rules.
 */
enum class NeuronType : uint8_t {
    Excitatory,    ///< Standard excitatory neurons that increase postsynaptic activity
    Inhibitory,    ///< Neurons that decrease postsynaptic activity via hyperpolarization
    Modulatory,    ///< Neurons that influence neuromodulatory systems and plasticity
    Sensory,       ///< Neurons that process and encode external/environmental input
    Motor,         ///< Neurons that trigger motor commands and actions
    Internal       ///< Internal processing neurons with complex computational roles
};

/**
 * @brief Enumeration of synapse types.
 * 
 * SynapseType categorizes synaptic connections based on their biophysical
 * and functional properties. Different synapse types implement different
 * transmission mechanisms and plasticity rules.
 */
enum class SynapseType : uint8_t {
    Excitatory,    ///< Standard glutamatergic synapses that depolarize postsynaptic neurons
    Inhibitory,    ///< GABAergic synapses that hyperpolarize postsynaptic neurons
    Modulatory,    ///< Synapses that influence neuromodulation and plasticity processes
    Electrical,    ///< Electrical synapses with direct ionic coupling
    GapJunction    ///< Alternative term for electrical synapses
};

/**
 * @brief Synaptic plasticity state flags.
 * 
 * PlasticityFlags indicates which plasticity mechanisms are active for a synapse.
 * Multiple plasticity mechanisms can operate simultaneously to produce complex
 * learning behaviors and adaptive responses.
 */
struct PlasticityFlags {
    bool hebbian : 1;        ///< Enables Hebbian learning (co-activity dependent)
    bool stdp : 1;           ///< Enables Spike-Timing-Dependent Plasticity
    bool reward_modulated : 1; ///< Enables reward-modulated learning
    bool structural : 1;     ///< Enables structural plasticity (connection formation/removal)
    bool eligible : 1;       ///< Indicates synapse is eligible for plasticity (e.g., STDP window)
    
    /**
     * @brief Default constructor initializes all plasticity flags to false.
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * @brief Developmental stage of the neural system.
 * 
 * DevelopmentalStage represents the current phase of brain development and
 * maturation. Each stage has characteristic plasticity rates, connectivity patterns,
 * and computational capabilities.
 */
enum class DevelopmentalStage : uint8_t {
    Initial,           ///< Early developmental stage with high plasticity
    CriticalPeriod,    ///< Sensitive period for specific learning (e.g., language)
    Maturation,        ///< Refinement and optimization phase
    Adult,             ///< Mature, stable computational capabilities
    Aging             ///< Age-related changes and potential decline
};

/**
 * @brief State of a neuron's firing process.
 * 
 * FiringState tracks the current state of a neuron's spike generation process.
 * Neurons transition through these states during their integration and spiking cycle.
 */
enum class FiringState : uint8_t {
    Resting,    ///< Neuron is at rest, not receiving sufficient input to reach threshold
    Active,     ///< Neuron is currently firing action potentials
    Refractory, ///< Neuron is in refractory period, unable to spike immediately
    Inhibited   ///< Neuron is under strong inhibitory influence
};

// =============================================================================
// EVENT AND HANDLE TYPES
// =============================================================================

/**
 * @brief Spike event for event-driven neural computation.
 * 
 * SpikeEvent captures the essential information about a neural spike for
 * event-driven simulation. These events are processed asynchronously to
 * enable efficient parallel computation and real-time behavior.
 */
struct SpikeEvent {
    NeuronId source_neuron;  ///< The neuron that generated the spike
    Timestamp timestamp;     ///< When the spike occurred (in seconds)
    SimulationStep step;     ///< Which simulation step generated the spike
    
    /**
     * @brief Default constructor creates an invalid spike event.
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Constructs a spike event with specified parameters.
     * 
     * @param nid Neuron identifier that spiked
     * @param ts Time when the spike occurred
     * @param s Simulation step when the spike occurred
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @brief Delayed spike event for synaptic transmission with delay.
 * 
 * DelayedSpikeEvent represents a spike that will be delivered to its target
 * neuron after a specified delay. Contains all information needed for synaptic
 * transmission including weight and type information.
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;      ///< Origin neuron of the spike
    NeuronId destination_neuron; ///< Target neuron to receive the spike
    SynapseId synapse_id;         ///< Identifier of the synaptic connection
    SynapticWeight weight;       ///< Synaptic weight at time of spike (frozen)
    SynapseType synapse_type;     ///< Type of synaptic connection
    Timestamp timestamp;         ///< When the spike occurs at source
    Timestamp delivery_time;    ///< When spike reaches destination (timestamp + delay)
    SimulationStep step;         ///< Current simulation step
    SimulationStep delivery_step; ///< Step when spike should be delivered
    bool is_excitatory;          ///< Cached for quick lookup (optimization)
    
    /**
     * @brief Default constructor creates an invalid delayed spike event.
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Constructs a delayed spike event with all parameters.
     * 
     * @param src Source neuron identifier
     * @param dst Destination neuron identifier
     * @param syn Synapse identifier
     * @param w Synaptic weight at spike time
     * @param type Type of synapse
     * @param ts Spike timestamp at source
     * @param delivery Spike delivery timestamp at destination
     * @param s Current simulation step
     * @param deliveryS Step when spike should be delivered
     */
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// =============================================================================
// INVALID SENTINELS
// =============================================================================

/**
 * @brief Sentinel value for invalid neuron identifiers.
 * 
 * INVALID_NEURON_ID represents an invalid or uninitialized neuron reference.
 * Should never be assigned to a valid neuron and used to indicate error states.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);

/**
 * @brief Sentinel value for invalid synapse identifiers.
 * 
 * INVALID_SYNAPSE_ID represents an invalid or uninitialized synapse reference.
 * Used for indicating error conditions and uninitialized states.
 */
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);

/**
 * @brief Sentinel value for invalid region identifiers.
 * 
 * INVALID_REGION_ID represents an invalid or uninitialized region reference.
 * Used for indicating error conditions and uninitialized states.
 */
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);

/**
 * @brief Sentinel value for invalid population identifiers.
 * 
 * INVALID_POPULATION_ID represents an invalid or uninitialized population reference.
 * Used for indicating error conditions and uninitialized states.
 */
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// =============================================================================
// HASH SPECIALIZATION
// =============================================================================

/**
 * @brief Hash specialization for NeuronId to enable use in hash-based containers.
 * 
 * This specialization allows NeuronId to be used as keys in unordered containers
 * like std::unordered_map and std::unordered_set, which is essential for efficient
 * O(1) lookup operations in the neural simulation.
 */
namespace std {
template<> struct hash<nlm::NeuronId> {
    size_t operator()(const nlm::NeuronId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::SynapseId> {
    size_t operator()(const nlm::SynapseId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::RegionId> {
    size_t operator()(const nlm::RegionId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::PopulationId> {
    size_t operator()(const nlm::PopulationId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
} // namespace std

} // namespace nlm
