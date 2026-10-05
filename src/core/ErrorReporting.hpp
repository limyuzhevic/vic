// Enhanced error reporting and handling for NLM
// Provides detailed error messages with context, categorization, recovery suggestions

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <stacktrace>
#include <exception>
#include <typeinfo>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error categories
enum class ErrorCategory {
    System,
    Brain,
    Memory,
    Performance,
    Configuration,
    Network,
    InputOutput,
    Simulation,
    Experiment,
    Development,
    UserInput,
    Resource,
    Security,
    Plugin,
    Unknown
};

// Error context information
struct ErrorContext {
    std::string operation;
    std::string file;
    int line;
    std::string function;
    std::string component;
    std::map<std::string, std::string> additionalData;
    
    ErrorContext() : line(0) {}
    ErrorContext(const std::string& operation, const std::string& file, int line,
                 const std::string& function, const std::string& component)
        : operation(operation), file(file), line(line), function(function), component(component) {}
};

// Error recovery suggestion
struct RecoverySuggestion {
    std::string action;
    std::string description;
    bool requiresRestart = false;
    bool requiresConfiguration = false;
    bool requiresUserInput = false;
    std::vector<std::string> steps;
    std::map<std::string, std::string> parameters;
    
    RecoverySuggestion() : requiresRestart(false), requiresConfiguration(false), requiresUserInput(false) {}
    RecoverySuggestion(const std::string& action, const std::string& description)
        : action(action), description(description) {}
};

// Stack trace entry
struct StackTraceEntry {
    std::string file;
    std::string function;
    int line;
    std::string sourceCode;
    
    StackTraceEntry() : line(0) {}
    StackTraceEntry(const std::string& file, const std::string& function, int line)
        : file(file), function(function), line(line) {}
};

// Error report
struct ErrorReport {
    std::string id;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    std::string description;
    std::string code;
    std::chrono::system_clock::time_point timestamp;
    ErrorContext context;
    std::vector<StackTraceEntry> stackTrace;
    std::vector<RecoverySuggestion> recoverySuggestions;
    bool isResolved{false};
    std::chrono::system_clock::time_point resolvedTime;
    std::string resolutionNotes;
    
    ErrorReport() : severity(ErrorSeverity::Error), category(ErrorCategory::Unknown), timestamp(std::chrono::system_clock::now()) {}
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorReport& error) = 0;
    virtual bool canHandleError(const ErrorReport& error) const = 0;
};

class ErrorReporting {
public:
    ErrorReporting();
    ~ErrorReporting();
    
    // Configure error reporting
    void setLogErrors(bool logErrors);
    void setIncludeStackTrace(bool includeStackTrace);
    void setIncludeContext(bool includeContext);
    void setMaxStackTraceDepth(size_t depth);
    void setUseColors(bool useColors);
    void setShowRecoverySuggestions(bool show);
    void setEnableAutoReporting(bool enabled);
    void setErrorReportCallback(const std::function<void(const ErrorReport&)>& callback);
    
    // Error reporting
    std::string reportError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                           ErrorCategory category = ErrorCategory::Unknown, const std::string& code = "",
                           const ErrorContext& context = ErrorContext());
    std::string reportError(const std::exception& e, ErrorSeverity severity = ErrorSeverity::Error,
                           ErrorCategory category = ErrorCategory::Unknown, const std::string& code = "",
                           const ErrorContext& context = ErrorContext());
    
    // Error handling
    void handleError(const std::string& errorId, bool resolve = false,
                    const std::string& resolutionNotes = "");
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler);
    void removeErrorHandler(const std::string& handlerId);
    void clearErrorHandlers();
    
    // Error categorization
    ErrorCategory categorizeError(const std::string& message, const ErrorContext& context) const;
    std::string errorCategoryToString(ErrorCategory category) const;
    ErrorSeverity determineErrorSeverity(const std::string& message, ErrorCategory category) const;
    
    // Recovery suggestions
    std::vector<RecoverySuggestion> generateRecoverySuggestions(const ErrorReport& error) const;
    void addRecoverySuggestion(const std::string& errorId, const RecoverySuggestion& suggestion);
    void removeRecoverySuggestion(const std::string& errorId, const std::string& action);
    
    // Error management
    void clearResolvedErrors();
    void clearAllErrors();
    bool isErrorResolved(const std::string& errorId) const;
    std::vector<ErrorReport> getErrors(ErrorSeverity severity = ErrorSeverity::Error,
                                       ErrorCategory category = ErrorCategory::Unknown,
                                       bool includeResolved = false) const;
    const ErrorReport* getError(const std::string& errorId) const;
    
    // Error analysis
    std::string generateErrorReport(const std::string& errorId = "", bool includeStackTrace = true) const;
    std::string generateErrorAnalysisReport() const;
    std::string generateErrorSummaryReport() const;
    
    // Stack trace handling
    std::vector<StackTraceEntry> captureStackTrace(size_t maxDepth = 32) const;
    std::string formatStackTrace(const std::vector<StackTraceEntry>& stackTrace) const;
    
    // Context extraction
    ErrorContext extractContext() const;
    std::string formatErrorContext(const ErrorContext& context) const;
    
    // Export functionality
    bool exportErrorReport(const std::string& filepath, const std::string& format = "text",
                          const std::vector<std::string>& errorIds = {}) const;
    
    // Error logging
    void logError(const ErrorReport& error) const;
    void logError(const std::string& errorId) const;
    
    // Error statistics
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    double getErrorRate(ErrorSeverity severity = ErrorSeverity::Error) const;
    std::string getMostCommonErrorCategory() const;
    std::chrono::system_clock::time_point getLastErrorTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string generateErrorId() const;
    std::string extractErrorCode(const std::string& message) const;
    std::string extractErrorMessage(const std::exception& e) const;
    void createErrorReport(const std::string& message, ErrorSeverity severity,
                           ErrorCategory category, const std::string& code,
                           const ErrorContext& context, const std::vector<StackTraceEntry>& stackTrace,
                           std::string& errorId);
    void formatErrorReport(std::stringstream& ss, const ErrorReport& error, bool includeStackTrace) const;
};

} // namespace nlm
