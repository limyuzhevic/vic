#pragma once

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <sstream>

namespace nlm {

// ========== ERROR HANDLING SYSTEM ==========

// Base exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    NLMException(const std::string& component, const std::string& message)
        : std::runtime_error("NLM " + component + ": " + message) {}
};

// Configuration errors
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Config", message) {}
    ConfigException(const std::string& message, const std::string& context)
        : NLMException("Config: " + context + ": " + message) {}
};

// Neural computation errors
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message) : NLMException("Neural", message) {}
    NeuralException(const std::string& message, const std::string& context)
        : NLMException("Neural: " + context + ": " + message) {}
};

// Memory system errors
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("Memory", message) {}
    MemoryException(const std::string& message, const std::string& context)
        : NLMException("Memory: " + context + ": " + message) {}
};

// Neuromodulation errors
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message) : NLMException("Neuromodulation", message) {}
    NeuromodulationException(const std::string& message, const std::string& context)
        : NLMException("Neuromodulation: " + context + ": " + message) {}
};

// System validation errors
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message) : NLMException("Validation", message) {}
    ValidationException(const std::string& message, const std::string& context)
        : NLMException("Validation: " + context + ": " + message) {}
};

// Error context for better debugging
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string details;
    std::vector<std::string> stack_trace;
    
    ErrorContext(const std::string& comp, const std::string& op, const std::string& detail)
        : component(comp), operation(op), details(detail) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Component: " << component << "\n";
        oss << "Operation: " << operation << "\n";
        oss << "Details: " << details << "\n";
        if (!stack_trace.empty()) {
            oss << "Stack trace:\n";
            for (const auto& frame : stack_trace) {
                oss << "  " << frame << "\n";
            }
        }
        return oss.str();
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorContext& context) = 0;
    virtual void handleWarning(const std::string& message) = 0;
    virtual bool shouldThrow(const ErrorContext& context) const = 0;
};

// Default error handler (logs to logger)
class DefaultErrorHandler : public IErrorHandler {
public:
    void handleError(const ErrorContext& context) override {
        NLM_LOG_ERROR("Error in " + context.component + ": " + context.operation + " - " + context.details);
        for (const auto& frame : context.stack_trace) {
            NLM_LOG_ERROR("  " + frame);
        }
    }
    
    void handleWarning(const std::string& message) override {
        NLM_LOG_WARNING(message);
    }
    
    bool shouldThrow(const ErrorContext& context) const override {
        // Only throw critical errors
        return context.component == "Validation" || context.operation.find("critical") != std::string::npos;
    }
};

// Global error handler instance
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        errorHandler_ = handler;
    }
    
    void handleError(const ErrorContext& context) {
        if (errorHandler_) {
            errorHandler_->handleError(context);
            if (errorHandler_->shouldThrow(context)) {
                throw NLMException("Critical error in " + context.component + ": " + context.details);
            }
        }
    }
    
    void handleWarning(const std::string& message) {
        if (errorHandler_) {
            errorHandler_->handleWarning(message);
        }
    }
    
private:
    ErrorManager() {
        errorHandler_ = std::make_shared<DefaultErrorHandler>();
    }
    
    std::shared_ptr<IErrorHandler> errorHandler_;
};

// Error recovery result
enum class ErrorRecoveryResult {
    Success,
    RetryNeeded,
    CriticalFailure
};

// Error recovery interface
class IErrorRecovery {
public:
    virtual ~IErrorRecovery() = default;
    virtual ErrorRecoveryResult recover(const ErrorContext& context) = 0;
};

// Convenience functions for error handling
namespace Error {
    inline void throwWithContext(const std::string& component, const std::string& operation, const std::string& message) {
        throw NLMException(component, operation + ": " + message);
    }
    
    inline void logWithContext(const std::string& component, const std::string& operation, const std::string& message) {
        NLM_LOG_ERROR("Error in " + component + ": " + operation + " - " + message);
    }
    
    inline ErrorContext createContext(const std::string& component, const std::string& operation, const std::string& details) {
        return ErrorContext(component, operation, details);
    }
    
    inline void checkCondition(bool condition, const std::string& component, const std::string& operation, const std::string& message) {
        if (!condition) {
            throwWithContext(component, operation, message);
        }
    }
    
    inline void validateRange(double value, double min, double max, const std::string& component, const std::string& operation) {
        if (value < min || value > max) {
            throw ValidationException("Value " + std::to_string(value) + " outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]",
                                     component + " " + operation);
        }
    }
}

// ========== END ERROR HANDLING SYSTEM ==========

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
