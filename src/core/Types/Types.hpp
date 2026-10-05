#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <format>

namespace nlm {

// Template-based identifier system - eliminates code duplication
// while maintaining type safety and Pythonic interfaces

template<typename Derived, typename ValueType = uint64_t>
class TypedId {
public:
    ValueType value;

    // Constructors
    constexpr TypedId() : value(0) {}
    explicit constexpr TypedId(ValueType v) : value(v) {}

    // Comparison operators
    constexpr bool operator==(const TypedId& other) const = default;
    constexpr bool operator!=(const TypedId& other) const = default;
    constexpr bool operator<(const TypedId& other) const { return value < other.value; }
    constexpr bool operator<=(const TypedId& other) const { return value <= other.value; }
    constexpr bool operator>(const TypedId& other) const { return value > other.value; }
    constexpr bool operator>=(const TypedId& other) const { return value >= other.value; }

    // Type conversion
    constexpr operator ValueType() const { return value; }

    // Utility methods
    constexpr ValueType index() const { return value; }
    
    // String representation for Python
    std::string str() const {
        return std::format("<{}: {}>", getTypeName(), value);
    }

    std::string repr() const {
        return std::format("<{} object at {}>", getTypeName(), 
                          static_cast<void*>(this));
    }

    // Type conversion helpers
    static TypedId from_value(ValueType v) {
        return TypedId(v);
    }

protected:
    static std::string getTypeName() {
        std::string name = typeid(Derived).name();
        // Simple demangling for common types
        if (name.find("class nlm::") == 0) {
            name = name.substr(11);
        } else if (name.find("struct nlm::") == 0) {
            name = name.substr(13);
        }
        return name;
    }
};

// Specific ID types using the template - eliminates duplication
using NeuronId = TypedId<NeuronId, uint64_t>;
using SynapseId = TypedId<SynapseId, uint64_t>;
using RegionId = TypedId<RegionId, uint64_t>;
using PopulationId = TypedId<PopulationId, uint64_t>;

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

// Invalid ID sentinel - uses the new TypedId system
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Enhanced enum utilities for Python integration
namespace enums {

    // Enhanced string conversion utilities
    template<typename T>
    std::string enumToString(T value) {
        static_assert(std::is_enum_v<T>, "T must be an enum type");
        return std::format("{}({})", enumClassName<T>(), static_cast<uint8_t>(value));
    }

    template<typename T>
    std::string enumClassName() {
        std::string name = typeid(T).name();
        if (name.find("enum nlm::") == 0) {
            name = name.substr(10);
        } else if (name.find("enum enums::") == 0) {
            name = name.substr(15);
        }
        return name;
    }

    // String to enum conversion with error handling
    template<typename T>
    T stringToEnum(const std::string& name) {
        static_assert(std::is_enum_v<T>, "T must be an enum type");
        // Simple string-to-enum conversion for common values
        if constexpr (std::is_same_v<T, NeuronType>) {
            if (name == "Excitatory") return NeuronType::Excitatory;
            if (name == "Inhibitory") return NeuronType::Inhibitory;
            if (name == "Modulatory") return NeuronType::Modulatory;
            if (name == "Sensory") return NeuronType::Sensory;
            if (name == "Motor") return NeuronType::Motor;
            if (name == "Internal") return NeuronType::Internal;
        } else if constexpr (std::is_same_v<T, SynapseType>) {
            if (name == "Excitatory") return SynapseType::Excitatory;
            if (name == "Inhibitory") return SynapseType::Inhibitory;
            if (name == "Modulatory") return SynapseType::Modulatory;
            if (name == "Electrical") return SynapseType::Electrical;
            if (name == "GapJunction") return SynapseType::GapJunction;
        } else if constexpr (std::is_same_v<T, DevelopmentalStage>) {
            if (name == "Initial") return DevelopmentalStage::Initial;
            if (name == "CriticalPeriod") return DevelopmentalStage::CriticalPeriod;
            if (name == "Maturation") return DevelopmentalStage::Maturation;
            if (name == "Adult") return DevelopmentalStage::Adult;
            if (name == "Aging") return DevelopmentalStage::Aging;
        }
        throw std::invalid_argument("Unknown enum value: " + name);
    }

} // namespace enums

} // namespace nlm
