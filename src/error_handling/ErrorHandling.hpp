// Error Handling and User Feedback System for NLM
// Comprehensive error handling, user feedback, and reporting system

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>
#include <exception>

using namespace nlm;

// Error severity levels
enum class ErrorSeverity {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
    FATAL = 5
};

// Error categories
enum class ErrorCategory {
    SYSTEM_INITIALIZATION,
    CONFIGURATION,
    NEURAL_COMPUTATION,
    MEMORY_ALLOCATION,
    FILE_IO,
    NETWORK,
    PLUGIN_LOADING,
    SIMULATION_STEP,
    MOTOR_OUTPUT,
    SENSORY_INPUT,
    DEVELOPMENT,
    NEUROMODULATION,
    MEMORY_SYSTEM,
    PLANNING,
    ATTENTION,
    CONCEPT_FORMATION,
    PREDICTION,
    USER_INPUT,
    PERFORMANCE,
    UNKNOWN
};

// Error type
struct ErrorType {
    ErrorCategory category;
    std::string code;
    std::string description;
    std::string suggestedAction;
};

// User feedback levels
enum class FeedbackLevel {
    SILENT,
    ERROR_ONLY,
    INFO,
    PROGRESS,
    DETAILED
};

// User action request
enum class UserAction {
    CONTINUE,
    STOP,
    RETRY,
    CANCEL,
    DEBUG,
    RECONFIGURE,
    CONTACT_SUPPORT
};

// Error context information
struct ErrorContext {
    std::string operation;
    std::string location;
    std::string timestamp;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> systemState;
    std::vector<std::string> recentOperations;
};

// User feedback message
struct UserFeedback {
    std::string message;
    ErrorSeverity severity;
    FeedbackLevel level;
    UserAction action;
    std::string timestamp;
    bool requiresUserInput;
    std::map<std::string, std::string> metadata;
};

// Error exception class
class NLMError : public std::exception {
public:
    NLMError(ErrorType type, ErrorContext context, const std::string& details = "")
        : type_(type), context_(context), details_(details) {
        timestamp_ = getCurrentTimestamp();
        severity_ = determineSeverity(type);
    }

    ErrorType getType() const { return type_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const ErrorContext& getContext() const { return context_; }
    const std::string& getDetails() const { return details_; }
    const std::string& getTimestamp() const { return timestamp_; }
    
    const char* what() const noexcept override {
        return formattedMessage_.c_str();
    }

    std::string getFormattedMessage() const {
        return formattedMessage_;
    }

protected:
    void generateFormattedMessage() {
        std::stringstream ss;
        ss << "[" << timestamp_ << "] " << getSeverityString(severity_) << ": ";
        ss << type_.description;
        if (!type_.code.empty()) {
            ss << " (" << type_.code << ")";
        }
        ss << " - " << details_;
        if (!context_.operation.empty()) {
            ss << "\n  Operation: " << context_.operation;
            if (!context_.location.empty()) {
                ss << " (" << context_.location << ")";
            }
        }
        if (!type_.suggestedAction.empty()) {
            ss << "\n  Suggested action: " << type_.suggestedAction;
        }
        
        formattedMessage_ = ss.str();
    }

    ErrorSeverity determineSeverity(ErrorType type) {
        // Determine severity based on error category and type
        if (type.category == ErrorCategory::SYSTEM_INITIALIZATION ||
            type.category == ErrorCategory::MEMORY_ALLOCATION) {
            return ErrorSeverity::CRITICAL;
        } else if (type.category == ErrorCategory::NEURAL_COMPUTATION ||
                   type.category == ErrorCategory::SIMULATION_STEP) {
            return ErrorSeverity::ERROR;
        } else if (type.category == ErrorCategory::CONFIGURATION ||
                   type.category == ErrorCategory::FILE_IO) {
            return ErrorSeverity::WARNING;
        } else {
            return ErrorSeverity::INFO;
        }
    }

    std::string getSeverityString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::DEBUG: return "DEBUG";
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            case ErrorSeverity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

private:
    ErrorType type_;
    ErrorContext context_;
    std::string details_;
    std::string timestamp_;
    ErrorSeverity severity_;
    mutable std::string formattedMessage_;
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const NLMError& error) = 0;
    virtual UserAction getUserAction(const NLMError& error) = 0;
    virtual bool shouldContinue(const NLMError& error) = 0;
    virtual void logError(const NLMError& error) = 0;
    virtual void displayError(const NLMError& error) = 0;
};

// User feedback system
class UserFeedbackSystem {
public:
    UserFeedbackSystem();
    ~UserFeedbackSystem();

    // Configure feedback level
    void setFeedbackLevel(FeedbackLevel level);
    FeedbackLevel getFeedbackLevel() const;

