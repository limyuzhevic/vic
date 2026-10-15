// Error handling and recovery mechanisms for enhanced NLM Phase 2
// Provides robust error handling with recovery, retry logic, and detailed reporting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <memory>
#include <functional>
#include "core/Logger/Logger.hpp"

using namespace nlm;

// Error categories
enum class ErrorCategory {
    Configuration,
    Memory,
    Simulation,
    IO,
    System,
    Unknown
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Detailed error information structure
struct ErrorInfo {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string code;
    std::string context;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> suggestions;
    bool recoverable;
    int retryCount;
    double retryDelay;
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg, 
              const std::string& code = "", const std::string& context = "")
        : category(cat), severity(sev), message(msg), code(code), context(context),
          timestamp(std::chrono::system_clock::now()), 
          recoverable(true), retryCount(0), retryDelay(1.0) {}
};

// Exception types for different error categories
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::Unknown,
             ErrorSeverity severity = ErrorSeverity::Error, const std::string& code = "")
        : std::runtime_error(message), category(category), severity(severity), code(code) {}
    
    ErrorCategory getCategory() const { return category; }
    ErrorSeverity getSeverity() const { return severity; }
    const std::string& getCode() const { return code; }
    
protected:
    ErrorCategory category;
    ErrorSeverity severity;
    std::string code;
};

class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& code = "CONFIG_ERROR")
        : NLMError(message, ErrorCategory::Configuration, ErrorSeverity::Error, code) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const std::string& code = "MEMORY_ERROR")
        : NLMError(message, ErrorCategory::Memory, ErrorSeverity::Critical, code) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message, const std::string& code = "SIMULATION_ERROR")
        : NLMError(message, ErrorCategory::Simulation, ErrorSeverity::Error, code) {}
};

class IOError : public NLMError {
public:
    IOError(const std::string& message, const std::string& code = "IO_ERROR")
        : NLMError(message, ErrorCategory::IO, ErrorSeverity::Error, code) {}
};

// Error handler class with recovery capabilities
class ErrorHandler {
private:
    std::vector<ErrorInfo> errorHistory;
    std::map<ErrorCategory, int> errorCounts;
    std::chrono::system_clock::time_point sessionStart;
    bool enableRecovery;
    bool enableLogging;
    int maxRetries;
    double retryDelay;
    
public:
    ErrorHandler();
    
    void enableRecovery(bool enabled);
    void enableLogging(bool enabled);
    void setMaxRetries(int retries);
    void setRetryDelay(double delay);
    
    // Handle error with optional recovery
    bool handleError(const ErrorInfo& error);
    
    // Handle exception with context and recovery
    bool handleException(const std::exception& e, const std::string& context = "",
                        const std::map<std::string, std::string>& metadata = {});
    
    // Attempt to recover from error
    bool attemptRecovery(const ErrorInfo& error);
    
    // Generate suggestions based on error
    std::vector<std::string> generateSuggestions(const ErrorInfo& error);
    
    // Determine error category from exception
    ErrorCategory determineErrorCategory(const std::exception& e);
    
    // Determine error severity from exception
    ErrorSeverity determineErrorSeverity(const std::exception& e);
    
    // Check if error is recoverable
    bool isErrorRecoverable(const ErrorInfo& error);
    
    // Log error with appropriate level
    void logError(const ErrorInfo& error);
    
    // Get error statistics
    struct ErrorStatistics {
        int totalErrors = 0;
        int criticalErrors = 0;
        int memoryErrors = 0;
        int configurationErrors = 0;
        int simulationErrors = 0;
        int ioErrors = 0;
        double sessionDuration = 0.0;
    };
    
    ErrorStatistics getStatistics();
    
    // Get error history
    const std::vector<ErrorInfo>& getErrorHistory() const;
    
    // Get error count by category
    int getErrorCount(ErrorCategory category) const;
    
    // Clear error history
    void clearErrorHistory();
    
    // Check if there are any critical errors
    bool hasCriticalErrors() const;
};

// Global error handler instance
extern ErrorHandler globalErrorHandler;