#pragma once
  
#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <limits>
#include <utility>

namespace nlm {

// ============================================================================
// TYPE DEFINITIONS AND ENUMERATIONS
// ============================================================================

/**
 * @brief Base template for strongly-typed identifiers to eliminate code duplication
 * @details Provides comparison operators, increment, and index() method for ID types
 * 
 * @tparam Derived The derived ID type (e.g., NeuronId)
 * @tparam INVALID_VALUE The sentinel value for invalid IDs
 */
template<typename Derived, uint64_t INVALID_VALUE>
class IdBase {
public:
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid ID
     */
    constexpr IdBase() noexcept : value(0) {}
    
    /**
     * @brief Constructor from uint64_t value
     * 
     * @param v The numeric ID value
     */
    explicit constexpr IdBase(uint64_t v) noexcept : value(v) {}
    
    // Comparison operators
    constexpr bool operator==(const Derived& other) const noexcept = default;
    constexpr bool operator!=(const Derived& other) const noexcept = default;
    constexpr bool operator<(const Derived& other) const noexcept { return value < other.value; }
    
    // Increment operators
    constexpr Derived& operator++() noexcept { ++value; return static_cast<Derived&>(*this); }
    constexpr Derived operator++(int) noexcept { 
        Derived old = static_cast<Derived&>(*this); 
        ++value; 
        return old; 
    }
    
    /**
     * @brief Get the underlying index value
     * 
     * @return The uint64_t index value
     */
    constexpr uint64_t index() const noexcept { return value; }
    
    // Type traits for static type checking
    template<typename T>
    static constexpr bool is_same_type() { return std::is_same_v<T, Derived>; }
};

// ============================================================================
// STRONGLY-TYPED IDENTIFIERS
// ============================================================================

/**
 * @brief Strongly-typed neuron identifier
 * @details Provides safe neuron identification with INVALID_NEURON_ID sentinel
 * 
 * Uses the IdBase template to eliminate code duplication across ID types
 */
struct NeuronId : public IdBase<NeuronId, UINT64_MAX> {
    using Base = IdBase<NeuronId, UINT64_MAX>;
    
    // Inherit constructor from base class
    using Base::Base;
    
    /**
     * @brief Sentinel value for invalid neuron IDs
     */
    static constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
};

/**
 * @brief Strongly-typed synapse identifier
 * @details Provides safe synapse identification with INVALID_SYNAPSE_ID sentinel
 */
struct SynapseId : public IdBase<SynapseId, UINT64_MAX> {
    using Base = IdBase<SynapseId, UINT64_MAX>;
    
    using Base::Base;
    
    static constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
};

/**
 * @brief Strongly-typed region identifier
 * @details Provides safe region identification with INVALID_REGION_ID sentinel
 */
struct RegionId : public IdBase<RegionId, UINT64_MAX> {
    using Base = IdBase<RegionId, UINT64_MAX>;
    
    using Base::Base;
    
    static constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
};

/**
 * @brief Strongly-typed population identifier
 * @details Provides safe population identification with INVALID_POPULATION_ID sentinel
 */
struct PopulationId : public IdBase<PopulationId, UINT64_MAX> {
    using Base = IdBase<PopulationId, UINT64_MAX>;
    
    using Base::Base;
    
    static constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);
};

// ============================================================================
// SIMULATION TIME AND INDEX TYPES
// ============================================================================

/**
 * @brief Simulation step counter (integer)
 * @details Discrete time steps in the simulation
 */
using SimulationStep = uint64_t;

/**
 * @brief Simulation time in seconds (floating point)
 * @details Continuous time value for precise timing
 */
using Timestamp = double;

/**
 * @brief Timestep duration in seconds
 * @details Time interval between simulation steps
 */
using TimestepDuration = double;

/**
 * @brief Neuron index type for array access
 */
using NeuronIndex = uint64_t;

/**
 * @brief Synapse index type for array access
 */
using SynapseIndex = uint64_t;

/**
 * @brief Population index type for array access
 */
using PopulationIndex = uint64_t;

/**
 * @brief Region index type for array access
 */
using RegionIndex = uint64_t;

// ============================================================================
// NEURAL PHYSIOLOGY TYPES
// ============================================================================

/**
 * @brief Synaptic weight magnitude
 * @details Float value representing connection strength between neurons
 */
using SynapticWeight = float;

/**
 * @brief Membrane potential voltage
 * @details Neuron membrane potential in millivolts (mV)
 */
using MembranePotential = float;

/**
 * @brief Neuron firing rate
 * @details Number of action potentials per second (Hertz)
 */
using FiringRate = float;

/**
 * @brief Synaptic transmission delay
 * @details Number of simulation steps for spike propagation
 */
using Delay = uint32_t;

// ============================================================================
// NEURON AND SYNAPSE ENUMERATIONS
// ============================================================================

/**
 * @brief Neuron type classification
 * @details Biological/neural type of a neuron cell
 */
enum class NeuronType : uint8_t {
    Excitatory,    ///< Neuron that primarily releases excitatory neurotransmitters
    Inhibitory,    ///< Neuron that primarily releases inhibitory neurotransmitters  
    Modulatory,    ///< Neuron that provides neuromodulatory signals
    Sensory,       ///< Neuron that receives sensory input
    Motor,         ///< Neuron that produces motor output
    Internal       ///< Internal processing neuron
};

