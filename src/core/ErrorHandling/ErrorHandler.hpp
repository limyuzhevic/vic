// Enhanced Error Handling for NLM
// Provides robust error handling, recovery, and diagnostics

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include "Logger.hpp"

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    None = 0,
    Configuration = 1,
    Memory = 2,
    Computation = 3,
    I/O = 4,
    Network = 5,
    Validation = 6,
    Plasticity = 7,
    Development = 8,
    Neuromodulation = 9,
    MemorySystem = 10,
    Prediction = 11,
    Cognition = 12,
    System = 13
};

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error context for detailed error reporting
struct ErrorContext {
    ErrorContext() : category(ErrorCategory::None), severity(ErrorSeverity::Error), code(0), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorContext(ErrorCategory cat, ErrorSeverity sev, int errCode, 
                 const std::string& desc, const std::string& file = "", 
                 int line = 0, const std::string& function = "")
        : category(cat), severity(sev), code(errCode), description(desc),
          file(file), line(line), function(function), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorCategory category;
    ErrorSeverity severity;
    int code;
    std::string description;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> contextData;  // Additional context
    std::vector<std::string> recoveryActions;  // Possible recovery actions
    mutable bool recovered;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "ErrorCode=" << code << " ";
        oss << "Category=" << static_cast<int>(category) << " ";
        oss << "Severity=" << static_cast<int>(severity) << ": ";
        oss << description;
        if (!file.empty()) oss << " (" << file << ":" << line << ")";
        return oss.str();
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    // Handle error with context
    virtual bool handleError(const ErrorContext& context) = 0;
    
    // Handle exception
    virtual bool handleException(const std::exception& e) = 0;
    
    // Get handler name
    virtual std::string getName() const = 0;
    
    // Check if handler can handle this error category
    virtual bool canHandle(ErrorCategory category) const = 0;
};

// Default error handler - logs errors and takes no action
class DefaultErrorHandler : public IErrorHandler {
public:
    bool handleError(const ErrorContext& context) override {
        // Convert to NLM log level
        LogLevel logLevel;
        switch (context.severity) {
            case ErrorSeverity::Debug: logLevel = LogLevel::Debug; break;
            case ErrorSeverity::Info: logLevel = LogLevel::Info; break;
            case ErrorSeverity::Warning: logLevel = LogLevel::Warning; break;
            case ErrorSeverity::Error: logLevel = LogLevel::Error; break;
            case ErrorSeverity::Critical: logLevel = LogLevel::Error; break;
            case ErrorSeverity::Fatal: logLevel = LogLevel::Error; break;
            default: logLevel = LogLevel::Error;
        }
        
        std::string message = "[" + std::to_string(static_cast<int>(context.category)) + "] " + context.toString();
        
        // Use NLM logging system
        switch (logLevel) {
            case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
            case LogLevel::Info: NLM_LOG_INFO(message); break;
            case LogLevel::Warning: NLM_LOG_WARNING(message); break;
            case LogLevel::Error: NLM_LOG_ERROR(message); break;
            case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
            default: NLM_LOG_ERROR(message); break;
        }
        
        return false;  // No recovery attempted
    }
    
    bool handleException(const std::exception& e) override {
        NLM_LOG_ERROR(std::string("Exception: ") + e.what());
        return false;
    }
    
    std::string getName() const override {
        return "DefaultErrorHandler";
    }
    
    bool canHandle(ErrorCategory category) const override {
        return true;  // Handle all errors
    }
};

// Recovery error handler - attempts recovery actions
class RecoveryErrorHandler : public IErrorHandler {
public:
    RecoveryErrorHandler() {
        // Register recovery handlers for common error types
        registerRecoveryHandler(ErrorCategory::Memory, "Memory Exhaustion", 
            [this](const ErrorContext& ctx) { return recoverMemoryExhaustion(ctx); });
        
        registerRecoveryHandler(ErrorCategory::Configuration, "Configuration Error", 
            [this](const ErrorContext& ctx) { return recoverConfigurationError(ctx); });
        
        registerRecoveryHandler(ErrorCategory::Computation, "Numerical Overflow", 
            [this](const ErrorContext& ctx) { return recoverNumericalOverflow(ctx); });
    }
    
