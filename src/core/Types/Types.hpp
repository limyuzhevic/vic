#pragma once
/**
 * @file Types.hpp
 * @brief NLM Type System and Strongly-Typed Identifiers
 * 
 * This file defines the core type system for NLM, providing:
 * - Strongly-typed identifiers for all major entities (neurons, synapses, regions)
 * - Type-safe aliases for common data types
 * - Enumeration of important entity types
 * - Action potential state definitions
 * - Compatibility with C++20 concepts and constexpr
 * 
 * The type system ensures type safety throughout the NLM brain simulation,
 * preventing common programming errors like mixing neuron IDs with
 * synapse IDs or using invalid values.
 * 
 * @author NLM Research Team
 * @date Phase 6 Integration
 * @version 0.1.0
 */

#include <cstdint>
#include <type_traits>
#include <limits>
#include <string>

namespace nlm {

// =====================================================================
// Strongly-Typed Identifiers
// =====================================================================

/**
 * @class NeuronId
 * @brief Strongly-typed neuron identifier
 * 
 * NeuronId provides compile-time type safety for neuron references.
 * Each neuron has a unique identifier that prevents accidental mixing
 * of different types of neural entities.
 * 
 * Benefits:
 * - Compile-time type safety (cannot mix NeuronId with SynapseId)
 * - Bounds checking and validation
 * - Clear intent in code (NLM_LOG_INFO("Neuron " + std::to_string(id.index())))
 * - Easy serialization and debugging
 * 
 * Example usage:
 * @code
 * NeuronId neuron1(42);
 * NeuronId neuron2 = NeuronId(100);
 * if (neuron1 == neuron2) { /* neurons are the same */ }
 * @endcode
 */
struct NeuronId {
    uint64_t value;
    
    /**
     * @brief Default constructor creates invalid ID
     */
    constexpr NeuronId() : value(0) {}
    
    /**
     * @brief Construct from unsigned 64-bit integer
     * 
     * @param v Numeric identifier
     */
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Comparison operators
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    constexpr bool operator<=(const NeuronId& other) const { return value <= other.value; }
    constexpr bool operator>(const NeuronId& other) const { return value > other.value; }
    constexpr bool operator>=(const NeuronId& other) const { return value >= other.value; }
    
    // Arithmetic operations
    constexpr NeuronId operator+(uint64_t offset) const { return NeuronId(value + offset); }
    constexpr NeuronId& operator+=(uint64_t offset) { value += offset; return *this; }
    constexpr NeuronId operator-(uint64_t offset) const { return NeuronId(value - offset); }
    constexpr NeuronId& operator-=(uint64_t offset) { value -= offset; return *this; }
    
    /**
     * @brief Get the numeric value
     */
    constexpr uint64_t index() const { return value; }
    
    /**
     * @brief Check if this is an invalid ID
     */
    constexpr bool isValid() const { return value != 0 && value != UINT64_MAX; }
    
    /**
     * @brief Convert to string for display
     */
    std::string toString() const { return std::to_string(value); }
};

/**
 * @class SynapseId
 * @brief Strongly-typed synapse identifier
 * 
 * SynapseId ensures type-safe synapse references, preventing accidental
 * mixing with neuron IDs or other neural entity identifiers.
 */
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    std::string toString() const { return std::to_string(value); }
};

/**
 * @class RegionId
 * @brief Strongly-typed neural region identifier
 * 
 * RegionId identifies neural regions (compartments) within the brain.
 * Each region can contain multiple populations of neurons.
 */
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    std::string toString() const { return std::to_string(value); }
};

/**
 * @class PopulationId
 * @brief Strongly-typed neural population identifier
 * 
 * PopulationId identifies groups of neurons with similar characteristics
 * (e.g., sensory neurons, motor neurons, internal neurons).
 */
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    std::string toString() const { return std::to_string(value); }
};

// =====================================================================
// Core Simulation Types
// =====================================================================

/**
 * @typedef SimulationStep
 * @brief Simulation step counter (unsigned 64-bit)
 * 
 * Used for tracking simulation progress and timing. Allows for long
 * simulations without overflow.
 */
using SimulationStep = uint64_t;

/**
 * @typedef Timestamp
 * @brief Simulation time in seconds (double precision)
 * 
 * Uses double precision for accurate timing over long simulations.
 * Compatible with standard C++ time functions.
 */
using Timestamp = double;

/**
 * @typedef TimestepDuration
 * @brief Duration of a single simulation timestep
 * 
 * Typically small values (e.g., 0.001 for 1ms timesteps).
 */
using TimestepDuration = double;

/**
 * @typedef NeuronIndex
 * @brief Index type for neuron arrays (unsigned 64-bit)
 */
