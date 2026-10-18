#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
/**
 * @brief Strongly-typed neuron identifier for type safety
 * 
 * Provides compile-time type safety for neuron references without runtime overhead.
 * Used throughout the brain to prevent invalid neuron access.
 */
struct NeuronId {
    /** @brief Underlying unsigned 64-bit integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor
     * 
     * Creates invalid neuron ID (value 0)
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Constructor from uint64_t
     * 
     * @param v 64-bit unsigned integer value
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
     * @return true if values are not equal
     */
    constexpr bool operator!=(const NeuronId& other) const = default;
    
    /**
     * @brief Less-than comparison operator
     * 
     * @param other Other NeuronId to compare with
     * @return true if this value is less than other
     */
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Get underlying index value
     * 
     * @return uint64_t The stored value
     */
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
/**
 * @brief Strongly-typed synapse identifier for type safety
 * 
 * Provides compile-time type safety for synapse references without runtime overhead.
 * Used throughout the brain to prevent invalid synapse access.
 */
struct SynapseId {
    /** @brief Underlying unsigned 64-bit integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor
     * 
     * Creates invalid synapse ID (value 0)
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Constructor from uint64_t
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
     * @brief Less-than comparison operator
     */
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Get underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

// Region identifier
/**
 * @brief Strongly-typed region identifier for type safety
 * 
 * Provides compile-time type safety for brain region references without runtime overhead.
 * Used to organize neurons into functional regions.
 */
struct RegionId {
    /** @brief Underlying unsigned 64-bit integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor
     * 
     * Creates invalid region ID (value 0)
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Constructor from uint64_t
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
     * @brief Less-than comparison operator
     */
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Get underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

// Population identifier
/**
 * @brief Strongly-typed population identifier for type safety
 * 
 * Provides compile-time type safety for neuron population references without runtime overhead.
 * Used to group neurons with similar properties and functions.
 */
struct PopulationId {
    /** @brief Underlying unsigned 64-bit integer value */
    uint64_t value;
    