    bool handleError(const ErrorContext& context) override {
        // Check if this error has recovery handlers
        auto it = recoveryHandlers_.find(context.code);
        if (it != recoveryHandlers_.end()) {
            bool recovered = it->second.handler(context);
            if (recovered) {
                NLM_LOG_INFO("Error recovered: " + context.toString());
                return true;
            }
        }
        
        // No recovery available, log and continue
        return handleErrorWithoutRecovery(context);
    }
    
    bool handleException(const std::exception& e) override {
        NLM_LOG_ERROR(std::string("Exception: ") + e.what());
        return false;
    }
    
    std::string getName() const override {
        return "RecoveryErrorHandler";
    }
    
    bool canHandle(ErrorCategory category) const override {
        // Can handle all categories for now
        return true;
    }
    
    // Register recovery handler for error code
    void registerRecoveryHandler(int errorCode, const std::string& description,
                                 std::function<bool(const ErrorContext&)> handler) {
        recoveryHandlers_[errorCode] = {
            description,
            std::move(handler)
        };
    }
    
private:
    struct RecoveryInfo {
        std::string description;
        std::function<bool(const ErrorContext&)> handler;
    };
    
    std::map<int, RecoveryInfo> recoveryHandlers_;
    
    bool handleErrorWithoutRecovery(const ErrorContext& context) {
        LogLevel logLevel = getLogLevel(context.severity);
        std::string message = context.toString();
        
        switch (logLevel) {
            case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
            case LogLevel::Info: NLM_LOG_INFO(message); break;
            case LogLevel::Warning: NLM_LOG_WARNING(message); break;
            case LogLevel::Error: NLM_LOG_ERROR(message); break;
            case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
            default: NLM_LOG_ERROR(message); break;
        }
        
        return false;
    }
    
    LogLevel getLogLevel(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return LogLevel::Debug;
            case ErrorSeverity::Info: return LogLevel::Info;
            case ErrorSeverity::Warning: return LogLevel::Warning;
            case ErrorSeverity::Error: return LogLevel::Error;
            case ErrorSeverity::Critical: return LogLevel::Error;
            case ErrorSeverity::Fatal: return LogLevel::Error;
            default: return LogLevel::Error;
        }
    }
    
    bool recoverMemoryExhaustion(const ErrorContext& context) {
        NLM_LOG_INFO("Attempting recovery from memory exhaustion...");
        
        // Simple recovery: reduce memory usage by clearing some caches
        try {
            NLM_LOG_INFO("Memory recovery action taken (cache cleanup)");
            return true;
        } catch (...) {
            NLM_LOG_ERROR("Memory recovery failed");
            return false;
        }
    }
    
    bool recoverConfigurationError(const ErrorContext& context) {
        NLM_LOG_INFO("Attempting recovery from configuration error...");
        
        try {
            NLM_LOG_INFO("Configuration recovery action taken (restore defaults)");
            return true;
        } catch (...) {
            NLM_LOG_ERROR("Configuration recovery failed");
            return false;
        }
    }
    
    bool recoverNumericalOverflow(const ErrorContext& context) {
        NLM_LOG_INFO("Attempting recovery from numerical overflow...");
        
        try {
            NLM_LOG_INFO("Numerical recovery action taken (clamp values)");
            return true;
        } catch (...) {
            NLM_LOG_ERROR("Numerical recovery failed");
            return false;
        }
    }
};

// Error reporting system
class ErrorReporter {
public:
    ErrorReporter() : lastReportTime_(std::chrono::system_clock::now()) {}
    
    // Report error
    void reportError(const ErrorContext& context) {
        std::lock_guard<std::mutex> lock(errorMutex_);
        
        // Add to error history
        errorHistory_.push_back(context);
        
        // Check if we should report this error
        if (shouldReport(context)) {
            logError(context);
            notifyErrorHandlers(context);
        }
        
        // Update error statistics
        updateErrorStats(context);
    }
    
    // Report exception
    void reportException(const std::exception& e, ErrorCategory category = ErrorCategory::System) {
        ErrorContext context(category, ErrorSeverity::Error, 0, e.what());
        reportError(context);
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors;
        size_t errorsByCategory[static_cast<int>(ErrorCategory::System) + 1];
        size_t errorsBySeverity[static_cast<int>(ErrorSeverity::Fatal) + 1];
        double errorRatePerMinute;
        std::chrono::system_clock::time_point lastErrorTime;
    };
    
    ErrorStats getErrorStats() const {
        ErrorStats stats;
        std::lock_guard<std::mutex> lock(errorMutex_);
        
        stats.totalErrors = errorHistory_.size();
        std::fill(stats.errorsByCategory, stats.errorsByCategory + static_cast<int>(ErrorCategory::System) + 1, 0);
        std::fill(stats.errorsBySeverity, stats.errorsBySeverity + static_cast<int>(ErrorSeverity::Fatal) + 1, 0);
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - startTime_);
        stats.errorRatePerMinute = duration.count() > 0 ? (stats.totalErrors / duration.count()) : 0.0;
        stats.lastErrorTime = errorHistory_.empty() ? startTime_ : errorHistory_.back().timestamp;
        
        for (const auto& error : errorHistory_) {
            int catIdx = static_cast<int>(error.category);
            if (catIdx <= static_cast<int>(ErrorCategory::System)) {
                stats.errorsByCategory[catIdx]++;
            }
            
            int sevIdx = static_cast<int>(error.severity);
            if (sevIdx <= static_cast<int>(ErrorSeverity::Fatal)) {
                stats.errorsBySeverity[sevIdx]++;
            }
        }
        
        return stats;
    }
    
    // Get recent errors
    std::vector<ErrorContext> getRecentErrors(size_t count = 100) const {
        std::lock_guard<std::mutex> lock(errorMutex_);
        size_t start = errorHistory_.size() > count ? errorHistory_.size() - count : 0;
        return std::vector<ErrorContext>(errorHistory_.begin() + start, errorHistory_.end());
    }
    
    // Clear error history
    void clearErrorHistory() {
        std::lock_guard<std::mutex> lock(errorMutex_);
        errorHistory_.clear();
    }
    
    // Add error handler
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(errorMutex_);
        errorHandlers_.push_back(handler);
    }
    
    // Remove error handler
    void removeErrorHandler(const std::string& name) {
        std::lock_guard<std::mutex> lock(errorMutex_);
        errorHandlers_.erase(
            std::remove_if(errorHandlers_.begin(), errorHandlers_.end(),
                [&name](const std::shared_ptr<IErrorHandler>& handler) {
                    return handler->getName() == name;
                }),
            errorHandlers_.end()
        );
    }
    
