#pragma once

/**
 * @file Types.hpp
 * @brief Core type definitions for the NLM neural network simulation framework.
 * 
 * This header provides strongly-typed identifiers and definitions for simulation data
 * to ensure type safety and prevent programming errors through type confusion.
 * All types are defined within the `nlm` namespace.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Config/Config.hpp
 * @see src/brain/Neuron.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see src/agent/AgentBrain.hpp
 * @see src/experiments/Experiment.hpp
 */

#include <cstdint>
#include <type_traits>

namespace nlm {

/**
 * Strongly-typed identifiers for safety and type enforcement.
 * 
 * These structs provide type-safe identifiers to prevent accidental mixing
 * of different entity types. Each ID type has implicit conversion to uint64_t
 * for indexing operations while maintaining type safety at the interface level.
 */

/**
     * Strongly-typed neuron identifier.
     * 
     * Provides type-safe neuron identification to prevent accidental mixing of
     * different ID types. Unique across all neurons in the simulation.
     * 
     * @note Can be converted to uint64_t via index() method for array access
     * @see SynapseId, RegionId, PopulationId
     */
struct NeuronId {
    uint64_t value;
    
    /**
     * Default constructor creates an invalid neuron ID (0).
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Construct with explicit value
     * @param v Unique neuron identifier value
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /**
     * @brief Check equality with another NeuronId
     * @param other NeuronId to compare against
     * @return true if values are equal, false otherwise
     */
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /**
     * @brief Check inequality with another NeuronId
     * @param other NeuronId to compare against
     * @return true if values are not equal, false otherwise
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
     * @brief Compare with another NeuronId (lexicographic ordering)
     * @param other NeuronId to compare against
     * @return true if this ID is less than the other ID
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Get the raw index value for array access
     * @return The uint64_t value of this neuron ID
     */
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

/**
     * Strongly-typed synapse identifier.
     * 
     * Provides type-safe synapse identification to prevent confusion between
     * neuron and synapse identifiers. Unique across all synapses in the simulation.
     * 
     * @see NeuronId
     */
struct SynapseId {
    uint64_t value;
    
    /**
     * Default constructor creates an invalid synapse ID (0).
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Construct with explicit value
     * @param v Unique synapse identifier value
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    /**
     * @brief Check equality with another SynapseId
     * @param other SynapseId to compare against
     * @return true if values are equal, false otherwise
     */
    constexpr bool operator==(const SynapseId& other) const = default;
    
    /**
     * @brief Check inequality with another SynapseId
     * @param other SynapseId to compare against
     * @return true if values are not equal, false otherwise
     */
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    /**
     * @brief Compare with another SynapseId (lexicographic ordering)
     * @param other SynapseId to compare against
     * @return true if this ID is less than the other ID
     */
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Get the raw index value for array access
     * @return The uint64_t value of this synapse ID
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * Strongly-typed region identifier.
 * 
 * Provides type-safe identification for brain regions to prevent confusion
 * between regions and other entity types. A region represents a functional
 * subdivision of the brain with similar neuron types and connectivity patterns.
 * 
 * @see NeuronId, PopulationId
 */
struct RegionId {
    uint64_t value;
    
    /**
     * Default constructor creates an invalid region ID (0).
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Construct with explicit value
     * @param v Unique region identifier value
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    /**
     * @brief Check equality with another RegionId
     * @param other RegionId to compare against
     * @return true if values are equal, false otherwise
     */
    constexpr bool operator==(const RegionId& other) const = default;
    
    /**
     * @brief Check inequality with another RegionId
     * @param other RegionId to compare against
     * @return true if values are not equal, false otherwise
     */
    constexpr bool operator!=(const RegionId& other) const = default;
    
    /**
     * @brief Compare with another RegionId (lexicographic ordering)
     * @param other RegionId to compare against
     * @return true if this ID is less than the other ID
     */
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Get the raw index value for array access
     * @return The uint64_t value of this region ID
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * Strongly-typed population identifier.
 * 
 * Provides type-safe identification for neuron populations (groups of neurons
 * with similar properties and functions). A population represents a functional
 * unit within a brain region.
 * 
 * @see RegionId
 */
struct PopulationId {
    uint64_t value;
    
