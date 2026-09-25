// Base NLMError class with comprehensive error hierarchy
// Implements the base error class and all specific error subclasses

#pragma once

#include "ErrorCategory.hpp"
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <map>
#include <chrono>
#include <any>

namespace nlm {

// Error context information
struct ErrorContext {
    std::string component;             // Component name (e.g., "Config", "Random", "Brain")
    std::string operation;             // Operation being performed (e.g., "load_config", "generate_random")
    std::string file;                  // Source file where error occurred
    int line;                          // Line number where error occurred
    std::chrono::system_clock::time_point timestamp;  // When error occurred
    std::map<std::string, std::string> metadata;     // Additional metadata
    std::map<std::string, std::any> details;         // Detailed information
    
    ErrorContext() : line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& comp, const std::string& op, 
                 const std::string& file, int line = 0)
        : component(comp), operation(op), file(file), line(line) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Base NLMError class that inherits from std::runtime_error
class NLMError : public std::runtime_error {
protected:
    ErrorCategory category;
    ErrorSeverity severity;
    int errorCode;
    ErrorContext context;
    mutable std::string details;
    std::string recoveryMessage;
    std::map<std::string, std::string> debugInfo;
    mutable bool detailsComputed = false;

public:
    // Base error codes
    static constexpr int SUCCESS = 0;
    
    // Configuration error codes
    static constexpr int CONFIG_INVALID_VALUE = 1000;
    static constexpr int CONFIG_MISSING_FILE = 1001;
    static constexpr int CONFIG_JSON_PARSING = 1002;
    static constexpr int CONFIG_VALIDATION_FAILED = 1003;
    
    // Neural system error codes
    static constexpr int NEURON_NOT_FOUND = 2000;
    static constexpr int INVALID_SYNAPSES = 2001;
    static constexpr int SIMULATION_ERROR = 2002;
    static constexpr int BRAIN_INITIALIZATION_FAILED = 2003;
    
    // IO error codes
    static constexpr int IO_READ_ERROR = 3000;
    static constexpr int IO_WRITE_ERROR = 3001;
    static constexpr int IO_NETWORK_ERROR = 3002;
    static constexpr int FILE_NOT_FOUND = 3003;
    
    // Resource error codes
    static constexpr int RESOURCE_OUT_OF_MEMORY = 4000;
    static constexpr int RESOURCE_INVALID_PARAMS = 4001;
    static constexpr int RESOURCE_TIMEOUT = 4002;
    static constexpr int RESOURCE_QUOTA_EXCEEDED = 4003;
    
    // System error codes
    static constexpr int SYSTEM_INIT_FAILURE = 5000;
    static constexpr int SYSTEM_NOT_IMPLEMENTED = 5001;
    static constexpr int SYSTEM_INTERNAL_ERROR = 5002;
    static constexpr int SYSTEM_UNAVAILABLE = 5003;
    
    // Additional error codes
    static constexpr int VALIDATION_FAILED = 6000;
    static constexpr int MEMORY_ALLOCATION_FAILED = 6001;
    static constexpr int THREADING_ERROR = 6002;
    static constexpr int RANDOM_SEED_ERROR = 6003;

    NLMError(ErrorCategory cat, ErrorSeverity sev, int code,
             const std::string& message,
             const ErrorContext& ctx = ErrorContext())
        : std::runtime_error(message),
          category(cat), severity(sev), errorCode(code), context(ctx) {
        computeMessage();
    }

    virtual ~NLMError() noexcept = default;

    // Accessors
    ErrorCategory getCategory() const noexcept { return category; }
    ErrorSeverity getSeverity() const noexcept { return severity; }
    int getErrorCode() const noexcept { return errorCode; }
    const ErrorContext& getContext() const noexcept { return context; }
    const std::string& getDetails() const { computeMessage(); return details; }
    const std::string& getRecoveryMessage() const { return recoveryMessage; }
    const std::map<std::string, std::string>& getDebugInfo() const { return debugInfo; }

    // Utility methods
    std::string getCategoryString() const;
    std::string getSeverityString() const;
    std::string getFormattedMessage() const;

    // Debug information
    void addDebugInfo(const std::string& key, const std::string& value) {
        debugInfo[key] = value;
    }
    
    template<typename T>
    void addDebugInfo(const std::string& key, T&& value) {
        std::stringstream ss;
        ss << value;
        debugInfo[key] = ss.str();
    }

    // Recovery message
    void setRecoveryMessage(const std::string& msg) {
        recoveryMessage = msg;
    }

    // Check if error can be recovered
    virtual bool isRecoverable() const {
        return severity < ErrorSeverity::FATAL;
    }

protected:
    virtual void computeMessage() const;
};

// Specific error subclasses for each category

// Configuration errors
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message,
                      const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::CONFIG_INVALID_VALUE, ErrorSeverity::ERROR,
                  NLMError::CONFIG_INVALID_VALUE, message, ctx) {}

    ConfigurationError(int code, const std::string& message,
                      const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::CONFIG_INVALID_VALUE, ErrorSeverity::ERROR,
                  code, message, ctx) {}
};

class InvalidConfigValueError : public ConfigurationError {
public:
    InvalidConfigValueError(const std::string& key, 
                          const std::string& expected,
                          const std::string& actual,
                          const ErrorContext& ctx = ErrorContext())
        : ConfigurationError(NLMError::CONFIG_INVALID_VALUE,
                            "Invalid configuration value: key=\"" + key +
                            "\", expected=\"" + expected +
                            "\", actual=\"" + actual + "\"", ctx) {
        addDebugInfo("config_key", key);
        addDebugInfo("expected_value", expected);
        addDebugInfo("actual_value", actual);
        setRecoveryMessage("Check configuration file and correct the value");
    }
};

class MissingConfigFileError : public ConfigurationError {
public:
    MissingConfigFileError(const std::string& filename,
                         const ErrorContext& ctx = ErrorContext())
        : ConfigurationError(NLMError::CONFIG_MISSING_FILE,
                            "Missing configuration file: \"" + filename + "\"", ctx) {
        addDebugInfo("missing_file", filename);
        setRecoveryMessage("Ensure configuration file exists or provide default values");
    }
};

class JSONParsingError : public ConfigurationError {
public:
    JSONParsingError(const std::string& file, const std::string& details,
                    const ErrorContext& ctx = ErrorContext())
        : ConfigurationError(NLMError::CONFIG_JSON_PARSING,
                            "JSON parsing error in file \"" + file + "\": " + details, ctx) {
        addDebugInfo("file", file);
        addDebugInfo("parse_error", details);
        setRecoveryMessage("Fix JSON syntax in configuration file");
    }
};

class ValidationError : public ConfigurationError {
public:
    ValidationError(const std::string& field, const std::string& expected,
                   const std::string& actual,
                   const ErrorContext& ctx = ErrorContext())
        : ConfigurationError(NLMError::CONFIG_VALIDATION_FAILED,
                            "Validation failed for field \"" + field +
                            "\": expected \"" + expected +
                            "\", got \"" + actual + "\"", ctx) {
        addDebugInfo("field", field);
        addDebugInfo("expected", expected);
        addDebugInfo("actual", actual);
        setRecoveryMessage("Correct the configuration field value");
    }
};

// Neural system errors
class NeuralSystemError : public NLMError {
public:
    NeuralSystemError(const std::string& message,
                     const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::NEURON_NOT_FOUND, ErrorSeverity::ERROR,
                  NLMError::NEURON_NOT_FOUND, message, ctx) {}

    NeuralSystemError(int code, const std::string& message,
                     const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::NEURON_NOT_FOUND, ErrorSeverity::ERROR,
                  code, message, ctx) {}
};

class NeuronNotFoundError : public NeuralSystemError {
public:
    NeuronNotFoundError(const std::string& neuronId,
                       const ErrorContext& ctx = ErrorContext())
        : NeuralSystemError("Neuron not found: \"" + neuronId + "\"", ctx) {
        addDebugInfo("neuron_id", neuronId);
        setRecoveryMessage("Create neuron or use existing neuron ID");
    }
};

class InvalidSynapseError : public NeuralSystemError {
public:
    InvalidSynapseError(const std::string& source,
                       const std::string& target,
                       const std::string& reason,
                       const ErrorContext& ctx = ErrorContext())
        : NeuralSystemError("Invalid synapse: " + reason, ctx) {
        addDebugInfo("source_neuron", source);
        addDebugInfo("target_neuron", target);
        addDebugInfo("reason", reason);
        setRecoveryMessage("Check synapse connection rules and valid neuron IDs");
    }
};

class BrainInitializationError : public NeuralSystemError {
public:
    BrainInitializationError(const std::string& reason,
                            const ErrorContext& ctx = ErrorContext())
        : NeuralSystemError(NLMError::BRAIN_INITIALIZATION_FAILED, reason, ctx) {
        setRecoveryMessage("Check brain configuration and resources");
    }
};

// IO errors
class IOError : public NLMError {
public:
    IOError(const std::string& message,
           const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::IO_READ_ERROR, ErrorSeverity::ERROR,
                  NLMError::IO_READ_ERROR, message, ctx) {}

    IOError(int code, const std::string& message,
           const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::IO_READ_ERROR, ErrorSeverity::ERROR,
                  code, message, ctx) {}
};

class FileReadError : public IOError {
public:
    FileReadError(const std::string& filename, const std::string& reason,
                 const ErrorContext& ctx = ErrorContext())
        : IOError("Failed to read file \"" + filename + "\": " + reason, ctx) {
        addDebugInfo("filename", filename);
        addDebugInfo("read_error", reason);
        setRecoveryMessage("Check file permissions and disk space");
    }
};

class FileWriteError : public IOError {
public:
    FileWriteError(const std::string& filename, const std::string& reason,
                 const ErrorContext& ctx = ErrorContext())
        : IOError("Failed to write file \"" + filename + "\": " + reason, ctx) {
        addDebugInfo("filename", filename);
        addDebugInfo("write_error", reason);
        setRecoveryMessage("Check disk space and file permissions");
    }
};

class NetworkError : public IOError {
public:
    NetworkError(const std::string& endpoint, const std::string& reason,
                const ErrorContext& ctx = ErrorContext())
        : IOError("Network error to endpoint \"" + endpoint + "\": " + reason, ctx) {
        addDebugInfo("endpoint", endpoint);
        addDebugInfo("network_error", reason);
        setRecoveryMessage("Check network connectivity and endpoint availability");
    }
};

// Resource errors
class ResourceError : public NLMError {
public:
    ResourceError(const std::string& message,
                 const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::RESOURCE_OUT_OF_MEMORY, ErrorSeverity::ERROR,
                  NLMError::RESOURCE_OUT_OF_MEMORY, message, ctx) {}
};

class OutOfMemoryError : public ResourceError {
public:
    OutOfMemoryError(size_t requestedBytes, size_t availableBytes,
                    const ErrorContext& ctx = ErrorContext())
        : ResourceError("Out of memory: requested " + 
                       std::to_string(requestedBytes) + 
                       " bytes, available " + 
                       std::to_string(availableBytes) + " bytes", ctx) {
        addDebugInfo("requested_bytes", std::to_string(requestedBytes));
        addDebugInfo("available_bytes", std::to_string(availableBytes));
        setRecoveryMessage("Reduce memory usage or increase system memory");
    }
};

class TimeoutError : public ResourceError {
public:
    TimeoutError(const std::string& operation, double timeoutSeconds,
                const ErrorContext& ctx = ErrorContext())
        : ResourceError("Timeout during " + operation + 
                       " (timeout: " + std::to_string(timeoutSeconds) + "s)", ctx) {
        addDebugInfo("operation", operation);
        addDebugInfo("timeout_seconds", std::to_string(timeoutSeconds));
        setRecoveryMessage("Increase timeout value or optimize operation");
    }
};

class QuotaExceededError : public ResourceError {
public:
    QuotaExceededError(const std::string& resource, size_t limit,
                     const ErrorContext& ctx = ErrorContext())
        : ResourceError("Resource quota exceeded for " + resource +
                       " (limit: " + std::to_string(limit) + ")", ctx) {
        addDebugInfo("resource", resource);
        addDebugInfo("limit", std::to_string(limit));
        setRecoveryMessage("Reduce resource usage or increase quota");
    }
};

// System errors
class SystemError : public NLMError {
public:
    SystemError(const std::string& message,
               const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::SYSTEM_INIT_FAILURE, ErrorSeverity::ERROR,
                  NLMError::SYSTEM_INIT_FAILURE, message, ctx) {}
};

class InitializationError : public SystemError {
public:
    InitializationError(const std::string& component,
                       const std::string& reason,
                       const ErrorContext& ctx = ErrorContext())
        : SystemError("Failed to initialize " + component + ": " + reason, ctx) {
        addDebugInfo("component", component);
        addDebugInfo("initialization_error", reason);
        setRecoveryMessage("Check system resources and dependencies");
    }
};

class NotImplementedError : public SystemError {
public:
    NotImplementedError(const std::string& feature,
                       const ErrorContext& ctx = ErrorContext())
        : SystemError("Feature not implemented: " + feature, ctx) {
        addDebugInfo("feature", feature);
        setRecoveryMessage("Implement the required feature or use an alternative");
    }
};

class InternalError : public SystemError {
public:
    InternalError(const std::string& message,
                 const ErrorContext& ctx = ErrorContext())
        : SystemError(NLMError::SYSTEM_INTERNAL_ERROR, message, ctx) {
        setRecoveryMessage("Report this error and check system state");
    }
};

