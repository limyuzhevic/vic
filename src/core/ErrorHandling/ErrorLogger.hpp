// Enhanced error handling and logging utilities
// Provides comprehensive error reporting and recovery mechanisms

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <atomic>

namespace nlm {

// Error types for NLM
enum class NLMErrorType {
    Configuration,
    Memory,
    Computation,
    FileIO,
    Network,
    Plasticity,
    Neuromodulation,
    System,
    UserInput,
    Unknown
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

// Enhanced error information structure
struct NLMError {
    NLMErrorType type;
    ErrorSeverity severity;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
    std::map<std::string, std::string> details;
    std::shared_ptr<NLMError> relatedError;
    
    NLMError() : type(NLMErrorType::Unknown), severity(ErrorSeverity::Info),
                 line(0), timestamp(std::chrono::system_clock::now()) {}
    
    NLMError(NLMErrorType t, ErrorSeverity s, const std::string& msg,
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : type(t), severity(s), message(msg), file(file), line(line),
          function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced logger with error handling
class ErrorLogger {
public:
    ErrorLogger();
    ~ErrorLogger();
    
    // Log error with full context
    void logError(const NLMError& error);
    
    // Log error with parameters
    void logError(NLMErrorType type, ErrorSeverity severity,
                  const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::string& context = "");
    
    // Set minimum severity for errors to log
    void setMinimumSeverity(ErrorSeverity severity);
    
    // Get minimum severity
    ErrorSeverity getMinimumSeverity() const;
    
    // Get recent errors
    std::vector<NLMError> getRecentErrors(size_t limit = 100) const;
    
    // Clear recent errors
    void clearErrors();
    
    // Check if error recovery is possible
    bool canRecover(const NLMError& error) const;
    
    // Attempt error recovery
    bool recover(const NLMError& error);
    
    // Set up error recovery callback
    void setRecoveryCallback(const std::function<bool(const NLMError&)>& callback);
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        size_t criticalErrors = 0;
        size_t recoveredErrors = 0;
        std::chrono::system_clock::time_point firstError;
        std::chrono::system_clock::time_point lastError;
    };
    
    ErrorStats getStats() const;
    
    // Generate error report
    std::string generateErrorReport() const;
    
    // Generate system health status
    std::string generateHealthReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global error logger instance
ErrorLogger& getErrorLogger();

// Error handling macros with context
#define NLM_THROW_ERROR(type, severity, message) \
    nlm::getErrorLogger().logError(nlm::NLMError(\
        type, severity, message, __FILE__, __LINE__, __func__\
    ));

#define NLM_THROW_ERROR_WITH_CONTEXT(type, severity, message, context) \
    nlm::getErrorLogger().logError(nlm::NLMError(\
        type, severity, message, __FILE__, __LINE__, __func__\
    )); nlm::NLMError error(\
        type, severity, message, __FILE__, __LINE__, __func__\
    ); error.context = context; nlm::getErrorLogger().logError(error);

#define NLM_LOG_ERROR(type, severity, message) \
    nlm::getErrorLogger().logError(type, severity, message, __FILE__, __LINE__, __func__);

// Error recovery utilities
namespace error_recovery {
    
    // Check if error can be recovered based on type and context
    bool isRecoverable(NLMErrorType type, const std::string& context);
    
    // Attempt to recover from specific error
    bool attemptRecovery(const NLMError& error);
    
    // Get recovery strategies for error type
    std::vector<std::string> getRecoveryStrategies(NLMErrorType type);
    
    // Validate system state after error recovery
    bool validateSystemState();
    
    // Reset system to safe state
    bool resetToSafeState();
}

// System health monitoring
namespace health_monitor {
    
    // System health status
    struct SystemHealth {
        bool isHealthy = true;
        std::vector<std::string> issues;
        std::chrono::system_clock::time_point lastCheck;
        size_t errorCount = 0;
        size_t warningCount = 0;
        double memoryUsage = 0.0;
        double cpuUsage = 0.0;
        double uptime = 0.0;
    };
    
    // Get current system health
    SystemHealth getSystemHealth();
    
    // Check specific system component
    bool checkComponent(const std::string& component);
    
    // Register system component
    void registerComponent(const std::string& name,
                          std::function<bool()> healthCheck);
    
    // Set health check interval
    void setHealthCheckInterval(std::chrono::milliseconds interval);
    
    // Get health check status
    bool isHealthy();
    
    // Generate health report
    std::string generateHealthReport();
}

// Configuration error validation
namespace config_validation {
    
    // Validate configuration value
    bool validateConfigValue(const std::string& key, const std::string& value);
    
    // Validate configuration file
    bool validateConfigFile(const std::string& filepath);
    
    // Get configuration validation errors
    std::vector<std::string> getConfigurationErrors();
    
    // Apply configuration fixes
    std::vector<std::string> applyConfigurationFixes();
}

// Enhanced logging with error context
namespace enhanced_logging {
    
    // Log operation with error context
    void logOperation(const std::string& operation,
                     const std::string& context = "",
                     bool success = true);
    
    // Log error with recovery information
    void logErrorWithRecovery(const NLMError& error);
    
    // Log warning with suggested actions
    void logWarningWithActions(const std::string& warning,
                              const std::vector<std::string>& suggestedActions);
    
    // Log performance metrics
    void logPerformance(const std::string& operation,
                       std::chrono::duration<double> duration,
                       const std::map<std::string, double>& metrics);
}

} // namespace nlm
