#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

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
    
    // Serialization
    nlohmann::json toJson() const;
    static NeuronId fromJson(const nlohmann::json& j);
    std::string toString() const;
    
    // Factory functions
    static NeuronId make(uint64_t value) { return NeuronId(value); }
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
    
    // Serialization
    nlohmann::json toJson() const;
    static SynapseId fromJson(const nlohmann::json& j);
    std::string toString() const;
    
    // Factory functions
    static SynapseId make(uint64_t value) { return SynapseId(value); }
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
    
    // Serialization
    nlohmann::json toJson() const;
    static RegionId fromJson(const nlohmann::json& j);
    std::string toString() const;
    
    // Factory functions
    static RegionId make(uint64_t value) { return RegionId(value); }
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
    
    // Serialization
    nlohmann::json toJson() const;
    static PopulationId fromJson(const nlohmann::json& j);
    std::string toString() const;
    
    // Factory functions
    static PopulationId make(uint64_t value) { return PopulationId(value); }
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

// Synaptic plasticity state flags
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

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
    
    nlohmann::json toJson() const;
    static SpikeEvent fromJson(const nlohmann::json& j);
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
    
    nlohmann::json toJson() const;
    static DelayedSpikeEvent fromJson(const nlohmann::json& j);
};

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// JSON serialization functions for enums
nlohmann::json toJson(NeuronType type);
NeuronType fromJsonNeuronType(const nlohmann::json& j);

nlohmann::json toJson(SynapseType type);
SynapseType fromJsonSynapseType(const nlohmann::json& j);

nlohmann::json toJson(DevelopmentalStage stage);
DevelopmentalStage fromJsonDevelopmentalStage(const nlohmann::json& j);

nlohmann::json toJson(FiringState state);
FiringState fromJsonFiringState(const nlohmann::json& j);

// Type conversion utilities
std::string toString(const NeuronId& id);
std::string toString(const SynapseId& id);
std::string toString(const RegionId& id);
std::string toString(const PopulationId& id);

// JSON serialization helper functions
template<typename T>
nlohmann::json toJson(const T& value);

template<typename T>
T fromJson(const nlohmann::json& j);

// Factory functions
inline NeuronId makeNeuronId(uint64_t value) { return NeuronId(value); }
inline SynapseId makeSynapseId(uint64_t value) { return SynapseId(value); }
inline RegionId makeRegionId(uint64_t value) { return RegionId(value); }
inline PopulationId makePopulationId(uint64_t value) { return PopulationId(value); }

} // namespace nlm