// Validation errors
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::VALIDATION_FAILED, ErrorSeverity::WARNING,
                  NLMError::VALIDATION_FAILED, message, ctx) {}
};

// Memory errors
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::MEMORY_ALLOCATION_FAILED, ErrorSeverity::ERROR,
                  NLMError::MEMORY_ALLOCATION_FAILED, message, ctx) {}
};

// Threading errors
class ThreadingError : public NLMError {
public:
    ThreadingError(const std::string& message,
                  const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::THREADING_ERROR, ErrorSeverity::ERROR,
                  NLMError::THREADING_ERROR, message, ctx) {}
};

// Random number generation errors
class RandomError : public NLMError {
public:
    RandomError(const std::string& message,
               const ErrorContext& ctx = ErrorContext())
        : NLMError(ErrorCategory::RANDOM_SEED_ERROR, ErrorSeverity::ERROR,
                  NLMError::RANDOM_SEED_ERROR, message, ctx) {}
};

// Inline implementations

inline std::string NLMError::getCategoryString() const {
    switch (category) {
        case ErrorCategory::CONFIG_INVALID_VALUE: return "CONFIG_INVALID_VALUE";
        case ErrorCategory::CONFIG_MISSING_FILE: return "CONFIG_MISSING_FILE";
        case ErrorCategory::CONFIG_JSON_PARSING: return "CONFIG_JSON_PARSING";
        case ErrorCategory::CONFIG_VALIDATION_FAILED: return "CONFIG_VALIDATION_FAILED";
        case ErrorCategory::NEURON_NOT_FOUND: return "NEURON_NOT_FOUND";
        case ErrorCategory::INVALID_SYNAPSES: return "INVALID_SYNAPSES";
        case ErrorCategory::SIMULATION_ERROR: return "SIMULATION_ERROR";
        case ErrorCategory::BRAIN_INITIALIZATION_FAILED: return "BRAIN_INITIALIZATION_FAILED";
        case ErrorCategory::IO_READ_ERROR: return "IO_READ_ERROR";
        case ErrorCategory::IO_WRITE_ERROR: return "IO_WRITE_ERROR";
        case ErrorCategory::IO_NETWORK_ERROR: return "IO_NETWORK_ERROR";
        case ErrorCategory::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCategory::RESOURCE_OUT_OF_MEMORY: return "RESOURCE_OUT_OF_MEMORY";
        case ErrorCategory::RESOURCE_INVALID_PARAMS: return "RESOURCE_INVALID_PARAMS";
        case ErrorCategory::RESOURCE_TIMEOUT: return "RESOURCE_TIMEOUT";
        case ErrorCategory::RESOURCE_QUOTA_EXCEEDED: return "RESOURCE_QUOTA_EXCEEDED";
        case ErrorCategory::SYSTEM_INIT_FAILURE: return "SYSTEM_INIT_FAILURE";
        case ErrorCategory::SYSTEM_NOT_IMPLEMENTED: return "SYSTEM_NOT_IMPLEMENTED";
        case ErrorCategory::SYSTEM_INTERNAL_ERROR: return "SYSTEM_INTERNAL_ERROR";
        case ErrorCategory::SYSTEM_UNAVAILABLE: return "SYSTEM_UNAVAILABLE";
        case ErrorCategory::VALIDATION_FAILED: return "VALIDATION_FAILED";
        case ErrorCategory::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
        case ErrorCategory::THREADING_ERROR: return "THREADING_ERROR";
        case ErrorCategory::RANDOM_SEED_ERROR: return "RANDOM_SEED_ERROR";
        case ErrorCategory::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

inline std::string NLMError::getSeverityString() const {
    switch (severity) {
        case ErrorSeverity::INFO: return "INFO";
        case ErrorSeverity::WARNING: return "WARNING";
        case ErrorSeverity::ERROR: return "ERROR";
        case ErrorSeverity::CRITICAL: return "CRITICAL";
        case ErrorSeverity::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

inline std::string NLMError::getFormattedMessage() const {
    std::stringstream ss;
    ss << "[" << getSeverityString() << "] "
       << getCategoryString() << " (#" << errorCode << ")"
       << ": " << what();
    return ss.str();
}

inline void NLMError::computeMessage() const {
    if (detailsComputed) return;
    detailsComputed = true;

    std::stringstream ss;
    ss << "Error in " << context.component << ": " << context.operation;
    if (!context.file.empty()) {
        ss << " (" << context.file;
        if (context.line > 0) ss << ":" << context.line;
        ss << ")";
    }
    ss << ". " << what();
    details = ss.str();
}

} // namespace nlm
