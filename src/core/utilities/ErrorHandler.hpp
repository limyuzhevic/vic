// Error Handling Utility Class
// Create consistent error handling across all files

#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {

enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

enum class ErrorCategory {
    None,
    Memory,
    Configuration,
    Neural,
    Plasticity,
    Integration,
    MemoryManagement,
    Prediction,
    Neuromodulation,
    System,
    Validation
};

struct ErrorContext {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string sourceFile;
    size_t lineNumber;
    std::string functionName;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    
    // Stack trace information (platform-specific)
    std::string stackTrace;
    
    // Related errors
    std::vector<size_t> relatedErrorIds;
};

class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
    NLMException(const std::string& message, const std::string& file, size_t line)
        : std::runtime_error(formatWithContext(message, file, line)) {}
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getSourceFile() const { return sourceFile_; }
    size_t getLineNumber() const { return lineNumber_; }
    const std::string& getFunctionName() const { return functionName_; }
    const std::map<std::string, std::string>& getMetadata() const { return metadata_; }
    
    void setContext(ErrorCategory category, ErrorSeverity severity, const std::string& file,
                   size_t line, const std::string& function, const std::map<std::string, std::string>& metadata = {});
    
    std::string getFormattedMessage() const;
    std::string getFullReport() const;

private:
    static std::string formatWithContext(const std::string& message, const std::string& file, size_t line);
    
    ErrorCategory category_ = ErrorCategory::None;
    ErrorSeverity severity_ = ErrorSeverity::Error;
    std::string sourceFile_;
    size_t lineNumber_ = 0;
    std::string functionName_;
    std::map<std::string, std::string> metadata_;
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const ErrorContext& context, const std::string& formattedMessage)>;
    
    // Singleton instance
    static ErrorHandler& getInstance();
    
    // Configure error handling
    void configure(bool enableDetailedLogging = true,
                   size_t maxErrorHistory = 1000,
                   ErrorSeverity minSeverityToLog = ErrorSeverity::Warning,
                   bool enableErrorAggregation = true);

    // Log an error with context
    void logError(const std::string& message, ErrorCategory category = ErrorCategory::None,
                  ErrorSeverity severity = ErrorSeverity::Error, const std::string& file = "",
                  size_t line = 0, const std::string& function = "",
                  const std::map<std::string, std::string>& metadata = {});

    // Log an exception
    void logException(const std::exception& e, ErrorCategory category = ErrorCategory::System);

    // Set error callback
    void setErrorCallback(ErrorCallback callback);

    // Get recent errors
    const std::vector<ErrorContext>& getRecentErrors() const { return errorHistory_; }

    // Clear error history
    void clearErrors() { errorHistory_.clear(); }

    // Get error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        size_t errorsByCategory[static_cast<size_t>(ErrorCategory::Validation) + 1] = {0};
        size_t errorsBySeverity[static_cast<size_t>(ErrorSeverity::Fatal) + 1] = {0};
        float averageErrorsPerMinute = 0.0f;
    };

    ErrorStats getErrorStats() const;

    // Check if errors exceed threshold
    bool hasErrorsAboveThreshold(size_t threshold) const;

    // Validate system state
    bool validateSystemState() const;

private:
    ErrorHandler() = default;
    
    void addError(const ErrorContext& context);
    std::string formatError(const ErrorContext& context) const;
    void aggregateErrors();
    void cleanupOldErrors();

    std::vector<ErrorContext> errorHistory_;
    ErrorCallback errorCallback_;
    bool enableDetailedLogging_ = true;
    size_t maxErrorHistory_ = 1000;
    ErrorSeverity minSeverityToLog_ = ErrorSeverity::Warning;
    bool enableErrorAggregation_ = true;
    std::chrono::system_clock::time_point lastCleanupTime_;
};

// Convenience macros for error handling
#define NLM_ERROR(msg) ::nlm::ErrorHandler::getInstance().logError(msg, ::nlm::ErrorCategory::System, ::nlm::ErrorSeverity::Error, __FILE__, __LINE__, __func__)

#define NLM_THROW(msg, category) throw ::nlm::NLMException(msg, __FILE__, __LINE__)

#define NLM_ASSERT(condition, msg) if (!(condition)) { NLM_ERROR("Assertion failed: " + std::string(msg)); NLM_THROW("Assertion failed: " + std::string(msg), ::nlm::ErrorCategory::Validation); }

#define NLM_VALIDATE(condition, msg, category) if (!(condition)) { NLM_ERROR(std::string("Validation failed: ") + msg); }

// Error handling utility functions
std::string formatErrorMessage(const std::string& message, const std::string& context = "");
std::string getCurrentTimestamp();
std::string getStackTrace();

// Exception wrapper
class ErrorWrapper {
public:
    template<typename Func>
    static auto executeWithErrorHandling(Func&& func, ErrorCategory category = ErrorCategory::System,
                                         const std::string& operationName = "operation") -> decltype(func()) {
        try {
            return func();
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().logError(e.what(), category);
            throw;
        } catch (const std::exception& e) {
            ErrorHandler::getInstance().logError(std::string("Unexpected error: ") + e.what(), category);
            throw NLMException(std::string("Operation failed: ") + operationName + std::string(": ") + e.what(), category);
        }
    }
};

} // namespace nlm
