#pragma once
/**
 * @file Types.hpp
 * @brief Core type definitions for NLM
 * 
 * This file contains strongly-typed identifiers, enumerations, and fundamental
 * data types used throughout the NLM neural simulation system. The use of
 * strongly-typed ID structures enhances type safety and prevents ID confusion
 * between different entities (neurons, synapses, regions, etc.).
 * 
 * @author NLM Development Team
 * @version 1.0
 * @date 2026
 * 
 * @note All ID types are lightweight wrappers around uint64_t with proper
 *       comparison operators and indexing methods for efficient use in
 *       standard containers and algorithms.
 */

#include <cstdint>
#include <type_traits>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Simulation Library namespace
 * 
 * The NLM namespace encompasses all types, classes, and functions for the
 * Neural Simulation Library. It provides a unified interface for:
 * - Neural system simulation and computation
 * - Memory management and persistence
 * - Prediction and cognitive modeling
 * - Neuromodulation and learning
 * - Development and adaptation
 * 
 * @see Brain, Config, NeuralRegion
 */

// Strongly-typed identifiers for safety

/**
 * @struct NeuronId
 * @brief Strongly-typed identifier for neurons
 * 
 * This structure provides a type-safe identifier for neurons in the simulation.
 * It prevents accidental confusion between neuron IDs and other ID types.
 * The ID is backed by an unsigned 64-bit integer for efficient storage and
 * comparison operations.
 * 
 * @note Used throughout the system as the primary neuron identifier in all
 *       neural data structures and algorithms.
 */