/**
 * @brief Synapse type classification
 * @details Functional type of a synaptic connection
 */
enum class SynapseType : uint8_t {
    Excitatory,    ///< Synapse that increases postsynaptic neuron firing
    Inhibitory,    ///< Synapse that decreases postsynaptic neuron firing
    Modulatory,    ///< Synapse that provides modulatory effects
    Electrical,    ///< Electrical synapse (gap junction)
    GapJunction    ///< Direct electrical coupling between neurons
};

/**
 * @brief Synaptic plasticity state flags
 * @details Bit flags indicating which plasticity mechanisms are active
 */
struct PlasticityFlags {
    bool hebbian : 1;          ///< Hebbian (additive) plasticity enabled
    bool stdp : 1;             ///< Spike-timing dependent plasticity enabled
    bool reward_modulated : 1; ///< Reward-modulated plasticity enabled
    bool structural : 1;       ///< Structural plasticity enabled
    bool eligible : 1;         ///< Synapse is eligible for plasticity
    
    /**
     * @brief Default constructor initializes all flags to false
     */
    constexpr PlasticityFlags() noexcept : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

/**
 * @brief Developmental stage of the brain
 * @details Phase in the brain's developmental timeline
 */
enum class DevelopmentalStage : uint8_t {
    Initial,        ///< Beginning of brain development
    CriticalPeriod, ///< Period of high plasticity (early development)
    Maturation,     ///< Brain reaching adult-like functionality
    Adult,          ///< Full mature brain state
    Aging           ///< Age-related decline begins
};

// ============================================================================
// NEURAL EVENT TYPES
// ============================================================================

/**
 * @brief Single spike event for event-driven computation
 * @details Represents one action potential in time
 */
struct SpikeEvent {
    NeuronId source_neuron; ///< Neuron that generated the spike
    Timestamp timestamp;   ///< When the spike occurred
    SimulationStep step;   ///< Which simulation step (integer time)
    
    /**
     * @brief Default constructor creates invalid spike event
     */
    constexpr SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Construct spike event from components
     * 
     * @param nid Source neuron ID
     * @param ts Timestamp when spike occurred
     * @param s Simulation step when spike occurred
     */
    constexpr SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

/**
 * @brief Delayed spike event with full synaptic transmission information
 * @details Complete spike transmission including source, destination, and timing
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;     ///< Origin neuron of the spike
    NeuronId destination_neuron; ///< Target neuron of the spike
    SynapseId synapse_id;        ///< Which synapse transmitted the spike
    SynapticWeight weight;      ///< Synaptic weight at time of spike (frozen)
    SynapseType synapse_type;   ///< Type of the transmitting synapse
    Timestamp timestamp;       ///< When spike occurs at source
    Timestamp delivery_time;   ///< When spike reaches destination (timestamp + delay)
    SimulationStep step;       ///< Current simulation step
    SimulationStep delivery_step; ///< Step when spike should be delivered
    bool is_excitatory;         ///< Cached for fast lookup (synapse_type == Excitatory)
    
    /**
     * @brief Default constructor creates invalid delayed spike event
     */
    constexpr DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Construct complete delayed spike event
     * 
     * @param src Source neuron ID
     * @param dst Destination neuron ID
     * @param syn Synapse ID
     * @param w Synaptic weight
     * @param type Synapse type
     * @param ts Spike timestamp
     * @param delivery Delivery timestamp (ts + delay)
     * @param s Current simulation step
     * @param deliveryS Step when spike should be delivered
     */
    constexpr DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

/**
 * @brief Action potential state of a neuron
 * @details Current firing state in the neuron lifecycle
 */
enum class FiringState : uint8_t {
    Resting,      ///< Neuron at rest potential, not firing
    Active,       ///< Neuron currently firing action potential
    Refractory,   ///< Neuron in refractory period (cannot fire again)
    Inhibited     ///< Neuron is inhibited by external signals
};

/**
 * @brief Generic handle types for efficiency
 * @details Alternative integer-based access to neural entities
 */
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// ============================================================================
// SENTINEL VALUES
// ============================================================================

/**
 * @brief Invalid neuron ID sentinel value
 * @details Use to represent uninitialized or invalid neuron references
 */
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);

/**
 * @brief Invalid synapse ID sentinel value
 * @details Use to represent uninitialized or invalid synapse references
 */
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);

/**
 * @brief Invalid region ID sentinel value
 * @details Use to represent uninitialized or invalid region references
 */
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);

/**
 * @brief Invalid population ID sentinel value
 * @details Use to represent uninitialized or invalid population references
 */
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// ============================================================================
// TYPE TRAITS AND UTILITIES
// ============================================================================

namespace detail {
    /**
     * @brief Type trait to detect if a type is an ID (uses IdBase)
     */
    template<typename T>
    struct is_id_type : std::false_type {};
    
    template<typename Derived, uint64_t INVALID>
    struct is_id_type<IdBase<Derived, INVALID>> : std::true_type {};
    
    /**
     * @brief Compile-time constant for ID type checking
     */
    template<typename T>
    inline constexpr bool is_id_type_v = is_id_type<T>::value;
}

} // namespace nlm

// std::hash specializations for ID types (needed for unordered containers)
namespace std {
/**
 * @brief Hash specialization for NeuronId
 * @details Enables use of NeuronId in std::unordered_map and set containers
 */
template<> struct hash<nlm::NeuronId> {
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
