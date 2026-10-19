#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <chrono>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <stack_trace>
#include <source_location>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <variant>
#include <optional>
#include <functional>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

// Error categories
enum class ErrorCategory {
    NONE,
    CONFIGURATION,
    NEURAL_NETWORK,
    MEMORY_MANAGEMENT,
    PERFORMANCE,
    IO,
    INTEGRATION,
    SYSTEM,
    THREADING,
    VALIDATION,
    RECOVERY
};

// Error codes for specific error types
enum class ErrorCode {
    // Configuration errors
    CONFIG_INVALID_VALUE = 1000,
    CONFIG_MISSING_FILE = 1001,
    CONFIG_SCHEMA_VIOLATION = 1002,
    CONFIG_LOAD_FAILED = 1003,
    
    // Neural network errors
    NN_INITIALIZATION_FAILED = 2000,
    NN_COMPUTATION_ERROR = 2001,
    NN_INVALID_SHAPE = 2002,
    NN_DIMENSION_MISMATCH = 2003,
    
    // Memory management errors
    MEMORY_ALLOCATION_FAILED = 3000,
    MEMORY_ACCESS_VIOLATION = 3001,
    MEMORY_CORRUPTION = 3002,
    OUT_OF_MEMORY = 3003,
    
    // Performance errors
    TIMEOUT_EXCEEDED = 4000,
    RESOURCE_EXHAUSTION = 4001,
    PERFORMANCE_DEGRADATION =4002,
    
    // I/O errors
    FILE_NOT_FOUND = 5000,
    FILE_ACCESS_DENIED = 5001,
    NETWORK_CONNECTION_FAILED = 5002,
    SERIALIZATION_ERROR = 5003,
    
    // Integration errors
    COMPONENT_COMMUNICATION_FAILED = 6000,
    INTEGRATION_TIMEOUT = 6001,
    DEPENDENCY_MISSING = 6002,
    
    // System errors
    OS_ERROR = 7000,
    PERMISSION_DENIED = 7001,
    OUT_OF_BOUNDS = 7002,
    
    // Threading errors
    RACE_CONDITION = 8000,
    DEADLOCK_DETECTED = 8001,
    THREAD_LIMIT_EXCEEDED = 8002,
    
    // Validation errors
    INVALID_INPUT = 9000,
    PRECONDITION_FAILED = 9001,
    STATE_VALIDATION_FAILED = 9002,
    CONSISTENCY_ERROR = 9003,
    
    // Recovery errors
    RECOVERY_FAILED = 10000,
    FALLBACK_FAILED = 10001,
    DEGRADATION_FAILED = 10002
};

// Error context information
struct ErrorContext {
    std::string module;
    std::string function;
    std::string file;
    int line;
    std::string threadId;
    std::string processId;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> fields;
    
    // Stack trace
    std::string stackTrace;
    
    // Related error IDs for correlation
    std::vector<std::string> relatedErrors;
    
    // Performance metrics
    std::chrono::nanoseconds handlingTime;
    std::chrono::nanoseconds recoveryTime;
    
    ErrorContext() : line(0), processId(std::to_string(getpid())),
                     threadId(std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()))),
                     handlingTime(0), recoveryTime(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Error action types for recovery
enum class ErrorAction {
    THROW,
    LOG,
    RECOVER,
    DEGRADATE,
    FAIL_FAST,
    RETRY,
    IGNORE
};

// Recovery strategy
struct RecoveryStrategy {
    ErrorAction action;
    std::chrono::milliseconds timeout;
    int maxRetries;
    std::function<bool(const std::exception&)> condition;
    std::function<void()> recoveryAction;
    std::function<void()> fallbackAction;
    std::string description;
    
    RecoveryStrategy() : action(ErrorAction::THROW), timeout(std::chrono::milliseconds(100)),
                          maxRetries(1), description("Default recovery strategy") {}
};

// Base exception class
class NLMException : public std::exception {
protected:
    ErrorSeverity severity;
    ErrorCategory category;
    ErrorCode code;
    std::string message;
    ErrorContext context;
    std::shared_ptr<NLMException> cause;
    mutable std::string cachedWhat;
    
public:
    NLMException(ErrorSeverity sev, ErrorCategory cat, ErrorCode code,
                 const std::string& msg,
                 const ErrorContext& ctx = ErrorContext(),
                 const std::shared_ptr<NLMException>& causePtr = nullptr)
        : severity(sev), category(cat), code(code), message(msg), context(ctx), cause(causePtr) {
        cachedWhat = formatError();
    }
    
    virtual ~NLMException() noexcept = default;
    
    // Getters
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    ErrorCode getCode() const { return code; }
    const std::string& getMessage() const { return message; }
    const ErrorContext& getContext() const { return context; }
    const std::shared_ptr<NLMException>& getCause() const { return cause; }
    
    // Exception interface
    virtual const char* what() const noexcept override {
        return cachedWhat.c_str();
    }
    
    // String representation
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(category) << "] " << static_cast<int>(code) << ": " << message;
        if (!context.file.empty()) oss << " at " << context.file << ":" << context.line;
        if (!context.function.empty()) oss << " in " << context.function;
        if (cause) oss << " caused by: " << cause->what();
        return oss.str();
    }
    
protected:
    virtual std::string formatError() const {
        return toString();
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const std::shared_ptr<NLMException>& error) = 0;
    virtual bool canHandle(ErrorCategory category, ErrorCode code) const = 0;
    virtual ErrorAction getDefaultAction(ErrorCategory category) const = 0;
};

// Global error handler registry
class GlobalErrorHandler {
private:
    std::shared_ptr<IErrorHandler> currentHandler;
    std::mutex mutex;
    
    // Default error handler
    class DefaultErrorHandler : public IErrorHandler {
    public:
        void handleError(const std::shared_ptr<NLMException>& error) override {
            // Log to stderr with severity
            std::string severity = "ERROR";
            switch (error->getSeverity()) {
                case ErrorSeverity::DEBUG: severity = "DEBUG"; break;
                case ErrorSeverity::INFO: severity = "INFO"; break;
                case ErrorSeverity::WARNING: severity = "WARNING"; break;
                case ErrorSeverity::ERROR: severity = "ERROR"; break;
                case ErrorSeverity::CRITICAL: severity = "CRITICAL"; break;
                case ErrorSeverity::FATAL: severity = "FATAL"; break;
            }
            
            std::cerr << "[" << severity << "] " << error->what() << std::endl;
            
            if (error->getSeverity() >= ErrorSeverity::CRITICAL) {
                std::cerr << "FATAL ERROR - terminating program" << std::endl;
                std::exit(1);
            }
        }
        
        bool canHandle(ErrorCategory category, ErrorCode code) const override {
            return true;
        }
        
        ErrorAction getDefaultAction(ErrorCategory category) const override {
            if (category == ErrorCategory::MEMORY_MANAGEMENT || 
                category == ErrorCategory::SYSTEM) {
                return ErrorAction::FAIL_FAST;
            }
            return ErrorAction::LOG;
        }
    };
    
public:
    static GlobalErrorHandler& getInstance() {
        static GlobalErrorHandler instance;
        return instance;
    }
    
    void setHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        currentHandler = std::move(handler);
    }
    
    void handleError(const std::shared_ptr<NLMException>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (currentHandler) {
            currentHandler->handleError(error);
        } else {
            DefaultErrorHandler defaultHandler;
            defaultHandler.handleError(error);
        }
    }
    
    std::shared_ptr<IErrorHandler> getHandler() const {
        std::lock_guard<std::mutex> lock(mutex);
        return currentHandler;
    }
};

