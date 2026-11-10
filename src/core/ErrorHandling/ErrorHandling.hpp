#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <functional>

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    None,
    Configuration,
    Memory,
    Simulation,
    Network,
    Plasticity,
    MemorySystem,
    Prediction,
    Cognition,
    Neuromodulation,
    Development,
    Checkpoint,
    IO,
    System
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Structured error information
struct ErrorInfo {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string context;
    std::map<std::string, std::string> metadata;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string stackTrace;
    std::shared_ptr<Exception> innerException;
    
    ErrorInfo() : category(ErrorCategory::None), severity(ErrorSeverity::Info) {}
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg, 
              const std::string& ctx = "", const std::map<std::string, std::string>& meta = {})
        : category(cat), severity(sev), message(msg), context(ctx), metadata(meta),
          timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Custom exception classes for NLM
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCategory category = ErrorCategory::None)
        : std::runtime_error(message), category(category) {}
    
    virtual ~NLMException() = default;
    
    ErrorCategory getCategory() const { return category; }
    
protected:
    ErrorCategory category;
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) 
        : NLMException(message, ErrorCategory::Configuration) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) 
        : NLMException(message, ErrorCategory::Memory) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message) 
        : NLMException(message, ErrorCategory::Simulation) {}
};

class NetworkError : public NLMException {
public:
    NetworkError(const std::string& message) 
        : NLMException(message, ErrorCategory::Network) {}
};

class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message) 
        : NLMException(message, ErrorCategory::Plasticity) {}
};

// Error recovery strategies
enum class RecoveryStrategy {
    None,
    Reset,
    Fallback,
    Retry,
    Abort,
    ContinueWithDefault,
    UseCheckpoint,
    PerformRecoveryAction
};

// Recovery action type
using RecoveryAction = std::function<bool(const ErrorInfo&)>;

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    virtual void handleError(const ErrorInfo& error) = 0;
    
    virtual RecoveryStrategy getRecoveryStrategy(const ErrorInfo& error) = 0;
    
    virtual bool performRecovery(const ErrorInfo& error) = 0;
    
    virtual void logError(const ErrorInfo& error) = 0;
};

// Default error handler
class DefaultErrorHandler : public ErrorHandler {
public:
    void handleError(const ErrorInfo& error) override {
        // Log the error based on severity
        switch (error.severity) {
            case ErrorSeverity::Debug:
                logDebugError(error);
                break;
            case ErrorSeverity::Info:
                logInfoError(error);
                break;
            case ErrorSeverity::Warning:
                logWarningError(error);
                break;
            case ErrorSeverity::Error:
                logErrorError(error);
                break;
            case ErrorSeverity::Critical:
                logCriticalError(error);
                break;
            case ErrorSeverity::Fatal:
                logFatalError(error);
                break;
        }
    }
    
    RecoveryStrategy getRecoveryStrategy(const ErrorInfo& error) override {
        switch (error.severity) {
            case ErrorSeverity::Debug:
            case ErrorSeverity::Info:
                return RecoveryStrategy::None;
            case ErrorSeverity::Warning:
                return RecoveryStrategy::ContinueWithDefault;
            case ErrorSeverity::Error:
                return RecoveryStrategy::Reset;
            case ErrorSeverity::Critical:
                return RecoveryStrategy::UseCheckpoint;
            case ErrorSeverity::Fatal:
                return RecoveryStrategy::Abort;
        }
        return RecoveryStrategy::None;
    }
    
    bool performRecovery(const ErrorInfo& error) override {
        switch (getRecoveryStrategy(error)) {
            case RecoveryStrategy::Reset:
                return performResetRecovery(error);
            case RecoveryStrategy::Fallback:
                return performFallbackRecovery(error);
            case RecoveryStrategy::Retry:
                return performRetryRecovery(error);
            case RecoveryStrategy::UseCheckpoint:
                return performCheckpointRecovery(error);
            case RecoveryStrategy::PerformRecoveryAction:
                return performCustomRecovery(error);
            default:
                return false;
        }
    }
    
