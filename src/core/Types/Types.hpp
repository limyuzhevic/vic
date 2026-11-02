#pragma once

/**
 * @file Types.hpp
 * @brief Strongly-typed identifiers for type safety in the neural simulation
 * 
 * This file defines a comprehensive type system for the NLM brain simulation,
 * using strongly-typed identifiers to prevent common programming errors such as
 * mixing different neuron types or synapse types. All types are designed to be
 * constexpr where possible for compile-time safety and performance.
 * 
 * @author NLM Research Team
 * @date Phase 1 - Core Infrastructure
 * @version 1.0
 */

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

/**
 * @class NeuronId
 * @brief Strongly-typed identifier for neurons with safety guarantees
 * 
 * This identifier type ensures that neurons can only be referenced through
 * this specific type, preventing common bugs like accidentally using an
 * integer where a NeuronId is expected. The type system provides safety
 * while maintaining excellent performance with constexpr operations.
 * 
 * @details The NeuronId wraps a uint64_t value and provides:
 * - Operator== and operator!= for equality comparison
 * - Operator< for ordering/sorting
 * - ++ operators for iteration patterns
 * - index() accessor for the underlying value
 * - constexpr operations for compile-time safety
 * 
 * @note Used throughout the brain simulation to unambiguously reference
 *       specific neurons in populations, regions, or the entire brain.
 */
struct NeuronId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates an invalid (zero) neuron ID
     * 
     * This creates NeuronId with value = 0, which should be avoided in
     * production code as it may represent an uninitialized or invalid neuron.
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Construct neuron ID from raw value
     * 
     * @param v The raw neuron identifier value (0 to UINT64_MAX)
     * @warning Use with caution - ensure the value corresponds to a valid neuron
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator (automatically generated)
     * 
     * @param other The other NeuronId to compare with
     * @return true if both NeuronId objects have the same underlying value
     */
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /**
     * @brief Inequality comparison operator (automatically generated)
     * 
     * @param other The other NeuronId to compare with
     * @return true if NeuronId objects have different underlying values
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
     * @brief Less-than comparison for sorting and ordered containers
     * 
     * @param other The other NeuronId to compare with
     * @return true if this NeuronId's value is less than other's value
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Prefix increment operator for iteration
     * 
     * @return Reference to this NeuronId after incrementing its value
     * @note Useful for iterating through neuron sequences
     */
    constexpr NeuronId& operator++() { ++value; return *this; }
    
    /**
     * @brief Postfix increment operator for iteration
     * 
     * @return Copy of this NeuronId before incrementing its value
     * @note Returns the previous value, useful for range-based operations
     */
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    /**
     * @brief Get the raw underlying neuron index value
     * 
     * @return The uint64_t index value this NeuronId represents
     * @note Use sparingly - prefer using NeuronId methods when possible
     */
    constexpr uint64_t index() const { return value; }
};

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    constexpr NeuronId& operator++() { ++value; return *this; }
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
/**
 * @class SynapseId
 * @brief Strongly-typed identifier for synapses with safety guarantees
 * 
 * Similar to NeuronId but for synaptic connections. Ensures that synapses
 * can only be referenced through this specific type, preventing bugs where
 * integer values are mistakenly used for synapse references.
 * 
 * @details Synapse identifiers are used to uniquely identify individual
 * synaptic connections between neurons, allowing precise control over
 * synaptic plasticity and transmission.
 */
struct SynapseId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates an invalid (zero) synapse ID
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Construct synapse ID from raw value
     * 
     * @param v The raw synapse identifier value
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    constexpr SynapseId& operator++() { ++value; return *this; }
    constexpr SynapseId operator++(int) { SynapseId old = *this; ++value; return old; }
    
    /**
     * @brief Get the raw underlying synapse index value
     * 
     * @return The uint64_t index value this SynapseId represents
     */
    constexpr uint64_t index() const { return value; }
};

// Region identifier
/**
 * @class RegionId
 * @brief Strongly-typed identifier for brain regions with safety guarantees
 * 
 * Provides type safety for referencing different brain regions (e.g., cortex,
 * hippocampus, amygdala). Each region can contain multiple populations of
 * neurons with different characteristics.
 */
struct RegionId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates an invalid (zero) region ID
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Construct region ID from raw value
     * 
     * @param v The raw region identifier value
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    constexpr RegionId& operator++() { ++value; return *this; }
    constexpr RegionId operator++(int) { RegionId old = *this; ++value; return old; }
    
    /**
     * @brief Get the raw underlying region index value
     * 
     * @return The uint64_t index value this RegionId represents
     */
    constexpr uint64_t index() const { return value; }
};