// Thread-safe error manager
class ErrorManager {
private:
    std::mutex mutex;
    std::unordered_map<ErrorCategory, std::unordered_map<ErrorCode, RecoveryStrategy>> errorStrategies;
    std::atomic<size_t> errorCount;
    std::atomic<size_t> recoveryCount;
    std::chrono::system_clock::time_point lastErrorTime;
    
    // Error aggregation for batch processing
    std::vector<std::shared_ptr<NLMException>> errorBuffer;
    std::mutex bufferMutex;
    size_t maxBufferSize = 100;
    std::chrono::milliseconds bufferFlushInterval = std::chrono::milliseconds(1000);
    std::chrono::system_clock::time_point lastFlushTime;
    
public:
    ErrorManager() : errorCount(0), recoveryCount(0), lastFlushTime(std::chrono::system_clock::now()) {}
    
    // Record an error
    void recordError(const std::shared_ptr<NLMException>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        errorCount++;
        lastErrorTime = std::chrono::system_clock::now();
        
        // Add to buffer for batch processing
        {
            std::lock_guard<std::mutex> bufferLock(bufferMutex);
            errorBuffer.push_back(error);
            
            // Flush if buffer is full
            if (errorBuffer.size() >= maxBufferSize) {
                flushBuffer();
            }
        }
        
        // Handle the error
        handleErrorInternal(error);
    }
    
    // Configure recovery strategy for error type
    void configureError(ErrorCategory category, ErrorCode code, const RecoveryStrategy& strategy) {
        std::lock_guard<std::mutex> lock(mutex);
        errorStrategies[category][code] = strategy;
    }
    
    // Get recovery strategy for error type
    RecoveryStrategy getRecoveryStrategy(ErrorCategory category, ErrorCode code) {
        std::lock_guard<std::mutex> lock(mutex);
        auto catIt = errorStrategies.find(category);
        if (catIt != errorStrategies.end()) {
            auto codeIt = catIt->second.find(code);
            if (codeIt != catIt->second.end()) {
                return codeIt->second;
            }
        }
        return RecoveryStrategy(); // Default strategy
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors;
        size_t recoveryCount;
        std::chrono::system_clock::time_point lastErrorTime;
        std::unordered_map<ErrorCategory, size_t> errorsByCategory;
    };
    
    ErrorStats getStats() const {
        ErrorStats stats;
        stats.totalErrors = errorCount.load();
        stats.recoveryCount = recoveryCount.load();
        stats.lastErrorTime = lastErrorTime;
        // TODO: Implement per-category counting if needed
        return stats;
    }
    
private:
    // Internal error handling
    void handleErrorInternal(const std::shared_ptr<NLMException>& error) {
        ErrorCategory category = error->getCategory();
        ErrorCode code = error->getCode();
        
        RecoveryStrategy strategy = getRecoveryStrategy(category, code);
        
        switch (strategy.action) {
            case ErrorAction::THROW:
                // Re-throw the error
                throw *error.get();
                
            case ErrorAction::LOG:
                // Just log it
                GlobalErrorHandler::getInstance().handleError(error);
                break;
                
            case ErrorAction::RECOVER:
                if (strategy.recoveryAction) {
                    strategy.recoveryAction();
                    recoveryCount++;
                }
                break;
                
            case ErrorAction::DEGRADATE:
                if (strategy.fallbackAction) {
                    strategy.fallbackAction();
                    recoveryCount++;
                }
                break;
                
            case ErrorAction::FAIL_FAST:
                GlobalErrorHandler::getInstance().handleError(error);
                std::exit(1);
                break;
                
            case ErrorAction::RETRY:
                // Implement retry logic
                break;
                
            case ErrorAction::IGNORE:
                // Silently ignore
                break;
        }
    }
    
    // Flush buffered errors
    void flushBuffer() {
        std::lock_guard<std::mutex> bufferLock(bufferMutex);
        if (errorBuffer.empty()) return;
        
        // Aggregate errors
        for (const auto& error : errorBuffer) {
            GlobalErrorHandler::getInstance().handleError(error);
        }
        
        errorBuffer.clear();
    }
};

