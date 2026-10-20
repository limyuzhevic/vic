#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <map>

namespace nlm {

// Error codes for standardized error reporting
enum class ErrorCode {
    SUCCESS = 0,
    UNKNOWN_ERROR = 1,
    INVALID_ARGUMENT = 2,
    MEMORY_ALLOCATION_FAILED = 3,
    FILE_NOT_FOUND = 4,
    INVALID_FORMAT = 5,
    CONFIGURATION_ERROR = 6,
    NEURAL_NETWORK_ERROR = 7,
    PLASTICITY_ERROR = 8,
    TIMEOUT = 9,
    INVALID_STATE = 10
};

// Error hierarchy
class NLMError : public std::exception {
public:
    NLMError(ErrorCode code, const std::string& message) 
        : code_(code), message_(message) {}
    
    virtual ~NLMError() noexcept = default;
    
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
    
    ErrorCode getCode() const noexcept { return code_; }
    const std::string& getMessage() const noexcept { return message_; }
    
protected:
    ErrorCode code_;
    std::string message_;
};

class RuntimeError : public NLMError {
public:
    RuntimeError(const std::string& message) 
        : NLMError(ErrorCode::UNKNOWN_ERROR, message) {}
    
    RuntimeError(ErrorCode code, const std::string& message)
        : NLMError(code, message) {}
};

class InvalidArgumentError : public NLMError {
public:
    InvalidArgumentError(const std::string& message) 
        : NLMError(ErrorCode::INVALID_ARGUMENT, message) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message) 
        : NLMError(ErrorCode::MEMORY_ALLOCATION_FAILED, message) {}
};

class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message) 
        : NLMError(ErrorCode::CONFIGURATION_ERROR, message) {}

    ConfigurationError(ErrorCode code, const std::string& key, const std::string& expected,
                       const std::string& actual, const std::string& context = "") {
        std::stringstream ss;
        ss << "Configuration error for key \"" << key << "\": expected type "
           << expected << ", got \"" << actual << "\"";
        if (!context.empty()) ss << " (" << context << ")";
        message_ = ss.str();
        code_ = code;
        // Store additional info in message as well for easy access
        std::string extraInfo = " [key=\"" + key + "\", expected=\"" + expected + "\", actual=\"" + actual + "\"]";
        message_ = ss.str() + extraInfo;
    }
};

class NeuralNetworkError : public NLMError {
public:
    NeuralNetworkError(const std::string& message) 
        : NLMError(ErrorCode::NEURAL_NETWORK_ERROR, message) {}
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message) 
        : NLMError(ErrorCode::PLASTICITY_ERROR, message) {}
};

// Enhanced error reporting macro
#define defineNLMError(error, context, operation) \
    error + " during " + operation + " (" + context + ")"

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

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm
