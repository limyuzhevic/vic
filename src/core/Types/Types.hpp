#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety
// 
// Neuron identifier - provides type-safe neuron references throughout the system
// Prevents accidental type confusion between different identifier types
// Facilitates debugging with human-readable ID representations
struct NeuronId {
    uint64_t value;  ///< Internal integer value representing the neuron index
    
    /**
     * @brief Default constructor creates an invalid (zero) neuron ID
     * 
     * @note Use INVALID_NEURON_ID for explicitly invalid neuron references
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Constructor from unsigned 64-bit integer
     * 
     * @param v The neuron index value (typically from simulation)
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Comparison operators for use in containers and algorithms
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    /**
     * @brief Convert neuron ID to index value for array operations
     * 
     * @return The unsigned 64-bit index corresponding to this neuron ID
     */
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier - provides type-safe synaptic connection references
// Enables precise tracking of individual synaptic connections throughout the brain
struct SynapseId {
    uint64_t value;  ///< Internal integer value representing the synapse index
    
    /**
     * @brief Default constructor creates an invalid (zero) synapse ID
     * 
     * @note Use INVALID_SYNAPSE_ID for explicitly invalid synapse references
     */
    constexpr SynapseId() : value(0) {}
    
    /**
     * @brief Constructor from unsigned 64-bit integer
     * 
     * @param v The synapse index value (typically from simulation)
     */
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    // Comparison operators for use in containers and algorithms
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    /**
     * @brief Convert synapse ID to index value for array operations
     * 
     * @return The unsigned 64-bit index corresponding to this synapse ID
     */
    constexpr uint64_t index() const { return value; }
};

// Region identifier - provides type-safe brain region references
// Represents computational regions in the artificial brain architecture
struct RegionId {
    uint64_t value;  ///< Internal integer value representing the region index
    
    /**
     * @brief Default constructor creates an invalid (zero) region ID
     * 
     * @note Use INVALID_REGION_ID for explicitly invalid region references
     */
    constexpr RegionId() : value(0) {}
    
    /**
     * @brief Constructor from unsigned 64-bit integer
     * 
     * @param v The region index value (typically from simulation)
     */
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    // Comparison operators for use in containers and algorithms
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    /**
     * @brief Convert region ID to index value for array operations
     * 
     * @return The unsigned 64-bit index corresponding to this region ID
     */
    constexpr uint64_t index() const { return value; }
};

// Population identifier - provides type-safe neural population references
// Represents groups of neurons with shared properties and dynamics
struct PopulationId {
    uint64_t value;  ///< Internal integer value representing the population index
    
    /**
     * @brief Default constructor creates an invalid (zero) population ID
     * 
     * @note Use INVALID_POPULATION_ID for explicitly invalid population references
     */
    constexpr PopulationId() : value(0) {}
    
    /**
     * @brief Constructor from unsigned 64-bit integer
     * 
     * @param v The population index value (typically from simulation)
     */
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    // Comparison operators for use in containers and algorithms
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    /**
     * @brief Convert population ID to index value for array operations
     * 
     * @return The unsigned 64-bit index corresponding to this population ID
     */
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter - represents discrete time steps in the simulation
// Used for event-driven computation and spike timing
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point) - provides high-resolution timing
// Used for precise temporal measurements in continuous time systems
using Timestamp = double;

// Timestep duration in seconds - represents the integration step size for continuous dynamics
// Used in numerical integration of neuron membrane potential equations
using TimestepDuration = double;

// Index types for arrays - provides consistent indexing throughout the system
// Using uint64_t for large-scale neural simulations with potentially millions of neurons
using NeuronIndex = uint64_t;  ///< Index into neuron arrays, typically 0..N-1
using SynapseIndex = uint64_t; ///< Index into synapse arrays, typically 0..M-1  
using PopulationIndex = uint64_t;  ///< Index into population arrays
using RegionIndex = uint64_t;     ///< Index into region arrays

// Synaptic weight type - 32-bit floating point for memory efficiency
// Provides sufficient precision for typical synaptic weight ranges (-1.0 to 1.0)
using SynapticWeight = float;

// Membrane potential type - 32-bit floating point for neuron dynamics
// Standard biophysical range: -100mV to +50mV with typical resting potential near -70mV
using MembranePotential = float;

// Neuron firing rate type - 32-bit floating point for rate-based computation
// Represents instantaneous firing rate in Hertz (spikes per second)
using FiringRate = float;

// Connection delay in simulation steps - discrete delay representation
// Integer values provide exact timing control for synaptic transmission delays
using Delay = uint32_t;

// Typed enums for neuron types - provides type-safe neuron categorization
// Enables compile-time checking of neuron operations and behavior patterns
enum class NeuronType : uint8_t {
    Excitatory,      ///< Neurons that increase postsynaptic membrane potential (typical)
    Inhibitory,      ///< Neurons that decrease postsynaptic membrane potential  
    Modulatory,      ///< Neurons that affect plasticity or gain control
    Sensory,         ///< Input neurons receiving external stimuli
    Motor,           ///< Output neurons generating motor responses
    Internal         ///< Internal processing neurons without direct external connection
};

// Typed enums for synapse types - provides type-safe synaptic connection categorization
// Enables compile-time checking and optimization based on synapse characteristics
enum class SynapseType : uint8_t {
    Excitatory,      ///< Standard glutamatergic synapse increasing postsynaptic potential
    Inhibitory,      ///< GABAergic synapse decreasing postsynaptic potential
    Modulatory,      ///< Neuromodulatory synapse affecting plasticity or gain
    Electrical,      ///< Gap junction allowing direct electrical coupling
    GapJunction      ///< Alternative name for electrical synapses (synonym for Electrical)
};

// Synaptic plasticity state flags - bitfield indicating which plasticity rules are active
// Enables efficient storage and checking of plasticity rule eligibility
struct PlasticityFlags {
    bool hebbian : 1;              ///< Long-term potentiation (LTP) and depression (LTD) learning
    bool stdp : 1;                 ///< Spike-timing-dependent plasticity learning
    bool reward_modulated : 1;     ///< Reward-modulated learning (e.g., dopamine-dependent)
    bool structural : 1;           ///< Structural plasticity (connection formation/remodeling)
    bool eligible : 1;             ///< Whether synapse is currently eligible for plasticity
    
