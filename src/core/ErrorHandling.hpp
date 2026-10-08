// NLM Error Handling System
// Provides comprehensive error management with recovery mechanisms

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <thread>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL = 3,
    FATAL = 4
};

// Error categories
enum class ErrorCategory {
    SYSTEM = 0,        // System-level errors (memory, resources, etc.)
    CONFIG = 1,        // Configuration errors
    NEURAL = 2,        // Neural computation errors
    MEMORY = 3,        // Memory management errors
    IO = 4,            // Input/output errors
    NETWORK = 5,       // Network communication errors
    PLUGIN = 6,        // Plugin/extension errors
    USER = 7,          // User input/configuration errors
    DEVELOPMENT = 8,   // Development/automation errors
    UNKNOWN = 9
};

// Base exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR,
                 ErrorCategory category = ErrorCategory::UNKNOWN,
                 const std::string& component = "")
        : std::runtime_error(message), severity_(severity), category_(category),
          component_(component), timestamp_(std::chrono::system_clock::now()) {}
    
    ErrorSeverity getSeverity() const { return severity_; }
    ErrorCategory getCategory() const { return category_; }
    const std::string& getComponent() const { return component_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity_) << "] ";
        oss << "[" << static_cast<int>(category_) << "] ";
        if (!component_.empty()) {
            oss << "[" << component_ << "] ";
        }
        oss << what();
        return oss.str();
    }
    
protected:
    ErrorSeverity severity_;
    ErrorCategory category_;
    std::string component_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types
class NLMSystemError : public NLMException {
public:
    NLMSystemError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::SYSTEM) {}
};

class NLMConfigError : public NLMException {
public:
    NLMConfigError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::CONFIG) {}
};

class NLMNeuralError : public NLMException {
public:
    NLMNeuralError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::NEURAL) {}
};

class NLMMemoryError : public NLMException {
public:
    NLMMemoryError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::MEMORY) {}
};

class NLMIOError : public NLMException {
public:
    NLMIOError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::IO) {}
};

class NLMNetworkError : public NLMException {
public:
    NLMNetworkError(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR)
        : NLMException(message, severity, ErrorCategory::NETWORK) {}
};

// Error context for better diagnostics
struct ErrorContext {
    std::string operation;
    std::string details;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    int retryCount = 0;
    bool recovered = false;
    
    ErrorContext(const std::string& op, const std::string& detail = "")
        : operation(op), details(detail), timestamp(std::chrono::system_clock::now()) {}
};

// Error recovery strategy
enum class RecoveryStrategy {
    NONE,              // No recovery possible
    RETRY,             // Retry the operation
    FALLBACK,          // Use fallback implementation
    DEGRADE,           // Degrade functionality
    CIRCUIT_BREAKER,   // Open circuit breaker for external service
    RESTORE,           // Restore to previous working state
    IGNORE,            // Ignore this error
    ABORT              // Abort operation
};

// Error recovery action
class ErrorRecovery {
public:
    virtual ~ErrorRecovery() = default;
    virtual RecoveryStrategy getStrategy() const = 0;
    virtual bool canRecover(const ErrorContext& context) const = 0;
    virtual ErrorContext recover(const ErrorContext& context) = 0;
    virtual std::string getDescription() const = 0;
};

// Retry recovery strategy
class RetryRecovery : public ErrorRecovery {
public:
    RetryRecovery(int maxRetries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100))
        : maxRetries_(maxRetries), delay_(delay) {}
    
    RecoveryStrategy getStrategy() const override { return RecoveryStrategy::RETRY; }
    bool canRecover(const ErrorContext& context) const override { return context.retryCount < maxRetries_; }
    ErrorContext recover(const ErrorContext& context) override {
        ErrorContext newContext = context;
        newContext.retryCount++;
        
        if (delay_.count() > 0) {
            std::this_thread::sleep_for(delay_);
        }
        
        return newContext;
    }
    std::string getDescription() const override {
        return "Retry up to " + std::to_string(maxRetries_) + " times with " + 
               std::to_string(delay_.count()) + "ms delay";
    }
    
private:
    int maxRetries_;
    std::chrono::milliseconds delay_;
};

// Fallback recovery strategy
class FallbackRecovery : public ErrorRecovery {
public:
    FallbackRecovery(const std::function<void()>& fallbackAction)
        : fallbackAction_(fallbackAction) {}
    
    RecoveryStrategy getStrategy() const override { return RecoveryStrategy::FALLBACK; }
    bool canRecover(const ErrorContext& context) const override { return true; }
    ErrorContext recover(const ErrorContext& context) override {
        ErrorContext newContext = context;
        try {
            fallbackAction_();
            newContext.recovered = true;
        } catch (...) {
            newContext.recovered = false;
        }
        return newContext;
    }
    std::string getDescription() const override {
        return "Execute fallback action";
    }
    
private:
    std::function<void()> fallbackAction_;
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() = default;
    
    void registerRecovery(const std::string& category, std::unique_ptr<ErrorRecovery> recovery) {
        std::lock_guard<std::mutex> lock(mutex_);
        recoveries_[category] = std::move(recovery);
    }
    
    ErrorContext handleError(const NLMException& error, const ErrorContext& context) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        ErrorContext newContext = context;
        
        // Find recovery strategy based on error category and component
        std::string categoryKey = std::to_string(static_cast<int>(error.getCategory()));
        auto it = recoveries_.find(categoryKey);
        
        if (it != recoveries_.end()) {
            auto& recovery = it->second;
            if (recovery->canRecover(context)) {
                newContext = recovery->recover(context);
                logRecovery(error, newContext);
            }
        }
        
        // Log error if not recovered or critical
        if (!newContext.recovered || error.getSeverity() >= ErrorSeverity::CRITICAL) {
            logError(error, context);
        }
        
        return newContext;
    }
    
    void addErrorHandler(const std::string& component, std::function<void(const NLMException&)> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorHandlers_[component] = handler;
    }
    
    void logError(const NLMException& error, const ErrorContext& context) {
        std::string message = error.getFormattedMessage();
        if (!context.details.empty()) {
            message += " | Context: " + context.details;
        }
        message += " | Retries: " + std::to_string(context.retryCount);
        
        switch (error.getSeverity()) {
            case ErrorSeverity::INFO:
                NLM_LOG_INFO(message);
                break;
            case ErrorSeverity::WARNING:
                NLM_LOG_WARNING(message);
                break;
            case ErrorSeverity::ERROR:
                NLM_LOG_ERROR(message);
                break;
            case ErrorSeverity::CRITICAL:
                NLM_LOG_CRITICAL(message);
                break;
            case ErrorSeverity::FATAL:
                NLM_LOG_CRITICAL("FATAL: " + message);
                break;
        }
    }
    
    void logRecovery(const NLMException& error, const ErrorContext& context) {
        std::string message = "Recovered from error: ";
        message += error.getFormattedMessage();
        message += " | Recovery: " + context.recovered ? "Yes" : "No";
        NLM_LOG_INFO(message);
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        recoveries_.clear();
        errorHandlers_.clear();
    }
    
private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<ErrorRecovery>> recoveries_;
    std::unordered_map<std::string, std::function<void(const NLMException&)>> errorHandlers_;
};

// Circuit breaker for external services
class CircuitBreaker {
public:
    enum class State {
        CLOSED,    // Normal operation
        OPEN,      // Circuit is open, calls fail fast
        HALF_OPEN  // Testing if service is back
    };
    
    CircuitBreaker(int failureThreshold = 5, std::chrono::seconds timeout = std::chrono::seconds(60),
                   double successThreshold = 0.5)
        : failureThreshold_(failureThreshold), timeout_(timeout),
          successThreshold_(successThreshold), state_(State::CLOSED),
          lastFailureTime_(std::chrono::system_clock::now()), consecutiveFailures_(0), consecutiveSuccesses_(0) {}
    
    bool callAllowed() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == State::CLOSED) {
            return true;
        }
        
        if (state_ == State::OPEN) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastFailureTime_);
            
            if (duration.count() >= timeout_.count()) {
                state_ = State::HALF_OPEN;
                consecutiveSuccesses_ = 0;
                return true;
            }
            
            return false;
        }
        
        // HALF_OPEN state
        return true;
    }
    
    void recordSuccess() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == State::HALF_OPEN) {
            consecutiveSuccesses_++;
            if (consecutiveSuccesses_ >= static_cast<size_t>(failureThreshold_ * successThreshold_)) {
                state_ = State::CLOSED;
                consecutiveSuccesses_ = 0;
            }
        }
        
        consecutiveFailures_ = 0;
    }
    
    void recordFailure() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        consecutiveFailures_++;
        lastFailureTime_ = std::chrono::system_clock::now();
        
        if (consecutiveFailures_ >= static_cast<size_t>(failureThreshold_)) {
            state_ = State::OPEN;
        }
        
        consecutiveSuccesses_ = 0;
    }
    
    State getState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_;
    }
    
    std::string getStateString() const {
        switch (state_) {
            case State::CLOSED: return "CLOSED";
            case State::OPEN: return "OPEN";
            case State::HALF_OPEN: return "HALF_OPEN";
            default: return "UNKNOWN";
        }
    }
    
private:
    std::mutex mutex_;
    int failureThreshold_;
    std::chrono::seconds timeout_;
    double successThreshold_;
    State state_;
    std::chrono::system_clock::time_point lastFailureTime_;
    size_t consecutiveFailures_;
    size_t consecutiveSuccesses_;
};

// Enhanced logger with error handling capabilities
class ErrorHandlingLogger : public ILogger {
public:
    ErrorHandlingLogger(std::shared_ptr<Logger> baseLogger,
                       std::shared_ptr<ErrorRecoveryManager> recoveryManager = nullptr)
        : baseLogger_(baseLogger), recoveryManager_(recoveryManager) {}
    
    void log(const LogEntry& entry) override {
        // Log the entry using base logger
        baseLogger_->log(entry.level, entry.message, entry.file, entry.line, entry.function);
        
        // Check if this is an error that needs recovery
        if (entry.level >= LogLevel::Error) {
            handleLogEntry(entry);
        }
    }
    
    void setLevel(LogLevel level) override {
        baseLogger_->setLevel(level);
    }
    
    LogLevel getLevel() const override {
        return baseLogger_->getLevel();
    }
    
    // Error handling methods
    void handleException(const NLMException& error, const std::string& operation) {
        ErrorContext context(operation, error.what());
        
        if (recoveryManager_) {
            ErrorContext newContext = recoveryManager_->handleError(error, context);
            // Update the error context in the logger if needed
        } else {
            // Log error without recovery
            std::string message = "Exception: " + error.getFormattedMessage();
            if (operation != "") {
                message += " | Operation: " + operation;
            }
            NLM_LOG_ERROR(message);
        }
    }
    
    void setRecoveryManager(std::shared_ptr<ErrorRecoveryManager> recoveryManager) {
        recoveryManager_ = recoveryManager;
    }
    
private:
    std::shared_ptr<Logger> baseLogger_;
    std::shared_ptr<ErrorRecoveryManager> recoveryManager_;
    mutable std::mutex mutex_;
};

// Global error recovery manager instance
ErrorRecoveryManager& getGlobalErrorRecoveryManager() {
    static ErrorRecoveryManager manager;
    return manager;
}

// Global circuit breaker registry
class CircuitBreakerRegistry {
public:
    void registerBreaker(const std::string& name, std::shared_ptr<CircuitBreaker> breaker) {
        std::lock_guard<std::mutex> lock(mutex_);
        breakers_[name] = breaker;
    }
    
    std::shared_ptr<CircuitBreaker> getBreaker(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = breakers_.find(name);
        return (it != breakers_.end()) ? it->second : nullptr;
    }
    
    bool callAllowed(const std::string& name) {
        auto breaker = getBreaker(name);
        return breaker ? breaker->callAllowed() : true;
    }
    
    void recordSuccess(const std::string& name) {
        auto breaker = getBreaker(name);
        if (breaker) {
            breaker->recordSuccess();
        }
    }
    
    void recordFailure(const std::string& name) {
        auto breaker = getBreaker(name);
        if (breaker) {
            breaker->recordFailure();
        }
    }
    
private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<CircuitBreaker>> breakers_;
};

CircuitBreakerRegistry& getGlobalCircuitBreakerRegistry() {
    static CircuitBreakerRegistry registry;
    return registry;
}

// Exception wrapper for safe execution
namespace ExceptionSafeExecutor {
    static std::shared_ptr<ErrorRecoveryManager> globalErrorRecoveryManager;
    
