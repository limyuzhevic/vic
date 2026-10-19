# Advanced error handling and logging system for NLM
// Provides comprehensive error reporting, exception handling, and logging capabilities

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <functional>
#include "../core/Config/Config.hpp"

namespace nlm {

// Error categories
enum class ErrorCategory {
    None,
    Configuration,
    MemoryAllocation,
    NeuralSimulation,
    InputOutput,
    NetworkCommunication,
    Validation,
    Permission,
    ResourceExhaustion,
    Algorithm,
    Development,
    Performance,
    Internal
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error context information
struct ErrorContext {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string sourceFile;
    std::string sourceFunction;
    int sourceLine;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    std::string recoverySuggestion;
    std::map<std::string, std::string> metadata;
    std::string correlationId; // For tracking related errors
    
    ErrorContext()
        : category(ErrorCategory::None), severity(ErrorSeverity::Error),
          sourceLine(0), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorContext(ErrorCategory cat, ErrorSeverity sev, const std::string& file, 
                 const std::string& function, int line, const std::string& desc)
        : category(cat), severity(sev), sourceFile(file), sourceFunction(function),
          sourceLine(line), timestamp(std::chrono::system_clock::now()),
          description(desc) {}
};

// Error information for reporting
struct ErrorInfo {
    std::string id;
    ErrorContext context;
    std::string formattedMessage;
    std::vector<std::string> stackTrace;
    std::shared_ptr<Exception> rootCause; // For chained exceptions
    bool handled;
    std::chrono::system_clock::time_point handledTime;
    
    ErrorInfo() : handled(false) {}
    
