#pragma once

#include <cstdint>
#include <type_traits>

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

// Error handling for NLM operations
enum class ErrorCode {
    Success = 0,
    ComputationOverflow = 1,
    ComputationUnderflow = 2,
    InvalidParameter = 3,
    MemoryAllocationFailed = 4,
    FileIOError = 5,
    ParseError = 6,
    ValidationError = 7,
    SerializationError = 8,
    NetworkError = 9,
    ConcurrencyError = 10,
    ResourceExhausted = 11,
    NotImplemented = 12,
    Unknown = 13
};

struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::string function;
    LogLevel severity;
    int neuronId;
    int synapseId;
    int regionId;
    SimulationStep step;
    Timestamp time;
    
    ErrorInfo() 
        : code(ErrorCode::Unknown), message("Unknown error"), file("")
        , line(0), function(""), severity(LogLevel::Error), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
        
    ErrorInfo(ErrorCode c, const std::string& msg)
        : code(c), message(msg), file("")
        , line(0), function(""), severity(LogLevel::Error), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
        
    ErrorInfo(ErrorCode c, const std::string& msg, LogLevel sev)
        : code(c), message(msg), file("")
        , line(0), function(""), severity(sev), neuronId(-1)
        , synapseId(-1), regionId(-1), step(0), time(0.0) {}
};

// Exception-safe error handling class
class ErrorHandler {
private:
    std::vector<ErrorInfo> errorHistory;
    std::mutex errorMutex;
    static ErrorHandler* instance;
    
public:
    static ErrorHandler& getInstance() {
        if (!instance) {
            instance = new ErrorHandler();
        }
        return *instance;
    }
    
    // Log error with context
    void logError(ErrorCode code, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  LogLevel severity = LogLevel::Error,
                  int neuronId = -1, int synapseId = -1,
                  int regionId = -1,
                  SimulationStep step = 0, Timestamp time = 0.0) {
        std::lock_guard<std::mutex> lock(errorMutex);
        
        ErrorInfo error;
        error.code = code;
        error.message = message;
        error.file = file;
        error.line = line;
        error.function = function;
        error.severity = severity;
        error.neuronId = neuronId;
        error.synapseId = synapseId;
        error.regionId = regionId;
        error.step = step;
        error.time = time;
        
        errorHistory.push_back(error);
        
        // Also log to logger for immediate output
        std::ostringstream oss;
        oss << "[NEURON:" << neuronId << "][SYN:" << synapseId << "] " << message;
        Logger::getGlobal().log(severity, oss.str(), file, line, function);
    }
    
    // Log warning
    void logWarning(ErrorCode code, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "") {
        logError(code, message, file, line, function, LogLevel::Warning);
    }
    
    // Log info
    void logInfo(ErrorCode code, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "") {
        logError(code, message, file, line, function, LogLevel::Info);
    }
    
    // Check if error is recoverable
    bool isRecoverable(ErrorCode code) const {
        switch (code) {
            case ErrorCode::ComputationOverflow:
            case ErrorCode::ComputationUnderflow:
            case ErrorCode::InvalidParameter:
            case ErrorCode::ValidationError:
                return true; // Can recover by reducing load or using defaults
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::ResourceExhausted:
                return false; // Cannot recover without external resources
            case ErrorCode::FileIOError:
                return true; // Can recover with fallback or retry
            case ErrorCode::ParseError:
                return true; // Can recover with defaults
            case ErrorCode::SerializationError:
                return false; // Cannot recover corrupted data
            case ErrorCode::NetworkError:
                return true; // Can recover with retry
            case ErrorCode::ConcurrencyError:
                return true; // Can recover with backoff
            case ErrorCode::Success:
            case ErrorCode::NotImplemented:
            default:
                return false;
        }
    }
    
    // Get error history
    const std::vector<ErrorInfo>& getErrorHistory() const { return errorHistory; }
    
    // Clear error history
    void clearHistory() {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorHistory.clear();
    }
    
    // Check if any critical errors occurred
    bool hasCriticalErrors() const {
        for (const auto& error : errorHistory) {
            if (error.severity == LogLevel::Critical || error.severity == LogLevel::Error) {
                return true;
            }
        }
        return false;
    }
};

} // namespace nlm
