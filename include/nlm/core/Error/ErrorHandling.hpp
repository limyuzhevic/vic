// Comprehensive Error Handling System for NLM
// Advanced error hierarchy with categories, codes, and recovery mechanisms

#include "core/Error/ErrorHandling.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>
#include <chrono>
#include <stacktrace>

namespace nlm {

// Error code categories
enum class ErrorCategory {
    None = 0,
    System = 1,           // System-level errors (memory, OS, etc.)
    Configuration = 2,     // Configuration errors
    Neural = 3,           // Neural network errors
    Simulation = 4,       // Simulation-specific errors
    Memory = 5,           // Memory management errors
    IO = 6,               // Input/output errors
    Network = 7,          // Network/communication errors
    Plasticity = 8,       // Plasticity rule errors
    Development = 9,      // Development system errors
    Neuromodulation = 10, // Neuromodulation errors
    MemorySystem = 11,    // Memory system errors
    Prediction = 12,      // Prediction system errors
    Cognition = 13,       // Cognition system errors
    Validation = 14,      // Data validation errors
    Performance = 15,     // Performance monitoring errors
    Security = 16,        // Security-related errors
    File = 17,            // File system errors
    Resource = 18,       // Resource management errors
    Plugin = 19,          // Plugin/extension errors
    User = 20,            // User input errors
    Experimental = 21     // Experimental/unknown errors
};

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,          // Debug information
    Info = 1,           // Informational messages
    Warning = 2,        // Warning conditions
    Error = 3,          // Error conditions
    Critical = 4,       // Critical errors that may cause failure
    Fatal = 5           // Fatal errors that require immediate attention
};

// Error recovery actions
enum class ErrorRecoveryAction {
    None = 0,           // No recovery action
    Retry = 1,          // Retry the operation
    RetryWithBackoff = 2, // Retry with exponential backoff
    Skip = 3,          // Skip the problematic operation
    GracefulDegrade = 4, // Degrade functionality gracefully
    Revert = 5,        // Revert to previous state
    Shutdown = 6,      // Shutdown system
    ReportAndExit = 7,  // Report error and exit
    Continue = 8,      // Continue with degraded functionality
    LogAndContinue = 9, // Log error and continue
    Alert = 10,        // Send alert notification
    Restart = 11,      // Restart affected component
    Fallforward = 12    // Continue with next available path
};

// Detailed error information structure
struct ErrorDetails {
    std::string errorCode;
    std::string message;
    std::string description;
    std::string suggestion;
    std::string context;
    ErrorCategory category;
    ErrorSeverity severity;
    ErrorRecoveryAction recoveryAction;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    std::vector<std::string> relatedErrors;
    
    ErrorDetails() : line(0), 
        category(ErrorCategory::None), 
        severity(ErrorSeverity::Error), 
        recoveryAction(ErrorRecoveryAction::None) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorDetails(const std::string& code, const std::string& msg, ErrorCategory cat = ErrorCategory::None,
                 ErrorSeverity sev = ErrorSeverity::Error, ErrorRecoveryAction recovery = ErrorRecoveryAction::None)
        : errorCode(code), message(msg), category(cat), severity(sev), recoveryAction(recovery),
          line(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual void handleError(const ErrorDetails& error) = 0;
    virtual void handleWarning(const std::string& message) = 0;
    virtual void handleInfo(const std::string& message) = 0;
    virtual bool canHandle(ErrorCategory category) const = 0;
    virtual ErrorRecoveryAction getRecoveryAction(const ErrorDetails& error) const = 0;
    virtual bool shouldLog(ErrorCategory category, ErrorSeverity severity) const = 0;
    virtual std::string formatError(const ErrorDetails& error) const = 0;
};

// Default error handler
class DefaultErrorHandler : public ErrorHandler {
public:
    void handleError(const ErrorDetails& error) override {
        if (shouldLog(error.category, error.severity)) {
            logError(error);
        }
        
        if (error.severity == ErrorSeverity::Fatal || error.severity == ErrorSeverity::Critical) {
            logMessage("FATAL ERROR: " + error.message, LogLevel::Error);
            if (!error.suggestion.empty()) {
                logMessage("SUGGESTION: " + error.suggestion, LogLevel::Info);
            }
        }
    }
    
