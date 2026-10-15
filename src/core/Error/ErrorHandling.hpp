// Main error handling interface for NLM
// Comprehensive error management with structured logging

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include "ErrorBase.hpp"
#include "ErrorUtilities.hpp"
#include "ErrorManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Enhanced error handling namespace with all the advanced features
namespace ErrorHandling {

// Initialize the complete error handling and logging system
inline void initialize(const std::string& logDirectory = "./logs",
                       const LogRotationConfig& rotationConfig = LogRotationConfig()) {
    // Initialize advanced logging system
    getAdvancedLogger().initialize(logDirectory, rotationConfig);
    
    // Start log aggregator
    getLogAggregator().start();
    
    // Configure error recovery manager
    auto& recoveryManager = getErrorRecoveryManager();
    
    // Add default recovery actions
    recoveryManager.addRecoveryAction(ErrorType::ConfigurationError,
                                     std::make_shared<DegradeAction>("limited_mode", {}),
                                     RecoveryStrategy::Degrade);
    
    recoveryManager.addRecoveryAction(ErrorType::MemoryError,
                                     std::make_shared<RetryAction>(3, std::chrono::milliseconds(100)));
    
    recoveryManager.addRecoveryAction(ErrorType::FileIOError,
                                     std::make_shared<DegradeAction>("offline_mode", {}),
                                     RecoveryStrategy::Degrade);
    
    // Configure circuit breakers
    recoveryManager.configureCircuitBreaker("memory_allocator", 5, std::chrono::milliseconds(2000));
    recoveryManager.configureCircuitBreaker("file_io", 3, std::chrono::milliseconds(1000));
    
    // Log initialization
    getAdvancedLogger().logEvent(EventType::SystemStartup, "ErrorHandling", "system");
}

// Shutdown error handling system
inline void shutdown() {
    getAdvancedLogger().logEvent(EventType::SystemShutdown, "ErrorHandling", "system");
    
    getLogAggregator().stop();
    getAdvancedLogger().stop();
}

// Enhanced error throwing with context
inline void throwError(ErrorType type,
                      ErrorSeverity severity,
                      const std::string& component,
                      const std::string& operation,
                      const std::string& message = "") {
    throw Error(type, severity, component, operation, message);
}

// Enhanced error throwing with context
inline void throwError(const Error& error) {
    getAdvancedLogger().logError(error);
    throw;
}

// Graceful error handling with recovery
inline bool handleError(const Error& error,
                       const std::map<std::string, std::string>& context = {}) {
    // Log the error
    getAdvancedLogger().logError(error);
    
    // Attempt recovery
    return getErrorRecoveryManager().attemptRecovery(std::make_shared<Error>(error), context);
}

// Enhanced error checking macro
#define NLM_CHECK_AND_THROW(condition, errorType, severity, component, operation, message) \
    if (!(condition)) { \
        throw Error(errorType, severity, component, operation, message); \
    }

// Enhanced error checking with recovery macro
#define NLM_CHECK_AND_RECOVER(condition, errorType, severity, component, operation, message, recoveryAction) \
    if (!(condition)) { \
        Error error(errorType, severity, component, operation, message); \
        return getErrorRecoveryManager().attemptRecovery(std::make_shared<Error>(error), {}); \
    }

// Structured logging macros
#define NLM_LOG_ERROR_STRUCTURED(errorType, severity, component, operation, message, ...) \
    do { \
        Error error(errorType, severity, component, operation, message, ##__VA_ARGS__); \
        getAdvancedLogger().logError(error); \
    } while(0)

#define NLM_LOG_INFO_STRUCTURED(component, operation, message, ...) \
    do { \
        getAdvancedLogger().logInfo(message, component, operation); \
    } while(0)

#define NLM_LOG_WARNING_STRUCTURED(component, operation, message, ...) \
    do { \
        getAdvancedLogger().logWarning(message, component, operation); \
    } while(0)

// Performance logging macro
#define NLM_PERFORMANCE_LOG(metricName, value, component, unit) \
    getAdvancedLogger().logPerformance(metricName, value, component, unit)

// Event logging macro
#define NLM_EVENT_LOG(eventType, component, source, data) \
    getAdvancedLogger().logEvent(eventType, component, source, data)

// System health check
inline bool checkSystemHealth() {
    auto& recoveryManager = getErrorRecoveryManager();
    
    // Check circuit breakers
    std::string circuitStates;
    for (const auto& [name, breaker] : recoveryManager.getCircuitBreakers()) {
        circuitStates += " " + name + ": " + (breaker->isOpen ? "OPEN" : "CLOSED");
    }
    
    NLM_LOG_INFO_STRUCTURED("system", "health_check", 
                          "System health check completed" + circuitStates);
    
    return true;
}

// Get comprehensive error statistics
inline std::string getErrorStatistics() {
    return getErrorRecoveryManager().getStatistics().getSummary();
}

// Get log analysis results
inline std::string getLogAnalysis(const std::string& component = "") {
    return getAdvancedLogger().analyzeLogs(component).getAnalysisSummary();
}

// Export logs in various formats
inline std::string exportLogs(const std::string& format = "json") {
    return getAdvancedLogger().exportLogs(format);
}

// Graceful degradation helper
inline bool degradeGracefully(const std::string& reason,
                             const std::string& component,
                             const std::map<std::string, std::string>& degradedState = {}) {
    return getAdvancedLogger().logEvent(EventType::Custom, component, "system",
                                       Json::Value::from(degradedState)) && true;
}

// Component-specific error helper methods
inline void throwBrainError(const std::string& operation,
                           const std::string& message,
                           ErrorSeverity severity = ErrorSeverity::Error) {
    throwError(ErrorType::BrainError, severity, "Brain", operation, message);
}

inline void throwNeuronError(const std::string& operation,
                            const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Warning) {
    throwError(ErrorType::NeuronError, severity, "Neuron", operation, message);
}

inline void throwSynapseError(const std::string& operation,
                             const std::string& message,
                             ErrorSeverity severity = ErrorSeverity::Warning) {
    throwError(ErrorType::SynapseError, severity, "Synapse", operation, message);
}

inline void throwMemoryError(const std::string& operation,
                            const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Critical) {
    throwError(ErrorType::MemoryError, severity, "Memory", operation, message);
}

inline void throwConfigurationError(const std::string& operation,
                                   const std::string& message,
                                   ErrorSeverity severity = ErrorSeverity::Warning) {
    throwError(ErrorType::ConfigurationError, severity, "Config", operation, message);
}

inline void throwSimulationError(const std::string& operation,
                                 const std::string& message,
                                 ErrorSeverity severity = ErrorSeverity::Error) {
    throwError(ErrorType::SimulationError, severity, "Simulation", operation, message);
}

// Exception translation helpers
inline Error translateException(const std::exception& e,
                                const std::string& component,
                                const std::string& operation) {
    return translateError(e, component, operation);
}

// Enhanced exception handling
inline void safeExecute(const std::string& component,
                       const std::string& operation,
                       std::function<void()> func,
                       bool shouldLog = true) {
    try {
        func();
        if (shouldLog) {
            NLM_LOG_INFO_STRUCTURED(component, operation, "Operation completed successfully");
        }
    } catch (const Error& e) {
        handleError(e);
    } catch (const std::exception& e) {
        auto error = translateException(e, component, operation);
        handleError(error);
    } catch (...) {
        Error error(ErrorType::UnknownError, ErrorSeverity::Critical,
                   component, operation, "Unknown exception occurred");
        handleError(error);
    }
}

// Circuit breaker helpers
inline bool checkCircuitBreaker(const std::string& circuitName) {
    return getErrorRecoveryManager().canExecute(circuitName);
}

inline void recordCircuitBreakerSuccess(const std::string& circuitName) {
    getErrorRecoveryManager().recordSuccess(circuitName);
}

inline void recordCircuitBreakerFailure(const std::string& circuitName) {
    getErrorRecoveryManager().recordFailure(circuitName);
}

// Performance tracking helper
inline void trackPerformance(const std::chrono::milliseconds& duration,
                             size_t memoryUsage,
                             const std::string& operation,
                             const std::string& component = "") {
    NLM_LOG_INFO_STRUCTURED(component, operation, "Performance: " +
                          std::to_string(duration.count()) + "ms, " +
                          std::to_string(memoryUsage) + " bytes");
}

} // namespace ErrorHandling

// Convenience functions for backward compatibility
inline void initializeErrorHandling(const std::string& logDirectory = "./logs") {
    ErrorHandling::initialize(logDirectory);
}

inline void shutdownErrorHandling() {
    ErrorHandling::shutdown();
}

inline void logSystemError(const Error& error) {
    ErrorHandling::handleError(error);
}

inline std::string getErrorStats() {
    return ErrorHandling::getErrorStatistics();
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
