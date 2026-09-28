/*
 * nlm/ErrorReporter.hpp - Centralized error reporting for the NLM system
 * Comprehensive error reporting with context, stack traces, and recovery suggestions
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include "nlm/Exceptions.hpp"

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error category
enum class ErrorCategory {
    None = 0,
    Configuration = 1,
    Neural = 2,
    Plasticity = 3,
    Memory = 4,
    IO = 5,
    Computation = 6,
    System = 7
};

// Error context information
struct ErrorContext {
    std::string source;
    std::string operation;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    
    ErrorContext() : timestamp(std::chrono::system_clock::now()) {}
    
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Detailed error information
struct ErrorInfo {
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    std::string code;
    std::string file;
    int line;
    std::string function;
    ErrorContext context;
    std::vector<std::string> recoverySteps;
    std::map<std::string, std::string> additionalInfo;
    
    ErrorInfo(ErrorSeverity sev, ErrorCategory cat, const std::string& msg)
        : severity(sev), category(cat), message(msg), line(0) {}
    
    void addRecoveryStep(const std::string& step) {
        recoverySteps.push_back(step);
    }
    
    void addAdditionalInfo(const std::string& key, const std::string& value) {
        additionalInfo[key] = value;
    }
    
    std::string getFormattedError() const {
        std::ostringstream oss;
        oss << "[" << severityToString(severity) << "] " << categoryToString(category)
            << ": " << message;
        if (!file.empty()) {
            oss << "\n  Location: " << file << ":" << line;
        }
        if (!function.empty()) {
            oss << "\n  Function: " << function;
        }
        if (!context.source.empty()) {
            oss << "\n  Source: " << context.source;
        }
        if (!context.operation.empty()) {
            oss << "\n  Operation: " << context.operation;
        }
        if (!context.metadata.empty()) {
            oss << "\n  Metadata:";
            for (const auto& pair : context.metadata) {
                oss << "\n    " << pair.first << " = " << pair.second;
            }
        }
        if (!recoverySteps.empty()) {
            oss << "\n  Recovery steps:";
            for (size_t i = 0; i < recoverySteps.size(); ++i) {
                oss << "\n    " << (i + 1) << ". " << recoverySteps[i];
            }
        }
        return oss.str();
    }
    
private:
    static std::string severityToString(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    static std::string categoryToString(ErrorCategory cat) {
        switch (cat) {
            case ErrorCategory::None: return "NONE";
            case ErrorCategory::Configuration: return "CONFIGURATION";
            case ErrorCategory::Neural: return "NEURAL";
            case ErrorCategory::Plasticity: return "PLASTICITY";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::IO: return "IO";
            case ErrorCategory::Computation: return "COMPUTATION";
            case ErrorCategory::System: return "SYSTEM";
            default: return "UNKNOWN";
        }
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual bool shouldReport(const ErrorInfo& error) const = 0;
    virtual void setSeverityThreshold(ErrorSeverity severity) = 0;
    virtual ErrorSeverity getSeverityThreshold() const = 0;
};

// Console error handler
class ConsoleErrorHandler : public IErrorHandler {
public:
    ConsoleErrorHandler();
    ~ConsoleErrorHandler() override;
    
    void handleError(const ErrorInfo& error) override;
    bool shouldReport(const ErrorInfo& error) const override;
    void setSeverityThreshold(ErrorSeverity severity) override;
    ErrorSeverity getSeverityThreshold() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Error reporter class
class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter();
    
    // Disable copying
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
    
    // Add error handler
    void addHandler(std::shared_ptr<IErrorHandler> handler);
    
    // Remove all handlers
    void clearHandlers();
    
    // Report error
    void report(ErrorSeverity severity, ErrorCategory category, const std::string& message,
                const std::string& file = "", int line = 0,
                const std::string& function = "");
    
    // Convenience methods
    void debug(ErrorCategory category, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    void info(ErrorCategory category, const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    
    void warning(ErrorCategory category, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    void error(ErrorCategory category, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    void critical(ErrorCategory category, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    void fatal(ErrorCategory category, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    
    // Report exception
    void reportException(const std::exception& e,
                         ErrorCategory category = ErrorCategory::System,
                         const std::string& file = "",
                         int line = 0,
                         const std::string& function = "");
    
    // Report NLM exception
    void reportNLMException(const NLMException& e,
                            ErrorCategory category = ErrorCategory::System,
                            const std::string& file = "",
                            int line = 0,
                            const std::string& function = "");
    
    // Set severity threshold
    void setSeverityThreshold(ErrorSeverity severity);
    
    // Check if error level is enabled
    bool isEnabled(ErrorSeverity severity) const;
    
    // Get error statistics
    size_t getErrorCount(ErrorSeverity severity) const;
    size_t getTotalErrorCount() const;
    
    // Clear statistics
    void clearStatistics();
    
    // Get global error reporter instance
    static ErrorReporter& getGlobal();
    
    // Set global error reporter
    static void setGlobal(std::shared_ptr<ErrorReporter> reporter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