    void handleWarning(const std::string& message) override {
        if (shouldLog(ErrorCategory::None, ErrorSeverity::Warning)) {
            logMessage("WARNING: " + message, LogLevel::Warning);
        }
    }
    
    void handleInfo(const std::string& message) override {
        if (shouldLog(ErrorCategory::None, ErrorSeverity::Info)) {
            logMessage("INFO: " + message, LogLevel::Info);
        }
    }
    
    bool canHandle(ErrorCategory category) const override {
        return true; // Default handler handles all categories
    }
    
    ErrorRecoveryAction getRecoveryAction(const ErrorDetails& error) const override {
        return error.recoveryAction;
    }
    
    bool shouldLog(ErrorCategory category, ErrorSeverity severity) const override {
        if (severity == ErrorSeverity::Debug) {
            return debugMode;
        }
        return severity >= minLogSeverity;
    }
    
    std::string formatError(const ErrorDetails& error) const override {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(error.severity) << "] ";
        oss << error.message;
        if (!error.errorCode.empty()) {
            oss << " (Error Code: " << error.errorCode << ")";
        }
        return oss.str();
    }
    
    void setDebugMode(bool enabled) {
        debugMode = enabled;
    }
    
    void setMinLogSeverity(ErrorSeverity severity) {
        minLogSeverity = severity;
    }
    
protected:
    virtual void logMessage(const std::string& message, LogLevel level) {
        std::cout << message << std::endl;
    }
    
    virtual void logError(const ErrorDetails& error) {
        logMessage(formatError(error), LogLevel::Error);
        if (!error.file.empty()) {
            logMessage("  File: " + error.file, LogLevel::Debug);
        }
        if (error.line > 0) {
            logMessage("  Line: " + std::to_string(error.line), LogLevel::Debug);
        }
        if (!error.function.empty()) {
            logMessage("  Function: " + error.function, LogLevel::Debug);
        }
        if (!error.description.empty()) {
            logMessage("  Description: " + error.description, LogLevel::Debug);
        }
        if (!error.suggestion.empty()) {
            logMessage("  Suggestion: " + error.suggestion, LogLevel::Info);
        }
        if (!error.stackTrace.empty()) {
            logMessage("  Stack Trace:", LogLevel::Debug);
            for (const auto& frame : error.stackTrace) {
                logMessage("    " + frame, LogLevel::Debug);
            }
        }
    }
    
private:
    bool debugMode = false;
    ErrorSeverity minLogSeverity = ErrorSeverity::Error;
};

// Error classification utility
class ErrorClassifier {
public:
    static ErrorCategory classifyException(const std::exception& e) {
        std::string message = e.what();
        std::string lowerMessage = toLower(message);
        
        if (lowerMessage.find("memory") != std::string::npos || 
            lowerMessage.find("allocation") != std::string::npos ||
            lowerMessage.find("out of memory") != std::string::npos) {
            return ErrorCategory::Memory;
        } else if (lowerMessage.find("configuration") != std::string::npos ||
                   lowerMessage.find("parse") != std::string::npos ||
                   lowerMessage.find("invalid") != std::string::npos) {
            return ErrorCategory::Configuration;
        } else if (lowerMessage.find("neural") != std::string::npos ||
                   lowerMessage.find("synapse") != std::string::npos ||
                   lowerMessage.find("neuron") != std::string::npos) {
            return ErrorCategory::Neural;
        } else if (lowerMessage.find("file") != std::string::npos ||
                   lowerMessage.find("io") != std::string::npos ||
                   lowerMessage.find("read") != std::string::npos ||
                   lowerMessage.find("write") != std::string::npos) {
            return ErrorCategory::IO;
        } else if (lowerMessage.find("plasticit") != std::string::npos) {
            return ErrorCategory::Plasticity;
        } else if (lowerMessage.find("development") != std::string::npos) {
            return ErrorCategory::Development;
        } else if (lowerMessage.find("neuromod") != std::string::npos) {
            return ErrorCategory::Neuromodulation;
        } else if (lowerMessage.find("memory") != std::string::npos && 
                   (lowerMessage.find("system") != std::string::npos || lowerMessage.find("episol") != std::string::npos)) {
            return ErrorCategory::MemorySystem;
        } else if (lowerMessage.find("predict") != std::string::npos) {
            return ErrorCategory::Prediction;
        } else if (lowerMessage.find("cognit") != std::string::npos) {
            return ErrorCategory::Cognition;
        } else if (lowerMessage.find("validate") != std::string::npos) {
            return ErrorCategory::Validation;
        } else if (lowerMessage.find("performance") != std::string::npos) {
            return ErrorCategory::Performance;
        } else if (lowerMessage.find("security") != std::string::npos) {
            return ErrorCategory::Security;
        } else if (lowerMessage.find("network") != std::string::npos) {
            return ErrorCategory::Network;
        } else {
            return ErrorCategory::Experimental;
        }
    }
    