// Convenience functions for error creation
namespace Errors {
    
    // Create configuration errors
    inline std::shared_ptr<NLMException> configInvalid(const std::string& key, 
                                                       const std::string& expected,
                                                       const std::string& actual,
                                                       const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::ERROR,
            ErrorCategory::CONFIGURATION,
            ErrorCode::CONFIG_INVALID_VALUE,
            "Invalid value for key '" + key + "': expected '" + expected + "', got '" + actual + "'",
            ctx
        );
    }
    
    // Create neural network errors
    inline std::shared_ptr<NLMException> neuralNetwork(
        ErrorCode code,
        const std::string& message,
        const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::ERROR,
            ErrorCategory::NEURAL_NETWORK,
            code,
            message,
            ctx
        );
    }
    
    // Create memory errors
    inline std::shared_ptr<NLMException> memoryAllocation(
        size_t size,
        const std::string& reason,
        const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::CRITICAL,
            ErrorCategory::MEMORY_MANAGEMENT,
            ErrorCode::MEMORY_ALLOCATION_FAILED,
            "Memory allocation failed: size=" + std::to_string(size) + ", reason: " + reason,
            ctx
        );
    }
    
    // Create performance errors
    inline std::shared_ptr<NLMException> performanceTimeout(
        const std::string& operation,
        std::chrono::milliseconds duration,
        std::chrono::milliseconds limit,
        const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::WARNING,
            ErrorCategory::PERFORMANCE,
            ErrorCode::TIMEOUT_EXCEEDED,
            "Performance timeout: " + operation + " took " + 
            std::to_string(duration.count()) + "ms, limit was " + 
            std::to_string(limit.count()) + "ms",
            ctx
        );
    }
    
    // Create I/O errors
    inline std::shared_ptr<NLMException> ioError(
        ErrorCode code,
        const std::string& path,
        const std::string& reason,
        const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::ERROR,
            ErrorCategory::IO,
            code,
            "I/O error for path '" + path + "': " + reason,
            ctx
        );
    }
    
    // Create system errors
    inline std::shared_ptr<NLMException> systemError(
        const std::string& operation,
        const std::string& reason,
        const ErrorContext& ctx = ErrorContext()) {
        return std::make_shared<NLMException>(
            ErrorSeverity::ERROR,
            ErrorCategory::SYSTEM,
            ErrorCode::OS_ERROR,
            "System error during " + operation + ": " + reason,
            ctx
        );
    }
}

// Error handler implementations for specific categories
class ConfigurationErrorHandler : public IErrorHandler {
public:
    void handleError(const std::shared_ptr<NLMException>& error) override {
        std::cerr << "[CONFIG] " << error->what() << std::endl;
        // Log to config-specific log file
    }
    
    bool canHandle(ErrorCategory category, ErrorCode code) const override {
        return category == ErrorCategory::CONFIGURATION;
    }
    
    ErrorAction getDefaultAction(ErrorCategory category) const override {
        return ErrorAction::RECOVER;
    }
};

class NeuralNetworkErrorHandler : public IErrorHandler {
public:
    void handleError(const std::shared_ptr<NLMException>& error) override {
        std::cerr << "[NEURAL_NETWORK] " << error->what() << std::endl;
        // Could trigger model re-initialization
    }
    
    bool canHandle(ErrorCategory category, ErrorCode code) const override {
        return category == ErrorCategory::NEURAL_NETWORK;
    }
    
    ErrorAction getDefaultAction(ErrorCategory category) const override {
        return ErrorAction::RECOVER;
    }
};

