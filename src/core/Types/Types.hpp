#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

// NLM (熙然) - Neural Learning Machine
// Core Type System for Neural Simulation
//
// This file defines the type system for NLM, providing strongly-typed
// identifiers for safety and clarity throughout the neural simulation.
// 
// Copyright (c) 2026 NLM Development Team
// MIT License

/**
 * @file Types.hpp
 * @brief Strongly-typed identifier system for NLM
 * @author NLM Development Team
 * @date 2026
 * @version 0.1.0
 *
 * This file defines custom ID types for safety and clarity in neural simulation.
 * All neuron, synapse, region, and population identifiers are encapsulated
 * in dedicated types to prevent accidental mixing of different entities.
 * 
 * Features:
 * - Type safety through distinct ID types
 * - Immutable value storage with access methods
 * - Comparison operators for sorting and lookup
 * - Hashing support for STL containers
 * - Invalid ID sentinel values
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Learning Machine namespace
 * 
 * Top-level namespace for all NLM components.
 */

/**
 * @brief Neuron identifier with type safety
 * 
 * This struct provides a strongly-typed identifier for neurons, preventing
 * accidental mixing of neuron IDs with other entity IDs. NeuronId supports
 * comparison, arithmetic, and hashing operations.
 * 
 * @note Used throughout the brain, synapse, and dynamics layers to identify
 * individual neurons within neural regions and populations.
 */
struct NeuronId {
    /**
     * @brief Internal value storage
     * 
     * The actual identifier value as a 64-bit unsigned integer.
     */
    uint64_t value;
    
    /**
     * @brief Default constructor
     * 
     * Creates an invalid NeuronId (value = 0).
     */
    constexpr NeuronId() : value(DQE_0} {}
    
    /**
     * @brief Value constructor
     * 
     * @param v Initial value for the neuron identifier
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * 
     * @param other Other NeuronId to compare with
     * @return true if values are equal
     */
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * 
     * @param other Other NeuronId to compare with
     * @return true if values are different
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
    /Print docstring
     * @brief Less-than comparison for ordering
     * 
     * @param other Other NeuronId to compare with
     * @return true if this value is less than other's value
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * 
     * @return Reference to this NeuronId after increment
     */
    constexpr NeuronId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * 
     * @return Previous NeuronId value
     */
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    /**
     * @brief Get the underlying index value
     * 
     * @return The 64-bit unsigned integer value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Synapse identifier with type safety
 * 
 * This struct provides a strongly-typed identifier for synapses, preventing
 * accidental mixing with neuron or other entity identifiers.
 * 
 * @note Used throughout the plasticity and dynamics layers to identify
 * individual synaptic connections between neurons.
 */
struct SynapseId {
    /**
     * @brief Internal value storage
     */
    uint64_t value;
    
