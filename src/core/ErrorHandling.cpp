// Error handling implementation for NLM
// This file implements the comprehensive error handling and recovery system

#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stacktrace>
#include <signal.h>

namespace nlm {

// Global error handlers and reporters
namespace {
    std::vector<std::unique_ptr<IErrorHandler>> globalErrorHandlers;
    std::vector<std::unique_ptr<IErrorReporter>> globalErrorReporters;
    std::unique_ptr<ErrorRecoverySystem> globalRecoverySystem;
    std::mutex registryMutex;
}

// Signal handler for crashes
void signalHandler(int signum) {
    std::string signalName;
    switch (signum) {
        case SIGSEGV: signalName = "Segmentation fault"; break;
        case SIGABRT: signalName = "Aborted"; break;
        case SIGFPE: signalName = "Floating point exception"; break;
        case SIGILL: signalName = "Illegal instruction"; break;
        case SIGINT: signalName = "Interrupt"; break;
        case SIGTERM: signalName = "Terminated"; break;
        default: signalName = "Unknown signal"; break;
    }
    
    ErrorInfo error;
    error.context.category = ErrorCategory::Internal;
    error.context.severity = ErrorSeverity::Fatal;
    error.context.sourceFile = "signal handler";
    error.context.sourceFunction = "signalHandler";
    error.context.sourceLine = 0;
    error.context.description = "Program crashed with signal: " + signalName;
    error.context.recoverySuggestion = "Program terminated due to fatal error";
    error.formattedMessage = "FATAL: " + signalName;
    error.handled = false;
    
    // Report to all error reporters
    ErrorHandling::ErrorReporterRegistry::getInstance().reportError(error);
    
    // Exit with error code
    std::exit(signum);
}

// Initialize signal handlers
void initializeSignalHandlers() {
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTERM, signalHandler);
}

// Global error reporting initialization
void initializeGlobalErrorReporting() {
    // Register default error handlers
    registerDefaultErrorHandlers();
    
    // Register default error reporters
    registerDefaultErrorReporters();
    
    // Initialize recovery system
    globalRecoverySystem = std::make_unique<ErrorRecoverySystem>();
    
    // Initialize signal handlers
    initializeSignalHandlers();
}

// Default error handlers
class DefaultErrorHandler : public IErrorHandler {
public:
    DefaultErrorHandler() = default;
    
    void handleError(const ErrorInfo& error) override {
        // Log to stderr for immediate visibility
        logToStderr(error);
    }
    
    void handleException(const NLMException& exception) override {
        ErrorInfo error;
        error.context = exception.getContext();
        error.formattedMessage = exception.getMessage();
        error.stackTrace = exception.getStackTrace();
        error.handled = false;
        
        handleError(error);
    }
    
    bool canHandle(const ErrorContext& context) const override {
        // Can handle all errors
        return true;
    }
    
    ErrorSeverity getMinSeverity() const override {
        return ErrorSeverity::Debug;
    }
    
private:
    void logToStderr(const ErrorInfo& error) {
        std::string severityStr = errorSeverityToString(error.context.severity);
        
        // Color codes for terminal output
        std::string colorCode;
        switch (error.context.severity) {
            case ErrorSeverity::Debug: colorCode = "\033[36m"; break; // Cyan
            case ErrorSeverity::Info: colorCode = "\033[32m"; break; // Green
            case ErrorSeverity::Warning: colorCode = "\033[33m"; break; // Yellow
            case ErrorSeverity::Error: colorCode = "\033[31m"; break; // Red
            case ErrorSeverity::Critical: colorCode = "\033[35m"; break; // Magenta
            case ErrorSeverity::Fatal: colorCode = "\033[1;31m"; break; // Bold Red
            default: colorCode = "\033[0m";
        }
        
        std::cerr << colorCode << "[" << severityStr << "] " << error.getFormattedTimestamp() << " " << error.context.sourceFile << ":" << error.context.sourceLine << " " << error.context.sourceFunction << " " << error.formattedMessage << "\033[0m" << std::endl;
        
        if (!error.context.recoverySuggestion.empty()) {
            std::cerr << colorCode << "  Suggestion: " << error.context.recoverySuggestion << "\033[0m" << std::endl;
        }
    }
    
    std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::map<ErrorSeverity, std::string> severityStrings = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARNING"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRITICAL"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        
        auto it = severityStrings.find(severity);
        return it != severityStrings.end() ? it->second : "UNKNOWN";
    }
};

// Default error reporters
class DefaultErrorReporter : public IErrorReporter {
public:
    DefaultErrorReporter() = default;
    
    void reportError(const ErrorInfo& error) override {
        // Use the global error handlers to process the error
        ErrorHandling::ErrorHandlerRegistry::getInstance().reportError(error);
    }
    
    void reportException(const NLMException& exception) override {
        ErrorHandling::ErrorHandlerRegistry::getInstance().reportException(exception);
    }
    
    void reportRecovery(const ErrorInfo& error, const std::string& recoveryAction) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Log recovery action
        std::cout << "[RECOVERY] " << error.getFormattedTimestamp() << ": " << recoveryAction << std::endl;
        
