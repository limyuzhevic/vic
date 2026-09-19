#pragma once

#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <cstring>
#include <cassert>
#include <type_traits>

namespace nlm {

// Error code enumeration
enum class ErrorCode {
    // Success
    ERROR_OK = 0,
    
    // Null pointer errors
    ERROR_NULL_POINTER = 1,
    ERROR_NULL_BRAIN = 2,
    ERROR_NULL_NEURON = 3,
    ERROR_NULL_SYNAPSE = 4,
    ERROR_NULL_MEMORY = 5,
    
    // Validation errors
    ERROR_INVALID_PARAMETER = 10,
    ERROR_INVALID_CONFIG = 11,
    ERROR_INVALID_STATE = 12,
    ERROR_INVALID_OPERATION = 13,
    ERROR_OUT_OF_BOUNDS = 14,
    ERROR_NEGATIVE_VALUE = 15,
    
    // Resource errors
    ERROR_OUT_OF_MEMORY = 20,
    ERROR_OUT_OF_CPU = 21,
    ERROR_OUT_OF_GPU = 22,
    ERROR_FILE_NOT_FOUND = 23,
    ERROR_FILE_PERMISSION = 24,
    
    // Runtime errors
    ERROR_DIVISION_BY_ZERO = 30,
    ERROR_OVERFLOW = 31,
    ERROR_UNDERFLOW = 32,
    ERROR_CONVERGENCE_FAILED = 33,
    ERROR_TIMED_OUT = 34,
    ERROR_INTERRUPTED = 35,
    
    // Neural network errors
    ERROR_INVALID_NETWORK = 40,
    ERROR_WEIGHT_MISMATCH = 41,
    ERROR_BRAIN_CREATION_FAILED = 42,
    ERROR_SYNAPSE_CREATION_FAILED = 43,
    ERROR_NEUPRON_ACTIVATION_FAILED = 44,
    
    // System errors
    ERROR_SYSTEM_CALL_FAILED = 50,
    ERROR_OS_NOT_SUPPORTED = 51,
    ERROR_DRIVER_NOT_FOUND = 52,
    ERROR_HARDWARE_NOT_SUPPORTED = 53,
    
    // General errors
    ERROR_UNKNOWN = 99,
    ERROR_NOT_IMPLEMENTED = 100,
    ERROR_DEPRECATED = 101,
    ERROR_UNSUPPORTED = 102
};

// Error severity levels
enum class ErrorSeverity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3,
    Fatal = 4
};

// Source location information
struct SourceLocation {
    const char* file;
    int line;
    const char* function;
    
    SourceLocation(const char* f = nullptr, int l = 0, const char* func = nullptr)
        : file(f), line(l), function(func) {}
    
    std::string toString() const {
        std::stringstream ss;
        if (file) ss << file;
        if (line > 0) ss << ":" << line;
        if (function) ss << " (" << function << ")";
        return ss.str();
    }
};

// Error context
struct ErrorContext {
    SourceLocation location;
    std::string functionName;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> state;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext() : timestamp(std::chrono::system_clock::now()) {}
    
    void addParameter(const std::string& key, const std::string& value) {
        parameters[key] = value;
    }
    
    void addState(const std::string& key, const std::string& value) {
        state[key] = value;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Function: " << functionName << "\n";
        ss << "Location: " << location.toString() << "\n";
        ss << "Parameters:\n";
        for (const auto& p : parameters) {
            ss << "  " << p.first << " = " << p.second << "\n";
        }
        ss << "State:\n";
        for (const auto& s : state) {
            ss << "  " << s.first << " = " << s.second << "\n";
        }
        return ss.str();
    }
};