    /**
     * @brief Default constructor
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the synapse identifier
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other SynapseId to compare with
     * @return true if values are equal
     */
    constexpr bool operator==(const SynapseId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other SynapseId to compare with
     * @return true if values are different
     */
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordering
     * @param other Other SynapseId to compare with
     * @return true if this value is less than other's value
     */
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this SynapseId after increment
     */
    constexpr SynapseId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Previous SynapseId value
     */
    constexpr SynapseId operator++(int) { SynapseId old = *this; ++value; return old; }
    
    /**
     * @brief Get the underlying index value
     * @return The 64-bit unsigned integer value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Brain region identifier with type safety
 * 
 * This struct identifies a logical group of neural populations within
 * the brain. Regions organize neurons hierarchically: Brain -> Region -> Population -> Neuron.
 * 
 * @note Used in Brain, NeuralRegion, and NeuralPopulation classes to organize
 * neural tissue into functional or anatomical groups.
 */
struct RegionId {
    /**
     * @brief Internal value storage
     */
    uint64_t value;
    
    /**
     * @brief Default constructor
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the region identifier
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other RegionId to compare with
     * @return true if values are equal
     */
    constexpr bool operator==(const RegionId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other RegionId to compare with
     * @return true if values are different
     */
    constexpr bool operator!=(const RegionId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordering
     * @param other Other RegionId to compare with
     * @return true if this value is less than other's value
     */
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this RegionId after increment
     */
    constexpr RegionId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Previous RegionId value
     */
    constexpr RegionId operator++(int) { RegionId old = *this; ++value; return old; }
    
    /**
     * @brief Get the underlying index value
     * @return The 64-bit unsigned integer value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Neural population identifier with type safety
 * 
 * This struct identifies a group of neurons with similar properties or functions
 * within a brain region. Populations are homogeneous groups of neurons.
 * 
 * @note Used in NeuralRegion and Brain classes to organize neurons into
 * functionally or anatomically meaningful groups.
 */
struct PopulationId {
    /**
     * @brief Internal value storage
     */
    uint64_t value;
    
    /**
     * @brief Default constructor
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Value constructor
     * @param v Initial value for the population identifier
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * @param other Other PopulationId to compare with
     * @return true if values are equal
     */
    constexpr bool operator==(const PopulationId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * @param other Other PopulationId to compare with
     * @return true if values are different
     */
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordering
     * @param other Other PopulationId to compare with
     * @return true if this value is less than other's value
     */
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Pre-increment operator
     * @return Reference to this PopulationId after increment
     */
    constexpr PopulationId& operator++() { ++value; return *this; }
    
    /**
     * @brief Post-increment operator
     * @return Previous PopulationId value
     */
    constexpr PopulationId operator++(int) { PopulationId old = *this; ++value; return old; }
    
    /**
     * @brief Get the underlying index value
     * @return The 64-bit unsigned integer value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Simulation step counter type
 * 
 * Represents discrete simulation steps in the neural network. Used for
 * time progression, spike scheduling, and event timing.
 * 
 * @note This is a 64-bit unsigned integer to support long simulations
 * without overflow. Typical step counts are in the thousands to millions.
 */
using SimulationStep = uint64_t;

/**
 * @brief Simulation time in seconds (floating point)
 * 
 * Represents continuous simulation time in seconds. Used for:
 * - Spike event timestamps
 * - Synaptic delays
 * - Neuromodulation timing
 * - External stimulus timing
 * 
 * @note Double precision provides sufficient accuracy for typical
 * neural simulation time scales (milliseconds to days).
 */
using Timestamp = double;

/**
 * @brief Timestep duration in seconds
 * 
 * Represents the duration of a single simulation step. This is the
 * integration time step for the neural dynamics simulation.
 * 
 * @note Typically very small (0.001 for 1ms steps). Must be consistent
 * with the brain's simulation timestep configuration.
 */
using TimestepDuration = double;

/**
 * @brief Neuron index type for array operations
 * 
 * Unsigned 64-bit integer for indexing into neuron arrays within
 * populations or regions. Allows efficient vectorized operations.
 */
using NeuronIndex = uint64_t;

/**
 * @brief Synapse index type for array operations
 * 
 * Unsigned 64-bit integer for indexing into synapse arrays within
 * populations or regions. Allows efficient vectorized operations.
 */
using SynapseIndex = uint64_t;

/**
 * @brief Population index type for array operations
 * 
 * Unsigned 64-bit integer for indexing into population arrays within
 * brain regions. Allows efficient vectorized operations.
 */
using PopulationIndex = uint64_t;

/**
 * @brief Region index type for array operations
 * 
 * Unsigned 64-bit integer for indexing into region arrays within
 * the brain. Allows efficient vectorized operations.
 */
using RegionIndex = uint64_t;

/**
 * @brief Synaptic weight type
 * 
 * Floating-point type for synaptic strengths. Range is typically 0.0 to 1.0,
 * but can be negative for inhibitory synapses or modified by plasticity rules.
 * 
 * @note Single precision (float) provides good performance while maintaining
 * sufficient precision for neural simulations.
 */
using SynapticWeight = float;

/**
 * @brief Membrane potential type
 * 
 * Floating-point type for neuronal membrane potentials. Range is typically
 * -100.0mV to +50mV, depending on the neuron model (LIF, I&F, etc.).
 * 
 * @note Single precision (float) provides good performance while maintaining
 * sufficient precision for neuronal dynamics.
 */
using MembranePotential = float;

/**
 * @brief Neuron firing rate type
 * 
 * Floating-point type for neuronal firing rates, typically in Hz (spikes/second).
 * Can represent instantaneous firing rates or time-averaged rates.
 * 
 * @note Single precision (float) provides good performance for rate calculations.
 */
using FiringRate = float;

/**
 * @brief Connection delay in simulation steps
 * 
 * Unsigned 32-bit integer representing synaptic transmission delays in
 * simulation steps. Allows for realistic spike propagation timing.
 * 
 * @note Typical values are small (1-100 steps) depending on distance and
 * conduction velocity. 32-bit provides sufficient range for all realistic
 * neural distances.
 */
using Delay = uint32_t;

/**
 * @brief Neuron type enumeration
 * 
 * Defines the functional types of neurons in the network. Each neuron has
 * a specific type that determines its behavior, properties, and connectivity patterns.
 */
enum class NeuronType : uint8_t {
    Excitatory,   ///< Neuron that typically releases excitatory neurotransmitters
    Inhibitory,   ///< Neuron that typically releases inhibitory neurotransmitters
    Modulatory,    ///< Neuron that releases neuromodulators affecting network state
    Sensory,      ///< Neuron that receives external sensory input
    Motor,        ///< Neuron that generates motor outputs
    Internal      ///< Neuron that represents internal states or abstract concepts
};

/**
 * @brief Synapse type enumeration
 * 
 * Defines the biophysical and functional types of synaptic connections.
 * Different synapse types have different transmission properties.
 */
enum class SynapseType : uint8_t {
    Excitatory,    ///< Chemical synapse that typically excites postsynaptic neuron
    Inhibitory,    ///< Chemical synapse that typically inhibits postsynaptic neuron
    Modulatory,     ///< Synapse that releases neuromodulators
    Electrical,     ///< Direct electrical coupling (gap junction)
    GapJunction     ///< Alternative name for electrical synapses
};

/**
 * @brief Synaptic plasticity state flags
 * 
 * Bit field indicating which plasticity mechanisms are active for a synapse.
 * Used to control Hebbian, STDP, reward-modulated, and structural plasticity.
 * 
 * @note Compiled as a struct with bitfields for memory efficiency.
 */
struct PlasticityFlags {
    /**
     * @brief Hebbian plasticity enabled flag
     * 
     * Enables classical Hebbian learning (fire together, wire together).
     */
    bool hebbian : 1;
    
    /**
     * @brief Spike-timing-dependent plasticity enabled flag
     * 
     * Enables STDP (spike-timing-dependent plasticity) based on relative timing
     * of pre- and postsynaptic spikes.
     */
    bool stdp : 1;
    
    /**
     * @brief Reward-modulated plasticity enabled flag
     * 
     * Enables plasticity rules that are gated by reward prediction errors
     * or other neuromodulatory signals.
     */
    bool reward_modulated : 1;
    
    /**
     * @brief Structural plasticity enabled flag
     */
    bool structural : 1;
    
    /**
     * @brief Synapse eligibility flag
     * 
     * Indicates whether the synapse is currently eligible for plasticity.
     * May be cleared by neuromodulators or after learning has occurred.
     */
    bool eligible : 1;
    
    /**
     * @brief Default constructor
     * 
     * Initializes all flags to false (plasticity disabled).
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * @brief Neural developmental stage enumeration
 * 
 * Represents the current developmental phase of the brain or individual neurons.
 * Different stages have different plasticity rules and connectivity patterns.
 */
enum class DevelopmentalStage : uint8_t {
    Initial,       ///< Initial state with maximum plasticity
    CriticalPeriod, ///< Period of high plasticity for specific learning
    Maturation,    ///< Gradual refinement of connections
    Adult,         ///< Mature, stable network
    Aging          ///< Declining plasticity and function
};

/**
 * @brief Spike event for event-driven computation
 * 
 * Represents a single neuron spike event with timing information. Used
 * for event-driven spike propagation and precise temporal computation.
 * 
 * @note This structure is used in the SpikeSystem for efficient spike
 * handling and synaptic transmission.
 */
struct SpikeEvent {
    /**
     * @brief Neuron that generated the spike
     */
    NeuronId source_neuron;
    
    /**
     * @brief Absolute timestamp when spike occurred (in seconds)
     */
    Timestamp timestamp;
    
    /**
     * @brief Simulation step when spike occurred
     */
    SimulationStep step;
    
    /**
     * @brief Default constructor
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Constructor with parameters
     * @param nid Neuron ID that spiked
     * @param ts Timestamp of spike
     * @param s Simulation step of spike
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @brief Delayed spike event for synaptic transmission
 * 
 * Represents a spike that will be delivered after a synaptic delay.
 * Includes frozen weight information for synaptic transmission calculations.
 * 
 * @note Used in synaptic transmission to handle timing of spike delivery
 * to postsynaptic neurons.
 */
struct DelayedSpikeEvent {
    /**
     * @brief Neuron that generated the spike
     */
    NeuronId source_neuron;
    
    /**
     * @brief Target neuron that will receive the spike
     */
    NeuronId destination_neuron;
    
    /**
     * @brief Synapse through which spike is transmitted
     */
    SynapseId synapse_id;
    
    /**
     * @brief Synaptic weight at time of spike (frozen)
     */
    SynapticWeight weight;
    
    /**
     * @brief Type of synapse transmitting the spike
     */
    SynapseType synapse_type;
    
    /**
     * @brief Absolute timestamp when spike occurs
     */
    Timestamp timestamp;
    
    /**
     * @brief Absolute timestamp when spike reaches destination
     */
    Timestamp delivery_time;
    
    /**
     * @brief Simulation step when spike occurs
     */
    SimulationStep step;
    
    /**
     * @brief Simulation step when spike should be delivered
     */
    SimulationStep delivery_step;
    
    /**
     * @brief Cached flag for fast excitatory/inhibitory lookup
     */
    bool is_excitatory;
    
    /**
     * @brief Default constructor
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Constructor with parameters
     * @param src Source neuron ID
     * @param dst Destination neuron ID
     * @param syn Synapse ID
     * @param w Synaptic weight
     * @param type Synapse type
     * @param ts Spike timestamp
     * @param delivery Spike delivery timestamp
     * @param s Simulation step
     * @param deliveryS Delivery simulation step
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
 * @brief Neuron firing state enumeration
 * 
 * Represents the current electrophysiological state of a neuron.
 * These states drive the neuron's behavior and dynamics.
 */
enum class FiringState : uint8_t {
    Resting,     ///< Neuron at rest, not firing
    Active,      ///< Neuron currently firing action potentials
    Refractory,   ///< Neuron recovering from spike, cannot fire
    Inhibited     ///< Neuron under strong inhibition, cannot fire
};

/**
 * @brief Generic handle types for efficiency
 * 
 * Integer handles for efficient storage and manipulation of neuron and synapse
 * references. Handles are more efficient than pointers for large arrays.
 */
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

/**
 * @brief Invalid ID sentinel value
 * 
 * Represents an invalid or uninitialized ID. Should be used when a valid ID
 * is required but none exists.
 * 
 * @note All invalid ID values are set to UINT64_MAX, which is unlikely to
 * be a valid ID in properly sized networks.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm

/**
 * @brief Standard hash specializations for ID types
 * 
 * Provides hash support for use in STL containers (unordered_map, etc.).
 * Required because our custom ID types use value-based equality.
 */
namespace std {
    /**
     * @brief Hash specialization for NeuronId
     * 
     * Hash function based on the 64-bit value field, enabling use in
     * hash-based containers.
     */
    template<> struct hash<nlm::NeuronId> {
        /**
         * @brief Compute hash of a NeuronId
         * 
         * @param id NeuronId to hash
         * @return Hash value
         */
        size_t operator()(const nlm::NeuronId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
    };

    /**
     * @brief Hash specialization for SynapseId
     */
    template<> struct hash<nlm::SynapseId> {
        size_t operator()(const nlm::SynapseId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
    };

    /**
     * @brief Hash specialization for RegionId
     */
    template<> struct hash<nlm::RegionId> {
        size_t operator()(const nlm::RegionId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
    };

    /**
     * @brief Hash specialization for PopulationId
     */
    template<> struct hash<nlm::PopulationId> {
        size_t operator()(const nlm::PopulationId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
    };
} // namespace std