    /**
     * Default constructor creates an invalid population ID (0).
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Construct with explicit value
     * @param v Unique population identifier value
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    /**
     * @brief Check equality with another PopulationId
     * @param other PopulationId to compare against
     * @return true if values are equal, false otherwise
     */
    constexpr bool operator==(const PopulationId& other) const = default;
    
    /**
     * @brief Check inequality with another PopulationId
     * @param other PopulationId to compare against
     * @return true if values are not equal, false otherwise
     */
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    /**
     * @brief Compare with another PopulationId (lexicographic ordering)
     * @param other PopulationId to compare against
     * @return true if this ID is less than the other ID
     */
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Get the raw index value for array access
     * @return The uint64_t value of this population ID
     */
    constexpr uint64_t index() const { return value; }
};

/**
     * Simulation step counter type.
     * 
     * Unsigned 64-bit integer representing discrete time steps in the simulation.
     * Increments by 1 for each simulation iteration. Used for time integration
     * in the simulation algorithm.
     */
using SimulationStep = uint64_t;

/**
 * Simulation time in seconds (floating point).
 * 
 * Double precision floating point value representing real-world time in seconds.
 * Used for continuous time simulation and spike timing calculations.
 */
using Timestamp = double;

/**
 * Timestep duration in seconds.
 * 
 * Double precision floating point value representing the duration of each
 * simulation timestep in seconds. Used for numerical integration of differential
 * equations in neuron dynamics.
 */
using TimestepDuration = double;

/**
 * Index types for array indexing operations.
 * 
 * These types provide semantic meaning to array indices in data structures
 * for better code readability and potential future type checking.
 */
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

/**
 * Synaptic weight type.
 * 
 * Single precision floating point representing the strength of synaptic connections.
 * Positive values indicate excitatory connections, negative values indicate inhibitory.
 * Range typically [-1.0, 1.0] for normalized weights, but can exceed this range.
 */
using SynapticWeight = float;

/**
 * Membrane potential type.
 * 
 * Single precision floating point representing the electrical potential across
 * the neuron's membrane in millivolts (mV). Typical values range from -70 mV (resting)
 * to -40 mV (threshold) to +50 mV (peak depolarization).
 */
using MembranePotential = float;

/**
 * Neuron firing rate type.
 * 
 * Single precision floating point representing the neuron's firing rate in Hertz (Hz).
 * Measured as spikes per second. Typically 0.0 to several hundred Hz.
 */
using FiringRate = float;

/**
 * Connection delay in simulation steps.
 * 
 * Unsigned 32-bit integer representing synaptic transmission delay as an integer
 * number of simulation timesteps. Converts to real time using TimestepDuration.
 */
using Delay = uint32_t;

/**
 * Neuron type enumeration.
 * 
 * Defines the functional categories of neurons in the neural network.
 * Each type has characteristic properties and typical behaviors.
 */
enum class NeuronType : uint8_t {
    /** Excitatory neuron that releases excitatory neurotransmitters */
    Excitatory,
    
    /** Inhibitory neuron that releases inhibitory neurotransmitters */
    Inhibitory,
    
    /** Modulatory neuron that affects network-wide neuromodulation */
    Modulatory,
    
    /** Sensory neuron that receives external input */
    Sensory,
    
    /** Motor neuron that produces motor commands */
    Motor,
    
    /** Internal neuron in a computational hub */
    Internal
};

/**
 * Synapse type enumeration.
 * 
 * Defines the biophysical and functional types of synaptic connections.
 */
enum class SynapseType : uint8_t {
    /** Chemical excitatory synapse with AMPA/NMDA receptors */
    Excitatory,
    
    /** Chemical inhibitory synapse with GABA receptors */
    Inhibitory,
    
    /** Modulatory synapse releasing neuromodulators */
    Modulatory,
    
    /** Electrical synapse with gap junctions */
    Electrical,
    