    void logError(const ErrorInfo& error) override {
        // Redirect to NLM logging system
        switch (error.severity) {
            case ErrorSeverity::Debug:
                NLM_LOG_DEBUG(error.message);
                break;
            case ErrorSeverity::Info:
                NLM_LOG_INFO(error.message);
                break;
            case ErrorSeverity::Warning:
                NLM_LOG_WARNING(error.message);
                break;
            case ErrorSeverity::Error:
                NLM_LOG_ERROR(error.message);
                break;
            case ErrorSeverity::Critical:
                NLM_LOG_ERROR("CRITICAL: " + error.message);
                break;
            case ErrorSeverity::Fatal:
                NLM_LOG_ERROR("FATAL: " + error.message);
                break;
        }
    }
    
protected:
    virtual void logDebugError(const ErrorInfo& error) {
        NLM_LOG_DEBUG("DEBUG: " + error.message);
    }
    
    virtual void logInfoError(const ErrorInfo& error) {
        NLM_LOG_INFO("INFO: " + error.message);
    }
    
    virtual void logWarningError(const ErrorInfo& error) {
        NLM_LOG_WARNING("WARNING: " + error.message);
    }
    
    virtual void logErrorError(const ErrorInfo& error) {
        NLM_LOG_ERROR("ERROR: " + error.message);
    }
    
    virtual void logCriticalError(const ErrorInfo& error) {
        NLM_LOG_ERROR("CRITICAL: " + error.message);
    }
    
    virtual void logFatalError(const ErrorInfo& error) {
        NLM_LOG_ERROR("FATAL: " + error.message);
    }
    
    virtual bool performResetRecovery(const ErrorInfo& error) {
        // Perform a soft reset of the system
        NLM_LOG_INFO("Attempting reset recovery for error: " + error.message);
        // In a real implementation, this would reset the brain state
        return true;
    }
    
    virtual bool performFallbackRecovery(const ErrorInfo& error) {
        // Use fallback values or behaviors
        NLM_LOG_INFO("Attempting fallback recovery for error: " + error.message);
        return true;
    }
    
    virtual bool performRetryRecovery(const ErrorInfo& error) {
        // Retry the operation that failed
        NLM_LOG_INFO("Attempting retry recovery for error: " + error.message);
        return true;
    }
    
    virtual bool performCheckpointRecovery(const ErrorInfo& error) {
        // Load from the last checkpoint
        NLM_LOG_INFO("Attempting checkpoint recovery for error: " + error.message);
        return true;
    }
    
    virtual bool performCustomRecovery(const ErrorInfo& error) {
        // Perform custom recovery logic
        NLM_LOG_INFO("Attempting custom recovery for error: " + error.message);
        return true;
    }
};

// Global error handler instance
inline ErrorHandler& errorHandler = DefaultErrorHandler::getInstance();

// Error reporting utilities
inline void reportError(ErrorCategory category, ErrorSeverity severity, 
                       const std::string& message, const std::string& context = "",
                       const std::map<std::string, std::string>& metadata = {}) {
    ErrorInfo error(category, severity, message, context, metadata);
    errorHandler.handleError(error);
}

inline void reportConfigurationError(const std::string& message, const std::string& context = "") {
    reportError(ErrorCategory::Configuration, ErrorSeverity::Error, message, context);
}

inline void reportMemoryError(const std::string& message, const std::string& context = "") {
    reportError(ErrorCategory::Memory, ErrorSeverity::Error, message, context);
}

inline void reportSimulationError(const std::string& message, const std::string& context = "") {
    reportError(ErrorCategory::Simulation, ErrorSeverity::Error, message, context);
}

inline void reportNetworkError(const std::string& message, const std::string& context = "") {
    reportError(ErrorCategory::Network, ErrorSeverity::Error, message, context);
}

// Exception throwing utilities
inline void throwNLMException(const std::string& message, ErrorCategory category = ErrorCategory::None) {
    throw NLMException(message, category);
}

inline void throwConfigurationException(const std::string& message) {
    throw ConfigurationError(message);
}

inline void throwMemoryException(const std::string& message) {
    throw MemoryError(message);
}

inline void throwSimulationException(const std::string& message) {
    throw SimulationError(message);
}

inline void throwNetworkException(const std::string& message) {
    throw NetworkError(message);
}

// Error recovery utilities
inline bool tryRecover(const std::string& operation, std::function<bool()> operationFunc) {
    try {
        return operationFunc();
    } catch (const NLMException& e) {
        ErrorInfo error(ErrorCategory::System, ErrorSeverity::Error, 
                       "Operation failed: " + std::string(e.what()), operation);
        errorHandler.handleError(error);
        RecoveryStrategy strategy = errorHandler.getRecoveryStrategy(error);
        return errorHandler.performRecovery(error);
    } catch (const std::exception& e) {
        ErrorInfo error(ErrorCategory::System, ErrorSeverity::Error,
                       "System error in operation: " + std::string(e.what()), operation);
        errorHandler.handleError(error);
        return false;
    }
}