    std::string getFormattedTimestamp() const {
        auto time_t = std::chrono::system_clock::to_time_t(context.timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
};

// NLM-specific exceptions
class NLMException : public std::exception {
protected:
    ErrorContext context_;
    std::string message_;
    std::vector<std::string> stackTrace_;
    
public:
    NLMException(ErrorCategory category, ErrorSeverity severity,
                 const std::string& file, const std::string& function, int line,
                 const std::string& message)
        : context_(category, severity, file, function, line, message),
          message_(message) {
        captureStackTrace();
    }
    
    virtual ~NLMException() noexcept = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const ErrorContext& getContext() const noexcept {
        return context_;
    }
    
    const std::string& getMessage() const noexcept {
        return message_;
    }
    
    const std::vector<std::string>& getStackTrace() const noexcept {
        return stackTrace_;
    }
    
    std::string getFormattedDetails() const {
        std::ostringstream oss;
        oss << "=== NLM Exception ===" << std::endl;
        oss << "Time: " << getFormattedTimestamp() << std::endl;
        oss << "Category: " << errorCategoryToString(context_.category) << std::endl;
        oss << "Severity: " << errorSeverityToString(context_.severity) << std::endl;
        oss << "File: " << context_.sourceFile << std::endl;
        oss << "Function: " << context_.sourceFunction << std::endl;
        oss << "Line: " << context_.sourceLine << std::endl;
        oss << "Message: " << message_ << std::endl;
        if (!context_.recoverySuggestion.empty()) {
            oss << "Recovery: " << context_.recoverySuggestion << std::endl;
        }
        oss << "Stack Trace:" << std::endl;
        for (const auto& frame : stackTrace_) {
            oss << "  " << frame << std::endl;
        }
        return oss.str();
    }
    
protected:
    void captureStackTrace() {
        // Capture stack trace using platform-specific methods
        // This is a simplified implementation
        stackTrace_.push_back("  At " + context_.sourceFunction + ":" + std::to_string(context_.sourceLine));
        stackTrace_.push_back("  File: " + context_.sourceFile);
    }
    
    std::string getFormattedTimestamp() const {
        auto time_t = std::chrono::system_clock::to_time_t(context_.timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "None"},
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::MemoryAllocation, "Memory Allocation"},
            {ErrorCategory::NeuralSimulation, "Neural Simulation"},
            {ErrorCategory::InputOutput, "Input/Output"},
            {ErrorCategory::NetworkCommunication, "Network Communication"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::Permission, "Permission"},
            {ErrorCategory::ResourceExhaustion, "Resource Exhaustion"},
            {ErrorCategory::Algorithm, "Algorithm"},
            {ErrorCategory::Development, "Development"},
            {ErrorCategory::Performance, "Performance"},
            {ErrorCategory::Internal, "Internal"}
        };
        
        auto it = categoryStrings.find(category);
        return it != categoryStrings.end() ? it->second : "Unknown";
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
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

// Specific NLM exception types
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& file, const std::string& function, int line,
                           const std::string& message)
        : NLMException(ErrorCategory::Configuration, ErrorSeverity::Error, file, function, line, message) {}
};

class MemoryAllocationException : public NLMException {
public:
    MemoryAllocationException(const std::string& file, const std::string& function, int line,
                             const std::string& message)
        : NLMException(ErrorCategory::MemoryAllocation, ErrorSeverity::Critical, file, function, line, message) {}
};

class NeuralSimulationException : public NLMException {
public:
    NeuralSimulationException(const std::string& file, const std::string& function, int line,
                             const std::string& message)
        : NLMException(ErrorCategory::NeuralSimulation, ErrorSeverity::Error, file, function, line, message) {}
};

class InputOutputException : public NLMException {
public:
    InputOutputException(const std::string& file, const std::string& function, int line,
                        const std::string& message)
        : NLMException(ErrorCategory::InputOutput, ErrorSeverity::Error, file, function, line, message) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& file, const std::string& function, int line,
                        const std::string& message)
        : NLMException(ErrorCategory::Validation, ErrorSeverity::Warning, file, function, line, message) {}
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual void handleException(const NLMException& exception) = 0;
    virtual bool canHandle(const ErrorContext& context) const = 0;
    virtual ErrorSeverity getMinSeverity() const = 0;
};

// Error reporter interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    
    virtual void reportError(const ErrorInfo& error) = 0;
    virtual void reportException(const NLMException& exception) = 0;
    virtual void reportRecovery(const ErrorInfo& error, const std::string& recoveryAction) = 0;
    virtual std::vector<ErrorInfo> getRecentErrors(size_t limit = 100) = 0;
    virtual void clearErrors() = 0;
    virtual size_t getErrorCount() const = 0;
};

// Console error reporter
class ConsoleErrorReporter : public IErrorReporter {
public:
    ConsoleErrorReporter() = default;
    
    void reportError(const ErrorInfo& error) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Format error based on severity
        std::string colorCode;
        std::string severityStr = errorContextToString(error.context.severity);
        
        switch (error.context.severity) {
            case ErrorSeverity::Debug:
                colorCode = "\033[36m"; // Cyan
                break;
            case ErrorSeverity::Info:
                colorCode = "\033[32m"; // Green
                break;
            case ErrorSeverity::Warning:
                colorCode = "\033[33m"; // Yellow
                break;
            case ErrorSeverity::Error:
                colorCode = "\033[31m"; // Red
                break;
            case ErrorSeverity::Critical:
                colorCode = "\033[35m"; // Magenta
                break;
            case ErrorSeverity::Fatal:
                colorCode = "\033[1;31m"; // Bold Red
                break;
            default:
                colorCode = "\033[0m";
        }
        
        std::cout << colorCode << "[" << severityStr << "] " << getFormattedTimestamp(error) << " " << error.formattedMessage << "\033[0m" << std::endl;
        
        if (!error.context.sourceFile.empty()) {
            std::cout << colorCode << "  File: " << error.context.sourceFile << ", " << error.context.sourceFunction << ":" << error.context.sourceLine << "\033[0m" << std::endl;
        }
        
        if (!error.context.recoverySuggestion.empty()) {
            std::cout << colorCode << "  Suggestion: " << error.context.recoverySuggestion << "\033[0m" << std::endl;
        }
        
