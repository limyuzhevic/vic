// Error utilities and recovery manager
// Advanced error handling infrastructure

#ifndef NLM_ERROR_UTILITIES_HPP
#define NLM_ERROR_UTILITIES_HPP

#include "ErrorBase.hpp"
#include <unordered_map>
#include <deque>
#include <thread>
#include <future>
#include <sstream>

namespace nlm {

// Error statistics for monitoring
struct ErrorStatistics {
    size_t totalErrors = 0;
    size_t errorsBySeverity[6] = {0}; // Debug, Info, Warning, Error, Critical, Fatal
    size_t errorsByType[20] = {0};    // All ErrorType enum values
    std::chrono::system_clock::time_point firstErrorTime;
    std::chrono::system_clock::time_point lastErrorTime;
    std::map<std::string, size_t> errorsByComponent;
    
    ErrorStatistics() {
        firstErrorTime = std::chrono::system_clock::now();
        lastErrorTime = firstErrorTime;
    }
    
    void recordError(const Error& error) {
        totalErrors++;
        errorsBySeverity[static_cast<size_t>(error.getSeverity())]++;
        errorsByType[static_cast<size_t>(error.getType())]++;
        errorsByComponent[error.getContext().component]++;
        lastErrorTime = std::chrono::system_clock::now();
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Error Statistics:\n";
        oss << "  Total errors: " << totalErrors << "\n";
        oss << "  By severity:\n";
        oss << "    Debug: " << errorsBySeverity[0] << "\n";
        oss << "    Info: " << errorsBySeverity[1] << "\n";
        oss << "    Warning: " << errorsBySeverity[2] << "\n";
        oss << "    Error: " << errorsBySeverity[3] << "\n";
        oss << "    Critical: " << errorsBySeverity[4] << "\n";
        oss << "    Fatal: " << errorsBySeverity[5] << "\n";
        oss << "  By type:\n";
        for (int i = 0; i < 20; ++i) {
            if (errorsByType[i] > 0) {
                oss << "    " << static_cast<ErrorType>(i) << ": " << errorsByType[i] << "\n";
            }
        }
        oss << "  By component:\n";
        for (const auto& [comp, count] : errorsByComponent) {
            oss << "    " << comp << ": " << count << "\n";
        }
        return oss.str();
    }
};

// Circuit breaker state
struct CircuitBreakerState {
    std::string name;
    bool isOpen = false;
    size_t failureCount = 0;
    size_t successCount = 0;
    std::chrono::system_clock::time_point lastFailureTime;
    std::chrono::milliseconds timeout = std::chrono::milliseconds(5000); // 5 seconds
    size_t failureThreshold = 5; // Number of failures before opening
    size_t halfOpenSuccessThreshold = 3; // Successes needed to close
    
    CircuitBreakerState(const std::string& n) : name(n) {}
    
    bool isReady() const {
        if (!isOpen) return true;
        if (std::chrono::system_clock::now() - lastFailureTime > timeout) {
            isOpen = false;
            failureCount = 0;
            return true;
        }
        return false;
    }
    
    void recordFailure() {
        failureCount++;
        lastFailureTime = std::chrono::system_clock::now();
        if (failureCount >= failureThreshold) {
            isOpen = true;
        }
    }
    
    void recordSuccess() {
        if (isOpen) {
            successCount++;
            if (successCount >= halfOpenSuccessThreshold) {
                isOpen = false;
                failureCount = 0;
                successCount = 0;
            }
        }
    }
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() {
        // Initialize with default circuit breakers
        circuitBreakers_["memory_allocator"] = std::make_shared<CircuitBreakerState>("memory_allocator");
        circuitBreakers_["file_io"] = std::make_shared<CircuitBreakerState>("file_io");
        circuitBreakers_["network"] = std::make_shared<CircuitBreakerState>("network");
        circuitBreakers_["database"] = std::make_shared<CircuitBreakerState>("database");
    }
    
    // Circuit breaker control
    bool canExecute(const std::string& circuitName) {
        auto it = circuitBreakers_.find(circuitName);
        if (it == circuitBreakers_.end()) {
            return true; // Unknown circuit, allow execution
        }
        return it->second->isReady();
    }
    
    void recordFailure(const std::string& circuitName) {
        auto it = circuitBreakers_.find(circuitName);
        if (it != circuitBreakers_.end()) {
            it->second->recordFailure();
        }
    }
    