    /**
     * @brief Default constructor initializes all plasticity flags to false
     * 
     * @note Synapse is not eligible for plasticity until explicitly enabled
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental stage - represents the brain's developmental timeline
// Influences plasticity rates, excitability, and structural development
enum class DevelopmentalStage : uint8_t {
    Initial,         ///< Initial developmental phase with high plasticity
    CriticalPeriod,  ///< Sensitive period for specific learning and skill acquisition
    Maturation,      ///< Continued refinement and strengthening of existing pathways
    Adult,           ///< Stable adult brain with limited structural plasticity
    Aging            ///< Age-related decline in plasticity and function
};

// Spike event for event-driven computation - represents a single neural spike
// Contains source identification and timing information for precise spike processing
struct SpikeEvent {
    NeuronId source_neuron;  ///< The neuron that generated the spike
    Timestamp timestamp;     ///< Absolute time when the spike occurred (seconds)
    SimulationStep step;    ///< Discrete simulation step when the spike occurred
    
    /**
     * @brief Default constructor creates an invalid zeroed spike event
     * 
     * @note For valid spike events, use the three-parameter constructor
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Create a spike event with specified source and timing
     * 
     * @param nid The neuron that generated the spike
     * @param ts Absolute time of spike occurrence (seconds)
     * @param s Discrete simulation step of spike occurrence
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay - represents a spike that must be transmitted to a postsynaptic neuron
// Contains all information needed for delayed synaptic transmission including weight and timing
struct DelayedSpikeEvent {
    NeuronId source_neuron;        ///< The neuron that generated the spike
    NeuronId destination_neuron;   ///< The postsynaptic neuron that will receive the spike
    SynapseId synapse_id;           ///< The specific synaptic connection transmitting this spike
    SynapticWeight weight;         ///< Synaptic weight at time of spike (frozen for this transmission)
    SynapseType synapse_type;      ///< Type of synapse for transmission properties
    Timestamp timestamp;           ///< When the spike actually occurs (absolute time)
    Timestamp delivery_time;       ///< When the spike reaches destination (timestamp + delay)
    SimulationStep step;           ///< Current simulation step when spike occurred
    SimulationStep delivery_step;  ///< Simulation step when spike should be delivered
    bool is_excitatory;            ///< Cached boolean for fast transmission logic
    
    /**
     * @brief Default constructor creates an invalid zeroed delayed spike event
     * 
     * @note For valid delayed spike events, use the eight-parameter constructor
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Create a delayed spike event with complete transmission information
     * 
     * @param src Source neuron that generated the spike
     * @param dst Destination neuron that will receive the spike
     * @param syn The synaptic connection transmitting this spike
     * @param w Synaptic weight at time of spike (frozen)
     * @param type Type of synapse (determines transmission properties)
     * @param ts Absolute time when spike occurs
     * @param delivery Absolute time when spike reaches destination
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

// Action potential state - represents the current firing state of a neuron
// Used in integrate-and-fire neuron models to track spiking behavior
enum class FiringState : uint8_t {
    Resting,     ///< Neuron is below threshold, not currently firing
    Active,      ///< Neuron has crossed threshold and is currently firing
    Refractory,  ///< Neuron is in refractory period after a spike
    Inhibited    ///< Neuron is actively inhibited and cannot fire
};

// Generic handle types for efficiency - provides opaque handles to objects
// Enables faster copying and comparison than passing full objects
using NeuronHandle = uint64_t;  ///< Opaque handle to a neuron (32-bit for memory efficiency)
using SynapseHandle = uint64_t; ///< Opaque handle to a synapse (32-bit for memory efficiency)

// Invalid ID sentinel - represents an invalid or uninitialized ID reference
// Used to distinguish between valid zero-IDs and truly invalid references
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
/**
 * @brief Represents an invalid neuron reference
 * 
 * @note Should be used instead of NeuronId() when checking for invalid IDs
 * @see INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
 */
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
/**
 * @brief Represents an invalid synapse reference
 * 
 * @note Should be used instead of SynapseId() when checking for invalid IDs
 * @see INVALID_NEURON_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
 */
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
/**
 * @brief Represents an invalid region reference
 * 
 * @note Should be used instead of RegionId() when checking for invalid IDs
 * @see INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_POPULATION_ID
 */
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);
/**
 * @brief Represents an invalid population reference
 * 
 * @note Should be used instead of PopulationId() when checking for invalid IDs
 * @see INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID
 */

} // namespace nlm
