// Enhanced error handling for NLM
// Provides comprehensive error handling with context, recovery, and diagnostics

#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <sstream>
#include <chrono>
#include <mutex>
#include <exception>

namespace nlm {
namespace error {

// Error severity levels
enum class Severity {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
    FATAL = 5
};

// Error categories
enum class Category {
    MEMORY_ALLOCATION,
    NEURAL_COMPUTATION,
    PLASTICITY,
    MEMORY_SYSTEM,
    COGNITION,
    NEUROMODULATION,
    DEVELOPMENT,
    CONFIG,
    IO,
    NETWORK,
    SYSTEM,
    UNKNOWN
};

// Error context information
struct ErrorContext {
    std::string file;
    std::string function;
    int line;
    std::string operation;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ErrorContext() : line(0), timestamp(std::chrono::system_clock::now()) {}
    ErrorContext(const std::string& f, const std::string& fn, int l, const std::string& op)
        : file(f), function(fn), line(l), operation(op), timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced error class with context and recovery
class Error {
public:
    Error(Severity severity, Category category, const std::string& message,
           const ErrorContext& context = ErrorContext())
        : severity(severity), category(category), message(message), context(context),
          recoveryLevel(0), retryCount(0), maxRetries(0) {}
    
    // Add additional context
    void addContext(const std::string& key, const std::string& value) {
        context.metadata[key] = value;
    }
    
    // Set recovery options
    void setRecoveryLevel(int level, int retries = 0) {
        recoveryLevel = level;
        maxRetries = retries;
        retryCount = 0;
    }
    
    // Attempt recovery
    bool attemptRecovery() {
        if (recoveryLevel == 0) return true;  // No recovery needed
        if (retryCount >= maxRetries) return false;
        retryCount++;
        return recover();
    }
    
    // Virtual recovery method - subclasses should override
    virtual bool recover() {
        return false;
    }
    
    // Get error report
    std::string report() const {
        std::ostringstream oss;
        oss << "[SEVERITY: " << static_cast<int>(severity) << "] ";
        oss << "[CATEGORY: " << static_cast<int>(category) << "] ";
        oss << "[OPERATION: " << context.operation << "] ";
        oss << "[FILE: " << context.file << ":" << context.line << "] ";
        oss << "[FUNCTION: " << context.function << "]\n";
        oss << "MESSAGE: " << message << "\n";
        
        if (!context.metadata.empty()) {
            oss << "METADATA:\n";
            for (const auto& pair : context.metadata) {
                oss << "  " << pair.first << ": " << pair.second << "\n";
            }
        }
        
        return oss.str();
    }
    
    // Getters
    Severity getSeverity() const { return severity; }
    Category getCategory() const { return category; }
    const std::string& getMessage() const { return message; }
    const ErrorContext& getContext() const { return context; }
    bool isRecoverable() const { return recoveryLevel > 0; }
    bool canRetry() const { return retryCount < maxRetries; }
    
protected:
    Severity severity;
    Category category;
    std::string message;
    ErrorContext context;
    int recoveryLevel;
    int retryCount;
    int maxRetries;
    
private:
    virtual bool recover() { return false; }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    // Handle error - return true if error was handled, false if should propagate
    virtual bool handle(const Error& error) = 0;
    
    // Check if handler can handle this type of error
    virtual bool canHandle(Severity severity, Category category) const = 0;
    
    // Get handler name
    virtual const char* getName() const = 0;
};

// Default error handler that throws exceptions
class DefaultErrorHandler : public IErrorHandler {
public:
    bool handle(const Error& error) override {
        throw error;
    }
    
    bool canHandle(Severity severity, Category category) const override {
        return true;  // Handle all errors
    }
    
    const char* getName() const override {
        return "DefaultErrorHandler";
    }
};

// Logging error handler that logs errors
class LoggingErrorHandler : public IErrorHandler {
public:
    LoggingErrorHandler(std::function<void(const std::string&)> logger)
        : logger(logger) {}
    
    bool handle(const Error& error) override {
        std::ostringstream oss;
        oss << "ERROR: " << error.report();
        logger(oss.str());
        return true;  // Error was handled
    }
    
    bool canHandle(Severity severity, Category category) const override {
        return severity >= Severity::WARNING;  // Only handle warnings and above
    }
    
    const char* getName() const override {
        return "LoggingErrorHandler";
    }
    
private:
    std::function<void(const std::string&) > logger;
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    static ErrorRecoveryManager& getInstance() {
        static ErrorRecoveryManager instance;
        return instance;
    }
    
    // Add error handler
    void addHandler(std::unique_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.push_back(std::move(handler));
    }
    
    // Remove error handler
    void removeHandler(const char* name) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.erase(
            std::remove_if(handlers.begin(), handlers.end(),
                [name](const std::unique_ptr<IErrorHandler>& handler) {
                    return strcmp(handler->getName(), name) == 0;
                }),
            handlers.end()
        );
    }
    
    // Handle error
    bool handleError(const Error& error) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try each handler until one can handle it
        for (const auto& handler : handlers) {
            if (handler->canHandle(error.getSeverity(), error.getCategory())) {
                if (handler->handle(error)) {
                    return true;  // Error was handled
                }
            }
        }
        
        return false;  // No handler could handle this error
    }
    
    // Set default handler (if none provided)
    void ensureDefaultHandler() {
        std::lock_guard<std::mutex> lock(mutex);
        if (handlers.empty()) {
            handlers.push_back(std::make_unique<DefaultErrorHandler>());
        }
    }
    
private:
    ErrorRecoveryManager() = default;
    ~ErrorRecoveryManager() = default;
    ErrorRecoveryManager(const ErrorRecoveryManager&) = delete;
    ErrorRecoveryManager& operator=(const ErrorRecoveryManager&) = delete;
    