    static ErrorSeverity classifySeverity(const std::exception& e) {
        std::string message = e.what();
        std::string lowerMessage = toLower(message);
        
        if (lowerMessage.find("fatal") != std::string::npos ||
            lowerMessage.find("critical") != std::string::npos ||
            lowerMessage.find("out of memory") != std::string::npos ||
            lowerMessage.find("segmentation fault") != std::string::npos) {
            return ErrorSeverity::Fatal;
        } else if (lowerMessage.find("error") != std::string::npos) {
            return ErrorSeverity::Error;
        } else if (lowerMessage.find("warning") != std::string::npos) {
            return ErrorSeverity::Warning;
        } else {
            return ErrorSeverity::Info;
        }
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() : retryCount(0), maxRetries(3), backoffMultiplier(2.0), currentBackoff(1.0) {}
    
    ErrorRecoveryAction determineRecoveryAction(const ErrorDetails& error, const std::exception& exception) {
        // Check for retryable errors
        if (isRetryable(error, exception)) {
            retryCount++;
            if (retryCount <= maxRetries) {
                return ErrorRecoveryAction::RetryWithBackoff;
            }
        }
        
        // Check for skipable errors
        if (isSkippable(error, exception)) {
            return ErrorRecoveryAction::Skip;
        }
        
        // Check for graceful degrade errors
        if (isGracefulDegradeError(error, exception)) {
            return ErrorRecoveryAction::GracefulDegrade;
        }
        
        // Check for revert errors
        if (isRevertable(error, exception)) {
            return ErrorRecoveryAction::Revert;
        }
        
        // Check for shutdown errors
        if (isShutdownRequired(error, exception)) {
            return ErrorRecoveryAction::Shutdown;
        }
        
        // Default recovery action
        return error.recoveryAction;
    }
    
    void resetRetryCount() {
        retryCount = 0;
        currentBackoff = 1.0;
    }
    
    double getCurrentBackoff() const {
        return currentBackoff;
    }
    
    void increaseBackoff() {
        currentBackoff *= backoffMultiplier;
    }
    
    void setMaxRetries(int retries) {
        maxRetries = retries;
    }
    
    void setBackoffMultiplier(double multiplier) {
        backoffMultiplier = multiplier;
    }
    
protected:
    virtual bool isRetryable(const ErrorDetails& error, const std::exception& exception) const {
        std::string message = exception.what();
        std::string lowerMessage = toLower(message);
        
        // Check for transient errors
        if (lowerMessage.find("timeout") != std::string::npos ||
            lowerMessage.find("temporary") != std::string::npos ||
            lowerMessage.find("transient") != std::string::npos ||
            lowerMessage.find("retry") != std::string::npos) {
            return true;
        }
        
        // Check for network-related errors
        if (error.category == ErrorCategory::Network ||
            error.category == ErrorCategory::IO) {
            return true;
        }
        
        return false;
    }
    
    virtual bool isSkippable(const ErrorDetails& error, const std::exception& exception) const {
        std::string message = exception.what();
        std::string lowerMessage = toLower(message);
        
        // Check for non-critical errors
        if (lowerMessage.find("not implemented") != std::string::npos ||
            lowerMessage.find("unsupported") != std::string::npos ||
            lowerMessage.find("feature not available") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    virtual bool isGracefulDegradeError(const ErrorDetails& error, const std::exception& exception) const {
        std::string message = exception.what();
        std::string lowerMessage = toLower(message);
        
        // Check for performance-related errors
        if (lowerMessage.find("performance") != std::string::npos ||
            lowerMessage.find("slow") != std::string::npos ||
            lowerMessage.find("resource") != std::string::npos) {
            return true;
        }
        
        // Check for optional features
        if (error.category == ErrorCategory::None && 
            error.recoveryAction == ErrorRecoveryAction::GracefulDegrade) {
            return true;
        }
        
        return false;
    }
    
    virtual bool isRevertable(const ErrorDetails& error, const std::exception& exception) const {
        std::string message = exception.what();
        std::string lowerMessage = toLower(message);
        
        // Check for state-related errors
        if (lowerMessage.find("state") != std::string::npos &&
            (lowerMessage.find("corrupt") != std::string::npos ||
             lowerMessage.find("invalid") != std::string::npos)) {
            return true;
        }
        
        return false;
    }
    
    virtual bool isShutdownRequired(const ErrorDetails& error, const std::exception& exception) const {
        std::string message = exception.what();
        std::string lowerMessage = toLower(message);
        
        // Check for fatal system errors
        if (lowerMessage.find("fatal") != std::string::npos ||
            lowerMessage.find("critical") != std::string::npos ||
            lowerMessage.find("system failure") != std::string::npos ||
            lowerMessage.find("panic") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    virtual std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
private:
    int retryCount;
    int maxRetries;
    double backoffMultiplier;
    double currentBackoff;
};

// Error reporting utilities
class ErrorReporter {
public:
    static std::string generateErrorReport(const std::vector<ErrorDetails>& errors) {
        std::ostringstream oss;
        oss << "ERROR REPORT" << std::endl;
        oss << "=============" << std::endl;
        oss << "Generated: " << getCurrentTimestamp() << std::endl;
        oss << "Total Errors: " << errors.size() << std::endl;
        oss << std::endl;
        
        // Group errors by category and severity
        std::map<ErrorCategory, std::vector<ErrorDetails>> categorizedErrors;
        for (const auto& error : errors) {
            categorizedErrors[error.category].push_back(error);
        }
        
        // Report by category
        for (const auto& [category, categoryErrors] : categorizedErrors) {
            oss << "Category: " << static_cast<int>(category) << " (" << categoryErrors.size() << " errors)" << std::endl;
            
            // Group by severity
            std::map<ErrorSeverity, std::vector<ErrorDetails>> severityErrors;
            for (const auto& error : categoryErrors) {
                severityErrors[error.severity].push_back(error);
            }
            
            for (const auto& [severity, sevErrors] : severityErrors) {
                oss << "  Severity: " << static_cast<int>(severity) << " (" << sevErrors.size() << " errors)" << std::endl;
                for (const auto& error : sevErrors) {
                    oss << "    " << error.errorCode << ": " << error.message << std::endl;
                }
            }
            oss << std::endl;
        }
        
        // Generate summary
        oss << "SUMMARY:" << std::endl;
        oss << "  Fatal Errors: " << countErrorsBySeverity(errors, ErrorSeverity::Fatal) << std::endl;
        oss << "  Critical Errors: " << countErrorsBySeverity(errors, ErrorSeverity::Critical) << std::endl;
        oss << "  Errors: " << countErrorsBySeverity(errors, ErrorSeverity::Error) << std::endl;
        oss << "  Warnings: " << countErrorsBySeverity(errors, ErrorSeverity::Warning) << std::endl;
        oss << "  Info Messages: " << countErrorsBySeverity(errors, ErrorSeverity::Info) << std::endl;
        
        return oss.str();
    }
    
    static std::string generateErrorCode(const ErrorDetails& error) {
        std::ostringstream oss;
        
        // Use category (2 digits) and severity (1 digit)
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(error.category % 100);
        oss << std::setw(1) << static_cast<int>(error.severity);
        
        // Add hash of message for uniqueness
        std::hash<std::string> hasher;
        size_t hash = hasher(error.message);
        oss << std::setw(4) << std::setfill('0') << (hash % 10000);
        
        return oss.str();
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto inTimeT = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&inTimeT), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
private:
    static size_t countErrorsBySeverity(const std::vector<ErrorDetails>& errors, ErrorSeverity severity) {
        return std::count_if(errors.begin(), errors.end(),
            [severity](const ErrorDetails& error) { return error.severity == severity; });
    }
};

// Global error handling infrastructure
class GlobalErrorHandling {
public:
    static GlobalErrorHandling& getInstance() {
        static GlobalErrorHandling instance;
        return instance;
    }
    
    void registerErrorHandler(std::unique_ptr<ErrorHandler> handler) {
        errorHandlers.push_back(std::move(handler));
    }
    
    void unregisterErrorHandler(ErrorHandler* handler) {
        errorHandlers.erase(
            std::remove_if(errorHandlers.begin(), errorHandlers.end(),
                [handler](const std::unique_ptr<ErrorHandler>& h) { return h.get() == handler; }),
            errorHandlers.end()
        );
    }
    
    void setDefaultErrorHandler(std::unique_ptr<ErrorHandler> handler) {
        defaultErrorHandler = std::move(handler);
    }
    
    void setRecoveryManager(std::unique_ptr<ErrorRecoveryManager> manager) {
        recoveryManager = std::move(manager);
    }
    
    void handleError(const ErrorDetails& error) {
        ErrorDetails detailedError = error;
        detailedError.errorCode = ErrorReporter::generateErrorCode(detailedError);
        detailedError.timestamp = std::chrono::system_clock::now();
        
        // Get recovery action
        ErrorRecoveryAction recoveryAction = ErrorRecoveryAction::None;
        if (recoveryManager) {
            recoveryAction = recoveryManager->determineRecoveryAction(detailedError, std::runtime_error(error.message));
        } else {
            recoveryAction = detailedError.recoveryAction;
        }
        
        // Apply recovery action
        switch (recoveryAction) {
            case ErrorRecoveryAction::Retry:
            case ErrorRecoveryAction::RetryWithBackoff:
                // Implementation would be added here
                break;
            case ErrorRecoveryAction::Skip:
                // Skip the operation
                break;
            case ErrorRecoveryAction::GracefulDegrade:
                // Continue with degraded functionality
                break;
            case ErrorRecoveryAction::Revert:
                // Revert to previous state
                break;
            case ErrorRecoveryAction::Shutdown:
                // Shutdown system
                shutdown();
                break;
            case ErrorRecoveryAction::ReportAndExit:
                // Report error and exit
                reportAndExit(detailedError);
                break;
            case ErrorRecoveryAction::Continue:
                // Continue with degraded functionality
                break;
            case ErrorRecoveryAction::LogAndContinue:
                // Log error and continue
                if (defaultErrorHandler) {
                    defaultErrorHandler->handleError(detailedError);
                }
                break;
            case ErrorRecoveryAction::Alert:
                // Send alert notification
                sendAlert(detailedError);
                break;
            case ErrorRecoveryAction::Restart:
                // Restart affected component
                restartComponent(detailedError);
                break;
            case ErrorRecoveryAction::Fallforward:
                // Continue with next available path
                fallforward(detailedError);
                break;
            default:
                // No recovery action
                if (defaultErrorHandler) {
                    defaultErrorHandler->handleError(detailedError);
                }
                break;
        }
    }
    
    void handleWarning(const std::string& message) {
        ErrorDetails error;
        error.message = message;
        error.severity = ErrorSeverity::Warning;
        error.category = ErrorCategory::None;
        
        if (defaultErrorHandler) {
            defaultErrorHandler->handleWarning(error);
        }
    }
    
    void handleInfo(const std::string& message) {
        ErrorDetails error;
        error.message = message;
        error.severity = ErrorSeverity::Info;
        error.category = ErrorCategory::None;
        
        if (defaultErrorHandler) {
            defaultErrorHandler->handleInfo(error);
        }
    }
    
    void addError(const ErrorDetails& error) {
        errorList.push_back(error);
    }
    
    std::vector<ErrorDetails> getErrors() const {
        return errorList;
    }
    
    void clearErrors() {
        errorList.clear();
    }
    
    size_t getErrorCount() const {
        return errorList.size();
    }
    
    std::string generateReport() const {
        return ErrorReporter::generateErrorReport(errorList);
    }
    
    void setDebugMode(bool enabled) {
        if (defaultErrorHandler) {
            auto defaultHandler = std::dynamic_pointer_cast<DefaultErrorHandler>(defaultErrorHandler);
            if (defaultHandler) {
                defaultHandler->setDebugMode(enabled);
            }
        }
    }
    
    void setMinLogSeverity(ErrorSeverity severity) {
        if (defaultErrorHandler) {
            auto defaultHandler = std::dynamic_pointer_cast<DefaultErrorHandler>(defaultErrorHandler);
            if (defaultHandler) {
                defaultHandler->setMinLogSeverity(severity);
            }
        }
    }
    
protected:
    virtual void shutdown() {
        NLM_LOG_FATAL("System shutdown requested due to fatal error");
        std::exit(1);
    }
    
    virtual void reportAndExit(const ErrorDetails& error) {
        NLM_LOG_FATAL("Fatal error: " + error.message);
        if (defaultErrorHandler) {
            defaultErrorHandler->handleError(error);
        }
        std::exit(1);
    }
    
    virtual void sendAlert(const ErrorDetails& error) {
        // Implementation for sending alerts (email, webhook, etc.)
        NLM_LOG_WARNING("ALERT: " + error.message);
    }
    
    virtual void restartComponent(const ErrorDetails& error) {
        // Implementation for restarting affected component
        NLM_LOG_WARNING("RESTART: Restarting component due to error: " + error.message);
    }
    
    virtual void fallforward(const ErrorDetails& error) {
        // Implementation for falling forward to next available path
        NLM_LOG_WARNING("FALLFORWARD: Continuing with alternate path due to error: " + error.message);
    }
    
private:
    GlobalErrorHandling() : recoveryManager(std::make_unique<ErrorRecoveryManager>()) {
        registerErrorHandler(std::make_unique<DefaultErrorHandler>());
    }
    
    std::vector<std::unique_ptr<ErrorHandler>> errorHandlers;
    std::unique_ptr<ErrorHandler> defaultErrorHandler;
    std::unique_ptr<ErrorRecoveryManager> recoveryManager;
    std::vector<ErrorDetails> errorList;
    
    GlobalErrorHandling(const GlobalErrorHandling&) = delete;
    GlobalErrorHandling& operator=(const GlobalErrorHandling&) = delete;
};

// Convenience functions for error handling
void logError(const std::string& message, ErrorCategory category = ErrorCategory::None, 
              ErrorSeverity severity = ErrorSeverity::Error, ErrorRecoveryAction recovery = ErrorRecoveryAction::None) {
    ErrorDetails error("ERR_" + std::to_string(static_cast<int>(category)), message, category, severity, recovery);
    GlobalErrorHandling::getInstance().handleError(error);
}

void logWarning(const std::string& message, ErrorCategory category = ErrorCategory::None) {
    GlobalErrorHandling::getInstance().handleWarning(message);
}

void logInfo(const std::string& message, ErrorCategory category = ErrorCategory::None) {
    GlobalErrorHandling::getInstance().handleInfo(message);
}

void addSystemError(const std::exception& exception, const std::string& context = "") {
    ErrorDetails error;
    error.message = exception.what();
    error.description = context;
    error.category = ErrorClassifier::classifyException(exception);
    error.severity = ErrorClassifier::classifySeverity(exception);
    error.file = "Unknown";
    error.line = 0;
    error.function = "Unknown";
    
    GlobalErrorHandling::getInstance().handleError(error);
}

void setErrorDebugMode(bool enabled) {
    GlobalErrorHandling::getInstance().setDebugMode(enabled);
}

void setErrorMinLogSeverity(ErrorSeverity severity) {
    GlobalErrorHandling::getInstance().setMinLogSeverity(severity);
}

} // namespace nlm