    void recordSuccess(const std::string& circuitName) {
        auto it = circuitBreakers_.find(circuitName);
        if (it != circuitBreakers_.end()) {
            it->second->recordSuccess();
        }
    }
    
    // Error recovery with context
    bool attemptRecovery(std::shared_ptr<Error> error, 
                        const std::map<std::string, std::string>& context = {}) {
        if (!error->canRecover()) {
            return false;
        }
        
        // Find appropriate recovery action based on error type and context
        auto action = findRecoveryAction(error, context);
        if (!action) {
            return false;
        }
        
        return error->attemptRecovery(action);
    }
    
    // Add custom recovery action for error type
    void addRecoveryAction(ErrorType errorType, std::shared_ptr<RecoveryAction> action,
                          RecoveryStrategy strategy = RecoveryStrategy::Retry) {
        recoveryActions_[errorType].push_back({strategy, action});
    }
    
    // Configure circuit breaker
    void configureCircuitBreaker(const std::string& name, 
                                 size_t failureThreshold,
                                 std::chrono::milliseconds timeout,
                                 size_t halfOpenThreshold = 0) {
        auto it = circuitBreakers_.find(name);
        if (it != circuitBreakers_.end()) {
            it->second->failureThreshold = failureThreshold;
            it->second->timeout = timeout;
            if (halfOpenThreshold > 0) {
                it->second->halfOpenSuccessThreshold = halfOpenThreshold;
            }
        }
    }
    
    // Get circuit breaker state
    std::string getCircuitBreakerState(const std::string& name) const {
        auto it = circuitBreakers_.find(name);
        if (it == circuitBreakers_.end()) {
            return "unknown";
        }
        
        std::ostringstream oss;
        oss << "Circuit Breaker: " << it->first << "\n";
        oss << "  State: " << (it->second->isOpen ? "OPEN" : "CLOSED") << "\n";
        oss << "  Failure count: " << it->second->failureCount << "\n";
        oss << "  Success count: " << it->second->successCount << "\n";
        oss << "  Last failure: ";
        if (it->second->lastFailureTime.time_since_epoch().count() > 0) {
            auto time = std::chrono::system_clock::to_time_t(it->second->lastFailureTime);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        } else {
            oss << "Never";
        }
        return oss.str();
    }
    
    // Get error statistics
    const ErrorStatistics& getStatistics() const { return statistics_; }
    
    // Record error for statistics
    void recordError(const Error& error) {
        statistics_.recordError(error);
    }
    
private:
    std::shared_ptr<RecoveryAction> findRecoveryAction(std::shared_ptr<Error> error,
                                                      const std::map<std::string, std::string>& context) {
        ErrorType errorType = error->getType();
        auto it = recoveryActions_.find(errorType);
        if (it == recoveryActions_.end()) {
            return nullptr;
        }
        
        // Try to find suitable action based on context
        for (const auto& actionInfo : it->second) {
            auto& action = actionInfo.action;
            if (action->isAutoApplicable() && 
                action->getRequiredSeverity() >= error->getSeverity()) {
                return action;
            }
        }
        
        return nullptr;
    }
    
    struct RecoveryActionInfo {
        RecoveryStrategy strategy;
        std::shared_ptr<RecoveryAction> action;
    };
    
