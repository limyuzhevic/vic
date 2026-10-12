// Error Types for the NLM (Neural Learning Machine) System
// This file defines comprehensive error classification and severity levels

#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>

namespace nlm {

// Error Severity Levels
enum class ErrorSeverity {
    Debug = 0,        // Debugging information
    Info = 1,          // Informational messages
    Warning = 2,       // Warning conditions
    Error = 3,         // Error conditions
    Critical = 4,      // Critical errors requiring immediate attention
    Fatal = 5          // Fatal errors causing system failure
};

// Error Classification Types
enum class ErrorType {
    // System Errors
    SystemArchitecture = 0,    // System architecture issues
    MemoryAllocation = 1,      // Memory allocation failures
    ResourceExhaustion = 2,    // Resource exhaustion (CPU, memory, file handles)
    ThreadSafety = 3,          // Thread safety violations
    Deadlock = 4,              // Deadlock conditions
    
    // Configuration Errors
    ConfigurationInvalid = 5,  // Invalid configuration values
    ConfigurationMissing = 6,  // Missing required configuration
    ConfigurationParse = 7,    // Configuration parsing errors
    EnvironmentNotFound = 8,   // Environment configuration not found
    
    // Logic Errors
    LogicInvalid = 9,          // Invalid logical operations
    AssertionFailed = 10,      // Assertion failures
    InvariantBroken = 11,      // Broken invariants
    
    // Resource Errors
    FileNotFound = 12,         // File not found
    PermissionDenied = 13,     // Permission denied
    NetworkConnection = 14,    // Network connection issues
    Timeout = 15,              // Operation timeout
    
    // Neural Network Errors
    NeuronInvalid = 16,        // Invalid neuron operations
    SynapseInvalid = 17,       // Invalid synapse operations
    PopulationInvalid = 18,    // Invalid population operations
    RegionInvalid = 19,        // Invalid region operations
    
    // Memory System Errors
    WorkingMemoryFull = 20,    // Working memory capacity exceeded
    EpisodicMemoryCorrupted = 21, // Episodic memory corruption
    AssociativeMemoryInvalid = 22, // Associative memory errors
    
    // Prediction/Cognition Errors
    PredictionFailed = 23,     // Prediction system failures
    PlanningInvalid = 24,      // Planning system errors
    ConceptInvalid = 25,       // Concept formation errors
    AttentionInvalid = 26,     // Attention system errors
    
    // Neuromodulation Errors
    NeuromodulatorInvalid = 27, // Neuromodulator errors
    RewardInvalid = 28,         // Reward system errors
    PredictionErrorInvalid = 29, // Prediction error signal errors
    
    // Developmental Errors
    DevelopmentInvalid = 30,   // Developmental system errors
    MaturationInvalid = 31,     // Maturation phase errors
    SynaptogenesisInvalid = 32, // Synaptogenesis errors
    PruningInvalid = 33,       // Pruning errors
    
    // Performance Errors
    PerformanceDegradation = 34, // Performance degradation
    MemoryLeak = 35,            // Memory leaks detected
    CPUUsageHigh = 36,          // High CPU usage
    MemoryUsageHigh = 37,       // High memory usage
    
    // Monitoring Errors
    MonitoringSystemFailed = 38, // Monitoring system failures
    AlertingFailed = 39,         // Alerting system failures
    HealthCheckFailed = 40,      // Health check failures
    
    // Validation Errors
    InputInvalid = 41,          // Invalid input values
    InputSanitizationFailed = 42, // Input sanitization failed
    ValidationSchemaInvalid = 43, // Schema validation failed
    
    // Recovery Errors
    RecoveryFailed = 44,        // Error recovery failed
    RollbackFailed = 45,        // State rollback failed
    CheckpointInvalid = 46,     // Checkpoint operations failed
    
    // Integration Errors
    ExternalSystemFailed = 47,  // External system integration errors
    PluginLoadFailed = 48,      // Plugin loading errors
    ExtensionFailed = 49,       // Extension failures
    
    // Unknown/Error
    Unknown = 50,               // Unknown error type
    Uncategorized = 51          // Uncategorized errors
};

// Error Classification Function
ErrorType classifyError(const std::exception& e) noexcept;

// Error Classification from string
ErrorType errorTypeFromString(const std::string& typeStr) noexcept;

// Get error type string representation
std::string toString(ErrorType type) noexcept;

// Error Codes for specific error types (typedef for compatibility)
typedef int ErrorCode;

// Error Code mappings
namespace ErrorCodes {
    // System error codes
    static const ErrorCode SYSTEM_ARCHITECTURE = 1000;
    static const ErrorCode MEMORY_ALLOCATION = 1001;
    static const ErrorCode RESOURCE_EXHAUSTION = 1002;
    static const ErrorCode THREAD_SAFETY = 1003;
    static const ErrorCode DEADLOCK = 1004;
    
    // Configuration error codes
    static const ErrorCode CONFIGURATION_INVALID = 2000;
    static const ErrorCode CONFIGURATION_MISSING = 2001;
    static const ErrorCode CONFIGURATION_PARSE = 2002;
    static const ErrorCode ENVIRONMENT_NOT_FOUND = 2003;
    
    // Logic error codes
    static const ErrorCode LOGIC_INVALID = 3000;
    static const ErrorCode ASSERTION_FAILED = 3001;
    static const ErrorCode INVARIANT_BROKEN = 3002;
    
    // Resource error codes
    static const ErrorCode FILE_NOT_FOUND = 4000;
    static const ErrorCode PERMISSION_DENIED = 4001;
    static const ErrorCode NETWORK_CONNECTION = 4002;
    static const ErrorCode TIMEOUT = 4003;
    
    // Neural network error codes
    static const ErrorCode NEURON_INVALID = 5000;
    static const ErrorCode SYNAPSE_INVALID = 5001;
    static const ErrorCode POPULATION_INVALID = 5002;
    static const ErrorCode REGION_INVALID = 5003;
    
    // Memory system error codes
    static const ErrorCode WORKING_MEMORY_FULL = 6000;
    static const ErrorCode EPISODIC_MEMORY_CORRUPTED = 6001;
    static const ErrorCode ASSOCIATIVE_MEMORY_INVALID = 6002;
    
    // Performance error codes
    static const ErrorCode PERFORMANCE_DEGRADATION = 8000;
    static const ErrorCode MEMORY_LEAK = 8001;
    static const ErrorCode CPU_USAGE_HIGH = 8002;
    static const ErrorCode MEMORY_USAGE_HIGH = 8003;
    
    // Recovery error codes
    static const ErrorCode RECOVERY_FAILED = 9000;
    static const ErrorCode ROLLBACK_FAILED = 9001;
    static const ErrorCode CHECKPOINT_INVALID = 9002;
    
    // Unknown error code
    static const ErrorCode UNKNOWN = 9999;
}

// Error Context Information Structure
struct ErrorContext {
    ErrorType type;
    ErrorSeverity severity;
    ErrorCode code;
    std::string message;
    std::string sourceFile;
    std::string sourceFunction;
    int sourceLine;
    std::string stackTrace;
    std::string correlationId;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    ErrorContext() 
        : type(ErrorType::Unknown), 
          severity(ErrorSeverity::Error),
          code(ErrorCodes::UNKNOWN),
          sourceFile(""),
          sourceFunction(""),
          sourceLine(0),
          correlationId("") {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(ErrorType t, ErrorSeverity s, ErrorCode c, const std::string& msg)
        : type(t), severity(s), code(c), message(msg),
          sourceFile(""), sourceFunction(""), sourceLine(0),
          correlationId("") {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string getFormattedMessage() const;
    std::string getLocationInfo() const;
    std::string getMetadataString() const;
    bool isRecoverable() const;
    ErrorSeverity getEffectiveSeverity() const;
};

// Error Handling Result Structure
struct ErrorHandlingResult {
    bool success;
    std::string message;
    std::vector<ErrorContext> errors;
    std::vector<ErrorContext> warnings;
    
    static ErrorHandlingResult success() {
        return {true, "Operation completed successfully", {}, {}};
    }
    
    static ErrorHandlingResult failure(const std::string& msg) {
        ErrorContext ctx(ErrorType::Unknown, ErrorSeverity::Error, ErrorCodes::UNKNOWN, msg);
        return {false, msg, {ctx}, {}};
    }
    
    static ErrorHandlingResult failure(const ErrorContext& error) {
        return {false, error.message, {error}, {}};
    }
    
    ErrorHandlingResult addError(const ErrorContext& error) {
        errors.push_back(error);
        success = false;
        message = "Operation completed with errors";
        return *this;
    }
    
    ErrorHandlingResult addWarning(const ErrorContext& warning) {
        warnings.push_back(warning);
        message = "Operation completed with warnings";
        return *this;
    }
    
    std::string getSummary() const;
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Error Handling Policy
enum class ErrorHandlingPolicy {
    Strict,      // Fail fast on any error
    Graceful,    // Continue with error recovery
    Ignore,      // Ignore errors and continue
    Retry,       // Retry operation on failure
    Fallback     // Fall back to safe defaults
};

// Exception Safety Template
enum class ExceptionSafetyLevel {
    Basic,       // Basic exception safety (strong guarantee for destructors)
    Strong,      // Strong exception safety (no resource leaks)
    NoThrow,     // No-throw guarantee for critical operations
    Transaction  // Transaction-like operations (rollback on exception)
};

// Error Handler Interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    virtual void handleError(const ErrorContext& error) = 0;
    virtual void handleWarning(const ErrorContext& warning) = 0;
    virtual ErrorHandlingResult handleError(const ErrorHandlingResult& result) = 0;
    virtual bool shouldRethrow(const ErrorContext& error) const = 0;
    
    virtual void setErrorPolicy(ErrorHandlingPolicy policy) = 0;
    virtual ErrorHandlingPolicy getErrorPolicy() const = 0;
    
    virtual void setMaxRetries(int maxRetries) = 0;
    virtual int getMaxRetries() const = 0;
    
    virtual bool isHealthy() const = 0;
    virtual void reset() = 0;
};

// Default Error Handler
class DefaultErrorHandler : public ErrorHandler {
private:
    ErrorHandlingPolicy policy;
    int maxRetries;
    std::mutex mutex;
    
public:
    DefaultErrorHandler()
        : policy(ErrorHandlingPolicy::Graceful), maxRetries(3) {}
    
    DefaultErrorHandler(ErrorHandlingPolicy p, int retries = 3)
        : policy(p), maxRetries(retries) {}
    
    void handleError(const ErrorContext& error) override {
        std::lock_guard<std::mutex> lock(mutex);
        // Log error based on severity
        if (error.severity >= ErrorSeverity::Warning) {
            // Log error using the logging system
        }
    }
    
    void handleWarning(const ErrorContext& warning) override {
        std::lock_guard<std::mutex> lock(mutex);
        // Log warning
    }
    
    ErrorHandlingResult handleError(const ErrorHandlingResult& result) override {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& error : result.errors) {
            handleError(error);
        }
        for (const auto& warning : result.warnings) {
            handleWarning(warning);
        }
        return result;
    }
    
    bool shouldRethrow(const ErrorContext& error) const override {
        return error.severity >= ErrorSeverity::Critical;
    }
    
    void setErrorPolicy(ErrorHandlingPolicy p) override {
        std::lock_guard<std::mutex> lock(mutex);
        policy = p;
    }
    
    ErrorHandlingPolicy getErrorPolicy() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return policy;
    }
    
    void setMaxRetries(int retries) override {
        std::lock_guard<std::mutex> lock(mutex);
        maxRetries = retries;
    }
    
    int getMaxRetries() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return maxRetries;
    }
    
    bool isHealthy() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return policy != ErrorHandlingPolicy::Ignore;
    }
    
    void reset() override {
        std::lock_guard<std::mutex> lock(mutex);
        policy = ErrorHandlingPolicy::Graceful;
        maxRetries = 3;
    }
};

// Global Error Handler Management
class ErrorManager {
private:
    static std::unique_ptr<ErrorHandler> defaultHandler;
    static std::mutex mutex;
    
public:
    static void setDefaultHandler(std::unique_ptr<ErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        defaultHandler = std::move(handler);
    }
    
    static ErrorHandler* getDefaultHandler() {
        std::lock_guard<std::mutex> lock(mutex);
        return defaultHandler.get();
    }
    
    static void reportError(const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (defaultHandler) {
            defaultHandler->handleError(error);
        }
    }
    
    static void reportWarning(const ErrorContext& warning) {
        std::lock_guard<std::mutex> lock(mutex);
        if (defaultHandler) {
            defaultHandler->handleWarning(warning);
        }
    }
    
    static ErrorHandlingResult reportError(const ErrorHandlingResult& result) {
        std::lock_guard<std::mutex> lock(mutex);
        if (defaultHandler) {
            return defaultHandler->handleError(result);
        }
        return result;
    }
    
    static std::string generateCorrelationId();
    static ErrorContext createErrorContext(ErrorType type, ErrorSeverity severity,
                                           ErrorCode code, const std::string& message,
                                           const std::string& sourceFile = "",
                                           const std::string& sourceFunction = "",
                                           int sourceLine = 0);
};