// Graceful shutdown utilities
inline void gracefulShutdown(const std::string& reason, ErrorSeverity severity = ErrorSeverity::Fatal) {
    ErrorInfo error(ErrorCategory::System, severity, 
                   "Graceful shutdown: " + reason, "System");
    errorHandler.handleError(error);
    
    // Perform cleanup
    // In a real implementation, this would cleanup all resources
    NLM_LOG_INFO("System shutting down gracefully: " + reason);
}

// Safe execution wrapper
inline bool safeExecute(const std::string& operation, std::function<bool()> operationFunc,
                       bool continueOnError = true) {
    try {
        return operationFunc();
    } catch (const NLMException& e) {
        ErrorInfo error(ErrorCategory::System, ErrorSeverity::Error,
                       "Operation failed: " + std::string(e.what()), operation);
        errorHandler.handleError(error);
        
        if (!continueOnError) {
            return false;
        }
        
        RecoveryStrategy strategy = errorHandler.getRecoveryStrategy(error);
        return errorHandler.performRecovery(error);
    } catch (const std::exception& e) {
        ErrorInfo error(ErrorCategory::System, ErrorSeverity::Error,
                       "System error in operation: " + std::string(e.what()), operation);
        errorHandler.handleError(error);
        
        if (!continueOnError) {
            return false;
        }
        
        return false;
    }
}

// Error context manager
class ErrorContext {
public:
    ErrorContext(const std::string& operation) : operation(operation) {
        startTime = std::chrono::high_resolution_clock::now();
        NLM_LOG_INFO("Starting operation: " + operation);
    }
    
    ~ErrorContext() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        NLM_LOG_INFO("Completed operation: " + operation + " in " + 
                    std::to_string(duration) + "s");
    }
    
    template<typename T>
    ErrorContext& addContext(const std::string& key, const T& value) {
        context[key] = DebugTools::toString(value);
        return *this;
    }
    
    void reportError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        ErrorInfo error(ErrorCategory::System, severity, message, operation, context);
        errorHandler.handleError(error);
    }
    
private:
    std::string operation;
    std::chrono::high_resolution_clock::time_point startTime;
    std::map<std::string, std::string> context;
};

// Error monitoring and statistics
struct ErrorStatistics {
    size_t totalErrors = 0;
    size_t errorsByCategory[static_cast<size_t>(ErrorCategory::System)] = {0};
    size_t errorsBySeverity[static_cast<size_t>(ErrorSeverity::Fatal)] = {0};
    double errorRate = 0.0;
    std::chrono::high_resolution_clock::time_point startTime;
    
    void recordError(const ErrorInfo& error) {
        totalErrors++;
        errorsByCategory[static_cast<size_t>(error.category)]++;
        errorsBySeverity[static_cast<size_t>(error.severity)]++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        errorRate = static_cast<double>(totalErrors) / duration;
    }
    
    std::string getStatisticsReport() const {
        std::ostringstream report;
        report << "Error Statistics:\n";
        report << "  Total Errors: " << totalErrors << "\n";
        report << "  Error Rate: " << errorRate << " per second\n";
        
        report << "\nBy Category:\n";
        for (size_t i = 0; i < static_cast<size_t>(ErrorCategory::System); ++i) {
            if (errorsByCategory[i] > 0) {
                report << "    " << static_cast<size_t>(ErrorCategory(i)) << ": " 
                       << errorsByCategory[i] << "\n";
            }
        }
        
        report << "\nBy Severity:\n";
        for (size_t i = 0; i < static_cast<size_t>(ErrorSeverity::Fatal); ++i) {
            if (errorsBySeverity[i] > 0) {
                report << "    " << static_cast<size_t>(ErrorSeverity(i)) << ": " 
                       << errorsBySeverity[i] << "\n";
            }
        }
        
        return report.str();
    }
};

inline ErrorStatistics& getErrorStats() {
    static ErrorStatistics stats;
    return stats;
}

// Error monitoring wrapper
inline void monitorError(const ErrorInfo& error) {
    getErrorStats().recordError(error);
    errorHandler.handleError(error);
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H