// Population identifier
/**
 * @class PopulationId
 * @brief Strongly-typed identifier for neuron populations with safety guarantees
 * 
 * A population is a collection of neurons with similar properties and functions
 * (e.g., excitatory neurons, inhibitory neurons, sensory neurons). Each population
 * can be part of a region and contains many individual neurons.
 */
struct PopulationId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates an invalid (zero) population ID
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Construct population ID from raw value
     * 
     * @param v The raw population identifier value
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    constexpr PopulationId& operator++() { ++value; return *this; }
    constexpr PopulationId operator++(int) { PopulationId old = *this; ++value; return old; }
    
    /**
     * @brief Get the raw underlying population index value
     * 
     * @return The uint64_t index value this PopulationId represents
     */
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

// Simulation step counter
/**
 * @brief Simulation step counter type
 * 
 * Represents a discrete step in the neural simulation timeline.
 * Each step corresponds to the execution of one complete update cycle
 * for all neurons and synapses in the brain.
 * 
 * @details Simulation steps are monotonic and never decrease during
 * simulation execution. The step counter is used for:
 * - Timing and synchronization
 * - Tracking simulation progress
 * - Converting between step and time-based references
 * 
 * @note Steps are integer-based, making them suitable for array indexing
 *       and other discrete-time operations.
 */
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
/**
 * @brief Simulation time type representing continuous time
 * 
 * This type represents the actual elapsed time in the neural simulation,
 * measured in seconds. Unlike simulation steps, this allows for precise
 * time-based calculations and interpolation.
 * 
 * @details Unlike discrete simulation steps, timestamps are floating-point
 * values that can represent fractional seconds, enabling:
 * - Precise timing of events
 * - Continuous time calculations
 * - Real-time simulation modes
 * 
 * @note Time values start at 0.0 and increase monotonically as the
 *       simulation progresses.
 */
using Timestamp = double;

// Timestep duration in seconds
/**
 * @brief Timestep duration type for neural simulation updates
 * 
 * Represents the duration of each discrete simulation step, measured in
 * seconds. This controls the temporal resolution of the neural dynamics.
 * 
 * @details Common values:
 * - 0.001 seconds (1ms): Standard biological timescale
 * - 0.01 seconds (10ms): Faster simulation, coarser resolution
 * - 0.1 seconds (100ms): Very fast simulation
 * 
 * @note Smaller timesteps provide more accurate integration of neural
 *       dynamics but require more computational resources.
 */
using TimestepDuration = double;

// Index types for arrays
/**
 * @brief Type alias for neuron array index
 * 
 * This is the type used for indexing arrays of neurons. While functionally
 * identical to SimulationStep, this explicit type distinction improves
 * code readability and prevents accidental misuse.
 */
using NeuronIndex = uint64_t;

/**
 * @brief Type alias for synapse array index
 * 
 * This is the type used for indexing arrays of synapses. Synapses can be
 * numerous in large brains, so using a proper type helps with memory
 * management and clarity.
 */
using SynapseIndex = uint64_t;

/**
 * @brief Type alias for population array index
 * 
 * Used for indexing arrays of neuron populations within brain regions.
 * Provides type safety when working with population-level operations.
 */
using PopulationIndex = uint64_t;

/**
 * @brief Type alias for brain region array index
 * 
 * Used for indexing arrays of brain regions. Allows for hierarchical
 * organization of the brain into distinct functional areas.
 */
using RegionIndex = uint64_t;

// Synaptic weight type
/**
 * @brief Type for representing synaptic connection strengths
 * 
 * Synaptic weights determine the effectiveness of connections between
 * neurons. Positive weights represent excitatory connections, negative
 * weights represent inhibitory connections, and zero weights represent
 * absent connections.
 * 
 * @details Weight values typically range from -1.0 to +1.0 in normalized
 * form, but can vary depending on the specific plasticity model used.
 * Weight values are updated continuously through plasticity mechanisms.
 * 
 * @note Weight storage uses 32-bit floating point for efficiency while
 *       maintaining sufficient precision for neural dynamics.
 */
using SynapticWeight = float;

// Membrane potential type
/**
 * @brief Type for representing neuronal membrane potentials
 * 
 * Membrane potential (Vm) represents the electrical potential difference
 * across a neuron's membrane. In NLM, this is modeled using the Leaky
 * Integrate-and-Fire (LIF) model.
 * 
 * @details Typical ranges:
 * - Resting potential: ~ -70 mV
 * - Threshold potential: ~ -50 mV
 * - Reset potential: ~ -75 mV after spike
 * - Spike peak: Not modeled (abrupt transition)
 * 
 * @note Values are stored as floating point to allow for sub-millisecond
 *       precision in potential changes.
 */
using MembranePotential = float;