    /**
     * @brief Default constructor
     * 
     * Creates invalid population ID (value 0)
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Constructor from uint64_t
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
     * @brief Less-than comparison operator
     */
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Get underlying index value
     */
    constexpr uint64_t index() const { return value; }
};

// Modern C++17 type aliases
/**
 * @brief Alias for 64-bit unsigned integer used for simulation steps
 * 
 * Represents discrete simulation steps in the NLM brain simulation.
 */
using SimulationStep = uint64_t;

/**
 * @brief Alias for double precision floating-point time values
 * 
 * Represents time in seconds with high precision for neural dynamics calculations.
 */
using Timestamp = double;

/**
 * @brief Alias for timestep duration in seconds
 * 
 * Represents the duration of individual simulation steps.
 */
using TimestepDuration = double;

/**
 * @brief Alias for unsigned 64-bit integer used for indexing neurons
 * 
 * Used for array indexing and iteration over neuron collections.
 */
using NeuronIndex = uint64_t;

/**
 * @brief Alias for unsigned 64-bit integer used for indexing synapses
 * 
 * Used for array indexing and iteration over synapse collections.
 */
using SynapseIndex = uint64_t;

/**
 * @brief Alias for unsigned 64-bit integer used for indexing populations
 * 
 * Used for array indexing and iteration over population collections.
 */
using PopulationIndex = uint64_t;

/**
 * @brief Alias for unsigned 64-bit integer used for indexing regions
 * 
 * Used for array indexing and iteration over region collections.
 */
using RegionIndex = uint64_t;

/**
 * @brief Alias for single-precision floating-point synaptic weights
 * 
 * Represents connection strengths between neurons, ranging from -1.0 (inhibitory)
 * to +1.0 (excitatory).
 */
using SynapticWeight = float;

/**
 * @brief Alias for single-precision floating-point membrane potentials
 * 
 * Represents membrane voltages in millivolts, typically in range -100mV to +50mV.
 */
using MembranePotential = float;

/**
 * @brief Alias for single-precision floating-point firing rates
 * 
 * Represents neuron firing rates in Hertz (cycles per second).
 */
using FiringRate = float;

/**
 * @brief Alias for 32-bit unsigned integer used for connection delays
 * 
 * Represents synaptic transmission delays in simulation steps.
 */
using Delay = uint32_t;

/**
 * @brief Strongly-typed enum for neuron types
 * 
 * Defines different functional types of neurons in the brain architecture.
 */
enum class NeuronType : uint8_t {
    /** @brief Excitatory neurons that promote depolarization */
    Excitatory,
    /** @brief Inhibitory neurons that promote hyperpolarization */
    Inhibitory,
    /** @brief Modulatory neurons that release neuromodulators */
    Modulatory,
    /** @brief Sensory neurons that receive external input */
    Sensory,
    /** @brief Motor neurons that generate actions */
    Motor,
    /** @brief Internal neurons for internal processing */
    Internal
};

/**
 * @brief Strongly-typed enum for synapse types
 * 
 * Defines different functional types of synaptic connections.
 */
enum class SynapseType : uint8_t {
    /** @brief Classical excitatory glutamatergic synapses */
    Excitatory,
    /** @brief Classical inhibitory GABAergic synapses */
    Inhibitory,
    /** @brief Neuromodulatory synapses */
    Modulatory,
    /** @brief Electrical synapses (gap junctions) */
    Electrical,
    /** @brief Specialized gap junctions for electrical coupling */
    GapJunction
};

/**
 * @brief Strongly-typed bit field for synaptic plasticity flags
 * 
 * Compact representation of which plasticity mechanisms are enabled for a synapse.
 */
struct PlasticityFlags {
    /** @brief Enable Hebbian (co-activity) plasticity */
    bool hebbian : 1;
    /** @brief Enable Spike-Timing-Dependent Plasticity (STDP) */
    bool stdp : 1;
    /** @brief Enable reward-modulated plasticity */
    bool reward_modulated : 1;
    /** @brief Enable structural plasticity (synaptogenesis/pruning) */
    bool structural : 1;
    /** @brief Mark synapse as eligible for plasticity */
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
 * @brief Strongly-typed enum for developmental stages
 * 
 * Represents different phases of brain development and maturation.
 */
enum class DevelopmentalStage : uint8_t {
    /** @brief Initial state, high plasticity, basic development */
    Initial,
    /** @brief Critical period, heightened learning capacity */
    CriticalPeriod,
    /** @brief Maturation phase, refined connections */
    Maturation,
    /** @brief Adult stage, stable but still plastic */
    Adult,
    /** @brief Aging phase, declining plasticity */
    Aging
};

/**
 * @brief Event-driven spike representation
 * 
 * Captures the timing and source of neural spikes for event-driven processing.
 */
struct SpikeEvent {
    /** @brief Neuron that generated the spike */
    NeuronId source_neuron;
    /** @brief Timestamp when the spike occurred */
    Timestamp timestamp;
    /** @brief Simulation step when the spike occurred */
    SimulationStep step;
    
    /**
     * @brief Default constructor
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Constructor with parameters
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @brief Delayed spike representation with transmission timing
 * 
 * Represents a spike that needs to be delivered after a delay, capturing
 * the complete journey from source to destination.
 */
struct DelayedSpikeEvent {
    /** @brief Neuron that generated the spike */
    NeuronId source_neuron;
    /** @brief Neuron that will receive the spike */
    NeuronId destination_neuron;
    /** @brief Synapse through which the spike travels */
    SynapseId synapse_id;
    /** @brief Synaptic weight at time of spike (frozen) */
    SynapticWeight weight;  // Weight at time of spike (frozen)
    /** @brief Type of synaptic connection */
    SynapseType synapse_type;
    /** @brief Timestamp when spike occurs */
    Timestamp timestamp;      // When spike occurs
    /** @brief Timestamp when spike reaches destination */
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    /** @brief Current simulation step */
    SimulationStep step;      // Current simulation step
    /** @brief Step when spike should be delivered */
    SimulationStep delivery_step;  // Step when spike should be delivered
    /** @brief Cached excitatory/inhibitory flag for efficiency */
    bool is_excitatory;  // Cached for quick lookup
    
    /**
     * @brief Default constructor
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Constructor with all parameters
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
 * @brief Neural firing state enumeration
 * 
 * Represents the different states a neuron can be in during simulation.
 */
enum class FiringState : uint8_t {
    /** @brief Resting state, below threshold */
    Resting,
    /** @brief Active state, above threshold but not yet spiked */
    Active,
    /** @brief Refractory state, cannot fire this step */
    Refractory,
    /** @brief Inhibited state, actively suppressed */
    Inhibited
};

/**
 * @brief Generic handle types for efficient storage
 * 
 * Type-safe handle types that can be stored directly without indirection.
 * Used for efficient collection storage and retrieval.
 */
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

/**
 * @brief Sentinel value for invalid IDs
 * 
 * Used to represent invalid or null identifiers throughout the system.
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm
