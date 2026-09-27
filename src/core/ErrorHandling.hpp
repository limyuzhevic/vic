#pragma once

/**
 * NLM Error Handling System
 * 
 * Comprehensive error handling system with exception hierarchy for easy error handling,
 * error codes for programmatic error handling, error messages with detailed context,
 * error recovery utilities, and error reporting and logging.
 */

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Error severity levels for classification and handling
 */
enum class ErrorSeverity {
    INFO = 0,      /**< Informational message, not an error */
    WARNING = 1,   /**< Warning condition that may indicate a problem */
    ERROR = 2,     /**< Error condition that prevents normal operation */
    CRITICAL = 3,  /**< Critical error that prevents recovery */
    FATAL = 4      /**< Fatal error requiring immediate termination */
};

/**
 * Error codes for programmatic error handling
 * 
 * Each error code represents a specific type of error with a descriptive name
 * and optional error category. This allows for granular error handling and
 * logging in production systems.
 */
enum class ErrorCode {
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_FILE_INVALID_FORMAT = 1001,
    CONFIG_KEY_MISSING = 1002,
    CONFIG_VALUE_INVALID =1003,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED = 2000,
    MEMORY_POOL_EXHAUSTED = 2001,
    MEMORY_BOUNDS_VIOLATION =2002,
    MEMORY_CORRUPTION_DETECTED = 2003,
    
    // Validation errors
    VALIDATION_FAILED = 3000,
    VALIDATION_OUT_OF_RANGE = 3001,
    VALIDATION_TYPE_MISMATCH = 3002,
    VALIDATION_CONSTRAINT_VIOLATION =3003,
    
    // Performance errors
    PERFORMANCE_BOTTLE_SNECOUNTED = 4000,
    PERFORMANCE_TIMEOUT = 4001,
    PERFORMANCE_MEMORY_USAGE_HIGH = 4002,
    PERFORMANCE_SIMD_NOT_AVAILABLE =4003,
    
    // Training errors
    TRAINING_CONVERGENCE_FAILED = 5000,
    TRAINING_STABILITY_ISSUE = 5001,
    TRAINING_LEARNING_RATE_INVALID =5002,
    TRAINING_DATA_INSUFFICIENT =5003,
    
    // Neural computation errors
    NEURAL_COMPUTATION_FAILED = 6000,
    SPIKE_GENERATION_FAILED =6001,
    SYNAPTIC_DELAY_INVALID =6002,
    REFACTORY_PERIOD_VIOLATION =6003,
    
    // System errors
    SYSTEM_INITIALIZATION_FAILED = 7000,
    SYSTEM_NOT_READY =7001,
    SYSTEM_SHUTDOWN_IN_PROGRESS =7002,
    
    // IO/Interface errors
    FILE_IO_ERROR = 8000,
    NETWORK_COMMUNICATION_ERROR =8001,
    PYTHON_BINDINGS_ERROR =8002,
    
    // Internal errors
    INTERNAL_ERROR = 9000,
    UNEXPECTED_STATE =9001,
    CORRUPTION_DETECTED =9002,
    
    // Generic errors
    UNKNOWN_ERROR = 10000,
    NOT_IMPLEMENTED = 10001,
    OUT_OF_MEMORY = 10002,
    INVALID_PARAMETER = 10003
};

/**
 * Base NLM error with detailed context information
 * 
 * Provides comprehensive error information including error code, message, timestamp,
 * and context for debugging and error reporting. Supports error severity classification
 * and recovery suggestions.
 */
class NLMError : public std::exception {
public:
    /**
     * Construct a new NLMError
     * 
     * @param code Error code identifying the specific error type
     * @param message Human-readable error description
     * @param severity Severity level for error handling
     * @param context Additional context information for debugging
     */
    NLMError(ErrorCode code, const std::string& message, 
             ErrorSeverity severity = ErrorSeverity::ERROR,
             const std::string& context = "")
        : code_(code), message_(message), severity_(severity), 
          context_(context), timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMError() noexcept = default;
    
    /**
     * Get the error code
     * 
     * @return ErrorCode The error code
     */
    ErrorCode getCode() const noexcept { return code_; }
    
    /**
     * Get the error message
     * 
     * @return const std::string& The error message
     */
    const std::string& getMessage() const noexcept { return message_; }
    
    /**
     * Get the error severity
     * 
     * @return ErrorSeverity The error severity
     */
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    
    /**
     * Get the context information
     * 
     * @return const std::string& The context information
     */
    const std::string& getContext() const noexcept { return context_; }
    
    /**
     * Get the timestamp when the error occurred
     * 
     * @return const std::chrono::system_clock::time_point& Error timestamp
     */
    const std::chrono::system_clock::time_point& getTimestamp() const noexcept { return timestamp_; }
    
    /**
     * Get a formatted error description
     * 
     * @return std::string Formatted error description including code, message, and context
     */
    std::string getFullDescription() const {
        std::stringstream ss;
        ss << "Error Code: " << static_cast<int>(code_) << "\n"
           << "Severity: " << static_cast<int>(severity_) << "\n"
           << "Message: " << message_ << "\n"
           << "Context: " << context_ << "\n"
           << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp_);
        return ss.str();
    }
    
    /**
     * What exception provides - required for std::exception compliance
     * 
     * @return const char* The error message
     */
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    /**
     * Check if this error is recoverable
     * 
     * @return true If error can be recovered from
     * @return false If error requires system restart or error recovery
     */
    virtual bool isRecoverable() const noexcept {
        switch (severity_) {
            case ErrorSeverity::INFO:
            case ErrorSeverity::WARNING:
                return true;
            case ErrorSeverity::ERROR:
                return true; // Most errors are recoverable
            case ErrorSeverity::CRITICAL:
                return false;
            case ErrorSeverity::FATAL:
                return false;
            default:
                return false;
        }
    }
    
    /**
     * Get recovery suggestion
     * 
     * @return std::string Recovery suggestion based on error type
     */
    virtual std::string getRecoverySuggestion() const {
        switch (code_) {
            case ErrorCode::MEMORY_ALLOCATION_FAILED:
                return "Reduce memory usage or increase system memory";
            case ErrorCode::CONFIGURATION_ERROR:
                return "Check configuration files and parameters";
            case ErrorCode::VALIDATION_FAILED:
                return "Validate input parameters and data";
            case ErrorCode::PERFORMANCE_BOTTLE_SNECOUNTED:
                return "Optimize performance or reduce problem size";
            case ErrorCode::TRAINING_CONVERGENCE_FAILED:
                return "Adjust learning rate or increase training iterations";
            default:
                return "Check system logs and consider system restart";
        }
    }

protected:
    ErrorCode code_;
    std::string message_;
    ErrorSeverity severity_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
};

/**
 * Configuration-related error (NLM_ERROR)
 */
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::CONFIGURATION_ERROR, message, ErrorSeverity::ERROR, context) {}
};

/**
 * Memory allocation error (NLM_ERROR)
 */
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::MEMORY_ALLOCATION_FAILED, message, ErrorSeverity::ERROR, context) {}
};

/**
 * Validation error (NLM_ERROR)
 */
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::VALIDATION_FAILED, message, ErrorSeverity::WARNING, context) {}
};

/**
 * Performance-related error (NLM_ERROR)
 */
class PerformanceError : public NLMError {
public:
    PerformanceError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::PERFORMANCE_BOTTLE_SNECOUNTED, message, ErrorSeverity::WARNING, context) {}
};

/**
 * Training-related error (NLM_ERROR)
 */
class TrainingError : public NLMError {
public:
    TrainingError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::TRAINING_CONVERGENCE_FAILED, message, ErrorSeverity::ERROR, context) {}
};

/**
 * Neural computation error (NLM_ERROR)
 */
class NeuralComputationError : public NLMError {
public:
    NeuralComputationError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::NEURAL_COMPUTATION_FAILED, message, ErrorSeverity::ERROR, context) {}
};

/**
 * System error (NLM_ERROR)
 */
class SystemError : public NLMError {
public:
    SystemError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorCode::SYSTEM_INITIALIZATION_FAILED, message, ErrorSeverity::CRITICAL, context) {}
};

/**
 * Error reporting and logging utilities
 */
class ErrorReporter {
public:
    /**
     * Log an error with optional recovery action
     * 
     * @param error The error to log
     * @param recoveryAction Optional recovery action to attempt
     * @return true If error was handled successfully, false otherwise
     */
    static bool logError(const NLMError& error, const std::function<bool()>& recoveryAction = nullptr) {
        // Log error to appropriate output (console, file, etc.)
        std::cerr << "[" << static_cast<int>(error.getSeverity()) << "] " 
                  << "Error " << static_cast<int>(error.getCode()) << 
                  << ": " << error.getMessage() << std::endl;
        
        if (!error.getContext().empty()) {
            std::cerr << "Context: " << error.getContext() << std::endl;
        }
        
        // Attempt recovery if provided and error is recoverable
        if (error.isRecoverable() && recoveryAction) {
            try {
                if (recoveryAction()) {
                    std::cerr << "Recovery action succeeded" << std::endl;
                    return true;
                }
            } catch (const std::exception& e) {
                std::cerr << "Recovery action failed: " << e.what() << std::endl;
            }
        }
        
        return false;
    }
    
    /**
     * Get error code description
     * 
     * @param code Error code
     * @return std::string Human-readable error code description
     */
    static std::string getErrorCodeDescription(ErrorCode code) {
        static const std::unordered_map<ErrorCode, std::string> descriptions = {
            {ErrorCode::CONFIG_FILE_NOT_FOUND, "Configuration file not found"},
            {ErrorCode::CONFIG_FILE_INVALID_FORMAT, "Configuration file has invalid format"},
            {ErrorCode::CONFIG_KEY_MISSING, "Required configuration key is missing"},
            {ErrorCode::CONFIG_VALUE_INVALID, "Configuration value is invalid"},
            {ErrorCode::MEMORY_ALLOCATION_FAILED, "Memory allocation failed"},
            {ErrorCode::MEMORY_POOL_EXHAUSTED, "Memory pool is exhausted"},
            {ErrorCode::MEMORY_BOUNDS_VIOLATION, "Memory bounds violation detected"},
            {ErrorCode::VALIDATION_FAILED, "Validation failed"},
            {ErrorCode::PERFORMANCE_BOTTLE_SNECOUNTED, "Performance bottleneck detected"},
            {ErrorCode::TRAINING_CONVERGENCE_FAILED, "Training failed to converge"},
            {ErrorCode::UNKNOWN_ERROR, "Unknown error occurred"}
        };
        
        auto it = descriptions.find(code);
        return it != descriptions.end() ? it->second : "Unknown error";
    }
    
    /**
     * Convert error severity to string
     * 
     * @param severity Error severity
     * @return std::string String representation of severity
     */
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            case ErrorSeverity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

/**
 * Error context manager for thread-safe error tracking
 */
class ErrorContext {
public:
    /**
     * Add context to the current error handling operation
     * 
     * @param context Context information to add
     */
    static void addContext(const std::string& context) {
        std::lock_guard<std::mutex> lock(contextMutex_);
        contextStack_.push_back(context);
    }
    
    /**
     * Remove the most recent context
     */
    static void removeContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        if (!contextStack_.empty()) {
            contextStack_.pop_back();
        }
    }
    
    /**
     * Get current context stack
     * 
     * @return std::string Combined context string
     */
    static std::string getCurrentContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        std::stringstream ss;
        for (size_t i = 0; i < contextStack_.size(); ++i) {
            if (i > 0) ss << " -> ";
            ss << contextStack_[i];
        }
        return ss.str();
    }
    
    /**
     * Clear all contexts
     */
    static void clearContext() {
        std::lock_guard<std::mutex> lock(contextMutex_);
        contextStack_.clear();
    }

private:
    static std::vector<std::string> contextStack_;
    static std::mutex contextMutex_;
};

// Error handling macros for convenience
#define NLM_THROW_ERROR(code, message, context) throw nlm::NLMError(code, message, nlm::ErrorSeverity::ERROR, context)
#define NLM_THROW_CONFIG_ERROR(message, context) throw nlm::ConfigurationError(message, context)
#define NLM_THROW_MEMORY_ERROR(message, context) throw nlm::MemoryError(message, context)
#define NLM_THROW_VALIDATION_ERROR(message, context) throw nlm::ValidationError(message, context)
#define NLM_THROW_PERFORMANCE_ERROR(message, context) throw nlm::PerformanceError(message, context)
#define NLM_THROW_TRAINING_ERROR(message, context) throw nlm::TrainingError(message, context)

} // namespace nlm