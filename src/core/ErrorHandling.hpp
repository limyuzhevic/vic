#pragma once

#include <string>
#include <system_error>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <vector>
#include <format>

namespace nlm {

// Error categories for NLM system
enum class NLMErrorCategory : uint8_t {
    NoError = 0,
    Configuration = 1,
    Neural = 2,
    Memory = 3,
    Simulation = 4,
    Integration = 5,
    Performance = 6,
    ThreadSafety = 7,
    Resource = 8
};

// Custom error codes
enum class NLMErrorCode : uint32_t {
    // Configuration errors
    InvalidParameter = 0x10000,
    MissingRequiredParameter,
    InvalidRange,
    ConfigFileParseError,
    
    // Neural errors
    NeuronCreationFailed,
    InvalidConnection,
    SimulationStepError,
    PlasticityError,
    
    // Memory errors
    MemoryAllocationFailed,
    MemoryCorruptionDetected,
    MemoryPoolExhausted,
    
    // Simulation errors
    SimulationNotInitialized,
    InvalidTimeStep,
    StepSequenceError,
    
    // Integration errors
    IntegrationFailed,
    SystemNotReady,
    ComponentNotAvailable,
    
    // Performance errors
    PerformanceRegression,
    CacheMissHigh,
    SIMDNotAvailable,
    
    // Thread safety errors
    LockOrderViolation,
    DeadlockDetected,
    ThreadSafetyViolation,
    
    // Resource errors
    ResourceExhausted,
    ResourceLimitExceeded,
    ResourceInUse,
    
    // General errors
    UnknownError,
    NotImplemented,
    InvalidOperation
};

// Exception class hierarchy
class NLMException : public std::runtime_error {
public:
    NLMException(NLMErrorCode code, const std::string& message,
                  const std::string& context = "")
        : std::runtime_error(formatMessage(code, message, context)),
          errorCode(code), errorCategory(determineCategory(code)),
          context(context) {}
    
    NLMErrorCode getErrorCode() const noexcept { return errorCode; }
    NLMErrorCategory getErrorCategory() const noexcept { return errorCategory; }
    const std::string& getContext() const noexcept { return context; }
    
    // Factory methods for common exceptions
    static NLMException invalidParameter(const std::string& param, const std::string& value);
    static NLMException neuralError(const std::string& operation, const std::string& details);
    static NLMException memoryError(const std::string& operation, size_t size);
    static NLMException simulationError(const std::string& operation, const std::string& details);
    static NLMException integrationError(const std::string& system, const std::string& details);
    
protected:
    static std::string formatMessage(NLMErrorCode code, const std::string& message, const std::string& context);
    static NLMErrorCategory determineCategory(NLMErrorCode code);
    
private:
    NLMErrorCode errorCode;
    NLMErrorCategory errorCategory;
    std::string context;
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(NLMErrorCode code, const std::string& message,
                           const std::string& context = "")
        : NLMException(code, message, context) {}
    
    static ConfigurationException invalidParameter(const std::string& param, const std::string& value);
    static ConfigurationException missingParameter(const std::string& param);
    static ConfigurationException invalidRange(const std::string& param, double min, double max, double value);
};

class NeuralException : public NLMException {
public:
    NeuralException(NLMErrorCode code, const std::string& message,
                   const std::string& context = "")
        : NLMException(code, message, context) {}
    
    static NeuralException neuronCreationFailed(NeuronId id, const std::string& reason);
    static NeuralException invalidConnection(NeuronId source, NeuronId destination, const std::string& reason);
    static NeuralException simulationStepError(SimulationStep step, const std::string& reason);
};

class MemoryException : public NLMException {
public:
    MemoryException(NLMErrorCode code, const std::string& message,
                   const std::string& context = "")
        : NLMException(code, message, context) {}
    
    static MemoryException allocationFailed(size_t size, const std::string& context);
    static MemoryException corruptionDetected(const std::string& context);
    static MemoryException poolExhausted(const std::string& poolName, size_t capacity);
};

class SimulationException : public NLMException {
public:
    SimulationException(NLMErrorCode code, const std::string& message,
                       const std::string& context = "")
        : NLMException(code, message, context) {}
    
    static SimulationException notInitialized(const std::string& component);
    static SimulationException invalidTimeStep(TimestepDuration dt, const std::string& reason);
    static SimulationException stepSequenceError(SimulationStep step, const std::string& reason);
};

// Error reporting and handling interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    
    virtual void report(const NLMException& error) = 0;
    virtual void warn(const std::string& message) = 0;
    virtual void info(const std::string& message) = 0;
    
    // Set error handler callback
    virtual void setErrorHandler(std::function<void(const NLMException&)> handler) = 0;
    
    // Get error statistics
    virtual size_t getErrorCount(NLMErrorCategory category) const = 0;
    virtual size_t getTotalErrorCount() const = 0;
    
    // Clear error history
    virtual void clear() = 0;
};

// Global error reporter instance
ErrorReporter& getGlobalErrorReporter();

// Exception safety utilities
class ExceptionSafe {
public:
    template<typename T, typename F>
    static T executeWithRollback(const std::string& operation, F&& func) {
        try {
            return func();
        } catch (const std::exception& e) {
            // Log the error
            getGlobalErrorReporter().warn(std::format("Operation '{}' failed: {}", operation, e.what()));
            
            // Perform rollback if needed
            rollback();
            
            // Re-throw with additional context
            throw NLMException(NLMErrorCode::UnknownError, 
                             std::format("Operation '{}' failed: {}", operation, e.what()),
                             operation);
        }
    }
    
private:
    static void rollback();
};

// Macros for error handling
#define NLM_THROW_ERROR(code, message) \
    throw nlm::NLMException(nlm::NLMErrorCode::code, message, __func__)

#define NLM_THROW_CONFIG_ERROR(code, message) \
    throw nlm::ConfigurationException(nlm::NLMErrorCode::code, message, __func__)

#define NLM_THROW_NEURAL_ERROR(code, message) \
    throw nlm::NeuralException(nlm::NLMErrorCode::code, message, __func__)

#define NLM_THROW_MEMORY_ERROR(code, message) \
    throw nlm::MemoryException(nlm::NLMErrorCode::code, message, __func__)

#define NLM_THROW_SIMULATION_ERROR(code, message) \
    throw nlm::SimulationException(nlm::NLMErrorCode::code, message, __func__)

#define NLM_CHECK_CONDITION(condition, error_code, message) \
    if (!(condition)) { NLM_THROW_ERROR(error_code, message); }

#define NLM_CHECK_CONFIG(condition, error_code, message) \
    if (!(condition)) { NLM_THROW_CONFIG_ERROR(error_code, message); }

#define NLM_CHECK_NEURAL(condition, error_code, message) \
    if (!(condition)) { NLM_THROW_NEURAL_ERROR(error_code, message); }

#define NLM_CHECK_MEMORY(condition, error_code, message) \
    if (!(condition)) { NLM_THROW_MEMORY_ERROR(error_code, message); }

#define NLM_CHECK_SIMULATION(condition, error_code, message) \
    if (!(condition)) { NLM_THROW_SIMULATION_ERROR(error_code, message); }

} // namespace nlm