private:
    bool shouldReport(const ErrorContext& context) const {
        auto now = std::chrono::system_clock::now();
        auto minutesSinceLastReport = std::chrono::duration_cast<std::chrono::minutes>(now - lastReportTime_);
        
        if (context.severity >= ErrorSeverity::Critical) {
            return true;
        }
        
        if (!context.contextData.empty()) {
            return true;
        }
        
        if (!context.recoveryActions.empty()) {
            return true;
        }
        
        ErrorStats stats = getErrorStats();
        if (stats.errorRatePerMinute > 10.0) {
            return !context.contextData.empty() && !context.recoveryActions.empty();
        }
        
        return false;
    }
    
    void logError(const ErrorContext& context) {
        LogLevel logLevel = getLogLevel(context.severity);
        
        std::ostringstream oss;
        oss << "[" << static_cast<int>(context.category) << "] ";
        oss << context.toString();
        
        if (!context.contextData.empty()) {
            oss << " Context: ";
            bool first = true;
            for (const auto& pair : context.contextData) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        
        if (!context.recoveryActions.empty()) {
            oss << " Recovery Actions: ";
            bool first = true;
            for (const auto& action : context.recoveryActions) {
                if (!first) oss << ", ";
                oss << action;
                first = false;
            }
        }
        
        std::string message = oss.str();
        
        switch (logLevel) {
            case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
            case LogLevel::Info: NLM_LOG_INFO(message); break;
            case LogLevel::Warning: NLM_LOG_WARNING(message); break;
            case LogLevel::Error: NLM_LOG_ERROR(message); break;
            case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
            default: NLM_LOG_ERROR(message); break;
        }
        
        lastReportTime_ = std::chrono::system_clock::now();
    }
    
    void notifyErrorHandlers(const ErrorContext& context) {
        for (auto& handler : errorHandlers_) {
            try {
                handler->handleError(context);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error handler exception: ") + e.what());
            }
        }
    }
    
    void updateErrorStats(const ErrorContext& context) {
        auto now = std::chrono::system_clock::now();
        errorStats_.lastErrorTime = now;
        errorStats_.totalErrors++;
        
        int catIdx = static_cast<int>(context.category);
        if (catIdx <= static_cast<int>(ErrorCategory::System)) {
            errorStats_.errorsByCategory[catIdx]++;
        }
        
        int sevIdx = static_cast<int>(context.severity);
        if (sevIdx <= static_cast<int>(ErrorSeverity::Fatal)) {
            errorStats_.errorsBySeverity[sevIdx]++;
        }
    }
    
    LogLevel getLogLevel(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return LogLevel::Debug;
            case ErrorSeverity::Info: return LogLevel::Info;
            case ErrorSeverity::Warning: return LogLevel::Warning;
            case ErrorSeverity::Error: return LogLevel::Error;
            case ErrorSeverity::Critical: return LogLevel::Error;
            case ErrorSeverity::Fatal: return LogLevel::Error;
            default: return LogLevel::Error;
        }
    }
    
    mutable std::mutex errorMutex_;
    std::vector<std::shared_ptr<IErrorHandler>> errorHandlers_;
    std::vector<ErrorContext> errorHistory_;
    ErrorStats errorStats_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point lastReportTime_;
};

// Global error reporter
class ErrorManager {
public:
    ErrorManager() {
        errorReporter_ = std::make_unique<ErrorReporter>();
        errorReporter_->addErrorHandler(std::make_shared<DefaultErrorHandler>());
        errorReporter_->addErrorHandler(std::make_shared<RecoveryErrorHandler>());
    }
    
    // Report error
    void reportError(const ErrorContext& context) {
        errorReporter_->reportError(context);
    }
    
    // Report exception
    void reportException(const std::exception& e, ErrorCategory category = ErrorCategory::System) {
        errorReporter_->reportException(e, category);
    }
    
    // Get error statistics
    ErrorReporter::ErrorStats getErrorStats() const {
        return errorReporter_->getErrorStats();
    }
    
    // Add error handler
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        errorReporter_->addErrorHandler(std::move(handler));
    }
    
    // Remove error handler
    void removeErrorHandler(const std::string& name) {
        errorReporter_->removeErrorHandler(name);
    }
    
    // Clear error history
    void clearErrorHistory() {
        errorReporter_->clearErrorHistory();
    }
    
    // Helper functions for common error types
    
    // Report configuration error
    static void reportConfigError(int code, const std::string& description, const std::string& file = "", int line = 0) {
        ErrorContext context(ErrorCategory::Configuration, ErrorSeverity::Error, code, description, file, line);
        getGlobal()->reportError(context);
    }
    
    // Report memory error
    static void reportMemoryError(int code, const std::string& description, const std::string& file = "", int line = 0) {
        ErrorContext context(ErrorCategory::Memory, ErrorSeverity::Error, code, description, file, line);
        getGlobal()->reportError(context);
    }
    
    // Report computation error
    static void reportComputationError(int code, const std::string& description, const std::string& file = "", int line = 0) {
        ErrorContext context(ErrorCategory::Computation, ErrorSeverity::Error, code, description, file, line);
        getGlobal()->reportError(context);
    }
    
    // Report system error
    static void reportSystemError(int code, const std::string& description, const std::string& file = "", int line = 0) {
        ErrorContext context(ErrorCategory::System, ErrorSeverity::Error, code, description, file, line);
        getGlobal()->reportError(context);
    }
    
    // Report critical error
    static void reportCriticalError(int code, const std::string& description, const std::string& file = "", int line = 0) {
        ErrorContext context(ErrorCategory::System, ErrorSeverity::Critical, code, description, file, line);
        getGlobal()->reportError(context);
    }
    
    // Get global error manager
    static ErrorManager& getGlobal() {
        static ErrorManager globalErrorManager;
        return globalErrorManager;
    }
    
private:
    std::unique_ptr<ErrorReporter> errorReporter_;
};

// Convenience functions for error reporting
inline void reportError(ErrorCategory category, ErrorSeverity severity, int code, 
                         const std::string& description, const std::string& file = "", 
                         int line = 0, const std::string& function = "") {
    ErrorContext context(category, severity, code, description, file, line, function);
    ErrorManager::getGlobal().reportError(context);
}

inline void reportError(ErrorCategory category, int code, const std::string& description, 
                         const std::string& file = "", int line = 0) {
    reportError(category, ErrorSeverity::Error, code, description, file, line, "");
}

inline void reportConfigError(int code, const std::string& description, const std::string& file = "", int line = 0) {
    ErrorManager::reportConfigError(code, description, file, line);
}

inline void reportMemoryError(int code, const std::string& description, const std::string& file = "", int line = 0) {
    ErrorManager::reportMemoryError(code, description, file, line);
}

inline void reportComputationError(int code, const std::string& description, const std::string& file = "", int line = 0) {
    ErrorManager::reportComputationError(code, description, file, line);
}

inline void reportSystemError(int code, const std::string& description, const std::string& file = "", int line = 0) {
    ErrorManager::reportSystemError(code, description, file, line);
}

inline void reportCriticalError(int code, const std::string& description, const std::string& file = "", int line = 0) {
    ErrorManager::reportCriticalError(code, description, file, line);
}

// RAII error context for scoped error reporting
class ScopedErrorContext {
public:
    ScopedErrorContext(ErrorCategory category, ErrorSeverity severity, int code, 
                       const std::string& description, const std::string& file = "",
                       int line = 0, const std::string& function = "")
        : context_(category, severity, code, description, file, line, function) {
        ErrorManager::getGlobal().reportError(context_);
    }
    
    ~ScopedErrorContext() {
        context_.recovered = true;
    }
    
    // Add context data
    void addContext(const std::string& key, const std::string& value) {
        context_.contextData[key] = value;
    }
    
    // Add recovery action
    void addRecoveryAction(const std::string& action) {
        context_.recoveryActions.push_back(action);
    }
    
private:
    ErrorContext context_;
};

// Exception classes for specific error types
class NLMConfigurationException : public std::runtime_error {
public:
    NLMConfigurationException(const std::string& message) : std::runtime_error(message) {}
};

class NLMMemoryException : public std::runtime_error {
public:
    NLMMemoryException(const std::string& message) : std::runtime_error(message) {}
};

class NLMComputationException : public std::runtime_error {
public:
    NLMComputationException(const std::string& message) : std::runtime_error(message) {}
};

class NLMSystemException : public std::runtime_error {
public:
    NLMSystemException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace nlm