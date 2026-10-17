/**
 * @file Types.hpp
 * @brief Core type definitions for NLM neural simulation
 * 
 * This file defines strongly-typed identifiers and fundamental types used throughout
 * the NLM neural simulation framework. The type system is designed for safety and
 * clarity, preventing type confusion and enabling meaningful debugging.
 * 
 * The types include:
 * - Strongly-typed IDs for neural entities (neurons, synapses, regions, populations)
 * - Simulation timing and step counters
 * - Physical and neural quantities with appropriate precision
 * - Enumeration types for neural characteristics
 * - Spike and event structures for event-driven computation
 * - Sentinel values for invalid IDs
 * 
 * @note These type definitions follow Doxygen conventions and are designed to be
 *       self-documenting. Each type includes clear comments explaining its purpose
 *       and usage. The strongly-typed identifiers help catch programming errors
 *       at compile time rather than runtime.
 * 
 * @warning These type definitions are foundational to the NLM framework. Changes
 *         to these types may have widespread effects across the entire codebase.
 *         Ensure that any modifications maintain backward compatibility and
 *         follow the established patterns.
 * 
 * @see Brain.hpp - Uses these types extensively for neural simulation
 * @see SimpleWorld.hpp - Uses these types for world integration
 * @see Config.hpp - May define additional type aliases as needed
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety
/**
 * @brief Strongly-typed neuron identifier for memory safety and debugging
 * 
 * NeuronId provides a type-safe identifier for neurons in the neural network.
 * This strongly-typed approach prevents type confusion between different
 * neural entities and aids in debugging by making neuron identities explicit
 * in the code rather than just raw integers.
 * 
 * @note NeuronId is a value type that can be used in containers, comparisons,
 *       and as dictionary keys. The underlying uint64_t storage provides ample
 *       capacity for large neural networks.
 * 
 * @warning Always initialize NeuronId objects using the constructor rather than
 *         directly setting the value field. The value field is public for
 *         performance reasons but should not be modified after creation.
 * 
 * @see SynapseId - Similar identifier for synapses
 * @see RegionId - Similar identifier for brain regions
 * @see INVALID_NEURON_ID - Sentinel value for invalid neuron IDs
 */
struct NeuronId {
    /// Unique identifier for a neuron in the network
    uint64_t value;
    
    /// Default constructor creates a null neuron ID
    constexpr NeuronId() : value(0) {}
    
    /// Construct a neuron ID from raw value
    ///
    /// @param v Raw identifier value (0 typically represents null/invalid)
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    /// Compare two neuron IDs for equality
    constexpr bool operator==(const NeuronId& other) const = default;
    
    /// Compare two neuron IDs for inequality
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /// Compare two neuron IDs for ordering (enables use in sorted containers)
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /// Get the raw index value
    ///
    /// @return The raw uint64_t value stored in this ID
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
/**
 * @brief Strongly-typed synapse identifier for memory safety and debugging
 * 
 * SynapseId provides a type-safe identifier for synaptic connections in the neural
 * network. This prevents type confusion between neurons and synapses and helps
 * identify specific connections in the network topology.
 * 
 * @note SynapseId shares the same design pattern as NeuronId but for synaptic
 *       connections rather than neurons. It can be used in the same contexts.
 * 
 * @warning Synapse identifiers are assigned by the neural network implementation
 *         and should not be manually constructed except for testing purposes.
 *         Valid synapse IDs will be assigned during network initialization.
 * 
 * @see NeuronId - Similar identifier for neurons
 * @see SynapseId - Similar identifier for synapses
 * @see INVALID_SYNAPSE_ID - Sentinel value for invalid synapse IDs
 */
struct SynapseId {
    /// Unique identifier for a synapse in the network
    uint64_t value;
    
    /// Default constructor creates a null synapse ID
    constexpr SynapseId() : value(0) {}
    