// Base NLMError class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCode code = ErrorCode::ERROR_UNKNOWN,
             ErrorSeverity severity = ErrorSeverity::Error,
             const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : std::runtime_error(message), 
          code_(code), severity_(severity), location_(location),
          context_(ErrorContext()), caught_at_(std::chrono::system_clock::now()) {
    }
    
    NLMError(const std::string& message, const SourceLocation& location,
             ErrorCode code = ErrorCode::ERROR_UNKNOWN,
             ErrorSeverity severity = ErrorSeverity::Error,
             const std::exception& cause = std::exception())
        : std::runtime_error(message),
          code_(code), severity_(severity), location_(location),
          context_(ErrorContext()), caught_at_(std::chrono::system_clock::now()) {
        if (&cause != &std::exception()) {
            cause_ = cause.what();
        }
    }
    
    // Virtual destructor for proper cleanup
    virtual ~NLMError() noexcept = default;
    
    // Error code accessor
    ErrorCode getCode() const noexcept { return code_; }
    
    // Severity accessor
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    
    // Source location accessor
    SourceLocation getLocation() const noexcept { return location_; }
    
    // Context accessor
    const ErrorContext& getContext() const noexcept { return context_; }
    
    // Timestamp accessor
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return caught_at_; }
    
    // Cause accessor
    const std::string& getCause() const noexcept { return cause_; }
    
    // Whether this error has a cause
    bool hasCause() const noexcept { return !cause_.empty(); }
    
    // Set error context
    void setContext(const ErrorContext& ctx) { context_ = ctx; }
    
    // Add parameter to context
    void addParameter(const std::string& key, const std::string& value) {
        context_.addParameter(key, value);
    }
    
    // Add state to context
    void addState(const std::string& key, const std::string& value) {
        context_.addState(key, value);
    }
    
    // Set cause (for exception chaining)
    void setCause(const std::exception& cause) {
        cause_ = cause.what();
    }
    
    // Generate formatted message for user
    virtual std::string getUserMessage() const {
        std::stringstream ss;
        ss << "Error " << static_cast<int>(code_);
        if (severity_ != ErrorSeverity::Error) {
            ss << " (" << static_cast<int>(severity_) << ")";
        }
        ss << ": " << what();
        return ss.str();
    }
    
    // Generate detailed message for developers
    virtual std::string getDeveloperMessage() const {
        std::stringstream ss;
        ss << "NLMError at " << location_.toString() << "\n";
        ss << "Code: " << static_cast<int>(code_) << "\n";
        ss << "Severity: " << static_cast<int>(severity_) << "\n";
        ss << "Message: " << what() << "\n";
        if (!cause_.empty()) {
            ss << "Cause: " << cause_ << "\n";
        }
        if (!context_.functionName.empty()) {
            ss << "\nContext:" << std::endl;
            ss << context_.toString();
        }
        return ss.str();
    }
    
    // Generate error recovery suggestion
    virtual std::string getRecoverySuggestion() const {
        switch (code_) {
            case ErrorCode::ERROR_NULL_POINTER:
                return "Check if required object was properly initialized before use.";
            case ErrorCode::ERROR_INVALID_PARAMETER:
                return "Validate input parameters before calling function.";
            case ErrorCode::ERROR_OUT_OF_BOUNDS:
                return "Ensure array/vector indices are within valid range.";
            case ErrorCode::ERROR_OUT_OF_MEMORY:
                return "Reduce memory usage or increase available memory.";
            case ErrorCode::ERROR_DIVISION_BY_ZERO:
                return "Check denominator before performing division.";
            case ErrorCode::ERROR_CONVERGENCE_FAILED:
                return "Check learning rate or network architecture. Try increasing iterations.";
            case ErrorCode::ERROR_INVALID_CONFIG:
                return "Verify configuration file syntax and values.";
            case ErrorCode::ERROR_TIMEOUT:
                return "Increase timeout value or optimize performance.";
            default:
                return "Please check the error details above and consult documentation.";
        }
    }
    
    // Serialize error to string
    std::string serialize() const {
        std::stringstream ss;
        ss << "[ERROR]" << std::endl;
        ss << "code:" << static_cast<int>(code_) << std::endl;
        ss << "severity:" << static_cast<int>(severity_) << std::endl;
        ss << "location:" << location_.toString() << std::endl;
        ss << "message:" << what() << std::endl;
        ss << "timestamp:" << std::chrono::system_clock::to_time_t(caught_at_) << std::endl;
        if (!cause_.empty()) {
            ss << "cause:" << cause_ << std::endl;
        }
        return ss.str();
    }
    
    // Deserialize from string (static factory)
    static std::unique_ptr<NLMError> deserialize(const std::string& data) {
        // Simple deserialization - in production would use proper serialization
        auto error = std::make_unique<NLMError>("Deserialized error", ErrorCode::ERROR_UNKNOWN);
        // Parse the data (simplified implementation)
        return error;
    }
    
protected:
    ErrorCode code_;
    ErrorSeverity severity_;
    SourceLocation location_;
    ErrorContext context_;
    std::string cause_;
    std::chrono::system_clock::time_point caught_at_;
};

// Specific error type definitions
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_INVALID_CONFIG, ErrorSeverity::Error, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Check configuration file format and syntax. Ensure required fields are present.";
    }
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_BRAIN_CREATION_FAILED, ErrorSeverity::Error, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Verify brain parameters and ensure neural network architecture is valid.";
    }
};