// Error Classification Functions Implementation
inline ErrorType classifyError(const std::exception& e) noexcept {
    std::string what = e.what();
    
    // Check for known error patterns
    if (what.find("memory") != std::string::npos || 
        what.find("Memory") != std::string::npos) {
        return ErrorType::MemoryAllocation;
    }
    else if (what.find("file") != std::string::npos || 
             what.find("File") != std::string::npos) {
        return ErrorType::FileNotFound;
    }
    else if (what.find("permission") != std::string::npos) {
        return ErrorType::PermissionDenied;
    }
    else if (what.find("timeout") != std::string::npos) {
        return ErrorType::Timeout;
    }
    else if (what.find("connection") != std::string::npos) {
        return ErrorType::NetworkConnection;
    }
    else if (what.find("assert") != std::string::npos) {
        return ErrorType::AssertionFailed;
    }
    
    return ErrorType::Unknown;
}

inline ErrorType errorTypeFromString(const std::string& typeStr) noexcept {
    static const std::unordered_map<std::string, ErrorType> typeMap = {
        {"system", ErrorType::SystemArchitecture},
        {"memory", ErrorType::MemoryAllocation},
        {"resource", ErrorType::ResourceExhaustion},
        {"thread", ErrorType::ThreadSafety},
        {"config", ErrorType::ConfigurationInvalid},
        {"logic", ErrorType::LogicInvalid},
        {"file", ErrorType::FileNotFound},
        {"network", ErrorType::NetworkConnection},
        {"neuron", ErrorType::NeuronInvalid},
        {"synapse", ErrorType::SynapseInvalid},
        {"performance", ErrorType::PerformanceDegradation},
        {"recovery", ErrorType::RecoveryFailed},
        {"validation", ErrorType::InputInvalid}
    };
    
    auto it = typeMap.find(typeStr);
    return it != typeMap.end() ? it->second : ErrorType::Unknown;
}

inline std::string toString(ErrorType type) noexcept {
    switch (type) {
        case ErrorType::SystemArchitecture: return "System Architecture";
        case ErrorType::MemoryAllocation: return "Memory Allocation";
        case ErrorType::ResourceExhaustion: return "Resource Exhaustion";
        case ErrorType::ThreadSafety: return "Thread Safety";
        case ErrorType::Deadlock: return "Deadlock";
        case ErrorType::ConfigurationInvalid: return "Configuration Invalid";
        case ErrorType::ConfigurationMissing: return "Configuration Missing";
        case ErrorType::ConfigurationParse: return "Configuration Parse";
        case ErrorType::EnvironmentNotFound: return "Environment Not Found";
        case ErrorType::LogicInvalid: return "Logic Invalid";
        case ErrorType::AssertionFailed: return "Assertion Failed";
        case ErrorType::InvariantBroken: return "Invariant Broken";
        case ErrorType::FileNotFound: return "File Not Found";
        case ErrorType::PermissionDenied: return "Permission Denied";
        case ErrorType::NetworkConnection: return "Network Connection";
        case ErrorType::Timeout: return "Timeout";
        case ErrorType::NeuronInvalid: return "Neuron Invalid";
        case ErrorType::SynapseInvalid: return "Synapse Invalid";
        case ErrorType::PopulationInvalid: return "Population Invalid";
        case ErrorType::RegionInvalid: return "Region Invalid";
        case ErrorType::WorkingMemoryFull: return "Working Memory Full";
        case ErrorType::EpisodicMemoryCorrupted: return "Episodic Memory Corrupted";
        case ErrorType::AssociativeMemoryInvalid: return "Associative Memory Invalid";
        case ErrorType::PredictionFailed: return "Prediction Failed";
        case ErrorType::PlanningInvalid: return "Planning Invalid";
        case ErrorType::ConceptInvalid: return "Concept Invalid";
        case ErrorType::AttentionInvalid: return "Attention Invalid";
        case ErrorType::NeuromodulatorInvalid: return "Neuromodulator Invalid";
        case ErrorType::RewardInvalid: return "Reward Invalid";
        case ErrorType::PredictionErrorInvalid: return "Prediction Error Invalid";
        case ErrorType::DevelopmentInvalid: return "Development Invalid";
        case ErrorType::MaturationInvalid: return "Maturation Invalid";
        case ErrorType::SynaptogenesisInvalid: return "Synaptogenesis Invalid";
        case ErrorType::PruningInvalid: return "Pruning Invalid";
        case ErrorType::PerformanceDegradation: return "Performance Degradation";
        case ErrorType::MemoryLeak: return "Memory Leak";
        case ErrorType::CPUUsageHigh: return "CPU Usage High";
        case ErrorType::MemoryUsageHigh: return "Memory Usage High";
        case ErrorType::MonitoringSystemFailed: return "Monitoring System Failed";
        case ErrorType::AlertingFailed: return "Alerting Failed";
        case ErrorType::HealthCheckFailed: return "Health Check Failed";
        case ErrorType::InputInvalid: return "Input Invalid";
        case ErrorType::InputSanitizationFailed: return "Input Sanitization Failed";
        case ErrorType::ValidationSchemaInvalid: return "Validation Schema Invalid";
        case ErrorType::RecoveryFailed: return "Recovery Failed";
        case ErrorType::RollbackFailed: return "Rollback Failed";
        case ErrorType::CheckpointInvalid: return "Checkpoint Invalid";
        case ErrorType::ExternalSystemFailed: return "External System Failed";
        case ErrorType::PluginLoadFailed: return "Plugin Load Failed";
        case ErrorType::ExtensionFailed: return "Extension Failed";
        case ErrorType::Unknown: return "Unknown";
        case ErrorType::Uncategorized: return "Uncategorized";
        default: return "Unknown";
    }
}

inline std::string ErrorContext::getFormattedMessage() const {
    std::string result = "[" + std::to_string(static_cast<int>(type)) + "]";
    result += " [" + std::to_string(static_cast<int>(severity)) + "]";
    result += " " + message;
    
    if (!sourceFile.empty()) {
        result += "\n  at " + sourceFile;
        if (!sourceFunction.empty()) {
            result += "::" + sourceFunction;
        }
        if (sourceLine > 0) {
            result += ":" + std::to_string(sourceLine);
        }
    }
    
    return result;
}

inline std::string ErrorContext::getLocationInfo() const {
    std::string result;
    if (!sourceFile.empty()) {
        result += sourceFile;
        if (!sourceFunction.empty()) {
            result += "::" + sourceFunction;
        }
        if (sourceLine > 0) {
            result += ":" + std::to_string(sourceLine);
        }
    }
    return result;
}

inline std::string ErrorContext::getMetadataString() const {
    std::string result;
    bool first = true;
    for (const auto& pair : metadata) {
        if (!first) result += ", ";
        first = false;
        result += pair.first + "=" + pair.second;
    }
    return result;
}

inline bool ErrorContext::isRecoverable() const {
    switch (severity) {
        case ErrorSeverity::Debug:
        case ErrorSeverity::Info:
        case ErrorSeverity::Warning:
            return true;
        case ErrorSeverity::Error:
        case ErrorSeverity::Critical:
        case ErrorSeverity::Fatal:
            return false;
        default:
            return false;
    }
}

inline ErrorSeverity ErrorContext::getEffectiveSeverity() const {
    if (severity >= ErrorSeverity::Critical || severity >= ErrorSeverity::Fatal) {
        return ErrorSeverity::Critical;
    }
    return severity;
}

inline std::string ErrorHandlingResult::getSummary() const {
    std::string result;
    if (success) {
        result = "Operation completed successfully";
    } else {
        result = "Operation failed with " + std::to_string(errors.size()) + " errors";
        if (!warnings.empty()) {
            result += " and " + std::to_string(warnings.size()) + " warnings";
        }
    }
    return result;
}

inline std::string ErrorManager::generateCorrelationId() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::to_string(time_t);
    std::string threadId = std::to_string(std::this_thread::get_id());
    
    // Generate a simple correlation ID
    std::hash<std::string> hasher;
    size_t hash = hasher(timestamp + threadId);
    
    return "CORR_" + std::to_string(hash);
}

inline ErrorContext ErrorManager::createErrorContext(ErrorType type, ErrorSeverity severity,
                                                   ErrorCode code, const std::string& message,
                                                   const std::string& sourceFile,
                                                   const std::string& sourceFunction,
                                                   int sourceLine) {
    ErrorContext ctx(type, severity, code, message);
    ctx.sourceFile = sourceFile;
    ctx.sourceFunction = sourceFunction;
    ctx.sourceLine = sourceLine;
    ctx.correlationId = generateCorrelationId();
    return ctx;
}

} // namespace nlm
