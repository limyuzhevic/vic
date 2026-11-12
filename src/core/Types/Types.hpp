#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

/**
 * @brief Strongly-typed identifiers for safety in the NLM system
 * 
 * This header provides type-safe identifiers that prevent accidental mixing
 * of different numeric types in the neural simulation system. Each identifier
 * has its own semantic meaning and helps catch programming errors at compile-time.
 */

/**
 * @brief Neuron identifier
 * 
 * Unique identifier for neurons in the brain. Used to reference individual
 * neurons across all operations including synaptic connections, memory storage,
 * and simulation state management.
 */
struct NeuronId {
    uint64_t value;
    
    /**
     * @brief Default constructor
     * Creates an invalid neuron ID with value 0
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the neuron ID
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other NeuronId to compare with
     * @return true if both NeuronIds have the same value
     */
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other NeuronId to compare with
     * @return true if NeuronIds have different values
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
     * @brief Less-than comparison operator for sorting
     * @param other Other NeuronId to compare with
     * @return true if this NeuronId is less than the other
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this NeuronId after increment
     */
    constexpr NeuronId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Copy of NeuronId before increment
     */
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    /**
     * @brief Get the index value as a size_t
     * @return The numeric index of this neuron
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Synapse identifier
 * 
 * Unique identifier for synapses in the neural network. Each synapse has a
 * specific source and destination neuron, and this identifier is used to
 * reference synapses for plasticity, transmission, and state management.
 */
struct SynapseId {
    uint64_t value;
    
