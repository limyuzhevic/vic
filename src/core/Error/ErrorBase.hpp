// Enhanced error handling and logging system for NLM
// Comprehensive error hierarchy with context, recovery, and structured logging

#ifndef NLM_ERROR_BASE_HPP
#define NLM_ERROR_BASE_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>
#include <optional>
#include <exception>
#include <stdexcept>
#include <algorithm>

namespace nlm {

// Error classification for severity and type
enum class ErrorSeverity {
    Debug,           // Low impact, for debugging
    Info,            // Informational, not an error
    Warning,         // Non-critical issue that needs attention
    Error,           // Critical error affecting functionality
    Critical,        // Severe error, may cause system failure
    Fatal            // Catastrophic error, system may be unusable
};

enum class ErrorType {
    // Application-Level Errors
    SimulationError,        // Simulation related errors
    ConfigurationError,      // Configuration loading/saving errors
    InitializationError,     // Component initialization errors
    ValidationError,         // Parameter validation errors
    
    // Component-Level Errors
    BrainError,              // Brain subsystem errors
    NeuronError,             // Neuron computation errors
    SynapseError,            // Synapse connection errors
    MemoryError,             // Memory system errors
    
    // System-Level Errors
    MemoryAllocationError,   // Memory allocation failures
    FileIOError,             // File system errors
    NetworkError,            // Network communication errors
    ResourceError,           // Resource limitation errors
    
    // User-Level Errors
    InvalidInputError,       // Invalid user input
    UsageError,              // Incorrect usage errors
    PermissionError,         // Access permission errors
    
    // Runtime Errors
    TimeoutError,            // Operation timeout
    ConcurrencyError,        // Threading/concurrency issues
    StateError,              // Invalid state errors
    
    // Generic Errors
    UnknownError,            // Unknown/unexpected errors
    SystemError,             // Generic system errors
    NotImplementedError      // Not yet implemented features
};

// Error context for detailed debugging information
struct ErrorContext {
    std::string component;           // Component where error occurred
    std::string operation;           // Operation that failed
    std::string file;                // Source file
    int line;                        // Line number
    std::string function;            // Function where error occurred
    std::chrono::system_clock::time_point timestamp;
    
    // Additional context data
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> state;       // System state at time of error
    std::map<std::string, std::string> suggestions;  // Recovery suggestions
    
    // Performance context
    std::chrono::milliseconds executionTime;
    size_t memoryUsage;             // Memory usage in bytes
    
    // Error correlation
    std::string correlationId;      // For correlating related errors
    std::string parentErrorId;      // If this is a secondary error
    
    ErrorContext() 
        : line(0), executionTime(0), memoryUsage(0) {}
    
    ErrorContext(const std::string& comp, const std::string& op)
        : component(comp), operation(op), line(0), executionTime(0), memoryUsage(0) {}
};

// Recovery action interface
class RecoveryAction {
public:
    virtual ~RecoveryAction() = default;
    virtual bool execute() = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isAutoApplicable() const { return false; }
    virtual ErrorSeverity getRequiredSeverity() const { return ErrorSeverity::Warning; }
};

// Error recovery strategy
enum class RecoveryStrategy {
    None,                // No recovery possible
    Retry,               // Retry the operation
    Fallback,            // Use fallback configuration/behavior
    Degrade,             // Degrade functionality gracefully
    Restart,             // Restart component/subsystem
    CircuitBreaker,      // Temporarily disable functionality
    AutoHeal,            // Automatic self-healing
    UserIntervention     // Requires user intervention
};

// Structured error with context and recovery
class Error : public std::exception {
public:
    Error(const std::string& message, 
          ErrorType type = ErrorType::UnknownError,
          ErrorSeverity severity = ErrorSeverity::Error,
          const ErrorContext& context = ErrorContext())
        : message_(message)
        , type_(type)
        , severity_(severity)
        , context_(context)
        , timestamp_(std::chrono::system_clock::now())
        , recoveryAttempted_(false)
        , recoverySuccess_(false)
    {
        generateId();
        addStackTrace();
    }

    // Enhanced constructor for component-specific errors
    template<typename... Args>
    Error(ErrorType type, ErrorSeverity severity,
          const std::string& component,
          const std::string& operation,
          Args&&... args)
        : type_(type)
        , severity_(severity)
        , recoveryAttempted_(false)
        , recoverySuccess_(false)
    {
        generateId();
        
        // Format message from arguments
        std::ostringstream oss;
        oss << "[" << component << "] " << operation << ": ";
        formatArgs(oss, std::forward<Args>(args)...);
        message_ = oss.str();
        
        context_.component = component;
        context_.operation = operation;
        context_.timestamp = std::chrono::system_clock::now();
        addStackTrace();
    }

    virtual ~Error() = default;

    // Override what() to provide formatted error message
    virtual const char* what() const noexcept override {
        return getFormattedMessage().c_str();
    }

    // Get formatted message with all context
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[NLM] " << getTypeString() << " (" << getSeverityString() << ") " << message_;
        
        if (!context_.component.empty()) {
            oss << "\n    Component: " << context_.component;
        }
        if (!context_.operation.empty()) {
            oss << "\n    Operation: " << context_.operation;
        }
        if (!context_.correlationId.empty()) {
            oss << "\n    Correlation ID: " << context_.correlationId;
        }
        
        // Add parameter context
        if (!context_.parameters.empty()) {
            oss << "\n    Parameters:";
            for (const auto& [key, value] : context_.parameters) {
                oss << "\n      " << key << " = " << value;
            }
        }
        
        // Add suggestions
        if (!context_.suggestions.empty()) {
            oss << "\n    Suggestions:";
            for (const auto& [key, value] : context_.suggestions) {
                oss << "\n      " << key << ": " << value;
            }
        }
        
        return oss.str();
    }

    // Get human-readable type string
    std::string getTypeString() const {
        static const std::map<ErrorType, std::string> typeStrings = {
            {ErrorType::SimulationError, "SIMULATION"},
            {ErrorType::ConfigurationError, "CONFIGURATION"},
            {ErrorType::InitializationError, "INITIALIZATION"},
            {ErrorType::ValidationError, "VALIDATION"},
            {ErrorType::BrainError, "BRAIN"},
            {ErrorType::NeuronError, "NEURON"},
            {ErrorType::SynapseError, "SYNAPSE"},
            {ErrorType::MemoryError, "MEMORY"},
            {ErrorType::MemoryAllocationError, "MEMORY_ALLOC"},
            {ErrorType::FileIOError, "FILE_IO"},
            {ErrorType::NetworkError, "NETWORK"},
            {ErrorType::ResourceError, "RESOURCE"},
            {ErrorType::InvalidInputError, "INVALID_INPUT"},
            {ErrorType::UsageError, "USAGE"},
            {ErrorType::PermissionError, "PERMISSION"},
            {ErrorType::TimeoutError, "TIMEOUT"},
            {ErrorType::ConcurrencyError, "CONCURRENCY"},
            {ErrorType::StateError, "STATE"},
            {ErrorType::UnknownError, "UNKNOWN"},
            {ErrorType::SystemError, "SYSTEM"},
            {ErrorType::NotImplementedError, "NOT_IMPLEMENTED"}
        };
        
        auto it = typeStrings.find(type_);
        return (it != typeStrings.end()) ? it->second : "UNKNOWN";
    }

    // Get human-readable severity string
    std::string getSeverityString() const {
        static const std::map<ErrorSeverity, std::string> severityStrings = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARNING"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRITICAL"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        
        auto it = severityStrings.find(severity_);
        return (it != severityStrings.end()) ? it->second : "UNKNOWN";
    }

    // Get error ID
    const std::string& getId() const { return id_; }

    // Get error type
    ErrorType getType() const { return type_; }

    // Get error severity
    ErrorSeverity getSeverity() const { return severity_; }

    // Get error context
    const ErrorContext& getContext() const { return context_; }

    // Add additional parameter to context
    void addParameter(const std::string& key, const std::string& value) {
        context_.parameters[key] = value;
    }

    // Add recovery suggestion
    void addSuggestion(const std::string& suggestion, const std::string& category = "general") {
        context_.suggestions[suggestion] = category;
    }

    // Set performance metrics
    void setPerformanceMetrics(const std::chrono::milliseconds& execTime, size_t memUsage) {
        context_.executionTime = execTime;
        context_.memoryUsage = memUsage;
    }

    // Add correlation information
    void setCorrelationId(const std::string& correlationId) {
        context_.correlationId = correlationId;
    }