class NeuronError : public NLMError {
public:
    NeuronError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_NEUPRON_ACTIVATION_FAILED, ErrorSeverity::Error, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Check neuron parameters (thresholds, weights, connections). Ensure proper initialization.";
    }
};

class SynapseError : public NLMError {
public:
    SynapseError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_SYNAPSE_CREATION_FAILED, ErrorSeverity::Error, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Verify synaptic connection parameters and ensure source/target neurons are valid.";
    }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_OUT_OF_MEMORY, ErrorSeverity::Critical, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Reduce memory usage by clearing unused data, or increase system memory.";
    }
};

class NetworkError : public NLMError {
public:
    NetworkError(const std::string& message, const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
        : NLMError(message, ErrorCode::ERROR_INVALID_NETWORK, ErrorSeverity::Error, location) {}
    
    std::string getRecoverySuggestion() const override {
        return "Verify network architecture parameters (layer sizes, connections).";
    }
};

// Error utilities
class ErrorUtils {
public:
    // Get human-readable error code string
    static std::string getErrorCodeString(ErrorCode code) {
        static const std::map<ErrorCode, std::string> codeStrings = {
            {ErrorCode::ERROR_OK, "OK"},
            {ErrorCode::ERROR_NULL_POINTER, "Null Pointer"},
            {ErrorCode::ERROR_NULL_BRAIN, "Null Brain"},
            {ErrorCode::ERROR_NULL_NEURON, "Null Neuron"},
            {ErrorCode::ERROR_NULL_SYNAPSE, "Null Synapse"},
            {ErrorCode::ERROR_NULL_MEMORY, "Null Memory"},
            {ErrorCode::ERROR_INVALID_PARAMETER, "Invalid Parameter"},
            {ErrorCode::ERROR_INVALID_CONFIG, "Invalid Configuration"},
            {ErrorCode::ERROR_INVALID_STATE, "Invalid State"},
            {ErrorCode::ERROR_INVALID_OPERATION, "Invalid Operation"},
            {ErrorCode::ERROR_OUT_OF_BOUNDS, "Out of Bounds"},
            {ErrorCode::ERROR_NEGATIVE_VALUE, "Negative Value"},
            {ErrorCode::ERROR_OUT_OF_MEMORY, "Out of Memory"},
            {ErrorCode::ERROR_OUT_OF_CPU, "Out of CPU Resources"},
            {ErrorCode::ERROR_OUT_OF_GPU, "Out of GPU Resources"},
            {ErrorCode::ERROR_FILE_NOT_FOUND, "File Not Found"},
            {ErrorCode::ERROR_FILE_PERMISSION, "Permission Denied"},
            {ErrorCode::ERROR_DIVISION_BY_ZERO, "Division by Zero"},
            {ErrorCode::ERROR_OVERFLOW, "Overflow"},
            {ErrorCode::ERROR_UNDERFLOW, "Underflow"},
            {ErrorCode::ERROR_CONVERGENCE_FAILED, "Convergence Failed"},
            {ErrorCode::ERROR_TIMED_OUT, "Timeout"},
            {ErrorCode::ERROR_INTERRUPTED, "Interrupted"},
            {ErrorCode::ERROR_INVALID_NETWORK, "Invalid Network"},
            {ErrorCode::ERROR_WEIGHT_MISMATCH, "Weight Mismatch"},
            {ErrorCode::ERROR_BRAIN_CREATION_FAILED, "Brain Creation Failed"},
            {ErrorCode::ERROR_SYNAPSE_CREATION_FAILED, "Synapse Creation Failed"},
            {ErrorCode::ERROR_NEUPRON_ACTIVATION_FAILED, "Neuron Activation Failed"},
            {ErrorCode::ERROR_SYSTEM_CALL_FAILED, "System Call Failed"},
            {ErrorCode::ERROR_OS_NOT_SUPPORTED, "OS Not Supported"},
            {ErrorCode::ERROR_DRIVER_NOT_FOUND, "Driver Not Found"},
            {ErrorCode::ERROR_HARDWARE_NOT_SUPPORTED, "Hardware Not Supported"},
            {ErrorCode::ERROR_UNKNOWN, "Unknown Error"},
            {ErrorCode::ERROR_NOT_IMPLEMENTED, "Not Implemented"},
            {ErrorCode::ERROR_DEPRECATED, "Deprecated"},
            {ErrorCode::ERROR_UNSUPPORTED, "Unsupported"}
        };
        
        auto it = codeStrings.find(code);
        return it != codeStrings.end() ? it->second : "Unknown Error";
    }
    