    /**
     * @brief Default constructor
     * Creates an invalid synapse ID with value 0
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the synapse ID
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other SynapseId to compare with
     * @return true if both SynapseIds have the same value
     */
    constexpr bool operator==(const SynapseId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other SynapseId to compare with
     * @return true if SynapseIds have different values
     */
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    /**
     * @brief Less-than comparison operator for sorting
     * @param other Other SynapseId to compare with
     * @return true if this SynapseId is less than the other
     */
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this SynapseId after increment
     */
    constexpr SynapseId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Copy of SynapseId before increment
     */
    constexpr SynapseId operator++(int) { SynapseId old = *this; ++value; return old; }
    
    /**
     * @brief Get the index value as a size_t
     * @return The numeric index of this synapse
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Region identifier
 * 
 * Unique identifier for neural regions in the brain. Regions contain groups
 * of neurons with similar properties or functions, and this identifier is
 * used for organizing the brain's architecture and accessing specific regions.
 */
struct RegionId {
    uint64_t value;
    
    /**
     * @brief Default constructor
     * Creates an invalid region ID with value 0
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the region ID
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other RegionId to compare with
     * @return true if both RegionIds have the same value
     */
    constexpr bool operator==(const RegionId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other RegionId to compare with
     * @return true if RegionIds have different values
     */
    constexpr bool operator!=(const RegionId& other) const = default;
    
    /**
     * @brief Less-than comparison operator for sorting
     * @param other Other RegionId to compare with
     * @return true if this RegionId is less than the other
     */
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this RegionId after increment
     */
    constexpr RegionId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Copy of RegionId before increment
     */
    constexpr RegionId operator++(int) { RegionId old = *this; ++value; return old; }
    
    /**
     * @brief Get the index value as a size_t
     * @return The numeric index of this region
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Population identifier
 * 
 * Unique identifier for neuron populations within regions. Populations group
 * neurons by type and function (e.g., sensory, motor, inhibitory) and this
 * identifier is used for efficient access to neuron groups.
 */
struct PopulationId {
    uint64_t value;
    
    /**
     * @brief Default constructor
     * Creates an invalid population ID with value 0
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the population ID
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other PopulationId to compare with
     * @return true if both PopulationIds have the same value
     */
    constexpr bool operator==(const PopulationId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other PopulationId to compare with
     * @return true if PopulationIds have different values
     */
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    /**
     * @brief Less-than comparison operator for sorting
     * @param other Other PopulationId to compare with
     * @return true if this PopulationId is less than the other
     */
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this PopulationId after increment
     */
    constexpr PopulationId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Copy of PopulationId before increment
     */
    constexpr PopulationId operator++(int) { PopulationId old = *this; ++value; return old; }
    
    /**
     * @brief Get the index value as a size_t
     * @return The numeric index of this population
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Simulation step counter
 * 
 * Integer type representing simulation steps. Used for controlling the
 * temporal progression of the neural simulation and timing-related operations.
 */
using SimulationStep = uint64_t;

/**
 * @brief Simulation time in seconds (floating point)
 * 
 * Double precision floating-point type representing absolute time in seconds.
 * Used for recording spike times, synaptic delays, and precise timing of events.
 */
using Timestamp = double;

/**
 * @brief Timestep duration in seconds
 * 
 * Double precision floating-point type representing the duration of individual
 * simulation steps. Used for integration calculations and time-dependent operations.
 */
using TimestepDuration = double;

/**
 * @brief Index types for arrays
 * 
 * Convenience typedefs for standard indexing types used throughout the
 * neural simulation system for array bounds and container operations.
 */
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

/**
 * @brief Synaptic weight type
 * 
 * Single-precision floating-point type representing connection strengths
 * between neurons. Used for both excitatory and inhibitory synaptic weights.
 */
using SynapticWeight = float;

/**
 * @brief Membrane potential type
 * 
 * Single-precision floating-point type representing the electrical potential
 * across neuron membranes. Used for representing resting potential, threshold,
 * reset potential, and current membrane state.
 */
using MembranePotential = float;

/**
 * @brief Neuron firing rate type
 * 
 * Single-precision floating-point type representing neuron firing rates in Hz.
 * Used for rate-based computations and statistical analysis.
 */
using FiringRate = float;

/**
 * @brief Connection delay in simulation steps
 * 
 * Integer type representing synaptic transmission delays in terms of
 * discrete simulation steps. Used for temporal dynamics in spike propagation.
 */
using Delay = uint32_t;

/**
 * @brief Typed enums for neuron types
 * 
 * Enumeration of different neuron types found in the neural system.
 * Each type has specific functional properties and determines behavior.
 */
enum class NeuronType : uint8_t {
    /** @brief Excitatory neurons that increase membrane potential */
    Excitatory,
    /** @brief Inhibitory neurons that decrease membrane potential */
    Inhibitory,
    /** @brief Modulatory neurons that affect plasticity and global state */
    Modulatory,
    /** @brief Sensory neurons that receive external input */
    Sensory,
    /** @brief Motor neurons that produce output actions */
    Motor,
    /** @brief Internal neurons involved in internal processing */
    Internal
};

/**
 * @brief Typed enums for synapse types
 * 
 * Enumeration of different synapse types with distinct transmission properties.
 * Each type has different effects on postsynaptic neurons.
 */
enum class SynapseType : uint8_t {
    /** @brief Chemical excitatory synapses */
    Excitatory,
    /** @brief Chemical inhibitory synapses */
    Inhibitory,
    /** @brief Neuromodulatory synapses */
    Modulatory,
    /** @brief Electrical synapses (gap junctions) */
    Electrical,
    /** @brief Specialized electrical connections */
    GapJunction
};

/**
 * @brief Synaptic plasticity state flags
 * 
 * Bit-field struct tracking which plasticity mechanisms are enabled for
 * a synapse. Used for selective plasticity control and mechanism-specific updates.
 */
struct PlasticityFlags {
    bool hebbian : 1;           /**< @brief Hebbian (co-activation) plasticity enabled */
    bool stdp : 1;              /**< @brief Spike-Timing-Dependent Plasticity enabled */
    bool reward_modulated : 1;  /**< @brief Reward-modulated plasticity enabled */
    bool structural : 1;        /**< @brief Structural plasticity enabled */
    bool eligible : 1;          /**< @brief Synapse eligible for plasticity updates */
    
    /**
     * @brief Default constructor
     * Initializes all plasticity flags to false
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * @brief Developmental state
 * 
 * Enumeration representing different developmental stages of the neural system.
 * Each stage has characteristic plasticity rates and behavioral properties.
 */
enum class DevelopmentalStage : uint8_t {
    /** @brief Initial developmental state with high plasticity */
    Initial,
    /** @brief Critical period with enhanced learning capabilities */
    CriticalPeriod,
    /** @brief Maturation phase with moderate plasticity */
    Maturation,
    /** @brief Adult stage with stable properties */
    Adult,
    /** @brief Aging phase with declining plasticity */
    Aging
};

/**
 * @brief Spike event for event-driven computation
 * 
 * Records spike occurrence with precise timing information. Used for
 * spike-timing-dependent plasticity and temporal pattern analysis.
 */
struct SpikeEvent {
    NeuronId source_neuron;    /**< @brief ID of the neuron that generated the spike */
    Timestamp timestamp;       /**< @brief Exact time when the spike occurred */
    SimulationStep step;       /**< @brief Simulation step when spike occurred */
    
    /**
     * @brief Default constructor
     * Creates an invalid spike event
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Parameterized constructor
     * @param nid Neuron ID that fired
     * @param ts Time when the spike occurred
     * @param s Simulation step when spike occurred
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @brief Delayed spike event for synaptic transmission with delay
 * 
 * Represents a spike that will be delivered to its postsynaptic target
 * after a specified delay. Contains all information needed for synaptic transmission.
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;       /**< @brief Neuron that generated the spike */
    NeuronId destination_neuron;  /**< @brief Target neuron that will receive the spike */
    SynapseId synapse_id;         /**< @brief Synapse through which spike will transmit */
    SynapticWeight weight;       /**< @brief Synaptic weight at time of spike (frozen) */
    SynapseType synapse_type;     /**< @brief Type of synapse for transmission */
    Timestamp timestamp;          /**< @brief When spike occurs */
    Timestamp delivery_time;    /**< @brief When spike reaches destination (timestamp + delay) */
    SimulationStep step;         /**< @brief Current simulation step */
    SimulationStep delivery_step; /**< @brief Step when spike should be delivered */
    bool is_excitatory;          /**< @brief Cached for quick lookup whether this is excitatory */
    
    /**
     * @brief Default constructor
     * Creates an invalid delayed spike event
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Parameterized constructor
     * @param src Source neuron ID
     * @param dst Destination neuron ID
     * @param syn Synapse ID
     * @param w Synaptic weight
     * @param type Synapse type
     * @param ts Spike timestamp
     * @param delivery Spike delivery time
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

/**
 * @brief Action potential state
 * 
 * Enumeration representing different phases of a neuron's firing cycle.
 * Used for tracking neuron state during simulation and debugging.
 */
enum class FiringState : uint8_t {
    /** @brief Resting state with no input or activity */
    Resting,
    /** @brief Active state with high membrane potential */
    Active,
    /** @brief Refractory state after spike where neuron cannot fire */
    Refractory,
    /** @brief Inhibited state with suppressed activity */
    Inhibited
};

/**
 * @brief Generic handle types for efficiency
 * 
 * Integer types used as handles for neurons and synapses. These provide
 * lightweight references without requiring full object access.
 */
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

/**
 * @brief Invalid ID sentinel values
 * 
 * Special values representing invalid or uninitialized identifiers.
 * Used to initialize handles and detect invalid states.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm

namespace std {
/**
 * @brief Hash specialization for NeuronId
 * 
 * Enables NeuronId to be used as keys in unordered containers.
 */
template<> struct hash<nlm::NeuronId> {
    size_t operator()(const nlm::NeuronId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};

/**
 * @brief Hash specialization for SynapseId
 * 
 * Enables SynapseId to be used as keys in unordered containers.
 */
template<> struct hash<nlm::SynapseId> {
    size_t operator()(const nlm::SynapseId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};

/**
 * @brief Hash specialization for RegionId
 * 
 * Enables RegionId to be used as keys in unordered containers.
 */
template<> struct hash<nlm::RegionId> {
    size_t operator()(const nlm::RegionId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};

/**
 * @brief Hash specialization for PopulationId
 * 
 * Enables PopulationId to be used as keys in unordered containers.
 */
template<> struct hash<nlm::PopulationId> {
    size_t operator()(const nlm::PopulationId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
} // namespace std

(End of file - total 494 lines)