struct NeuronId {
    /** Underlying 64-bit unsigned integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid neuron ID
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Construct neuron ID from unsigned integer
     * 
     * @param v 64-bit unsigned integer value
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     * 
     * @param other NeuronId to compare with
     * @return true if both IDs have the same value
     */
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     * 
     * @param other NeuronId to compare with
     * @return true if IDs have different values
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
     * @brief Less-than comparison for use in ordered containers
     * 
     * @param other NeuronId to compare with
     * @return true if this ID is less than other
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Get the underlying index value
     * 
     * @return uint64_t The internal ID value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @struct SynapseId
 * @brief Strongly-typed identifier for synapses
 * 
 * Provides type-safe identification for synaptic connections between neurons.
 * This prevents confusion with other ID types and enables type-safe container
 * usage throughout the connectivity management system.
 */
struct SynapseId {
    /** Underlying 64-bit unsigned integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid synapse ID
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Construct synapse ID from unsigned integer
     * 
     * @param v 64-bit unsigned integer value
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     */
    constexpr bool operator==(const SynapseId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     */
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordered containers
     */
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Get the underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @struct RegionId
 * @brief Strongly-typed identifier for neural regions
 * 
 * Identifies anatomically or functionally distinct regions of the brain model.
 * Regions can contain multiple neurons and populations, and are used for
 * modular organization of the neural system.
 */
struct RegionId {
    /** Underlying 64-bit unsigned integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid region ID
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Construct region ID from unsigned integer
     * 
     * @param v 64-bit unsigned integer value
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     */
    constexpr bool operator==(const RegionId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     */
    constexpr bool operator!=(const RegionId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordered containers
     */
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Get the underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @struct PopulationId
 * @brief Strongly-typed identifier for neuron populations
 * 
 * Identifies groups of neurons that share similar properties, connectivity,
 * or functional roles within a region. Populations enable efficient management
 * of neuron groups.
 */
struct PopulationId {
    /** Underlying 64-bit unsigned integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid population ID
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Construct population ID from unsigned integer
     * 
     * @param v 64-bit unsigned integer value
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    /**
     * @brief Equality comparison operator
     */
    constexpr bool operator==(const PopulationId& other) const = default;
    
    /**
     * @brief Inequality comparison operator
     */
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    /**
     * @brief Less-than comparison for ordered containers
     */
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Get the underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

/**
 * @brief Simulation step counter type
 * 
 * Represents discrete time steps in the simulation. Each step corresponds to
 * a complete integration cycle of the neural dynamics.
 */
using SimulationStep = uint64_t;

/**
 * @brief Simulation time in seconds (floating point)
 * 
 * Represents continuous time in seconds for precise timing calculations
 * and event scheduling. Uses double precision for accuracy in long simulations.
 */
using Timestamp = double;

/**
 * @brief Timestep duration in seconds
 * 
 * The duration of each simulation step in seconds. Used for calculating
 * time-based dynamics and event scheduling.
 */
using TimestepDuration = double;

/**
 * @brief Neuron index type for arrays
 * 
 * Zero-based index into neuron arrays and data structures.
 * Used for efficient array access patterns.
 */
using NeuronIndex = uint64_t;

/**
 * @brief Synapse index type for arrays
 * 
 * Zero-based index into synapse arrays and connectivity matrices.
 */
using SynapseIndex = uint64_t;

/**
 * @brief Population index type for arrays
 * 
 * Zero-based index into population arrays.
 */
using PopulationIndex = uint64_t;

/**
 * @brief Region index type for arrays
 * 
 * Zero-based index into region arrays.
 */
using RegionIndex = uint64_t;

/**
 * @brief Synaptic weight type
 * 
 * Floating-point type for representing synaptic connection strengths.
 * Uses single precision for efficiency in neural computations.
 */
using SynapticWeight = float;

/**
 * @brief Membrane potential type
 * 
 * Represents the electrical potential across a neuron's membrane.
 * Values are typically in the range [-1.0, 1.0] normalized.
 */
using MembranePotential = float;

/**
 * @brief Neuron firing rate type
 * 
 * Represents the firing rate of a neuron in Hertz (events per second).
 */
using FiringRate = float;

/**
 * @brief Connection delay in simulation steps
 * 
 * Number of simulation steps required for a spike to travel from
 * source to destination neuron through a synaptic connection.
 */
using Delay = uint32_t;

/**
 * @brief Enumeration of neuron types
 * 
 * Defines the morphological and functional types of neurons in the model.
 * Each type has distinct electrophysiological properties and behavioral roles.
 */
enum class NeuronType : uint8_t {
    /** Regular spiking excitatory neuron with standard characteristics */
    Excitatory,
    /** Fast-spiking inhibitory neuron providing precise control */
    Inhibitory,
    /** Modulatory neuron affecting broader network state */
    Modulatory,
    /** Neuron that receives external sensory input */
    Sensory,
    /** Neuron that produces motor outputs */
    Motor,
    /** Internal neuron for internal processing */
    Internal
};

/**
 * @brief Enumeration of synapse types
 * 
 * Defines the biophysical properties of synaptic connections between neurons.
 */
enum class SynapseType : uint8_t {
    /** Classical glutamatergic excitatory synapse */
    Excitatory,
    /** Classical GABAergic inhibitory synapse */
    Inhibitory,
    /** Neuromodulatory synapse affecting neuron excitability */
    Modulatory,
    /** Direct electrical coupling between neurons */
    Electrical,
    /** Gap junction for rapid electrical transmission */
    GapJunction
};

/**
 * @struct PlasticityFlags
 * @brief Bit flags indicating synaptic plasticity mechanisms
 * 
 * This structure uses bit fields to efficiently store which plasticity
 * mechanisms are active for a particular synapse. This allows for flexible
 * combinations of plasticity rules.
 * 
 * @note The eligible flag indicates whether the synapse is currently eligible
 *       for plasticity based on neuromodulatory signals.
 */
struct PlasticityFlags {
    /** Hebbian (additive) plasticity mechanism enabled */
    bool hebbian : 1;
    /** Spike-Timing-Dependent Plasticity enabled */
    bool stdp : 1;
    /** Reward-modulated plasticity enabled */
    bool reward_modulated : 1;
    /** Structural plasticity enabled */
    bool structural : 1;
    /** Synapse currently eligible for plasticity */
    bool eligible : 1;
    
    /**
     * @brief Default constructor initializes all flags to false
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * @brief Enumeration of developmental stages
 * 
 * Represents the progressive stages of brain development from initial
 * formation through adulthood to aging. Each stage has characteristic
 * plasticities and capabilities.
 */
enum class DevelopmentalStage : uint8_t {
    /** Initial developmental stage with high plasticity */
    Initial,
    /** Critical period with experience-dependent refinement */
    CriticalPeriod,
    /** Maturation phase with stabilization */
    Maturation,
    /** Fully mature adult brain */
    Adult,
    /** Aging phase with declining plasticity */
    Aging
};

/**
 * @struct SpikeEvent
 * @brief Represents a single spike event in the neural network
 * 
 * Contains timing and identification information for a neuron's spike that
 * needs to be processed by the network. Used in the event-driven simulation
 * architecture for efficiency.
 */
struct SpikeEvent {
    /** Neuron that generated the spike */
    NeuronId source_neuron;
    /** Time when spike occurred */
    Timestamp timestamp;
    /** Simulation step when spike occurred */
    SimulationStep step;
    
    /**
     * @brief Default constructor creates empty spike event
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Construct spike event with full data
     * 
     * @param nid Neuron that spiked
     * @param ts Time when spike occurred
     * @param s Simulation step
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @struct DelayedSpikeEvent
 * @brief Represents a spike scheduled for future delivery
 * 
 * Contains all information needed to deliver a spike after a specified delay.
 * Used in the transmission system to handle synaptic delays efficiently.
 */
struct DelayedSpikeEvent {
    /** Neuron that generated the spike */
    NeuronId source_neuron;
    /** Destination neuron that will receive the spike */
    NeuronId destination_neuron;
    /** Synapse through which spike will travel */
    SynapseId synapse_id;
    /** Synaptic weight at time of spike (frozen for transmission) */
    SynapticWeight weight;
    /** Type of synapse (excitatory/inhibitory/etc.) */
    SynapseType synapse_type;
    /** When the spike will reach the source neuron */
    Timestamp timestamp;
    /** When the spike will reach destination neuron */
    Timestamp delivery_time;
    /** Current simulation step */
    SimulationStep step;
    /** Simulation step when spike should be delivered */
    SimulationStep delivery_step;
    /** Pre-computed excitability flag for efficiency */
    bool is_excitatory;
    
    /**
     * @brief Default constructor creates empty delayed spike event
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Construct delayed spike event with full data
     * 
     * @param src Source neuron
     * @param dst Destination neuron
     * @param syn Synapse ID
     * @param w Synaptic weight
     * @param type Synapse type
     * @param ts Spike timestamp
     * @param delivery Delivery timestamp
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
 * @brief Enumeration of neuron firing states
 * 
 * Represents the discrete state of a neuron in the firing state machine.
 * These states are used in the membrane potential dynamics and refractory period logic.
 */
enum class FiringState : uint8_t {
    /** Neuron is at resting potential, not firing */
    Resting,
    /** Neuron is actively firing action potentials */
    Active,
    /** Neuron is in refractory period after firing */
    Refractory,
    /** Neuron is inhibited and cannot fire */
    Inhibited
};

/**
 * @brief Generic handle type for neurons (efficiency optimization)
 * 
 * Used in performance-critical code where full NeuronId overhead is unnecessary.
 * Handles are not comparable or ordered, only unique within their scope.
 */
using NeuronHandle = uint64_t;

/**
 * @brief Generic handle type for synapses (efficiency optimization)
 * 
 * Used in performance-critical code for synapse references.
 */
using SynapseHandle = uint64_t;

/**
 * @brief Invalid neuron ID sentinel value
 * 
 * Represents an invalid neuron identifier. Should not be used as a valid
 * neuron ID in the simulation. Use INVALID_NEURON_ID instead of creating
 * NeuronId(UINT64_MAX) directly.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);

/**
 * @brief Invalid synapse ID sentinel value
 */
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);

/**
 * @brief Invalid region ID sentinel value
 */
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);

/**
 * @brief Invalid population ID sentinel value
 */
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm
