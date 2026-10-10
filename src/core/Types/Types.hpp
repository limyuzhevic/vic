#pragma once

#include <cstdint>
#include <type_traits>
#include <functional>  // Added for std::hash

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    // Postfix increment
    constexpr NeuronId operator++(int) { return NeuronId{value++}; }
    // Prefix increment  
    constexpr NeuronId& operator++() { return ++value, *this; }
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
    
    // Postfix increment
    constexpr SynapseId operator++(int) { return SynapseId{value++}; }
    // Prefix increment
    constexpr SynapseId& operator++() { return ++value, *this; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    // Postfix increment
    constexpr RegionId operator++(int) { return RegionId{value++}; }
    // Prefix increment
    constexpr RegionId& operator++() { return ++value, *this; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
    
    // Postfix increment
    constexpr PopulationId operator++(int) { return PopulationId{value++}; }
    // Prefix increment
    constexpr PopulationId& operator++() { return ++value, *this; }
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

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Typed enums for developmental stages
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Typed enums for firing states
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Plasticity flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
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
    template<>
    struct hash<nlm::NeuronId> {
        size_t operator()(const nlm::NeuronId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };

    template<>
    struct hash<nlm::SynapseId> {
        size_t operator()(const nlm::SynapseId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };

    template<>
    struct hash<nlm::RegionId> {
        size_t operator()(const nlm::RegionId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };

    template<>
    struct hash<nlm::PopulationId> {
        size_t operator()(const nlm::PopulationId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };

    template<>
    struct hash<nlm::NeuronType> {
        size_t operator()(const nlm::NeuronType& v) const {
            return std::hash<uint8_t>{}(static_cast<uint8_t>(v));
        }
    };

    template<>
    struct hash<nlm::SynapseType> {
        size_t operator()(const nlm::SynapseType& v) const {
            return std::hash<uint8_t>{}(static_cast<uint8_t>(v));
        }
    };

    template<>
    struct hash<nlm::DevelopmentalStage> {
        size_t operator()(const nlm::DevelopmentalStage& v) const {
            return std::hash<uint8_t>{}(static_cast<uint8_t>(v));
        }
    };

    template<>
    struct hash<nlm::FiringState> {
        size_t operator()(const nlm::FiringState& v) const {
            return std::hash<uint8_t>{}(static_cast<uint8_t>(v));
        }
    };
}

// Convenience hash_combine function for std::unordered_map
inline size_t hash_combine(size_t seed, const nlm::NeuronId& id) {
    return seed ^ (std::hash<uint64_t>{}(id.value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::SynapseId& id) {
    return seed ^ (std::hash<uint64_t>{}(id.value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::RegionId& id) {
    return seed ^ (std::hash<uint64_t>{}(id.value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::PopulationId& id) {
    return seed ^ (std::hash<uint64_t>{}(id.value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::NeuronType& v) {
    return seed ^ (std::hash<uint8_t>{}(static_cast<uint8_t>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::SynapseType& v) {
    return seed ^ (std::hash<uint8_t>{}(static_cast<uint8_t>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::DevelopmentalStage& v) {
    return seed ^ (std::hash<uint8_t>{}(static_cast<uint8_t>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(size_t seed, const nlm::FiringState& v) {
    return seed ^ (std::hash<uint8_t>{}(static_cast<uint8_t>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}
