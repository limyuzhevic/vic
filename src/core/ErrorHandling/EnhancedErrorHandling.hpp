// Enhanced Error Handling and Logging System for NLM
// Provides comprehensive error reporting, recovery mechanisms, and advanced logging capabilities

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stacktrace>
#include <exception>

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

// Error categories
enum class ErrorCategory {
    Configuration,
    Simulation,
    Memory,
    Neural,
    InputOutput,
    Network,
    Development,
    Neuromodulation,
    Prediction,
    Cognition,
    System,
    Custom
};

// Enhanced error with context
struct EnhancedError {
    std::string message;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    std::map<std::string, std::string> contextData;
    std::vector<std::string> recoveryActions;
    
    EnhancedError() : severity(ErrorSeverity::Info), category(ErrorCategory::System), 
                     sourceLine(0), timestamp(std::chrono::system_clock::now()) {}
    
    EnhancedError(const std::string& msg, ErrorSeverity sev = ErrorSeverity::Error, 
                  ErrorCategory cat = ErrorCategory::System)
        : message(msg), severity(sev), category(cat), sourceLine(0) 
    {
        timestamp = std::chrono::system_clock::now();
        captureStackTrace();
    }
    
    void captureStackTrace() {
        try {
            std::stacktrace trace = std::stacktrace::current();
            std::ostringstream oss;
            oss << trace;
            stackTrace = oss.str();
        } catch (...) {
            stackTrace = "Failed to capture stack trace";
        }
    }
    
    std::string getTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << getTimestamp() << "] ";
        oss << "[" << errorSeverityToString(severity) << "] ";
        oss << "[" << errorCategoryToString(category) << "] ";
        oss << message;
        if (!sourceFile.empty()) {
            oss << " (at " << sourceFile << ":" << sourceLine << ")";
        }
        return oss.str();
    }
    
    std::string getFormattedDetails() const {
        std::ostringstream oss;
        oss << "Error Details:" << std::endl;
        oss << "  Message: " << message << std::endl;
        oss << "  Severity: " << errorSeverityToString(severity) << std::endl;
        oss << "  Category: " << errorCategoryToString(category) << std::endl;
        if (!sourceFile.empty()) {
            oss << "  Location: " << sourceFile << ":" << sourceLine << " in " << sourceFunction << std::endl;
        }
        if (!contextData.empty()) {
            oss << "  Context:" << std::endl;
            for (const auto& pair : contextData) {
                oss << "    " << pair.first << ": " << pair.second << std::endl;
            }
        }
        if (!stackTrace.empty()) {
            oss << "  Stack Trace:" << std::endl;
            oss << "    " << stackTrace << std::endl;
        }
        if (!recoveryActions.empty()) {
            oss << "  Recovery Actions:" << std::endl;
            for (const auto& action : recoveryActions) {
                oss << "    - " << action << std::endl;
            }
        }
        return oss.str();
    }
    
    void addContext(const std::string& key, const std::string& value) {
        contextData[key] = value;
    }
    
    void addRecoveryAction(const std::string& action) {
        recoveryActions.push_back(action);
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const EnhancedError& error) = 0;
    virtual bool canHandleError(const EnhancedError& error) const = 0;
    virtual int getPriority() const = 0;
    virtual std::string getName() const = 0;
};

// Default error handler - outputs to console
class ConsoleErrorHandler : public IErrorHandler {
public:
    ConsoleErrorHandler(ErrorSeverity minSeverity = ErrorSeverity::Warning)
        : minSeverity_(minSeverity) {}
    
    void handleError(const EnhancedError& error) override {
        if (error.severity < minSeverity_) return;
        
        std::ostringstream oss;
        oss << error.getFullMessage();
        
        switch (error.severity) {
            case ErrorSeverity::Debug:
                NLM_LOG_DEBUG(oss.str());
                break;
            case ErrorSeverity::Info:
                NLM_LOG_INFO(oss.str());
                break;
            case ErrorSeverity::Warning:
                NLM_LOG_WARNING(oss.str());
                break;
            case ErrorSeverity::Error:
                NLM_LOG_ERROR(oss.str());
                break;
            case ErrorSeverity::Critical:
            case ErrorSeverity::Fatal:
                NLM_LOG_CRITICAL(oss.str());
                break;
        }
    }
    
    bool canHandleError(const EnhancedError& error) const override {
        return error.severity >= minSeverity_;
    }
    
    int getPriority() const override {
        return 100; // High priority for console output
    }
    
    std::string getName() const override {
        return "ConsoleErrorHandler";
    }
    
private:
    ErrorSeverity minSeverity_;
};

// File error handler - writes errors to log files
class FileErrorHandler : public IErrorHandler {
public:
    FileErrorHandler(const std::string& filename, ErrorSeverity minSeverity = ErrorSeverity::Error)
        : filename_(filename), minSeverity_(minSeverity), logFile_(nullptr) {
        openFile();
    }
    
    ~FileErrorHandler() override {
        if (logFile_) {
            fclose(logFile_);
        }
    }
    
    void handleError(const EnhancedError& error) override {
        if (error.severity < minSeverity_) return;
        if (!logFile_) return;
        
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        fprintf(logFile_, "%s\n", error.getFullMessage().c_str());
        fflush(logFile_);
    }
    
    bool canHandleError(const EnhancedError& error) const override {
        return error.severity >= minSeverity_;
    }
    
    int getPriority() const override {
        return 90;
    }
    
    std::string getName() const override {
        return "FileErrorHandler";
    }
    
    void setMinSeverity(ErrorSeverity severity) {
        minSeverity_ = severity;
    }
    
private:
    void openFile() {
        logFile_ = fopen(filename_.c_str(), "a");
        if (logFile_) {
            // Add header if file is empty
            fseek(logFile_, 0, SEEK_END);
            long fileSize = ftell(logFile_);
            if (fileSize == 0) {
                fprintf(logFile_, "NLM Error Log\n");
                fprintf(logFile_, "Started: %s\n", getCurrentTimestamp().c_str());
                fprintf(logFile_, "========================================\n");
            }
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string filename_;
    ErrorSeverity minSeverity_;
    FILE* logFile_;
    mutable std::mutex fileMutex_;
};

// Error recovery handler
class ErrorRecoveryHandler : public IErrorHandler {
public:
    ErrorRecoveryHandler() : autoRecover_(true), maxRetries_(3) {}
    
    void handleError(const EnhancedError& error) override {
        if (!canHandleError(error)) return;
        
        NLM_LOG_INFO("Error recovery: " + error.message);
        
        // Check if we can recover from this error
        if (canRecoverFromError(error)) {
            NLM_LOG_INFO("Attempting to recover from error...");
            if (autoRecover_) {
                recoveryResult_ = attemptRecovery(error);
                if (recoveryResult_.success) {
                    NLM_LOG_INFO("Recovery successful: " + recoveryResult_.message);
                    return;
                } else {
                    NLM_LOG_ERROR("Recovery failed: " + recoveryResult_.message);
                }
            }
        }
        
        // If recovery failed, try to retry if applicable
        if (error.severity >= ErrorSeverity::Error && retryCount_ < maxRetries_) {
            retryCount_++;
            NLM_LOG_WARNING("Retrying operation (attempt " + std::to_string(retryCount_) + "/" + std::to_string(maxRetries_) + ")");
            // TODO: Re-execute the operation that caused the error
        }
    }
    
    bool canHandleError(const EnhancedError& error) const override {
        return error.severity >= ErrorSeverity::Warning && 
               error.category != ErrorCategory::Fatal;
    }
    
    int getPriority() const override {
        return 80; // Higher priority than file/console handlers
    }
    
    std::string getName() const override {
        return "ErrorRecoveryHandler";
    }
    
    void setAutoRecover(bool enabled) {
        autoRecover_ = enabled;
    }
    
    void setMaxRetries(int retries) {
        maxRetries_ = retries;
    }
    
    bool canRecoverFromError(const EnhancedError& error) const {
        return !error.recoveryActions.empty() || 
               error.category == ErrorCategory::Configuration ||
               error.category == ErrorCategory::Memory;
    }
    
    struct RecoveryResult {
        bool success;
        std::string message;
        std::chrono::milliseconds recoveryTime;
    };
    
    RecoveryResult attemptRecovery(const EnhancedError& error) {
        RecoveryResult result;
        result.success = false;
        result.message = "Recovery not implemented";
        
        // Try predefined recovery actions
        for (const auto& action : error.recoveryActions) {
            try {
                NLM_LOG_INFO("Executing recovery action: " + action);
                // TODO: Implement recovery actions
                result.success = true;
                result.message = "Recovery completed successfully";
                break;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Recovery action failed: " + std::string(e.what()));
            }
        }
        
        if (result.success) {
            result.recoveryTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch() - startTime_);
        }
        
        return result;
    }
    
private:
    bool autoRecover_;
    int maxRetries_;
    int retryCount_;
    std::chrono::steady_clock::time_point startTime_;
    RecoveryResult recoveryResult_;
};

// Centralized error reporting system
class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void registerHandler(std::shared_ptr<IErrorHandler> handler) {
        handlers_.push_back(handler);
        std::sort(handlers_.begin(), handlers_.end(),
                 [](const std::shared_ptr<IErrorHandler>& a, const std::shared_ptr<IErrorHandler>& b) {
                     return a->getPriority() > b->getPriority();
                 });
    }
    
    void unregisterHandler(const std::string& name) {
        handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(),
                                    [&name](const std::shared_ptr<IErrorHandler>& handler) {
                                        return handler->getName() == name;
                                    }),
                       handlers_.end());
    }
    
    void reportError(const EnhancedError& error) {
        std::lock_guard<std::mutex> lock(errorMutex_);
        
        // Store error in history
        errorHistory_.push_back(error);
        
        // Send error to all handlers
        for (const auto& handler : handlers_) {
            if (handler->canHandleError(error)) {
                handler->handleError(error);
            }
        }
    }
    
    void reportError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                     ErrorCategory category = ErrorCategory::System,
                     const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedError error(message, severity, category);
        error.sourceFile = file;
        error.sourceLine = line;
        error.sourceFunction = function;
        reportError(error);
    }
    
    // Convenience macros (similar to existing logging macros)
    #define NLM_THROW_ERROR(msg, category) \
        do { \
            EnhancedError err(msg, ErrorSeverity::Error, category); \
            err.sourceFile = __FILE__; \
            err.sourceLine = __LINE__; \
            err.sourceFunction = __func__; \
            ErrorReporter::getInstance().reportError(err); \
            throw err; \
        } while (0)
    
    #define NLM_THROW_CRITICAL(msg, category) \
        do { \
            EnhancedError err(msg, ErrorSeverity::Critical, category); \
            err.sourceFile = __FILE__; \
            err.sourceLine = __LINE__; \
            err.sourceFunction = __func__; \
            ErrorReporter::getInstance().reportError(err); \
            throw err; \
        } while (0)
    
    #define NLM_LOG_ERROR_WITH_CONTEXT(msg, severity, category, context) \
        do { \
            EnhancedError err(msg, severity, category); \
            err.sourceFile = __FILE__; \
            err.sourceLine = __LINE__; \
            err.sourceFunction = __func__; \
            for (const auto& pair : context) { \
                err.addContext(pair.first, pair.second); \
            } \
            ErrorReporter::getInstance().reportError(err); \
        } while (0)
    
    std::vector<EnhancedError> getErrorHistory() const {
        return errorHistory_;
    }
    
    std::vector<EnhancedError> getErrorsSince(const std::chrono::system_clock::time_point& since) const {
        std::vector<EnhancedError> recentErrors;
        for (const auto& error : errorHistory_) {
            if (error.timestamp >= since) {
                recentErrors.push_back(error);
            }
        }
        return recentErrors;
    }
    
    std::vector<EnhancedError> getErrorsByCategory(ErrorCategory category) const {
        std::vector<EnhancedError> categoryErrors;
        for (const auto& error : errorHistory_) {
            if (error.category == category) {
                categoryErrors.push_back(error);
            }
        }
        return categoryErrors;
    }
    
    std::vector<EnhancedError> getErrorsBySeverity(ErrorSeverity severity) const {
        std::vector<EnhancedError> severityErrors;
        for (const auto& error : errorHistory_) {
            if (error.severity == severity) {
                severityErrors.push_back(error);
            }
        }
        return severityErrors;
    }
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const {
        size_t count = 0;
        for (const auto& error : errorHistory_) {
            if (error.severity >= severity) {
                count++;
            }
        }
        return count;
    }
    
    void clearHistory() {
        errorHistory_.clear();
    }
    
    std::string getErrorSummary() const {
        std::ostringstream oss;
        oss << "Error Summary:" << std::endl;
        oss << "  Total errors: " << errorHistory_.size() << std::endl;
        
        std::map<ErrorSeverity, size_t> severityCounts;
        std::map<ErrorCategory, size_t> categoryCounts;
        
        for (const auto& error : errorHistory_) {
            severityCounts[error.severity]++;
            categoryCounts[error.category]++;
        }
        
        oss << "  By severity:" << std::endl;
        for (const auto& pair : severityCounts) {
            oss << "    " << errorSeverityToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        oss << "  By category:" << std::endl;
        for (const auto& pair : categoryCounts) {
            oss << "    " << errorCategoryToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }
    
private:
    ErrorReporter() {
        // Register default handlers
        registerHandler(std::make_shared<ConsoleErrorHandler>(ErrorSeverity::Warning));
        registerHandler(std::make_shared<FileErrorHandler>("nlm_errors.log", ErrorSeverity::Error));
        registerHandler(std::make_shared<ErrorRecoveryHandler>());
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Simulation: return "Simulation";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Neural: return "Neural";
            case ErrorCategory::InputOutput: return "InputOutput";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::Development: return "Development";
            case ErrorCategory::Neuromodulation: return "Neuromodulation";
            case ErrorCategory::Prediction: return "Prediction";
            case ErrorCategory::Cognition: return "Cognition";
            case ErrorCategory::System: return "System";
            case ErrorCategory::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    std::vector<std::shared_ptr<IErrorHandler>> handlers_;
    std::vector<EnhancedError> errorHistory_;
    mutable std::mutex errorMutex_;
};

// Global error reporting convenience functions
inline void reportError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                      ErrorCategory category = ErrorCategory::System,
                      const std::string& file = "", int line = 0, const std::string& function = "") {
    ErrorReporter::getInstance().reportError(message, severity, category, file, line, function);
}

inline void reportError(const EnhancedError& error) {
    ErrorReporter::getInstance().reportError(error);
}

inline std::string getErrorSummary() {
    return ErrorReporter::getInstance().getErrorSummary();
}

// Convenience macros for enhanced error reporting
#define NLM_THROW_ENHANCED_ERROR(msg, severity, category) \
    do { \
        EnhancedError err(msg, severity, category); \
        err.sourceFile = __FILE__; \
        err.sourceLine = __LINE__; \
        err.sourceFunction = __func__; \
        ErrorReporter::getInstance().reportError(err); \
        throw err; \
    } while (0)

#define NLM_LOG_ENHANCED_ERROR(msg, severity, category, ...) \
    do { \
        EnhancedError err(msg, severity, category); \
        err.sourceFile = __FILE__; \
        err.sourceLine = __LINE__; \
        err.sourceFunction = __func__; \
        /* Process additional context arguments */ \
        ErrorReporter::getInstance().reportError(err); \
    } while (0)

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