// Neuron firing rate type
/**
 * @brief Type for representing neuron firing rates
 * 
 * Firing rate represents the average number of action potentials (spikes)
 * emitted by a neuron per unit time (typically Hertz, or spikes per second).
 * 
 * @details This type is used for:
 * - Monitoring overall brain activity
 * - Statistical analysis of neural patterns
 * - Calculating network-level statistics
 * 
 * @note Instantaneous firing rates can be calculated from spike timestamps,
 *       while this type typically represents averaged or smoothed rates.
 */
using FiringRate = float;

// Connection delay in simulation steps
/**
 * @brief Type for representing synaptic transmission delays
 * 
 * Delays represent the time required for a spike to propagate from one
 * neuron to another neuron across a synapse. This is modeled in terms
 * of simulation steps rather than absolute time for efficiency.
 * 
 * @details Common values:
 * - 0-5 steps: Fast synaptic transmission
 * - 10-50 steps: Typical biological delays
 * - 100+ steps: Slow synaptic transmission
 * 
 * @note Using simulation steps rather than absolute time makes delays
 *       independent of the actual timestep size, improving performance.
 */
using Delay = uint32_t;

// Typed enums for neuron types
/**
 * @brief Enumeration of neuron types in the NLM brain
 * 
 * Each neuron type has distinct electrical properties, connectivity patterns,
 * and functional roles within the neural network.
 * 
 * @enumclass NeuronType
 * 
 * @var NeuronType::Excitatory Neurons that increase the probability of
 *       postsynaptic neuron firing (positive membrane potential change)
 * 
 * @var NeuronType::Inhibitory Neurons that decrease the probability of
 *       postsynaptic neuron firing (negative membrane potential change)
 * 
 * @var NeuronType::Modulatory Neurons that release neuromodulators affecting
 *       multiple downstream neurons or brain regions
 * 
 * @var NeuronType::Sensory Neurons that receive input from the environment
 *       or external sensors
 * 
 * @var NeuronType::Motor Neurons that produce motor commands to influence
 *       the environment or agent's body
 * 
 * @var NeuronType::Internal Neurons that process information within the
 *       brain without direct environmental input/output
 * 
 * @note The sum of all neuron types typically represents the complete
 *       neural population of the brain.
 */
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
/**
 * @brief Enumeration of synapse types in the NLM brain
 * 
 * Different synapse types have distinct transmission properties, plasticity
 * rules, and functional characteristics within the neural network.
 * 
 * @enumclass SynapseType
 * 
 * @var SynapseType::Excitatory Synapses that increase postsynaptic neuron
 *       excitability (positive membrane potential change)
 * 
 * @var SynapseType::Inhibitory Synapses that decrease postsynaptic neuron
 *       excitability (negative membrane potential change)
 * 
 * @var SynapseType::Modulatory Synapses that release neuromodulators
 *       affecting broader brain regions
 * 
 * @var SynapseType::Electrical Direct electrical coupling between neurons
 *       without synaptic delay (gap junctions)
 * 
 * @var SynapseType::GapJunction Similar to electrical synapses but with
 *       different biophysical properties
 * 
 * @note The synapse type determines both the sign of the transmitted signal
 *       and the applicable plasticity rules.
 */
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
/**
 * @struct PlasticityFlags
 * @brief Bit field structure for tracking synaptic plasticity capabilities
 * 
 * This structure uses bit packing for efficient storage of multiple boolean
 * flags that determine how a synapse can change its weight over time.
 * The flags represent different plasticity mechanisms that can be active
 * or inactive for individual synapses.
 * 
 * @details The bit field structure provides memory efficiency (typically
 * 8 bits per synapse) while maintaining fast flag access for plasticity
 * computations. This is crucial for performance in large-scale neural
 * simulations with millions of synapses.
 * 
 * @note The eligible flag typically indicates whether a synapse is currently
 *       eligible for potentiation or depression based on timing and
 *       neuromodulatory signals.
 */
struct PlasticityFlags {
    bool hebbian : 1;              ///< Enables Hebbian learning (correlational)
    bool stdp : 1;                 ///< Enables Spike-Timing-Dependent Plasticity
    bool reward_modulated : 1;     ///< Enables reward-modulated plasticity
    bool structural : 1;          ///< Enables structural plasticity (new connections)
    bool eligible : 1;            ///< Indicates if synapse is eligible for changes
    