        // Store error in recent errors
        recentErrors_.push_back(error);
        if (recentErrors_.size() > maxErrors_) {
            recentErrors_.erase(recentErrors_.begin());
        }
    }
    
    void reportException(const NLMException& exception) override {
        ErrorInfo error;
        error.context = exception.getContext();
        error.formattedMessage = exception.getMessage();
        error.stackTrace = exception.getStackTrace();
        error.handled = false;
        
        reportError(error);
    }
    
    void reportRecovery(const ErrorInfo& error, const std::string& recoveryAction) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "\033[32m[RECOVERY] " << getFormattedTimestamp(error) << ": " << recoveryAction << "\033[0m" << std::endl;
        
        // Update error as handled
        for (auto& storedError : recentErrors_) {
            if (storedError.id == error.id) {
                storedError.handled = true;
                storedError.handledTime = std::chrono::system_clock::now();
                break;
            }
        }
    }
    
    std::vector<ErrorInfo> getRecentErrors(size_t limit = 100) override {
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
    
    // Export errors to file
    bool exportErrors(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        try {
            if (!std::filesystem::exists(std::filesystem::path(filename).parent_path())) {
                std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
            }
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "=== NLM Error Log ===" << std::endl;
            file << "Generated: " << getCurrentTimestamp() << std::endl;
            file << "Total Errors: " << recentErrors_.size() << std::endl;
            file << std::endl;
            
            for (const auto& error : recentErrors_) {
                file << "--- Error " << error.id << " ---" << std::endl;
                file << "Time: " << getFormattedTimestamp(error) << std::endl;
                file << "Category: " << errorContextToString(error.context.category) << std::endl;
                file << "Severity: " << errorContextToString(error.context.severity) << std::endl;
                file << "Message: " << error.formattedMessage << std::endl;
                file << "Handled: " << (error.handled ? "Yes" : "No") << std::endl;
                if (error.handled) {
                    file << "Handled Time: " << getFormattedTimestamp(error, error.handledTime) << std::endl;
                }
                file << std::endl;
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
private:
    std::string errorContextToString(const ErrorContext& context) {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "NONE"},
            {ErrorCategory::Configuration, "CONFIG"},
            {ErrorCategory::MemoryAllocation, "MEMORY"},
            {ErrorCategory::NeuralSimulation, "NEURAL"},
            {ErrorCategory::InputOutput, "IO"},
            {ErrorCategory::NetworkCommunication, "NETWORK"},
            {ErrorCategory::Validation, "VALIDATION"},
            {ErrorCategory::Permission, "PERMISSION"},
            {ErrorCategory::ResourceExhaustion, "RESOURCE"},
            {ErrorCategory::Algorithm, "ALGORITHM"},
            {ErrorCategory::Development, "DEVELOPMENT"},
            {ErrorCategory::Performance, "PERFORMANCE"},
            {ErrorCategory::Internal, "INTERNAL"}
        };
        
        std::string categoryStr = "UNKNOWN";
        auto it = categoryStrings.find(context.category);
        if (it != categoryStrings.end()) {
            categoryStr = it->second;
        }
        
        return categoryStr + "." + errorSeverityToString(context.severity);
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
    
    std::string getFormattedTimestamp(const ErrorInfo& error) {
        return error.getFormattedTimestamp();
    }
    
    std::string getFormattedTimestamp(const ErrorInfo& error, const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::mutex mutex_;
    std::vector<ErrorInfo> recentErrors_;
    static constexpr size_t maxErrors_ = 1000;
};

// Error recovery system
class ErrorRecoverySystem {
public:
    ErrorRecoverySystem() = default;
    
    // Register a recovery handler
    void registerRecoveryHandler(ErrorCategory category, ErrorSeverity severity,
                                 std::function<bool(const ErrorContext&)> canHandle,
                                 std::function<std::string(const ErrorContext&)> recover) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        RecoveryHandler handler;
        handler.canHandle = canHandle;
        handler.recover = recover;
        
        recoveryHandlers_.push_back(handler);
    }
    
    // Attempt to recover from an error
    std::string attemptRecovery(const ErrorContext& context, const std::string& errorMessage) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& handler : recoveryHandlers_) {
            if (handler.canHandle(context)) {
                try {
                    std::string result = handler.recover(context);
                    return result;
                } catch (const std::exception& e) {
                    return "Recovery failed: " + std::string(e.what());
                }
            }
        }
        
        return "No recovery handler available for this error";
    }
    
    // Get recovery statistics
    std::string getRecoveryStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ostringstream oss;
        oss << "=== Error Recovery Statistics ===" << std::endl;
        oss << "Total recovery handlers: " << recoveryHandlers_.size() << std::endl;
        oss << "Categories covered:" << std::endl;
        
        std::set<ErrorCategory> categories;
        for (const auto& handler : recoveryHandlers_) {
            // In a real implementation, this would extract categories from handlers
            categories.insert(ErrorCategory::None);
        }
        
        for (const auto& category : categories) {
            oss << "  - " << errorCategoryToString(category) << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear recovery handlers
    void clearRecoveryHandlers() {
        std::lock_guard<std::mutex> lock(mutex_);
        recoveryHandlers_.clear();
    }
    
private:
    struct RecoveryHandler {
        std::function<bool(const ErrorContext&)> canHandle;
        std::function<std::string(const ErrorContext&)> recover;
    };
    
    std::mutex mutex_;
    std::vector<RecoveryHandler> recoveryHandlers_;
    
    std::string errorCategoryToString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "None"},
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::MemoryAllocation, "Memory Allocation"},
            {ErrorCategory::NeuralSimulation, "Neural Simulation"},
            {ErrorCategory::InputOutput, "Input/Output"},
            {ErrorCategory::NetworkCommunication, "Network Communication"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::Permission, "Permission"},
            {ErrorCategory::ResourceExhaustion, "Resource Exhaustion"},
            {ErrorCategory::Algorithm, "Algorithm"},
            {ErrorCategory::Development, "Development"},
            {ErrorCategory::Performance, "Performance"},
            {ErrorCategory::Internal, "Internal"}
        };
        
        auto it = categoryStrings.find(category);
        return it != categoryStrings.end() ? it->second : "Unknown";
    }
};

