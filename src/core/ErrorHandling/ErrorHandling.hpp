#pragma once

#include "core/Logger/Logger.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>
#include <limits>

namespace nlm {

// Error categories for better error categorization
enum class ErrorCategory {
    Configuration,      // Configuration file parsing/validation errors
    Resource,           // Memory allocation or resource errors
    Initialization,     // System initialization failures
    Validation,         // Parameter validation errors
    Recovery,           // Fallback/recovery mechanism failures
    System             // General system errors
};

// Error severity levels
enum class ErrorSeverity {
    Warning,            // Non-critical issues that don't prevent operation
    Error,              // Critical issues that prevent operation but allow recovery
    Critical,           // Severe issues that prevent startup or require immediate attention
    Fatal               // Unrecoverable errors
};

// Enhanced error information structure
struct ErrorInfo {
    std::string message;              // Human-readable error message
    std::string code;                 // Error code for programmatic handling
    ErrorCategory category;           // Error category
    ErrorSeverity severity;           // Error severity
    std::string file;                 // Source file where error occurred
    int line;                         // Line number where error occurred
    std::string function;             // Function where error occurred
    std::unordered_map<std::string, std::string> context;  // Additional context data
    std::vector<std::string> recoveryActions;             // Possible recovery actions
    bool isHandled;                   // Whether error has been handled
    
    ErrorInfo() : message(), code(), category(ErrorCategory::System), 
                  severity(ErrorSeverity::Error), line(0), isHandled(false) {}
    
    ErrorInfo(const std::string& msg, ErrorCategory cat = ErrorCategory::System,
              ErrorSeverity sev = ErrorSeverity::Error)
        : message(msg), code(), category(cat), severity(sev), 
          line(0), isHandled(false) {}
};

// Exception class for fatal errors
class NeuralException : public std::runtime_error {
public:
    NeuralException(const std::string& message, 
                   ErrorCategory category = ErrorCategory::System,
                   ErrorSeverity severity = ErrorSeverity::Critical)
        : std::runtime_error(message), category_(category), severity_(severity) {}
    
    ErrorCategory category() const { return category_; }
    ErrorSeverity severity() const { return severity_; }
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
};

// Global error handler function type
using ErrorHandler = std::function<void(const ErrorInfo&)>;

// Error handling utilities
class ErrorHandling {
public:
    // Register a global error handler
    static void registerErrorHandler(ErrorHandler handler);
    
    // Handle an error (calls global handler if set, returns whether error is handled)
    static bool handleError(const ErrorInfo& error);
    
    // Create a standard error message format
    static std::string formatError(const ErrorInfo& error);
    
    // Validation utilities
    static bool validateNeuronCount(size_t count, const std::string& context = "");
    static bool validateSynapseWeight(float weight, const std::string& context = "");
    static bool validateSimulationTimestep(double timestep, const std::string& context = "");
    static bool validateConnectionProbability(float prob, const std::string& context = "");
    static bool validateRegionCount(size_t count, const std::string& context = "");
    
    // Recovery utilities
    static std::optional<size_t> suggestFallbackNeuronCount(size_t requested);
    static std::optional<float> suggestFallbackWeight(float requested);
    static std::optional<double> suggestFallbackTimestep(double requested);
    
    // Configuration validation
    static bool validateConfigFileFormat(const std::string& content, const std::string& filepath);
    
    // Safe parameter access with validation
    template<typename T>
    static std::optional<T> safeGetParameter(const Config& config, 
                                             const std::string& key,
                                             const T& defaultValue,
                                             const std::string& context = "",
                                             std::function<bool(const T&)> validator = nullptr);
    
    // Compare error severity
    static bool isMoreSevere(ErrorSeverity sev1, ErrorSeverity sev2);
    
    // Log error with context
    static void logError(const ErrorInfo& error);
    
private:
    static ErrorHandler globalErrorHandler_;
    static const std::unordered_map<ErrorCategory, std::string> categoryNames_;
    static const std::unordered_map<ErrorSeverity, std::string> severityNames_;
};

// Convenience macros for error handling
#define NLM_VALIDATE(condition, errorCode, errorMessage) \
    do { \
        if (!(condition)) { \
            ErrorInfo error(errorMessage, ErrorCategory::Validation, ErrorSeverity::Error); \
            error.code = errorCode; \
            error.file = __FILE__; \
            error.line = __LINE__; \
            error.function = __func__; \
            if (!ErrorHandling::handleError(error)) { \
                NLM_LOG_ERROR(std::string("Validation failed: ") + errorMessage + " (" + errorCode + ")"); \
            } \
        } \
    } while(0)

#define NLM_CHECK_INIT_FAILURE(condition, errorMessage) \
    do { \
        if (!(condition)) { \
            ErrorInfo error(errorMessage, ErrorCategory::Initialization, ErrorSeverity::Error); \
            error.file = __FILE__; \
            error.line = __LINE__; \
            error.function = __func__; \
            if (!ErrorHandling::handleError(error)) { \
                NLM_LOG_ERROR(errorMessage); \
                return false; \
            } \
        } \
    } while(0)

#define NLM_RECOVERABLE_ERROR(errorMessage, recoveryActions) \
    do { \
        ErrorInfo error(errorMessage, ErrorCategory::Recovery, ErrorSeverity::Error); \
        error.recoveryActions = recoveryActions; \
        error.file = __FILE__; \
        error.line = __LINE__; \
        error.function = __func__; \
        ErrorHandling::handleError(error); \
    } while(0)

} // namespace nlm