using NeuronIndex = uint64_t;

/**
 * @typedef SynapseIndex
 * @brief Index type for synapse arrays (unsigned 64-bit)
 */
using SynapseIndex = uint64_t;

/**
 * @typedef PopulationIndex
 * @brief Index type for population arrays (unsigned 64-bit)
 */
using PopulationIndex = uint64_t;

/**
 * @typedef RegionIndex
 * @brief Index type for region arrays (unsigned 64-bit)
 */
using RegionIndex = uint64_t;

// =====================================================================
// Value Types
// =====================================================================

/**
 * @typedef SynapticWeight
 * @brief Synaptic connection weight (floating point)
 * 
 * Range: Typically [-1.0, 1.0] where positive is excitatory and negative is inhibitory.
 * Used by both STDP and Hebbian plasticity rules.
 */
using SynapticWeight = float;

/**
 * @typedef MembranePotential
 * @brief Neuron membrane potential (floating point)
 * 
 * Range: Typically [-100.0, 50.0] mV depending on neuron model.
 * Measured in millivolts (mV) relative to resting potential.
 */
using MembranePotential = float;

/**
 * @typedef FiringRate
 * @brief Neuron firing rate (events per second)
 * 
 * Used for performance monitoring and analysis.
 */
using FiringRate = float;

/**
 * @typedef Delay
 * @brief Connection delay in simulation steps
 * 
 * Integer type for efficient timing calculations.
 */
using Delay = uint32_t;

// =====================================================================
// Enumeration Types
// =====================================================================

/**
 * @enum class NeuronType
 * @brief Classification of neuron types
 * 
 * Neurons are categorized based on their function and connectivity patterns.
 */
enum class NeuronType : uint8_t {
    /** Primary sensory input neurons */
    Excitatory = 0,
    /** Neurons that inhibit other neurons */
    Inhibitory,
    /** Neuromodulatory neurons (dopamine, serotonin, etc.) */
    Modulatory,
    /** Neurons that receive external sensory input */
    Sensory,
    /** Neurons that send motor output signals */
    Motor,
    /** Internal association and processing neurons */
    Internal,
    
    // Maximum value for array sizing
    Max = 6
};

/**
 * @enum class SynapseType
 * @brief Classification of synapse types
 * 
 * Different synapse types have different properties and plasticity rules.
 */
enum class SynapseType : uint8_t {
    /** Fast excitatory synaptic transmission */
    Excitatory = 0,
    /** Fast inhibitory synaptic transmission */
    Inhibitory,
    /** Slow neuromodulatory transmission */
    Modulatory,
    /** Electrical coupling (gap junctions) */
    Electrical,
    /** Specialized chemical synapses */
    GapJunction,
    
    // Maximum value for array sizing
    Max = 5
};

/**
 * @struct PlasticityFlags
 * @brief Bitfield indicating which plasticity rules apply to a synapse
 * 
 * Synapses can have multiple plasticity mechanisms simultaneously.
 */
struct PlasticityFlags {
    bool hebbian : 1;          ///< Hebbian learning rule
    bool stdp : 1;              ///< Spike-Timing-Dependent Plasticity
    bool reward_modulated : 1; ///< Reward-modulated plasticity
    bool structural : 1;       ///< Structural plasticity (growth/pruning)
    bool eligible : 1;         ///< Whether synapse is eligible for plasticity
    
    /**
     * @brief Default constructor sets all flags to false
     */
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
    
    /**
     * @brief Enable all plasticity rules
     */
    constexpr void enableAll() {
        hebbian = true;
        stdp = true;
        reward_modulated = true;
        structural = true;
        eligible = true;
    }
    
    /**
     * @brief Disable all plasticity rules
     */
    constexpr void disableAll() {
        hebbian = false;
        stdp = false;
        reward_modulated = false;
        structural = false;
        eligible = false;
    }
    
    /**
     * @brief Check if any plasticity rules are enabled
     */
    constexpr bool hasAny() const {
        return hebbian || stdp || reward_modulated || structural;
    }
    
    /**
     * @brief Get human-readable description of enabled rules
     */
    std::string toString() const {
        std::string result;
        if (hebbian) result += "Hebbian,";
        if (stdp) result += "STDP,";
        if (reward_modulated) result += "RewardMod,";
        if (structural) result += "Structural,";
        if (result.empty()) result = "None";
        // Remove trailing comma
        if (result.back() == ',') result.pop_back();
        return result;
    }
};

/**
 * @enum class DevelopmentalStage
 * @brief Current developmental stage of the brain
 * 
 * The brain undergoes developmental stages that affect plasticity rates
 * and learning capabilities.
 */
enum class DevelopmentalStage : uint8_t {
    /** Initial undifferentiated state */
    Initial = 0,
    /** Critical period with high plasticity */
    CriticalPeriod,
    /** Maturation phase with decreasing plasticity */
    Maturation,
    /** Adult stable state */
    Adult,
    /** Aging phase with reduced plasticity */
    Aging,
    
    // Maximum value for array sizing
    Max = 5
};

// =====================================================================
// Event Structures
// =====================================================================

/**
 * @struct SpikeEvent
 * @brief Represents a single neuron spike event
 * 
 * Used for event-driven computation and spike timing-dependent plasticity.
 */
struct SpikeEvent {
    NeuronId source_neuron;     ///< Which neuron fired
    Timestamp timestamp;       ///< When the spike occurred
    SimulationStep step;       ///< Simulation step when spike occurred
    
    /**
     * @brief Default constructor
     */
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    
    /**
     * @brief Construct from individual components
     */
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
    
    /**
     * @brief Convert to string for debugging
     */
    std::string toString() const {
        return "Spike from neuron " + source_neuron.toString() +
               " at time " + std::to_string(timestamp) +
               " (step " + std::to_string(step) + ")";
    }
};

/**
 * @struct DelayedSpikeEvent
 * @brief Spike event with transmission delay
 * 
 * Represents a spike that will be delivered to its target after a delay.
 * Used for synaptic transmission modeling.
 */
struct DelayedSpikeEvent {
    NeuronId source_neuron;          ///< Origin neuron
    NeuronId destination_neuron;     ///< Target neuron
    SynapseId synapse_id;            ///< Which synapse transmitted
    SynapticWeight weight;          ///< Synaptic weight at time of spike
    SynapseType synapse_type;       ///< Type of synapse
    Timestamp timestamp;            ///< When spike occurred
    Timestamp delivery_time;       ///< When spike should be delivered
    SimulationStep step;            ///< Current simulation step
    SimulationStep delivery_step;   ///< Step when spike should be delivered
    bool is_excitatory;             ///< Cached for quick lookup
    
    /**
     * @brief Default constructor
     */
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    /**
     * @brief Construct from individual components
     */
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
    
    /**
     * @brief Check if this event is ready to be delivered
     */
    bool isReady(SimulationStep currentStep) const {
        return currentStep >= delivery_step;
    }
    
    /**
     * @brief Get time until delivery (negative if overdue)
     */
    double timeUntilDelivery(SimulationStep currentStep) const {
        return (delivery_step - currentStep) * 0.001; // Assuming 1ms timestep
    }
    
    /**
     * @brief Convert to string for debugging
     */
    std::string toString() const {
        return "Spike from " + source_neuron.toString() + " to " + 
               destination_neuron.toString() + " via synapse " + 
               synapse_id.toString() + " (weight: " + std::to_string(weight) + ")";
    }
};

// =====================================================================
// Action Potential State
// =====================================================================

/**
 * @enum class FiringState
 * @brief Current state of a neuron's action potential
 * 
 * Represents different phases of the neuron's membrane potential dynamics.
 */
enum class FiringState : uint8_t {
    /** Resting state, ready to receive input */
    Resting = 0,
    /** Membrane potential above threshold, generating spike */
    Active,
    /** Refractory period after spike, cannot fire again */
    Refractory,
    /** Neuron is inhibited and cannot fire */
    Inhibited,
    
    // Maximum value for array sizing
    Max = 4
};

/**
 * @brief Get human-readable name for firing state
 */
inline std::string firingStateToString(FiringState state) {
    switch (state) {
        case FiringState::Resting: return "Resting";
        case FiringState::Active: return "Active";
        case FiringState::Refractory: return "Refractory";
        case FiringState::Inhibited: return "Inhibited";
        default: return "Unknown";
    }
}

// =====================================================================
// Sentinel Values and Constants
// =====================================================================

/**
 * @brief Invalid neuron ID sentinel value
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

/**
 * @brief Maximum number of neurons (safety limit)
 */
constexpr NeuronIndex MAX_NEURONS = UINT64_MAX;

/**
 * @brief Maximum number of synapses (safety limit)
 */
constexpr SynapseIndex MAX_SYNAPSES = UINT64_MAX;

/**
 * @brief Default simulation timestep (1ms)
 */
constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001;

/**
 * @brief Default simulation timestep in steps per second
 */
constexpr uint32_t DEFAULT_TIMESTEPS_PER_SECOND = 1000;

// =====================================================================
// Utility Functions
// =====================================================================

/**
 * @brief Check if a neuron ID is valid
 */
inline bool isValid(NeuronId id) {
    return id.isValid();
}

/**
 * @brief Check if a synapse ID is valid
 */