// Error utility functions
inline std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::CONFIG_INVALID_VALUE: return "CONFIG_INVALID_VALUE";
        case ErrorCode::CONFIG_MISSING_FILE: return "CONFIG_MISSING_FILE";
        case ErrorCode::CONFIG_SCHEMA_VIOLATION: return "CONFIG_SCHEMA_VIOLATION";
        case ErrorCode::CONFIG_LOAD_FAILED: return "CONFIG_LOAD_FAILED";
        case ErrorCode::NN_INITIALIZATION_FAILED: return "NN_INITIALIZATION_FAILED";
        case ErrorCode::NN_COMPUTATION_ERROR: return "NN_COMPUTATION_ERROR";
        case ErrorCode::NN_INVALID_SHAPE: return "NN_INVALID_SHAPE";
        case ErrorCode::NN_DIMENSION_MISMATCH: return "NN_DIMENSION_MISMATCH";
        case ErrorCode::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
        case ErrorCode::MEMORY_ACCESS_VIOLATION: return "MEMORY_ACCESS_VIOLATION";
        case ErrorCode::MEMORY_CORRUPTION: return "MEMORY_CORRUPTION";
        case ErrorCode::OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case ErrorCode::TIMEOUT_EXCEEDED: return "TIMEOUT_EXCEEDED";
        case ErrorCode::RESOURCE_EXHAUSTION: return "RESOURCE_EXHAUSTION";
        case ErrorCode::PERFORMANCE_DEGRADATION: return "PERFORMANCE_DEGRADATION";
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::FILE_ACCESS_DENIED: return "FILE_ACCESS_DENIED";
        case ErrorCode::NETWORK_CONNECTION_FAILED: return "NETWORK_CONNECTION_FAILED";
        case ErrorCode::SERIALIZATION_ERROR: return "SERIALIZATION_ERROR";
        case ErrorCode::COMPONENT_COMMUNICATION_FAILED: return "COMPONENT_COMMUNICATION_FAILED";
        case ErrorCode::INTEGRATION_TIMEOUT: return "INTEGRATION_TIMEOUT";
        case ErrorCode::DEPENDENCY_MISSING: return "DEPENDENCY_MISSING";
        case ErrorCode::OS_ERROR: return "OS_ERROR";
        case ErrorCode::PERMISSION_DENIED: return "PERMISSION_DENIED";
        case ErrorCode::OUT_OF_BOUNDS: return "OUT_OF_BOUNDS";
        case ErrorCode::RACE_CONDITION: return "RACE_CONDITION";
        case ErrorCode::DEADLOCK_DETECTED: return "DEADLOCK_DETECTED";
        case ErrorCode::THREAD_LIMIT_EXCEEDED: return "THREAD_LIMIT_EXCEEDED";
        case ErrorCode::INVALID_INPUT: return "INVALID_INPUT";
        case ErrorCode::PRECONDITION_FAILED: return "PRECONDITION_FAILED";
        case ErrorCode::STATE_VALIDATION_FAILED: return "STATE_VALIDATION_FAILED";
        case ErrorCode::CONSISTENCY_ERROR: return "CONSISTENCY_ERROR";
        case ErrorCode::RECOVERY_FAILED: return "RECOVERY_FAILED";
        case ErrorCode::FALLBACK_FAILED: return "FALLBACK_FAILED";
        case ErrorCode::DEGRADATION_FAILED: return "DEGRADATION_FAILED";
        default: return "UNKNOWN_ERROR";
    }
}

inline std::string errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::NONE: return "NONE";
        case ErrorCategory::CONFIGURATION: return "CONFIGURATION";
        case ErrorCategory::NEURAL_NETWORK: return "NEURAL_NETWORK";
        case ErrorCategory::MEMORY_MANAGEMENT: return "MEMORY_MANAGEMENT";
        case ErrorCategory::PERFORMANCE: return "PERFORMANCE";
        case ErrorCategory::IO: return "IO";
        case ErrorCategory::INTEGRATION: return "INTEGRATION";
        case ErrorCategory::SYSTEM: return "SYSTEM";
        case ErrorCategory::THREADING: return "THREADING";
        case ErrorCategory::VALIDATION: return "VALIDATION";
        case ErrorCategory::RECOVERY: return "RECOVERY";
        default: return "UNKNOWN";
    }
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H
