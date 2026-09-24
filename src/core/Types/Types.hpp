#pragma once

#include <cstdint>
#include <type_traits>
#include <functional>
#include <string>
#include <vector>

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

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
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
};

// Enum types
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Internal,
    Unknown
};

enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction,
    Unknown
};

// Firing state enum for neuron states
enum class FiringState {
    Resting,
    Active,
    Refractory
};

// Type aliases for simulation
using TimestepDuration = double;
using SimulationStep = uint64_t;
using Timestamp = double;
using MembranePotential = float;
using FiringRate = float;

// Synapse handle type alias
using SynapseHandle = SynapseId;

// Neuron index type alias
using NeuronIndex = NeuronId;

// Type aliases
using SynapticWeight = float;
using Delay = int;
using Timestamp = double;
using SimulationStep = uint64_t;

// Configuration types
enum class ConfigSource {
    Default,
    User,
    Experiment
};

// Spike event structure
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
};

// Plasticity flags structure
struct PlasticityFlags {
    bool hebbian = false;
    bool stdp = false;
    bool reward_modulated = false;
};

} // namespace nlm

// Hash function specializations for unordered containers
namespace std {
    template<> struct hash<nlm::NeuronId> {
        size_t operator()(const nlm::NeuronId& id) const {
            return hash<uint64_t>()(id.value);
        }
    };
    
    template<> struct hash<nlm::SynapseId> {
        size_t operator()(const nlm::SynapseId& id) const {
            return hash<uint64_t>()(id.value);
        }
    };
    
    template<> struct hash<nlm::RegionId> {
        size_t operator()(const nlm::RegionId& id) const {
            return hash<uint64_t>()(id.value);
        }
    };
    
    template<> struct hash<nlm::PopulationId> {
        size_t operator()(const nlm::PopulationId& id) const {
            return hash<uint64_t>()(id.value);
        }
    };
}