#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <system_error>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <ctime>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Error categories
enum class ErrorCategory {
    Configuration,    // Configuration-related errors
    FileIO,           // File input/output errors
    Validation,       // Parameter validation errors
    Network,          // Network communication errors
    Memory,           // Memory allocation errors
    Simulation,       // Simulation logic errors
    Logger,           // Logging errors
    Random,           // Random number generation errors
    System           // System-level errors
};

// Base exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                 ErrorCategory category = ErrorCategory::System,
                 ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(message), 
          category_(category),
          severity_(severity),
          timestamp_(std::chrono::system_clock::now()) {}
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    virtual std::string what() const noexcept override {
        return std::runtime_error::what();
    }
    
protected:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types for better error handling
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, 
                ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::Configuration, severity) {}
};

class FileIOError : public NLMException {
public:
    FileIOError(const std::string& message, 
                ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::FileIO, severity) {}
};

class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message,
                    ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, ErrorCategory::Validation, severity) {}
};

class LoggerError : public NLMException {
public:
    LoggerError(const std::string& message,
                ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::Logger, severity) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message,
                    ErrorSeverity severity = ErrorSeverity::Critical)
        : NLMException(message, ErrorCategory::Simulation, severity) {}
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void reportError(const NLMException& error) = 0;
    virtual void reportError(const std::string& message,
                           ErrorCategory category = ErrorCategory::System,
                           ErrorSeverity severity = ErrorSeverity::Error) = 0;
    virtual void setSeverityThreshold(ErrorSeverity threshold) = 0;
    virtual ErrorSeverity getSeverityThreshold() const = 0;
};

// Console error reporter (prints to stderr)
class ConsoleErrorReporter : public IErrorReporter {
public:
    ConsoleErrorReporter();
    ~ConsoleErrorReporter() override;
    
    void reportError(const NLMException& error) override;
    void reportError(const std::string& message,
                    ErrorCategory category = ErrorCategory::System,
                    ErrorSeverity severity = ErrorSeverity::Error) override;
    void setSeverityThreshold(ErrorSeverity threshold) override;
    ErrorSeverity getSeverityThreshold() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string categoryToString(ErrorCategory category) const;
    std::string severityToString(ErrorSeverity severity) const;
    std::string errorCodeToString(ErrorCategory category) const;
};

// Error context for detailed error information
struct ErrorContext {
    std::string file;
    int line;
    std::string function;
    std::string component;
    std::unordered_map<std::string, std::string> details;
    
    ErrorContext(const std::string& file = "",
                 int line = 0,
                 const std::string& function = "",
                 const std::string& component = "")
        : file(file), line(line), function(function), component(component) {}
    
    ErrorContext& addDetail(const std::string& key, const std::string& value) {
        details[key] = value;
        return *this;
    }
};

// Global error handling utilities
namespace ErrorHandling {
    // Set global error reporter
    void setGlobalErrorReporter(std::shared_ptr<IErrorReporter> reporter);
    
    // Get global error reporter
    std::shared_ptr<IErrorReporter> getGlobalErrorReporter();
    
    // Report error with context
    void reportError(const std::string& message,
                    const ErrorContext& context = ErrorContext(),
                    ErrorCategory category = ErrorCategory::System,
                    ErrorSeverity severity = ErrorSeverity::Error);
    
    void reportError(const NLMException& error,
                    const ErrorContext& context = ErrorContext());
    
    // Check if error should be reported based on severity threshold
    bool shouldReport(ErrorSeverity severity);
    
    // Create error context from current source location
    ErrorContext createContext(const std::string& component = "");
    
    // Helper functions
    std::string getCurrentTimestampString();
    std::string getTimestampString(std::chrono::system_clock::time_point timestamp);
    
    // Macros for convenient error reporting with source location
    #define NLM_REPORT_ERROR(msg, category, severity) \
        nlm::ErrorHandling::reportError(msg, \
            nlm::ErrorHandling::createContext("__component__"), \
            category, severity)
    
    #define NLM_THROW_ERROR(msg, errorClass, category, severity) \
        do { \
            auto error = errorClass(msg, severity); \
            nlm::ErrorHandling::reportError(error, nlm::ErrorHandling::createContext("__component__")); \
            throw error; \
        } while(0)
    
    #define NLM_VALIDATE(cond, msg) \
        do { \
            if (!(cond)) { \
                nlm::ErrorHandling::reportError(msg, nlm::ErrorHandling::createContext("Validation"), nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Warning); \
                return false; \
            } \
        } while(0)
} // namespace ErrorHandling

// Error recovery callback type
using ErrorRecoveryCallback = std::function<bool(const NLMException&)>;

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager();
    ~ErrorRecoveryManager();
    
    // Register recovery callback for specific error category
    void registerRecoveryCallback(ErrorCategory category, 
                                  ErrorRecoveryCallback callback);
    
    // Attempt to recover from an error
    bool attemptRecovery(const NLMException& error);
    
    // Clear recovery callbacks for a category
    void clearRecoveryCallbacks(ErrorCategory category);
    
    // Check if recovery is available for an error
    bool hasRecovery(ErrorCategory category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global error recovery manager instance
extern ErrorRecoveryManager globalErrorRecoveryManager;

} // namespace nlm

// Convenience macros
#define NLM_THROW_CONFIG_ERROR(msg) nlm::NLM_THROW_ERROR(msg, nlm::ConfigError, nlm::ErrorCategory::Configuration, nlm::ErrorSeverity::Error)
#define NLM_THROW_FILE_ERROR(msg) nlm::NLM_THROW_ERROR(msg, nlm::FileIOError, nlm::ErrorCategory::FileIO, nlm::ErrorSeverity::Error)
#define NLM_THROW_VALIDATION_ERROR(msg) nlm::NLM_THROW_ERROR(msg, nlm::ValidationError, nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Warning)
#define NLM_THROW_LOGGER_ERROR(msg) nlm::NLM_THROW_ERROR(msg, nlm::LoggerError, nlm::ErrorCategory::Logger, nlm::ErrorSeverity::Error)
#define NLM_THROW_SIMULATION_ERROR(msg) nlm::NLM_THROW_ERROR(msg, nlm::SimulationError, nlm::ErrorCategory::Simulation, nlm::ErrorSeverity::Critical)

#define NLM_REPORT_ERROR(msg, category, severity) nlm::ErrorHandling::reportError(msg, nlm::ErrorHandling::createContext(), category, severity)

#define NLM_VALIDATE(cond, msg) nlm::ErrorHandling::NLM_VALIDATE(cond, msg)