    // Get human-readable severity string
    static std::string getSeverityString(ErrorSeverity severity) {
        static const std::map<ErrorSeverity, std::string> severityStrings = {
            {ErrorSeverity::Info, "Info"},
            {ErrorSeverity::Warning, "Warning"},
            {ErrorSeverity::Error, "Error"},
            {ErrorSeverity::Critical, "Critical"},
            {ErrorSeverity::Fatal, "Fatal"}
        };
        
        auto it = severityStrings.find(severity);
        return it != severityStrings.end() ? it->second : "Unknown Severity";
    }
    
    // Create error context from parameters
    static ErrorContext createContext(const std::string& functionName) {
        ErrorContext ctx;
        ctx.functionName = functionName;
        return ctx;
    }
    
    // Check if error is recoverable
    static bool isRecoverable(ErrorCode code) {
        switch (code) {
            case ErrorCode::ERROR_OUT_OF_BOUNDS:
            case ErrorCode::ERROR_INVALID_PARAMETER:
            case ErrorCode::ERROR_INVALID_CONFIG:
            case ErrorCode::ERROR_TIMEOUT:
            case ErrorCode::ERROR_INVALID_STATE:
                return true;
            default:
                return false;
        }
    }
    
    // Get default severity for error code
    static ErrorSeverity getDefaultSeverity(ErrorCode code) {
        switch (code) {
            case ErrorCode::ERROR_OK:
            case ErrorCode::ERROR_NOT_IMPLEMENTED:
                return ErrorSeverity::Info;
            case ErrorCode::ERROR_UNKNOWN:
            case ErrorCode::ERROR_DEPRECATED:
            case ErrorCode::ERROR_UNSUPPORTED:
                return ErrorSeverity::Warning;
            case ErrorCode::ERROR_INVALID_PARAMETER:
            case ErrorCode::ERROR_OUT_OF_BOUNDS:
            case ErrorCode::ERROR_TIMEOUT:
                return ErrorSeverity::Error;
            case ErrorCode::ERROR_OUT_OF_MEMORY:
            case ErrorCode::ERROR_OUT_OF_CPU:
            case ErrorCode::ERROR_OUT_OF_GPU:
            case ErrorCode::ERROR_SYSTEM_CALL_FAILED:
                return ErrorSeverity::Critical;
            default:
                return ErrorSeverity::Error;
        }
    }
    
    // Format error for logging
    static std::string formatForLogging(const NLMError& error) {
        std::stringstream ss;
        ss << "[" << getSeverityString(error.getSeverity()) << "]";
        ss << " [" << getErrorCodeString(error.getCode()) << "]";
        ss << " " << error.getDeveloperMessage();
        return ss.str();
    }
    
    // Format error for user display
    static std::string formatForUser(const NLMError& error) {
        std::stringstream ss;
        ss << error.getUserMessage();
        std::string recovery = error.getRecoverySuggestion();
        if (!recovery.empty()) {
            ss << "\n\nSuggestion: " << recovery;
        }
        return ss.str();
    }
};

// Macro for easy error throwing with context
#define NLM_THROW(error_class, message) \
    do { \
        nlm::SourceLocation _loc(__FILE__, __LINE__, __FUNCTION__); \
        throw error_class(message, _loc); \
    } while(0)

#define NLM_THROW_WITH_CODE(error_class, message, code) \
    do { \
        nlm::SourceLocation _loc(__FILE__, __LINE__, __FUNCTION__); \
        throw error_class(message, _loc, code); \
    } while(0)

#define NLM_THROW_WITH_CONTEXT(error_class, message, context) \
    do { \
        nlm::SourceLocation _loc(__FILE__, __LINE__, __FUNCTION__); \
        auto _error = new error_class(message, _loc); \
        _error->setContext(context); \
        throw *_error; \
    } while(0)

// Macro for error checking
#define NLM_CHECK(condition, error_class, message) \
    do { \
        if (!(condition)) { \
            NLM_THROW(error_class, message); \
        } \
    } while(0)

#define NLM_CHECK_WITH_CODE(condition, error_class, message, code) \
    do { \
        if (!(condition)) { \
            NLM_THROW_WITH_CODE(error_class, message, code); \
        } \
    } while(0)

// Exception chaining helper
template<typename T>
void throwWithCause(T&& error, const std::exception& cause) {
    if (std::is_base_of<NLMError, typename std::decay<T>::type>::value) {
        static_cast<NLMError&>(error).setCause(cause);
    }
    throw std::forward<T>(error);
}

} // namespace nlm