    /**
     * @brief Default constructor initializes all flags to false
     * 
     * Creates a PlasticityFlags object with all plasticity mechanisms
     * disabled. This is the safe default state for synapses before any
     * plasticity training has occurred.
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
/**
 * @brief Enumeration of developmental stages in the neural system
 * 
 * These developmental stages model the temporal progression of brain
 * development, where different plasticity mechanisms and learning rules
 * are active during different phases. This allows for the implementation
 * of critical periods, maturation, and age-dependent learning patterns.
 * 
 * @enumclass DevelopmentalStage
 * 
 * @var DevelopmentalStage::Initial Represents the initial developmental state
 *       with maximum plasticity and rapid learning capabilities
 * 
 * @var DevelopmentalStage::CriticalPeriod A phase of heightened plasticity
 *       where certain sensory and cognitive abilities are especially
 *       responsive to environmental input
 * 
 * @var DevelopmentalStage::Maturation The phase where neural circuits mature
 *       and stabilize, with reduced plasticity but enhanced efficiency
 * 
 * @var DevelopmentalStage::Adult The fully mature developmental state with
 *       optimized neural connectivity and stable learning capabilities
 * 
 * @var DevelopmentalStage::Aging The aging phase where plasticity may
 *       decline and neural efficiency may change
 * 
 * @note The developmental stage typically progresses monotonically from
 *       Initial to Aging, and can affect many aspects of neural function
 *       including plasticity rates, excitability, and connectivity patterns.
 */
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
/**
 * @struct SpikeEvent
 * @brief Event structure representing a neuronal action potential
 * 
 * This structure represents a single spike event generated by a neuron,
 * containing all information needed to propagate the spike through the
 * neural network. The event-driven nature of NLM relies on these structures
 * for efficient simulation of spiking neural networks.
 * 
 * @details Spike events are generated when a neuron's membrane potential
 * crosses threshold and reset. Each event contains:
 * - Source neuron identifier for routing the spike
 * - Timestamp when the spike occurred (can be simulation time)
 * - Simulation step when spike was generated
 * - Propagation rules based on synapse types and delays
 * 
 * @note Spike events are processed in timestamp order to ensure correct
 *       temporal sequence of neural activity.
 */
struct SpikeEvent {
    NeuronId source_neuron;  ///< The neuron that generated the spike
    Timestamp timestamp;     ///< When the spike occurred (seconds)
    SimulationStep step;     ///< Simulation step when spike occurred
    
    /**
     * @brief Default constructor creates an invalid spike event
     * 
     * Creates a spike event with all fields set to their default values
     * (invalid/zero values). This represents an uninitialized or invalid
     * spike and should not be used in active simulation.
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Construct spike event with specified parameters
     * 
     * @param nid The source neuron identifier
     * @param ts The timestamp when the spike occurred
     * @param s The simulation step when the spike occurred
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
/**
 * @struct DelayedSpikeEvent
 * @brief Spike event structure with synaptic delay information
 * 
 * This structure represents a spike event that has been delayed by the
 * transmission time of a specific synapse. It contains all information
 * needed to deliver the spike to the postsynaptic neuron at the correct
 * time, including the synaptic weight and type information.
 * 
 * @details DelayedSpikeEvent extends SpikeEvent with synapse-specific
 * information:
 * - Destination neuron identifier for routing
 * - Synapse identifier for precise synaptic control
 * - Synaptic weight (frozen at time of spike) for transmission strength
 * - Synapse type for determining transmission properties
 * - Original timestamp for temporal accuracy
 * - Delivery time for delayed propagation
 * - Step tracking for simulation time
 * - Cached excitatory flag for fast lookup
 * 
 * @note The separation of SpikeEvent and DelayedSpikeEvent allows for
 *       efficient scheduling while maintaining precise temporal accuracy
 *       in spike propagation.
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;        ///< Source neuron that generated the spike
    NeuronId destination_neuron;   ///< Target neuron receiving the spike
    SynapseId synapse_id;          ///< Identifier of the synapse transmitting
    SynapticWeight weight;        ///< Synaptic weight at time of spike (frozen)
    SynapseType synapse_type;      ///< Type of synapse (determines transmission)
    Timestamp timestamp;          ///< When spike occurred (seconds)
    Timestamp delivery_time;      ///< When spike should reach destination
    SimulationStep step;          ///< Current simulation step
    SimulationStep delivery_step; ///< Simulation step when spike should be delivered
    bool is_excitatory;           ///< Cached for quick lookup (true for excitatory)
    
    /**
     * @brief Default constructor creates an invalid delayed spike event
     * 
     * Creates a delayed spike event with all fields set to their default
     * (invalid/zero) values. This represents an uninitialized or invalid
     * event and should not be used in active simulation.
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Construct delayed spike event with all necessary parameters
     * 
     * @param src Source neuron identifier
     * @param dst Destination neuron identifier
     * @param syn Synapse identifier
     * @param w Synaptic weight at time of spike
     * @param type Type of synapse
     * @param ts Original timestamp of spike
     * @param delivery When spike should be delivered to destination
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

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm

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