    // Add error handler
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler);

    // Handle errors
    UserAction handleError(const NLMError& error);
    void reportError(const NLMError& error);

    // Get feedback history
    std::vector<UserFeedback> getFeedbackHistory() const;
    std::vector<NLMError> getErrorHistory() const;

    // Clear history
    void clearHistory();

    // Configuration
    void enableConsoleOutput(bool enable);
    void enableFileOutput(const std::string& filename);
    void enableLogRotation(size_t maxSizeMB, size_t maxFiles);

    // Statistics
    size_t getTotalErrors() const;
    size_t getErrorsBySeverity(ErrorSeverity severity) const;
    double getErrorRate() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void processFeedback(const UserFeedback& feedback);
    void writeToFile(const UserFeedback& feedback);
    std::string formatFeedback(const UserFeedback& feedback) const;
};

// Error recovery system
class ErrorRecoverySystem {
public:
    ErrorRecoverySystem();
    ~ErrorRecoverySystem();

    // Register recovery strategies
    void registerRecoveryStrategy(ErrorCategory category, 
                                  std::function<UserAction(const NLMError&)> strategy);
    void registerRecoveryStrategy(ErrorType type,
                                  std::function<UserAction(const NLMError&)> strategy);

    // Handle error with recovery
    UserAction handleWithRecovery(const NLMError& error);

    // Check if error is recoverable
    bool isRecoverable(const NLMError& error) const;

    // Configure recovery
    void setMaxRetries(size_t maxRetries);
    void setRecoveryTimeout(double timeout);
    void enableAutomaticRecovery(bool enable);

    // Get recovery statistics
    size_t getRecoveryAttempts() const;
    size_t getRecoveryFailures() const;
    double getRecoverySuccessRate() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    UserAction attemptRecovery(const NLMError& error);
};

// Error reporting system
class ErrorReportingSystem {
public:
    ErrorReportingSystem();
    ~ErrorReportingSystem();

    // Send error reports
    void sendErrorReport(const NLMError& error);
    void sendBatchErrorReports(const std::vector<NLMError>& errors);

    // Configure reporting
    void setReportEndpoint(const std::string& endpoint);
    void setApiKey(const std::string& apiKey);
    void enableEmailReports(bool enable, const std::string& from, const std::vector<std::string>& to);
    void enableSystemLogs(bool enable);

    // Report configuration
    void setIncludeSystemInfo(bool include);
    void setIncludeStackTrace(bool include);
    void setIncludePerformanceData(bool include);

    // Report filtering
    void setReportFilter(std::function<bool(const NLMError&)> filter);
    void addReportIncludeCategory(ErrorCategory category);
    void addReportExcludeCategory(ErrorCategory category);

    // Report compression
    void setCompressionEnabled(bool enable);
    void setEncryptionEnabled(bool enable);

    // Get report statistics
    size_t getReportsSent() const;
    size_t getReportsFailed() const;
    double getReportSuccessRate() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void sendReport(const NLMError& error);
    std::string createReportPayload(const NLMError& error) const;
    bool sendReportToEndpoint(const std::string& payload);
};

// Error statistics
struct ErrorStatistics {
    double errorRate;
    size_t totalErrors;
    size_t errorsBySeverity[6]; // DEBUG, INFO, WARNING, ERROR, CRITICAL, FATAL
    size_t errorsByCategory[20]; // All ErrorCategory values
    double recoverySuccessRate;
    double reportSuccessRate;
    double averageResolutionTime;
    size_t lastErrorTime;
    std::map<std::string, double> errorTrends;
    std::vector<std::string> commonErrors;
};

// Global error handling instance
extern UserFeedbackSystem gUserFeedbackSystem;
extern ErrorRecoverySystem gErrorRecoverySystem;
extern ErrorReportingSystem gErrorReportingSystem;

// Convenience functions
namespace ErrorHandling {
    // Report error
    void report(const NLMError& error);
    void report(ErrorCategory category, const std::string& code, const std::string& details);
    void report(ErrorCategory category, const std::string& message);

    // Check if operation should continue
    bool shouldContinue(const NLMError& error);

    // Get error statistics
    ErrorStatistics getStatistics();

    // Configure error handling
    void configure(FeedbackLevel level);
    void setErrorHandler(std::shared_ptr<IErrorHandler> handler);

    // Error recovery
    void enableAutoRecovery(bool enable);
    UserAction attemptRecovery(const NLMError& error);

    // User feedback
    void showProgress(const std::string& message);
    void showInfo(const std::string& message);
    void showWarning(const std::string& message);
    void showError(const std::string& message);
    void showFatal(const std::string& message);

    // Error context
    void setErrorContext(const std::string& operation, const std::string& location);
    void addErrorContext(const std::string& key, const std::string& value);

    // Error filtering
    void ignoreErrorCategory(ErrorCategory category);
    void ignoreErrorSeverity(ErrorSeverity severity);

    // Logging
    void enableLogging(const std::string& filename);
    void disableLogging();
    bool isLoggingEnabled();
}