    /// Construct a synapse ID from raw value
    ///
    /// @param v Raw identifier value
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    /// Compare two synapse IDs for equality
    constexpr bool operator==(const SynapseId& other) const = default;
    
    /// Compare two synapse IDs for inequality
    constexpr bool operator!=(const SynapseId& other) const = default;
    
    /// Compare two synapse IDs for ordering
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /// Get the raw index value
    ///
    /// @return The raw uint64_t value stored in this ID
    constexpr uint64_t index() const { return value; }
};

// Region identifier
/**
 * @brief Strongly-typed region identifier for brain regions
 * 
 * RegionId provides a type-safe identifier for neural regions in the brain.
 * Regions group neurons with similar properties and functions, such as sensory,
 * motor, or internal regions. This strongly-typed identifier helps manage
 * the complex regional organization of the brain.
 * 
 * @note Brain regions are fundamental organizational units in the NLM framework.
 *       Each region contains neurons and manages local processing before
 *       communicating with other regions through inter-regional connections.
 * 
 * @warning Region IDs are assigned by the brain implementation during network
 *         initialization. They should not be manually assigned except for
 *         testing purposes. Valid region IDs will be assigned during brain
 *         construction.
 * 
 * @see NeuronId - Neuron identifier for neurons within regions
 * @see INVALID_REGION_ID - Sentinel value for invalid region IDs
 */
struct RegionId {
    /// Unique identifier for a brain region
    uint64_t value;
    
    /// Default constructor creates a null region ID
    constexpr RegionId() : value(0) {}
    
    /// Construct a region ID from raw value
    ///
    /// @param v Raw identifier value
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    /// Compare two region IDs for equality
    constexpr bool operator==(const RegionId& other) const = default;
    
    /// Compare two region IDs for inequality
    constexpr bool operator!=(const RegionId& other) const = default;
    
    /// Compare two region IDs for ordering
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /// Get the raw index value
    ///
    /// @return The raw uint64_t value stored in this ID
    constexpr uint64_t index() const { return value; }
};

// Population identifier
/**
 * @brief Strongly-typed population identifier for neuron groups
 * 
 * PopulationId provides a type-safe identifier for groups of neurons with similar
 * properties. Populations represent collections of neurons that process the same
 * type of information or have similar functional roles, such as visual processing
 * neurons or motor control neurons.
 * 
 * @note Populations are organizational units that group neurons for efficiency
 *       and clarity. They can contain multiple neurons and are often used for
 *       population coding in neural processing.
 * 
 * @warning Population identifiers are assigned by the neural network implementation
 *         and should not be manually constructed except for testing purposes.
 *         Valid population IDs will be assigned during network initialization.
 * 
 * @see NeuronId - Individual neuron identifiers within populations
 * @see INVALID_POPULATION_ID - Sentinel value for invalid population IDs
 */
struct PopulationId {
    /// Unique identifier for a neuron population
    uint64_t value;
    
    /// Default constructor creates a null population ID
    constexpr PopulationId() : value(0) {}
    
    /// Construct a population ID from raw value
    ///
    /// @param v Raw identifier value
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    /// Compare two population IDs for equality
    constexpr bool operator==(const PopulationId& other) const = default;
    
    /// Compare two population IDs for inequality
    constexpr bool operator!=(const PopulationId& other) const = default;
    