    std::vector<std::unique_ptr<IErrorHandler>> handlers;
    std::mutex mutex;
};

// Error macros for easy use
#define NLM_THROW_ERROR(severity, category, message) \
    do { \
        nlm::error::ErrorContext ctx(__FILE__, __func__, __LINE__, #message); \
        nlm::error::Error err(severity, category, message, ctx); \
        if (!nlm::error::ErrorRecoveryManager::getInstance().handleError(err)) { \
            throw err; \
        } \
    } while(0)

#define NLM_THROW_ERROR_WITH_CONTEXT(severity, category, message, contextOp) \
    do { \
        nlm::error::ErrorContext ctx(__FILE__, __func__, __LINE__, contextOp); \
        nlm::error::Error err(severity, category, message, ctx); \
        if (!nlm::error::ErrorRecoveryManager::getInstance().handleError(err)) { \
            throw err; \
        } \
    } while(0)

#define NLM_LOG_ERROR(message) \
    nlm::error::ErrorContext ctx(__FILE__, __func__, __LINE__, #message); \
    nlm::error::Error err(nlm::error::Severity::ERROR, nlm::error::Category::SYSTEM, message, ctx); \
    nlm::error::ErrorRecoveryManager::getInstance().handleError(err)

// System error types
class MemoryAllocationError : public Error {
public:
    MemoryAllocationError(const std::string& operation, size_t size, const ErrorContext& ctx = ErrorContext())
        : Error(Severity::CRITICAL, Category::MEMORY_ALLOCATION,
                "Memory allocation failed: " + operation + " (" + std::to_string(size) + " bytes)", ctx) {}
    
    bool recover() override {
        // Try to free memory
        return freeMemory() > 0;
    }
    
private:
    size_t freeMemory() {
        // Implementation depends on system
        return 0;
    }
};

class NeuralComputationError : public Error {
public:
    NeuralComputationError(const std::string& operation, const std::string& details,
                          const ErrorContext& ctx = ErrorContext())
        : Error(Severity::ERROR, Category::NEURAL_COMPUTATION,
                "Neural computation failed: " + operation + ": " + details, ctx) {}
    
    bool recover() override {
        // Reset state to valid state
        return resetSystemState();
    }
    
private:
    bool resetSystemState() {
        // Implementation depends on system
        return true;
    }
};

class PlasticityError : public Error {
public:
    PlasticityError(const std::string& rule, const std::string& details,
                   const ErrorContext& ctx = ErrorContext())
        : Error(Severity::WARNING, Category::PLASTICITY,
                "Plasticity rule failed: " + rule + ": " + details, ctx) {}
    
    bool recover() override {
        // Disable failing plasticity rule
        return disablePlasticityRule();
    }
    
private:
    bool disablePlasticityRule() {
        // Implementation depends on system
        return true;
    }
};

class ConfigError : public Error {
public:
    ConfigError(const std::string& operation, const std::string& key, const std::string& details,
                const ErrorContext& ctx = ErrorContext())
        : Error(Severity::ERROR, Category::CONFIG,
                "Configuration error: " + operation + " for key '" + key + "': " + details, ctx) {}
    
    bool recover() override {
        // Restore to default configuration
        return restoreDefaults();
    }
    
private:
    bool restoreDefaults() {
        // Implementation depends on system
        return true;
    }
};

class IOError : public Error {
public:
    IOError(const std::string& operation, const std::string& path, const std::string& details,
            const ErrorContext& ctx = ErrorContext())
        : Error(Severity::ERROR, Category::IO,
                "IO error: " + operation + " for path '" + path + "': " + details, ctx) {}
    
    bool recover() override {
        // Retry with different path or permissions
        return retryOperation();
    }
    
private:
    bool retryOperation() {
        // Implementation depends on system
        return false;
    }
};

// Error utilities
namespace utils {
    // Check if error can be recovered
    inline bool canRecover(const Error& error) {
        return error.isRecoverable() && error.canRetry();
    }
    
    // Get error severity string
    inline const char* severityToString(Severity severity) {
        switch (severity) {
            case Severity::DEBUG: return "DEBUG";
            case Severity::INFO: return "INFO";
            case Severity::WARNING: return "WARNING";
            case Severity::ERROR: return "ERROR";
            case Severity::CRITICAL: return "CRITICAL";
            case Severity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    // Get error category string
    inline const char* categoryToString(Category category) {
        switch (category) {
            case Category::MEMORY_ALLOCATION: return "MEMORY_ALLOCATION";
            case Category::NEURAL_COMPUTATION: return "NEURAL_COMPUTATION";
            case Category::PLASTICITY: return "PLASTICITY";
            case Category::MEMORY_SYSTEM: return "MEMORY_SYSTEM";
            case Category::COGNITION: return "COGNITION";
            case Category::NEUROMODULATION: return "NEUROMODULATION";
            case Category::DEVELOPMENT: return "DEVELOPMENT";
            case Category::CONFIG: return "CONFIG";
            case Category::IO: return "IO";
            case Category::NETWORK: return "NETWORK";
            case Category::SYSTEM: return "SYSTEM";
            case Category::UNKNOWN: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
}

} // namespace error

// Global error handler setup
namespace nlm {
    inline void setupDefaultErrorHandling() {
        error::ErrorRecoveryManager::getInstance().ensureDefaultHandler();
    }
    
    inline void addLoggingErrorHandler(std::function<void(const std::string&)> logger) {
        error::ErrorRecoveryManager::getInstance().addHandler(
            std::make_unique<error::LoggingErrorHandler>(logger)
        );
    }
    
    inline void removeErrorHandler(const char* name) {
        error::ErrorRecoveryManager::getInstance().removeHandler(name);
    }
}

} // namespace nlm