    /** Chemical synapse with neuromodulatory effects */
    GapJunction
};

/**
 * Synaptic plasticity state flags.
 * 
 * Bit field representing which plasticity mechanisms are active and available
 * for a particular synapse or neuron. Used to control plasticity computation.
 * 
 * @see PlasticityRule
 * @see STDP
 * @see HebbianRule
 */
struct PlasticityFlags {
    bool hebbian : 1;           /**< Flag indicating Hebbian plasticity is enabled */
    bool stdp : 1;              /**< Flag indicating STDP plasticity is enabled */
    bool reward_modulated : 1;  /**< Flag indicating reward-modulated plasticity is enabled */
    bool structural : 1;        /**< Flag indicating structural plasticity is enabled */
    bool eligible : 1;          /**< Flag indicating synapse is eligible for plasticity */
    
    /**
     * Default constructor initializes all flags to false.
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * Developmental stage enumeration.
 * 
 * Represents the developmental phase of the brain/network, influencing plasticity
 * rates, connection formation, and learning capabilities.
 */
enum class DevelopmentalStage : uint8_t {
    /** Initial state with no connections or capabilities */
    Initial,
    
    /** Period of heightened plasticity and connection formation */
    CriticalPeriod,
    
    /** Maturation phase with stable connections */
    Maturation,
    
    /** Adult stage with fully mature capabilities */
    Adult,
    
    /** Aging stage with declining performance */
    Aging
};

/**
 * Spike event for event-driven computation.
 * 
 * Records a neuronal spike occurrence with timing information for use in
 * event-driven simulation algorithms and STDP calculations.
 */
struct SpikeEvent {
    NeuronId source_neuron;     /**< ID of the neuron that generated the spike */
    Timestamp timestamp;        /**< Real-world time when the spike occurred */
    SimulationStep step;        /**< Discrete simulation step when the spike occurred */
    
    /**
     * Default constructor creates an invalid spike event.
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Construct spike event with specified parameters
     * @param nid Neuron ID that generated the spike
     * @param ts Time when the spike occurred
     * @param s Simulation step when the spike occurred
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * Delayed spike event for synaptic transmission with delay.
 * 
 * Extends SpikeEvent to include synaptic transmission information such as
 * destination neuron, weight, and timing of delayed spike delivery. Essential
 * for modeling axonal delays and synaptic transmission timing.
 * 
 * @see SpikeEvent
 * @see Synapse
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;     /**< Source neuron that generated the spike */
    NeuronId destination_neuron; /**< Target neuron that should receive the spike */
    SynapseId synapse_id;        /**< Identifier of the synaptic connection */
    SynapticWeight weight;      /**< Weight at time of spike (frozen value) */
    SynapseType synapse_type;    /**< Type of the synaptic connection */
    Timestamp timestamp;        /**< When spike occurs at source */
    Timestamp delivery_time;   /**< When spike reaches destination (timestamp + delay) */
    SimulationStep step;        /**< Current simulation step */
    SimulationStep delivery_step; /**< Step when spike should be delivered */
    bool is_excitatory;         /**< Cached for quick lookup (true if excitatory) */
    
    /**
     * Default constructor creates an invalid delayed spike event.
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Construct delayed spike event with specified parameters
     * @param src Source neuron ID
     * @param dst Destination neuron ID
     * @param syn Synapse ID
     * @param w Synaptic weight at time of spike
     * @param type Type of synaptic connection
     * @param ts Time when spike occurs at source
     * @param delivery Time when spike should be delivered
     * @param s Current simulation step
     * @param deliveryS Simulation step when spike should be delivered
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
 * Action potential state enumeration.
 * 
 * Represents the current electrophysiological state of a neuron.
 */
enum class FiringState : uint8_t {
    /** Resting state (below firing threshold) */
    Resting,
    
    /** Active state (capable of firing) */
    Active,
    
    /** Refractory state (cannot fire) */
    Refractory,
    
    /** Inhibited state (actively suppressed) */
    Inhibited
};

/**
 * Generic handle types for efficiency.
 * 
 * These types provide opaque handles to objects (neurons and synapses) that
 * can be stored and passed without exposing pointer indirection. Implemented
 * as 64-bit integers for efficient memory usage.
 */
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

/**
 * Invalid ID sentinel values.
 * 
 * Special values representing invalid or uninitialized IDs. Used as sentinel
 * values in algorithms that need to distinguish between valid and invalid IDs.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm
