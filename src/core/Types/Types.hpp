#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety with improved C++20 features

// Neuron identifier with enhanced operations
template<uint64_t MaxValue = UINT64_MAX>
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    constexpr NeuronId(const NeuronId& other) = default;
    constexpr NeuronId& operator=(const NeuronId& other) = default;
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    constexpr bool operator<=(const NeuronId& other) const { return value <= other.value; }
    constexpr bool operator>(const NeuronId& other) const { return value > other.value; }
    constexpr bool operator>=(const NeuronId& other) const { return value >= other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    // Hash support for unordered containers (C++20)
    constexpr bool is_valid() const { return value != 0 && value != MaxValue; }
    constexpr bool is_invalid() const { return !is_valid(); }
};

// Synapse identifier with improved safety
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    constexpr SynapseId(const SynapseId& other) = default;
    constexpr SynapseId& operator=(const SynapseId& other) = default;
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    constexpr bool operator<=(const SynapseId& other) const { return value <= other.value; }
    constexpr bool operator>(const SynapseId& other) const { return value > other.value; }
    constexpr bool operator>=(const SynapseId& other) const { return value >= other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    // Chain comparison operations (C++20)
    template<typename Other>
    constexpr auto operator<=>(const Other& other) const = default;
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    constexpr RegionId(const RegionId& other) = default;
    constexpr RegionId& operator=(const RegionId& other) = default;
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    constexpr bool operator<=(const RegionId& other) const { return value <= other.value; }
    constexpr bool operator>(const RegionId& other) const { return value > other.value; }
    constexpr bool operator>=(const RegionId& other) const { return value >= other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    template<typename Other>
    constexpr auto operator<=>(const Other& other) const = default;
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    constexpr PopulationId(const PopulationId& other) = default;
    constexpr PopulationId& operator=(const PopulationId& other) = default;
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    constexpr bool operator<=(const PopulationId& other) const { return value <= other.value; }
    constexpr bool operator>(const PopulationId& other) const { return value > other.value; }
    constexpr bool operator>=(const PopulationId& other) const { return value >= other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    template<typename Other>
    constexpr auto operator<=>(const Other& other) const = default;
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

// Enhanced typed enums for neuron types with conversion functions
enum class NeuronType : uint8_t {
    Excitatory = 0,
    Inhibitory = 1,
    Modulatory = 2,
    Sensory = 3,
    Motor = 4,
    Internal = 5,
    Invalid = 255
};

// Helper functions for NeuronType
constexpr const char* NeuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Invalid";
    }
}

constexpr bool is_valid_neuron_type(NeuronType type) {
    return static_cast<uint8_t>(type) < 6;
}

// Enhanced typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory = 0,
    Inhibitory = 1,
    Modulatory = 2,
    Electrical = 3,
    GapJunction = 4,
    Invalid = 255
};

constexpr const char* SynapseTypeToString(SynapseType type) {
    switch (type) {
        case SynapseType::Excitatory: return "Excitatory";
        case SynapseType::Inhibitory: return "Inhibitory";
        case SynapseType::Modulatory: return "Modulatory";
        case SynapseType::Electrical: return "Electrical";
        case SynapseType::GapJunction: return "GapJunction";
        default: return "Invalid";
    }
}

constexpr bool is_valid_synapse_type(SynapseType type) {
    return static_cast<uint8_t>(type) < 5;
}

// Enhanced synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
    
    // Check if any plasticity rule is enabled
    constexpr bool has_any_plasticity() const {
        return hebbian || stdp || reward_modulated || structural;
    }
    
    // Enable all plasticity rules
    constexpr void enable_all() {
        hebbian = true;
        stdp = true;
        reward_modulated = true;
        structural = true;
        eligible = true;
    }
};

// Enhanced developmental state with better lifecycle management
enum class DevelopmentalStage : uint8_t {
    Initial = 0,        // Neurogenesis phase
    CriticalPeriod = 1, // Sensitive period for learning
    Maturation = 2,     // Synaptogenesis peak
    Adult = 3,          // Stable network
    Aging = 4           // Decline phase
};

constexpr const char* DevelopmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

constexpr float get_plasticity_modulation(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return 1.0f;    // High plasticity
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;    // Stable
        case DevelopmentalStage::Aging: return 0.1f;    // Low plasticity
        default: return 0.5f;
    }
}

// Enhanced spike event with better memory efficiency and structued bindings support
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
    
    // Factory method for convenience
    static SpikeEvent create(NeuronId neuron, Timestamp time) {
        return SpikeEvent(neuron, time, static_cast<SimulationStep>(time * 1000.0));
    }
};

// Enhanced delayed spike event with better temporal consistency
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
    
    // Factory method with calculated delivery times
    static DelayedSpikeEvent create(NeuronId src, NeuronId dst, SynapseId syn,
                                   SynapticWeight w, SynapseType type,
                                   Timestamp ts, Delay delay, SimulationStep currentStep) {
        Timestamp delivery_time = ts + static_cast<Timestamp>(delay) * 0.001; // Assume 1ms timestep
        SimulationStep delivery_step = currentStep + delay;
        return DelayedSpikeEvent(src, dst, syn, w, type, ts, delivery_time, currentStep, delivery_step);
    }
};

// Enhanced action potential state with better state management
enum class FiringState : uint8_t {
    Resting = 0,
    Active = 1,
    Refractory = 2,
    Inhibited = 3,
    Reset = 4,
    Dead = 5
};

constexpr const char* FiringStateToString(FiringState state) {
    switch (state) {
        case FiringState::Resting: return "Resting";
        case FiringState::Active: return "Active";
        case FiringState::Refractory: return "Refractory";
        case FiringState::Inhibited: return "Inhibited";
        case FiringState::Reset: return "Reset";
        case FiringState::Dead: return "Dead";
        default: return "Unknown";
    }
}

constexpr bool is_active_state(FiringState state) {
    return state == FiringState::Active || state == FiringState::Refractory;
}

// Invalid ID sentinels with improved validation
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Type traits for enhanced type safety
namespace traits {
    template<typename T>
    struct is_identifier : std::false_type {};
    
    template<typename U>
    struct is_identifier<NeuronId<U>> : std::true_type {};
    
    template<typename U>
    struct is_identifier<SynapseId> : std::true_type {};
    
    template<typename U>
    struct is_identifier<RegionId> : std::true_type {};
    
    template<typename U>
    struct is_identifier<PopulationId> : std::true_type {};
    
    // Check if type is an identifier
    template<typename T>
    constexpr bool is_identifier_v = is_identifier<T>::value;
    
    // Get value type from identifier
    template<typename Identifier>
    struct value_type_of {
        using type = typename std::remove_const<decltype(std::declval<Identifier>().value)>::type;
    };
    
    template<typename U>
    using identifier_value_t = typename value_type_of<NeuronId<U>>::type;
}

// Structured bindings helper for spike events
struct SpikeEventWithBinding {
    NeuronId neuron_id;
    Timestamp time;
    SimulationStep step;
    
    SpikeEventWithBinding(const SpikeEvent& spike) 
        : neuron_id(spike.source_neuron), time(spike.timestamp), step(spike.step) {}
    
    // Allow structured bindings for easy access
    auto as_tuple() const {
        return std::tie(neuron_id, time, step);
    }
};

} // namespace nlm