// Global error handling utilities
namespace ErrorHandling {
    // Global error handler registry
    class ErrorHandlerRegistry {
    public:
        static ErrorHandlerRegistry& getInstance() {
            static ErrorHandlerRegistry instance;
            return instance;
        }
        
        void registerErrorHandler(std::unique_ptr<IErrorHandler> handler) {
            if (handler) {
                handlers_.push_back(std::move(handler));
                std::sort(handlers_.begin(), handlers_.end(),
                         [](const auto& a, const auto& b) {
                             return a->getMinSeverity() < b->getMinSeverity();
                         });
            }
        }
        
        void unregisterErrorHandler(const IErrorHandler* handler) {
            handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(),
                                           [handler](const auto& h) { return h.get() == handler; }),
                            handlers_.end());
        }
        
        void reportError(const ErrorInfo& error) {
            for (const auto& handler : handlers_) {
                if (handler->canHandle(error.context) && 
                    error.context.severity >= handler->getMinSeverity()) {
                    handler->handleError(error);
                }
            }
        }
        
        void reportException(const NLMException& exception) {
            for (const auto& handler : handlers_) {
                if (exception.getContext().severity >= handler->getMinSeverity()) {
                    handler->handleException(exception);
                }
            }
        }
        
    private:
        ErrorHandlerRegistry() = default;
        std::vector<std::unique_ptr<IErrorHandler>> handlers_;
    };
    
    // Global error reporter registry
    class ErrorReporterRegistry {
    public:
        static ErrorReporterRegistry& getInstance() {
            static ErrorReporterRegistry instance;
            return instance;
        }
        
        void registerErrorReporter(std::unique_ptr<IErrorReporter> reporter) {
            if (reporter) {
                reporters_.push_back(std::move(reporter));
            }
        }
        
        void unregisterErrorReporter(const IErrorReporter* reporter) {
            reporters_.erase(std::remove_if(reporters_.begin(), reporters_.end(),
                                            [reporter](const auto& r) { return r.get() == reporter; }),
                             reporters_.end());
        }
        
        void reportError(const ErrorInfo& error) {
            for (const auto& reporter : reporters_) {
                reporter->reportError(error);
            }
        }
        
        void reportException(const NLMException& exception) {
            for (const auto& reporter : reporters_) {
                reporter->reportException(exception);
            }
        }
        
        void reportRecovery(const ErrorInfo& error, const std::string& recoveryAction) {
            for (const auto& reporter : reporters_) {
                reporter->reportRecovery(error, recoveryAction);
            }
        }
        
        std::vector<ErrorInfo> getAllErrors(size_t limit = 100) {
            std::vector<ErrorInfo> allErrors;
            for (const auto& reporter : reporters_) {
                std::vector<ErrorInfo> errors = reporter->getRecentErrors(limit);
                allErrors.insert(allErrors.end(), errors.begin(), errors.end());
            }
            return allErrors;
        }
        
        void clearAllErrors() {
            for (const auto& reporter : reporters_) {
                reporter->clearErrors();
            }
        }
        
    private:
        ErrorReporterRegistry() = default;
        std::vector<std::unique_ptr<IErrorReporter>> reporters_;
    };
    
    // Exception conversion utilities
    template<typename T>
    T convertToException(const std::exception& e) {
        // Convert standard exceptions to NLM exceptions
        return T("", "", 0, e.what());
    }
    
    // Error context extraction from current state
    ErrorContext extractErrorContext(const std::string& file, const std::string& function, int line) {
        ErrorContext context;
        context.sourceFile = file;
        context.sourceFunction = function;
        context.sourceLine = line;
        return context;
    }
    
    // Error severity level from string
    ErrorSeverity parseErrorSeverity(const std::string& level) {
        std::string lower = level;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "debug") return ErrorSeverity::Debug;
        if (lower == "info") return ErrorSeverity::Info;
        if (lower == "warning") return ErrorSeverity::Warning;
        if (lower == "error") return ErrorSeverity::Error;
        if (lower == "critical") return ErrorSeverity::Critical;
        if (lower == "fatal") return ErrorSeverity::Fatal;
        
        return ErrorSeverity::Error;
    }
}