    std::map<ErrorType, std::vector<RecoveryActionInfo>> recoveryActions_;
    std::map<std::string, std::shared_ptr<CircuitBreakerState>> circuitBreakers_;
    ErrorStatistics statistics_;
};

// Global error recovery manager instance
inline ErrorRecoveryManager& getErrorRecoveryManager() {
    static ErrorRecoveryManager manager;
    return manager;
}

// Error classification utilities
inline std::string errorTypeToString(ErrorType type) {
    static const std::map<ErrorType, std::string> typeStrings = {
        {ErrorType::SimulationError, "SimulationError"},
        {ErrorType::ConfigurationError, "ConfigurationError"},
        {ErrorType::InitializationError, "InitializationError"},
        {ErrorType::ValidationError, "ValidationError"},
        {ErrorType::BrainError, "BrainError"},
        {ErrorType::NeuronError, "NeuronError"},
        {ErrorType::SynapseError, "SynapseError"},
        {ErrorType::MemoryError, "MemoryError"},
        {ErrorType::MemoryAllocationError, "MemoryAllocationError"},
        {ErrorType::FileIOError, "FileIOError"},
        {ErrorType::NetworkError, "NetworkError"},
        {ErrorType::ResourceError, "ResourceError"},
        {ErrorType::InvalidInputError, "InvalidInputError"},
        {ErrorType::UsageError, "UsageError"},
        {ErrorType::PermissionError, "PermissionError"},
        {ErrorType::TimeoutError, "TimeoutError"},
        {ErrorType::ConcurrencyError, "ConcurrencyError"},
        {ErrorType::StateError, "StateError"},
        {ErrorType::UnknownError, "UnknownError"},
        {ErrorType::SystemError, "SystemError"},
        {ErrorType::NotImplementedError, "NotImplementedError"}
    };
    
    auto it = typeStrings.find(type);
    return (it != typeStrings.end()) ? it->second : "UnknownError";
}

inline std::string errorSeverityToString(ErrorSeverity severity) {
    static const std::map<ErrorSeverity, std::string> severityStrings = {
        {ErrorSeverity::Debug, "DEBUG"},
        {ErrorSeverity::Info, "INFO"},
        {ErrorSeverity::Warning, "WARNING"},
        {ErrorSeverity::Error, "ERROR"},
        {ErrorSeverity::Critical, "CRITICAL"},
        {ErrorSeverity::Fatal, "FATAL"}
    };
    
    auto it = severityStrings.find(severity);
    return (it != severityStrings.end()) ? it->second : "UNKNOWN";
}

// Error handling macros and utilities
#define NLM_ERROR_GUARD(component, operation, ...) \
    try { \
        __VA_ARGS__ \
    } catch (const std::exception& e) { \
        translateError(e, component, operation); \
    }

#define NLM_ERROR_RECOVERY(error, context) \
    getErrorRecoveryManager().attemptRecovery(error, context)

#define NLM_CIRCUIT_BREAKER(name) \
    if (!getErrorRecoveryManager().canExecute(name)) { \
        throw Error("Circuit breaker is open for: " + std::string(#name), \
                   ErrorType::ResourceError, ErrorSeverity::Error); \
    }

#define NLM_CIRCUIT_SUCCESS(name) \
    getErrorRecoveryManager().recordSuccess(name)

#define NLM_CIRCUIT_FAILURE(name) \
    getErrorRecoveryManager().recordFailure(name)

#define NLM_RECOVERY_ACTION(action) \
    std::make_shared<action>()

// Performance tracking helper
inline void trackPerformance(std::chrono::milliseconds duration, 
                              size_t memoryUsage,
                              const std::string& operation,
                              const std::string& component = "") {
    // Would integrate with performance logging system
    // For now, just log at debug level
    if (memoryUsage > 0) {
        NLM_LOG_DEBUG("Performance: " + operation + " took " + 
                     std::to_string(duration.count()) + "ms, " +
                     std::to_string(memoryUsage) + " bytes");
    }
}

// Error context enrichment
inline void enrichErrorContext(Error& error,
                               const std::string& file = "",
                               int line = 0,
                               const std::string& function = "",
                               const std::map<std::string, std::string>& extraContext = {}) {
    ErrorContext& ctx = error.getContext();
    if (!file.empty()) ctx.file = file;
    if (line > 0) ctx.line = line;
    if (!function.empty()) ctx.function = function;
    
    for (const auto& [key, value] : extraContext) {
        ctx.parameters[key] = value;
    }
}

// Graceful degradation helper
inline bool degradeGracefully(const std::string& reason,
                             const std::string& component,
                             const std::map<std::string, std::string>& degradedState = {}) {
    NLM_LOG_WARNING("Graceful degradation: " + reason + " in component " + component);
    
    // Would store degraded state somewhere
    // For now, just log and continue
    return true;
}

// Health monitoring helper
inline bool checkHealth(const std::string& component,
                       std::function<bool()> healthCheck,
                       std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
    std::future<bool> result = std::async(std::launch::async, healthCheck);
    auto status = result.wait_for(timeout);
    
    if (status == std::future_status::ready) {
        return result.get();
    } else {
        NLM_LOG_ERROR("Health check timeout for component: " + component);
        return false;
    }
}

} // namespace nlm

#endif // NLM_ERROR_UTILITIES_HPP