inline bool isValid(SynapseId id) {
    return id.value != 0 && id.value != UINT64_MAX;
}

/**
 * @brief Format a NeuronId for display
 */
inline std::string formatNeuronId(NeuronId id) {
    return "N" + std::to_string(id.index());
}

/**
 * @brief Format a SynapseId for display
 */
inline std::string formatSynapseId(SynapseId id) {
    return "S" + std::to_string(id.index());
}

/**
 * @brief Format a RegionId for display
 */
inline std::string formatRegionId(RegionId id) {
    return "R" + std::to_string(id.index());
}

/**
 * @brief Format a PopulationId for display
 */
inline std::string formatPopulationId(PopulationId id) {
    return "P" + std::to_string(id.index());
}

/**
 * @brief Get maximum value for a given NeuronType
 */
inline uint32_t getNeuronTypeCount() {
    return static_cast<uint32_t>(NeuronType::Max) + 1;
}

/**
 * @brief Get maximum value for a given SynapseType
 */
inline uint32_t getSynapseTypeCount() {
    return static_cast<uint32_t>(SynapseType::Max) + 1;
}

/**
 * @brief Get maximum value for a given DevelopmentalStage
 */
inline uint32_t getDevelopmentalStageCount() {
    return static_cast<uint32_t>(DevelopmentalStage::Max) + 1;
}

/**
 * @brief Get maximum value for a given FiringState
 */
inline uint32_t getFiringStateCount() {
    return static_cast<uint32_t>(FiringState::Max) + 1;
}

/**
 * @brief Linear interpolation between two values
 */
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

/**
 * @brief Clamp value between min and max
 */
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

/**
 * @brief Convert degree to radians
 */
inline float degreesToRadians(float degrees) {
    return degrees * 3.14159265359f / 180.0f;
}

/**
 * @brief Convert radians to degrees
 */
inline float radiansToDegrees(float radians) {
    return radians * 180.0f / 3.14159265359f;
}

} // namespace nlm

// =====================================================================
// Convenience Aliases
// =====================================================================

// These provide commonly used type combinations for convenience
using nlm::NeuronId;
using nlm::SynapseId;
using nlm::RegionId;
using nlm::PopulationId;
using nlm::SimulationStep;
using nlm::Timestamp;
using nlm::TimestepDuration;
using nlm::NeuronType;
using nlm::SynapseType;
using nlm::PlasticityFlags;
using nlm::DevelopmentalStage;
using nlm::FiringState;
using nlm::SpikeEvent;
using nlm::DelayedSpikeEvent;
using nlm::SynapticWeight;
using nlm::MembranePotential;
using nlm::FiringRate;
using nlm::Delay;

// =====================================================================
// Type Traits for Compile-Time Operations
// =====================================================================

namespace nlm::type_traits {

/**
 * @struct is_identifier_type
 * @brief Type trait to check if a type is a valid identifier type
 */
template<typename T>
struct is_identifier_type : std::false_type {};

template<> struct is_identifier_type<NeuronId> : std::true_type {};
template<> struct is_identifier_type<SynapseId> : std::true_type {};
template<> struct is_identifier_type<RegionId> : std::true_type {};
template<> struct is_identifier_type<PopulationId> : std::true_type {};

/**
 * @struct is_numeric_type
 * @brief Type trait to check if a type is a numeric type
 */
template<typename T>
struct is_numeric_type : std::false_type {};

template<> struct is_numeric_type<int> : std::true_type {};
template<> struct is_numeric_type<int64_t> : std::true_type {};
template<> struct is_numeric_type<double> : std::true_type {};
template<> struct is_numeric_type<float> : std::true_type {};
template<> struct is_numeric_type<NeuronId> : std::true_type {};
template<> struct is_numeric_type<SynapseId> : std::true_type {};

/**
 * @struct is_enum_type
 * @brief Type trait to check if a type is an enum
 */
template<typename T>
struct is_enum_type : std::false_type {};

template<> struct is_enum_type<NeuronType> : std::true_type {};
template<> struct is_enum_type<SynapseType> : std::true_type {};
template<> struct is_enum_type<DevelopmentalStage> : std::true_type {};
template<> struct is_enum_type<FiringState> : std::true_type {};

} // namespace nlm::type_traits

// Convenience macros for type checking
template<typename T>
using enable_if_identifier = std::enable_if_t<nlm::type_traits::is_identifier_type<T>::value, T>;

template<typename T>
using enable_if_numeric = std::enable_if_t<nlm::type_traits::is_numeric_type<T>::value, T>;

template<typename T>
using enable_if_enum = std::enable_if_t<nlm::type_traits::is_enum_type<T>::value, T>;

// End of Types.hpp