// NLM-specific exception macros
#define NLM_THROW_CONFIGURATION_EXCEPTION(message) \
    throw ConfigurationException(__FILE__, __FUNCTION__, __LINE__, message)

#define NLM_THROW_MEMORY_ALLOCATION_EXCEPTION(message) \
    throw MemoryAllocationException(__FILE__, __FUNCTION__, __LINE__, message)

#define NLM_THROW_NEURAL_SIMULATION_EXCEPTION(message) \
    throw NeuralSimulationException(__FILE__, __FUNCTION__, __LINE__, message)

#define NLM_THROW_INPUT_OUTPUT_EXCEPTION(message) \
    throw InputOutputException(__FILE__, __FUNCTION__, __LINE__, message)

#define NLM_THROW_VALIDATION_EXCEPTION(message) \
    throw ValidationException(__FILE__, __FUNCTION__, __LINE__, message)

#define NLM_TRY_CATCH_ERROR(handler) \
    try { \
        handler; \
    } catch (const NLMException& e) { \
        ErrorHandling::ErrorHandlerRegistry::getInstance().reportException(e); \
        throw; \
    } catch (const std::exception& e) { \
        NLMException nlmException(ErrorCategory::Internal, ErrorSeverity::Error, __FILE__, __FUNCTION__, __LINE__, e.what()); \
        ErrorHandling::ErrorHandlerRegistry::getInstance().reportException(nlmException); \
        throw; \
    }

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