    // Convert to structured format (JSON-like)
    std::string toStructuredString() const {
        std::ostringstream oss;
        oss << "{\"timestamp\": \"" << getTimestampString() << "\", \"error_id\": \"" << id_ 
            << "\", \"type\": \"" << getTypeString() << "\", \"severity\": \"" << getSeverityString()
            << "\", \"message\": \"" << escapeString(message_) << "\"";
        
        if (!context_.component.empty()) {
            oss << ", \"component\": \"" << context_.component << "\"";
        }
        if (!context_.operation.empty()) {
            oss << ", \"operation\": \"" << context_.operation << "\"";
        }
        
        if (!context_.parameters.empty()) {
            oss << ", \"parameters\": {";
            bool first = true;
            for (const auto& [key, value] : context_.parameters) {
                if (!first) oss << ", ";
                oss << "\"" << escapeString(key) << "\": \"" << escapeString(value) << "\"";
                first = false;
            }
            oss << "}";
        }
        
        oss << "}";
        return oss.str();
    }

    // Check if error can be recovered
    bool canRecover() const {
        return recoveryStrategy_ != RecoveryStrategy::None;
    }

    // Attempt recovery with given action
    bool attemptRecovery(std::shared_ptr<RecoveryAction> action) {
        if (!canRecover()) return false;
        
        recoveryAttempted_ = true;
        recoverySuccess_ = action->execute();
        
        if (recoverySuccess_) {
            addSuggestion("Recovery successful via: " + action->getDescription(), "recovery");
        } else {
            addSuggestion("Recovery failed: " + action->getDescription(), "recovery_failure");
        }
        
        return recoverySuccess_;
    }

    // Get recovery strategy
    RecoveryStrategy getRecoveryStrategy() const { return recoveryStrategy_; }

    // Set recovery strategy
    void setRecoveryStrategy(RecoveryStrategy strategy) { recoveryStrategy_ = strategy; }

    // Get whether recovery was attempted
    bool wasRecoveryAttempted() const { return recoveryAttempted_; }

    // Get whether recovery was successful
    bool wasRecoverySuccessful() const { return recoverySuccess_; }

    // Add stack trace
    void addStackTrace() {
        // Implementation would capture stack trace here
        // For now, just add a placeholder
        context_.state["stack_trace"] = "Stack trace capture not implemented yet";
    }

    // Get timestamp string
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }

    // Escape string for JSON
    std::string escapeString(const std::string& str) const {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }

protected:
    // Format arguments for error message
    template<typename T>
    void formatArgs(std::ostringstream& oss, const T& arg) const {
        oss << arg;
    }

    template<typename T, typename... Args>
    void formatArgs(std::ostringstream& oss, const T& arg, Args&&... args) const {
        oss << arg;
        formatArgs(oss, std::forward<Args>(args)...);
    }

    void generateId() {
        // Generate unique ID (simplified - could use UUID in production)
        static int counter = 0;
        id_ = "ERR_" + std::to_string(++counter) + "_" +
              std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }

private:
    std::string id_;
    std::string message_;
    ErrorType type_;
    ErrorSeverity severity_;
    ErrorContext context_;
    std::chrono::system_clock::time_point timestamp_;
    RecoveryStrategy recoveryStrategy_ = RecoveryStrategy::None;
    bool recoveryAttempted_;
    bool recoverySuccess_;
    std::string stackTrace_;
};

// Template class for component-specific errors
template<ErrorType EType>
class ComponentError : public Error {
public:
    template<typename... Args>
    ComponentError(ErrorSeverity severity, const std::string& component,
                   const std::string& operation, Args&&... args)
        : Error(EType, severity, component, operation, std::forward<Args>(args)...) {}
};

// Specialized error types for common scenarios
class SimulationError : public Error {
public:
    SimulationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                    const ErrorContext& context = ErrorContext())
        : Error(message, ErrorType::SimulationError, severity, context) {}
};

class ConfigurationError : public Error {
public:
    ConfigurationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning,
                      const ErrorContext& context = ErrorContext())
        : Error(message, ErrorType::ConfigurationError, severity, context) {}
};

class InitializationError : public Error {
public:
    InitializationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical,
                       const ErrorContext& context = ErrorContext())
        : Error(message, ErrorType::InitializationError, severity, context) {}
};

class MemoryError : public Error {
public:
    MemoryError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical,
               const ErrorContext& context = ErrorContext())
        : Error(message, ErrorType::MemoryError, severity, context) {}
};

class ValidationError : public Error {
public:
    ValidationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning,
                   const ErrorContext& context = ErrorContext())
        : Error(message, ErrorType::ValidationError, severity, context) {}
};

// Helper functions for common error scenarios
inline ErrorContext createErrorContext(const std::string& component,
                                       const std::string& operation,
                                       const std::string& file = "",
                                       int line = 0,
                                       const std::string& function = "") {
    ErrorContext context(component, operation);
    context.file = file;
    context.line = line;
    context.function = function;
    context.timestamp = std::chrono::system_clock::now();
    return context;
}

// Exception translation helpers
inline void translateError(const std::exception& e, const std::string& component,
                           const std::string& operation) {
    // Convert standard exceptions to our error types
    try {
        throw;
    } catch (const std::bad_alloc&) {
        throw MemoryError("Memory allocation failed", ErrorSeverity::Critical,
                         createErrorContext(component, operation));
    } catch (const std::runtime_error& e) {
        throw Error(std::string("Runtime error: ") + e.what(),
                  ErrorType::SystemError, ErrorSeverity::Error,
                  createErrorContext(component, operation));
    } catch (const std::out_of_range& e) {
        throw ValidationError(std::string("Index out of range: ") + e.what(),
                            ErrorSeverity::Warning,
                            createErrorContext(component, operation));
    } catch (...) {
        throw Error("Unknown exception occurred", ErrorType::UnknownError,
                  ErrorSeverity::Critical, createErrorContext(component, operation));
    }
}

// Error classification for filtering and handling
inline ErrorSeverity classifyError(const Error& error) {
    // Basic classification based on type and severity
    if (error.getType() == ErrorType::MemoryError ||
        error.getType() == ErrorType::MemoryAllocationError) {
        return ErrorSeverity::Critical;
    }
    
    if (error.getType() == ErrorType::ConfigurationError) {
        return ErrorSeverity::Warning;
    }
    
    if (error.getType() == ErrorType::ValidationError) {
        return ErrorSeverity::Warning;
    }
    
    return error.getSeverity();
}

// Error recovery action implementations
class RetryAction : public RecoveryAction {
public:
    RetryAction(int maxRetries, std::chrono::milliseconds delay)
        : maxRetries_(maxRetries), delay_(delay), currentRetry_(0) {}
    
    bool execute() override {
        if (currentRetry_ < maxRetries_) {
            currentRetry_++;
            // Sleep for delay (simplified - would use proper sleep in implementation)
            return true;
        }
        return false;
    }
    
    std::string getDescription() const override {
        return "Retry operation (" + std::to_string(currentRetry_) + "/" + std::to_string(maxRetries_) + ")";
    }
    
    bool isAutoApplicable() const override { return true; }
    
private:
    int maxRetries_;
    std::chrono::milliseconds delay_;
    int currentRetry_;
};

class DegradeAction : public RecoveryAction {
public:
    DegradeAction(const std::string& degradedMode, const std::map<std::string, std::string>& params)
        : degradedMode_(degradedMode), parameters_(params) {}
    
    bool execute() override {
        // Switch to degraded mode (simplified implementation)
        return true;
    }
    
    std::string getDescription() const override {
        return "Switch to degraded mode: " + degradedMode_;
    }
    
    bool isAutoApplicable() const override { return true; }
    
private:
    std::string degradedMode_;
    std::map<std::string, std::string> parameters_;
};

// Error translation macros for common patterns
#define NLM_THROW(type, severity, component, operation, ...) \
    throw type(#component ": " #operation, severity, createErrorContext(component, operation, __FILE__, __LINE__, __func__), ##__VA_ARGS__)

#define NLM_THROW_WITH_CONTEXT(type, severity, context) \
    throw type("Error with context", severity, context)

#define NLM_CHECK(condition, errorType, severity, component, operation, message) \
    if (!(condition)) { \
        throw errorType(message, severity, createErrorContext(component, operation, __FILE__, __LINE__, __func__)); \
    }

// Specialized error types for different components
using SimulationError = nlm::SimulationError;
using ConfigurationError = nlm::ConfigurationError;
using InitializationError = nlm::InitializationError;
using MemoryError = nlm::MemoryError;
using ValidationError = nlm::ValidationError;

} // namespace nlm

#endif // NLM_ERROR_BASE_HPP