    /// Compare two population IDs for ordering
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /// Get the raw index value
    ///
    /// @return The raw uint64_t value stored in this ID
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
/**
 * @brief Simulation step counter for discrete time simulation
 * 
 * SimulationStep is a typedef for uint64_t representing the current step
 * in the discrete-time neural simulation. Each step represents a fixed time
 * interval in the simulation clock, used for coordinating neural dynamics
 * and state updates.
 * 
 * @note Simulation steps are monotonically increasing and form the primary
 *       time axis for the neural simulation. Each step corresponds to a
 *       specific time interval determined by the simulation timestep.
 * 
 * @warning Simulation steps should always increase and never decrease or
 *         wrap around (due to uint64_t overflow). The simulation implementation
 *         must handle step overflow gracefully.
 * 
 * @see Timestamp - Real time value corresponding to simulation steps
 * @see TimestepDuration - Duration of each simulation step
 * @see SpikeEvent::step - Spike events include simulation step information
 */
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
/**
 * @brief Real simulation time in seconds (floating-point precision)
 * 
 * Timestamp represents the actual simulation time in seconds with floating-point
 * precision. This allows for sub-millisecond timing accuracy and is essential
 * for time-dependent processes like synaptic transmission delays and spike timing.
 * 
 * @note Timestamp values increase monotonically throughout the simulation.
 *       The zero value typically represents the start of the simulation,
 *       though some processes may use negative timestamps for initialization.
 * 
 * @warning Timestamp arithmetic must handle floating-point precision carefully.
 *         Cumulative rounding errors can occur over long simulations. For some
 *         applications, using simulation steps (SimulationStep) may be more
 *         appropriate for exact timing.
 * 
 * @see SimulationStep - Discrete time steps for simulation coordination
 * @see SpikeEvent::timestamp - Spike events store precise timing information
 * @see DelayedSpikeEvent - Uses timestamps for spike scheduling
 */
using Timestamp = double;

// Timestep duration in seconds
/**
 * @brief Duration of each simulation time step in seconds
 * 
 * TimestepDuration represents the length of time each simulation step
 * covers. This determines the temporal resolution of the simulation and
 * affects the stability and accuracy of the neural dynamics computation.
 * 
 * @note The timestep duration should be chosen carefully:
 *       - Too large: May miss fast dynamics and be unstable
 *       - Too small: May be computationally expensive but more accurate
 *       - Typical values: 0.001 to 0.1 seconds depending on neural scale
 * 
 * @warning The timestep duration must be positive and compatible with the
 *         neural model being simulated. Different neural models may have different
 *         stability requirements and timestep constraints.
 * 
 * @see SimulationStep - Number of steps simulated
 * @see Timestamp - Real time obtained by multiplying steps by duration
 */
using TimestepDuration = double;

// Index types for arrays
/**
 * @brief Index type for neuron arrays (uint64_t)
 * 
 * NeuronIndex provides a type-safe index for accessing neurons in arrays
 * and containers. Using a dedicated type helps distinguish between different
 * kinds of indices and improves code readability.
 * 
 * @note NeuronIndex is used wherever integer indices are needed for neuron
 *       arrays, containers, or data structures. The uint64_t type provides
 *       ample capacity for large neural networks with millions of neurons.
 * 
 * @see SynapseIndex - Similar index type for synapses
 * @see PopulationIndex - Similar index type for populations
 * @see RegionIndex - Similar index type for regions
 */
using NeuronIndex = uint64_t;

/**
 * @brief Index type for synapse arrays (uint64_t)
 * 
 * SynapseIndex provides a type-safe index for accessing synapses in arrays
 * and containers. Synapses connect neurons in the neural network and are
 * organized in synaptic matrices or adjacency lists.
 * 
 * @note SynapseIndex is used for indexing synaptic connections and adjacency
 *       matrices. It should match the capacity needed for the total number
 *       of synapses in the neural network.
 * 
 * @see NeuronIndex - Similar index type for neurons
 * @see SynapseId - Synapse identifier (different from index)
 */
using SynapseIndex = uint64_t;

/**
 * @brief Index type for population arrays (uint64_t)
 * 
 * PopulationIndex provides a type-safe index for accessing neuron populations
 * in arrays and containers. Populations group neurons with similar properties
 * and functions.
 * 
 * @note PopulationIndex is used when working with collections of populations
 *       or when iterating over population-level data structures. It should
 *       accommodate the total number of populations in the network.
 * 
 * @see NeuronIndex - Similar index type for individual neurons
 * @see PopulationId - Population identifier (different from index)
 */
using PopulationIndex = uint64_t;

/**
 * @brief Index type for region arrays (uint64_t)
 * 
 * RegionIndex provides a type-safe index for accessing brain regions in arrays
 * and containers. Regions are fundamental organizational units containing
 * neurons and managing local processing.
 * 
 * @note RegionIndex is used for indexing brain regions and regional data
 *       structures. It should accommodate the total number of regions in
 *       the brain model.
 * 
 * @see NeuronIndex - Similar index type for neurons
 * @see RegionId - Region identifier (different from index)
 */
using RegionIndex = uint64_t;

// Synaptic weight type
/**
 * @brief Synaptic weight type (32-bit floating point)
 * 
 * SynapticWeight represents the strength of connections between neurons.
 * Weights are floating-point values that can be positive (excitatory) or
 * negative (inhibitory). The magnitude determines the influence of one
 * neuron on another.
 * 
 * @note Synaptic weights can be modified by plasticity rules during
 *       learning and development. The float type provides a good balance
 *       between precision and memory efficiency for large networks.
 * 
 * @warning Weight values should be kept within reasonable bounds to prevent
 *         numerical instability. Very large weights can cause overflow,
 *         while very small weights can lose precision.
 * 
 * @see PlasticityFlags - Flags controlling weight modification
 * @see DelayedSpikeEvent::weight - Weight frozen at time of spike
 */
using SynapticWeight = float;

// Membrane potential type
/**
 * @brief Membrane potential type (32-bit floating point)
 * 
 * MembranePotential represents the electrical potential across a neuron's
 * membrane. This value determines whether the neuron will fire (spike) when
 * it reaches threshold. Positive values typically indicate depolarization
 * (increased excitability), while negative values indicate hyperpolarization
 * (decreased excitability).
 * 
 * @note Membrane potential is the fundamental variable in spiking neuron
 *       models. It integrates synaptic inputs and determines neural output.
 *       The float type provides good precision while being memory efficient.
 * 
 * @warning Membrane potential values should be kept within biologically
 *         plausible ranges to maintain model stability and prevent numerical
 *         issues. Typical resting potential is around -70mV.
 * 
 * @see NeuronType - Different neuron types may have different potential dynamics
 * @see injectCurrent() - Function for modifying membrane potential
 */
using MembranePotential = float;

// Neuron firing rate type
/**
 * @brief Neuron firing rate type (32-bit floating point)
 * 
 * FiringRate represents the frequency at which a neuron fires action potentials
 * (spikes). This is a derived quantity that can be computed from spike trains
 * or used as a target for homeostatic regulation.
 * 
 * @note Firing rates are typically measured in spikes per second (Hz).
 *       The float type provides sufficient precision for most biological
 *       and computational purposes.
 * 
 * @warning Firing rates should be kept within biologically plausible ranges
 *         to maintain model realism and prevent numerical issues. Typical
 *         firing rates range from 0 Hz (silent) to hundreds of Hz.
 * 
 * @see getAverageFiringRate() - Function for computing population firing rates
 */
using FiringRate = float;

// Connection delay in simulation steps
/**
 * @brief Connection delay type (32-bit unsigned integer)
 * 
 * Delay represents the transmission delay for signals between neurons in
 * simulation steps. This determines how many time steps must elapse before
 * a spike from one neuron affects another neuron.
 * 
 * @note Delays are essential for realistic neural dynamics, allowing for
 *       temporal patterns and realistic signal propagation. They can model
 *       biological delays, transmission time, or processing time.
 * 
 * @warning Delay values should be chosen carefully:
 *         - Too small: May cause instability or unrealistic timing
 *         - Too large: May be computationally expensive
 *         - Typical values: 1-100 steps depending on network scale
 * 
 * @see InterRegionConnection::delay - Delay for inter-regional connections
 * @see SpikeEvent::step - Spike events include simulation step information
 * @see DelayedSpikeEvent - Uses delays for spike scheduling
 */
using Delay = uint32_t;