    template<typename Func>
    auto execute(Func&& func, const std::string& operation = "") -> std::optional<decltype(func())> {
        try {
            auto result = func();
            return result;
        } catch (const NLMException& e) {
            if (globalErrorRecoveryManager) {
                ErrorContext context(operation, e.what());
                globalErrorRecoveryManager->handleError(e, context);
            }
            throw; // Re-throw after logging/recovery
        } catch (const std::exception& e) {
            NLMSystemError systemError("Unexpected error: " + std::string(e.what()), ErrorSeverity::CRITICAL);
            if (globalErrorRecoveryManager) {
                ErrorContext context(operation, e.what());
                globalErrorRecoveryManager->handleError(systemError, context);
            }
            throw systemError;
        }
    }
    
    template<typename Func>
    void executeVoid(Func&& func, const std::string& operation = "") {
        try {
            func();
        } catch (const NLMException& e) {
            if (globalErrorRecoveryManager) {
                ErrorContext context(operation, e.what());
                globalErrorRecoveryManager->handleError(e, context);
            }
            throw;
        } catch (const std::exception& e) {
            NLMSystemError systemError("Unexpected error: " + std::string(e.what()), ErrorSeverity::CRITICAL);
            if (globalErrorRecoveryManager) {
                ErrorContext context(operation, e.what());
                globalErrorRecoveryManager->handleError(systemError, context);
            }
            throw systemError;
        }
    }
}

// Convenience functions for error handling
inline void setupDefaultErrorHandling() {
    // Register default recovery strategies
    auto& manager = getGlobalErrorRecoveryManager();
    
    // Add retry recovery for system errors
    manager.registerRecovery(std::to_string(static_cast<int>(ErrorCategory::SYSTEM)),
                           std::make_unique<RetryRecovery>(3, std::chrono::milliseconds(100)));
    
    // Add fallback recovery for configuration errors
    manager.registerRecovery(std::to_string(static_cast<int>(ErrorCategory::CONFIG)),
                           std::make_unique<FallbackRecovery>([]() {
                               // Default fallback: log warning and continue
                               NLM_LOG_WARNING("Using default configuration values");
                           }));
}

inline void setGlobalErrorRecovery(std::shared_ptr<ErrorRecoveryManager> manager) {
    ExceptionSafeExecutor::globalErrorRecoveryManager = manager;
}

inline std::shared_ptr<ErrorRecoveryManager> getGlobalErrorRecovery() {
    return ExceptionSafeExecutor::globalErrorRecoveryManager;
}

// Circuit breaker convenience functions
inline void registerCircuitBreaker(const std::string& name,
                                 int failureThreshold = 5,
                                 std::chrono::seconds timeout = std::chrono::seconds(60),
                                 double successThreshold = 0.5) {
    auto breaker = std::make_shared<CircuitBreaker>(failureThreshold, timeout, successThreshold);
    getGlobalCircuitBreakerRegistry().registerBreaker(name, breaker);
}

inline bool callAllowed(const std::string& name) {
    return getGlobalCircuitBreakerRegistry().callAllowed(name);
}

inline void recordSuccess(const std::string& name) {
    getGlobalCircuitBreakerRegistry().recordSuccess(name);
}

inline void recordFailure(const std::string& name) {
    getGlobalCircuitBreakerRegistry().recordFailure(name);
}

// Safe execution with error handling
inline template<typename Func>
auto safeExecute(Func&& func, const std::string& operation = "", ErrorCategory category = ErrorCategory::UNKNOWN) {
    return ExceptionSafeExecutor::execute(std::forward<Func>(func), operation);
}

inline template<typename Func>
void safeExecuteVoid(Func&& func, const std::string& operation = "", ErrorCategory category = ErrorCategory::UNKNOWN) {
    ExceptionSafeExecutor::executeVoid(std::forward<Func>(func), operation);
}

} // namespace nlm

// Global initialization function
inline void initializeNLMErrorHandling() {
    nlm::setupDefaultErrorHandling();
}

// Cleanup function
inline void cleanupNLMErrorHandling() {
    auto& manager = nlm::getGlobalErrorRecoveryManager();
    manager.clear();
}