        // Update error as handled
        for (auto& storedError : recentErrors_) {
            if (storedError.id == error.id) {
                storedError.handled = true;
                storedError.handledTime = std::chrono::system_clock::now();
                break;
            }
        }
    }
    
    std::vector<ErrorInfo> getRecentErrors(size_t limit) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (limit >= recentErrors_.size()) {
            return recentErrors_;
        }
        
        std::vector<ErrorInfo> result;
        result.reserve(limit);
        auto start = recentErrors_.end() - limit;
        result.insert(result.end(), start, recentErrors_.end());
        return result;
    }
    
    void clearErrors() override {
        std::lock_guard<std::mutex> lock(mutex_);
        recentErrors_.clear();
    }
    
    size_t getErrorCount() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return recentErrors_.size();
    }
    
    // Store error for later retrieval
    void storeError(const ErrorInfo& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        recentErrors_.push_back(error);
        if (recentErrors_.size() > maxErrors_) {
            recentErrors_.erase(recentErrors_.begin());
        }
    }
    
private:
    std::mutex mutex_;
    std::vector<ErrorInfo> recentErrors_;
    static constexpr size_t maxErrors_ = 1000;
};

// Register default handlers and reporters
void registerDefaultErrorHandlers() {
    ErrorHandling::ErrorHandlerRegistry::getInstance().registerErrorHandler(
        std::make_unique<DefaultErrorHandler>());
}

void registerDefaultErrorReporters() {
    auto reporter = std::make_unique<DefaultErrorReporter>();
    
    // Store the reporter for error storage
    auto* rawReporter = reporter.get();
    
    ErrorHandling::ErrorReporterRegistry::getInstance().registerErrorReporter(std::move(reporter));
}

// Error handling initialization
void initializeErrorHandling() {
    initializeGlobalErrorReporting();
}

// Error handling cleanup
void cleanupErrorHandling() {
    ErrorHandling::ErrorReporterRegistry::getInstance().clearAllErrors();
}

// Global error handler registration functions
void registerErrorHandler(std::unique_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(registryMutex);
    ErrorHandling::ErrorHandlerRegistry::getInstance().registerErrorHandler(std::move(handler));
}

void unregisterErrorHandler(const IErrorHandler* handler) {
    std::lock_guard<std::mutex> lock(registryMutex);
    ErrorHandling::ErrorHandlerRegistry::getInstance().unregisterErrorHandler(handler);
}

void registerErrorReporter(std::unique_ptr<IErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(registryMutex);
    ErrorHandling::ErrorReporterRegistry::getInstance().registerErrorReporter(std::move(reporter));
}

void unregisterErrorReporter(const IErrorReporter* reporter) {
    std::lock_guard<std::mutex> lock(registryMutex);
    ErrorHandling::ErrorReporterRegistry::getInstance().unregisterErrorReporter(reporter);
}

void registerRecoveryHandler(ErrorCategory category, ErrorSeverity severity,
                            std::function<bool(const ErrorContext&)> canHandle,
                            std::function<std::string(const ErrorContext&)> recover) {
    std::lock_guard<std::mutex> lock(registryMutex);
    if (globalRecoverySystem) {
        globalRecoverySystem->registerRecoveryHandler(category, severity, canHandle, recover);
    }
}

std::string attemptRecovery(const ErrorContext& context, const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(registryMutex);
    if (globalRecoverySystem) {
        return globalRecoverySystem->attemptRecovery(context, errorMessage);
    }
    return "No recovery system available";
}

std::string getRecoveryStatistics() {
    std::lock_guard<std::mutex> lock(registryMutex);
    if (globalRecoverySystem) {
        return globalRecoverySystem->getRecoveryStatistics();
    }
    return "Recovery system not initialized";
}

// Error information utilities
std::string generateErrorId() {
    // Generate a unique error ID
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << "ERR_" << time_t << "_" << std::this_thread::get_id();
    return oss.str();
}

ErrorInfo createErrorInfo(const ErrorContext& context, const std::string& message) {
    ErrorInfo info;
    info.id = generateErrorId();
    info.context = context;
    info.formattedMessage = message;
    info.handled = false;
    return info;
}

// Macro for easy error handling in NLM code
#define NLM_ERROR_TRY \
    ErrorHandling::ErrorHandlerRegistry::getInstance().registerErrorHandler(\
        std::make_unique<DefaultErrorHandler>()); \
    try

#define NLM_ERROR_CATCH \
    } catch (const std::exception& e) { \
        ErrorHandling::ErrorReporterRegistry::getInstance().reportException(\
            NLMException(ErrorCategory::Internal, ErrorSeverity::Error, __FILE__, __FUNCTION__, __LINE__, e.what())); \
        throw; \
    }

// Safe execution with error handling
template<typename T>
T safeExecute(const std::string& operationName, std::function<T()> operation) {
    try {
        return operation();
    } catch (const NLMException& e) {
        ErrorHandling::ErrorReporterRegistry::getInstance().reportException(e);
        throw;
    } catch (const std::exception& e) {
        NLMException nlmException(ErrorCategory::Internal, ErrorSeverity::Error, __FILE__, __FUNCTION__, __LINE__, 
                                  "Error in " + operationName + ": " + std::string(e.what()));
        ErrorHandling::ErrorReporterRegistry::getInstance().reportException(nlmException);
        throw;
    }
}

// Performance monitoring with error handling
class SafePerformanceMonitor {
public:
    template<typename T>
    T monitoredOperation(const std::string& operationName, std::function<T()> operation) {
        auto startTime = std::chrono::system_clock::now();
        
        try {
            T result = operation();
            
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Log successful operation
            logOperationSuccess(operationName, duration.count(), true);
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Log failed operation
            logOperationSuccess(operationName, duration.count(), false);
            
            throw;
        }
    }
    
private:
    void logOperationSuccess(const std::string& operationName, long long durationMs, bool success) {
        std::string status = success ? "SUCCESS" : "FAILED";
        std::cout << "[" << status << "] " << operationName << " completed in " << durationMs << "ms" << std::endl;
    }
};

} // namespace nlm
