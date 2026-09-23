#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <mutex>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Critical,  // System cannot continue operation
    Error,     // Operation failed but system can continue
    Warning,   // Non-critical issue, system continues
    Info,      // Informational message
    Debug      // Debug information
};

// Error categories
enum class ErrorCategory {
    NullPointer,           // Null pointer access
    MemoryAccess,          // Memory access violations
    Configuration,         // Invalid configuration values
    FileIO,                // File input/output errors
    ResourceManagement,    // Memory/resource management issues
    Synchronization,       // Thread safety issues
    Validation,            // Input validation failures
    System,                // General system errors
    Custom                 // Custom error types
};

// Detailed error context
struct ErrorContext {
    std::string file;           // Source file where error occurred
    std::string function;        // Function where error occurred
    int line;                   // Line number in source file
    std::string component;      // Component name (Brain, Neuron, etc.)
    std::string operation;      // Operation being performed
    std::unordered_map<std::string, std::string> metadata;  // Additional context
    
    ErrorContext() : line(0) {}
    
    ErrorContext(const std::string& file, const std::string& function, 
                 int line, const std::string& component = "",
                 const std::string& operation = "")
        : file(file), function(function), line(line), 
          component(component), operation(operation) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Error in " << component << "::" << operation << "\n";
        ss << "  Location: " << file << ":" << line << " in " << function << "\n";
        if (!metadata.empty()) {
            ss << "  Context: ";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        return ss.str();
    }
};

// Base error class
class Error : public std::runtime_error {
protected:
    ErrorSeverity severity;
    ErrorCategory category;
    ErrorContext context;
    mutable std::string message;
    
public:
    Error(ErrorSeverity sev, ErrorCategory cat, const std::string& msg,
          const ErrorContext& ctx = ErrorContext())
        : std::runtime_error(msg), severity(sev), category(cat), context(ctx) {
        this->message = msg;
    }
    
    virtual ~Error() = default;
    
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const ErrorContext& getContext() const { return context; }
    const std::string& getMessage() const { return message; }
    
    std::string format() const {
        std::stringstream ss;
        ss << "[Severity: " << static_cast<int>(severity) << "] "
           << "[Category: " << static_cast<int>(category) << "] "
           << getMessage() << "\n" << context.toString();
        return ss.str();
    }
};

// Specific error types
class CriticalError : public Error {
public:
    CriticalError(const std::string& msg, const ErrorContext& ctx = ErrorContext())
        : Error(ErrorSeverity::Critical, ErrorCategory::Custom, msg, ctx) {}
};

class MemoryError : public Error {
public:
    MemoryError(const std::string& msg, const ErrorContext& ctx = ErrorContext())
        : Error(ErrorSeverity::Critical, ErrorCategory::MemoryAccess, msg, ctx) {}
};

class NullPointerError : public Error {
public:
    NullPointerError(const std::string& msg, const ErrorContext& ctx = ErrorContext())
        : Error(ErrorSeverity::Critical, ErrorCategory::NullPointer, msg, ctx) {}
};

class ConfigurationError : public Error {
public:
    ConfigurationError(const std::string& msg, const ErrorContext& ctx = ErrorContext())
        : Error(ErrorSeverity::Error, ErrorCategory::Configuration, msg, ctx) {}
};

class FileIOError : public Error {
public:
    FileIOError(const std::string& msg, const ErrorContext& ctx = ErrorContext())
        : Error(ErrorSeverity::Error, ErrorCategory::FileIO, msg, ctx) {}
};

// Error recovery strategy
enum class ErrorRecovery {
    Immediate,      // Attempt to recover immediately
    Graceful,       // Continue with default/safe values
    Abort,          // Abort current operation
    Retry,          // Retry operation
    Shutdown,       // Shutdown system
    None            // No recovery
};

// Error reporter interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void report(const Error& error) = 0;
    virtual void setErrorSeverity(ErrorSeverity severity) = 0;
    virtual bool isErrorReportingEnabled() const = 0;
};

// Console error reporter
class ConsoleErrorReporter : public ErrorReporter {
public:
    void report(const Error& error) override {
        if (!isErrorReportingEnabled()) return;
        
        std::string formatted = error.format();
        switch (error.getSeverity()) {
            case ErrorSeverity::Critical:
                // Use color codes for critical errors
                std::cerr << "\033[1;31m" << formatted << "\033[0m" << std::endl;
                break;
            case ErrorSeverity::Error:
                std::cerr << "\033[31m" << formatted << "\033[0m" << std::endl;
                break;
            case ErrorSeverity::Warning:
                std::cerr << "\033[33m" << formatted << "\033[0m" << std::endl;
                break;
            case ErrorSeverity::Info:
                std::cerr << "\033[36m" << formatted << "\033[0m" << std::endl;
                break;
            case ErrorSeverity::Debug:
                std::cerr << "\033[90m" << formatted << "\033[0m" << std::endl;
                break;
        }
    }
    
    void setErrorSeverity(ErrorSeverity severity) override {
        // Could store severity preference
    }
    
    bool isErrorReportingEnabled() const override {
        return true;  // Always report to console
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual ErrorRecovery handle(const Error& error) = 0;
    virtual bool shouldHandle(ErrorCategory category) const = 0;
    virtual void setContinueOnError(bool continueOnError) = 0;
};

// Default error handler
class DefaultErrorHandler : public ErrorHandler {
private:
    bool continueOnError;
    std::vector<std::unique_ptr<ErrorReporter>> reporters;
    mutable std::mutex mutex;
    
public:
    DefaultErrorHandler() : continueOnError(true) {
        reporters.push_back(std::make_unique<ConsoleErrorReporter>());
    }
    
    ErrorRecovery handle(const Error& error) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Report error through all reporters
        for (auto& reporter : reporters) {
            reporter->report(error);
        }
        
        // Determine recovery strategy based on error severity and category
        if (error.getSeverity() == ErrorSeverity::Critical) {
            if (error.getCategory() == ErrorCategory::NullPointer || 
                error.getCategory() == ErrorCategory::MemoryAccess) {
                return ErrorRecovery::Abort;
            }
            return ErrorRecovery::Shutdown;
        }
        
        if (error.getSeverity() == ErrorSeverity::Error) {
            if (error.getCategory() == ErrorCategory::Configuration ||
                error.getCategory() == ErrorCategory::FileIO) {
                return continueOnError ? ErrorRecovery::Graceful : ErrorRecovery::Abort;
            }
            return ErrorRecovery::Abort;
        }
        
        if (error.getSeverity() == ErrorSeverity::Warning) {
            return continueOnError ? ErrorRecovery::Immediate : ErrorRecovery::None;
        }
        
        if (error.getSeverity() == ErrorSeverity::Info || 
            error.getSeverity() == ErrorSeverity::Debug) {
            return ErrorRecovery::None;
        }
        
        return ErrorRecovery::None;
    }
    
    bool shouldHandle(ErrorCategory category) const override {
        return true;  // Handle all categories
    }
    
    void setContinueOnError(bool continueOnError) override {
        std::lock_guard<std::mutex> lock(mutex);
        this->continueOnError = continueOnError;
    }
    
    void addReporter(std::unique_ptr<ErrorReporter> reporter) {
        std::lock_guard<std::mutex> lock(mutex);
        reporters.push_back(std::move(reporter));
    }
};

// Global error handler manager
class ErrorManager {
private:
    std::unique_ptr<DefaultErrorHandler> errorHandler;
    std::vector<std::unique_ptr<ErrorHandler>> customHandlers;
    mutable std::mutex mutex;
    
    ErrorManager() {
        errorHandler = std::make_unique<DefaultErrorHandler>();
    }
    
public:
    static ErrorManager& instance() {
        static ErrorManager instance;
        return instance;
    }
    
    void reportError(const Error& error) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try custom handlers first
        for (auto& handler : customHandlers) {
            if (handler->shouldHandle(error.getCategory())) {
                ErrorRecovery recovery = handler->handle(error);
                if (recovery != ErrorRecovery::None) {
                    return;  // Handled by custom handler
                }
            }
        }
        
        // Fall back to default handler
        ErrorRecovery recovery = errorHandler->handle(error);
        
        switch (recovery) {
            case ErrorRecovery::Abort:
                throw;  // Re-throw the error
            case ErrorRecovery::Graceful:
                // Continue with safe defaults
                break;
            case ErrorRecovery::Immediate:
                // Attempt recovery
                break;
            case ErrorRecovery::Retry:
                // Retry logic would go here
                break;
            case ErrorRecovery::Shutdown:
                // Shutdown system
                std::exit(1);
            case ErrorRecovery::None:
            default:
                break;
        }
    }
    
    void setContinueOnError(bool continueOnError) {
        std::lock_guard<std::mutex> lock(mutex);
        errorHandler->setContinueOnError(continueOnError);
    }
    
    void addErrorHandler(std::unique_ptr<ErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        customHandlers.push_back(std::move(handler));
    }
    
    void addReporter(std::unique_ptr<ErrorReporter> reporter) {
        std::lock_guard<std::mutex> lock(mutex);
        errorHandler->addReporter(std::move(reporter));
    }
};

// Configuration validation utilities
namespace Validation {
    template<typename T>
    bool isValid(const T& value, const std::string& errorMsg, ErrorCategory category = ErrorCategory::Validation) {
        if (!std::isfinite(value)) {
            ConfigurationError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
            ErrorManager::instance().reportError(error);
            return false;
        }
        return true;
    }
    
    bool isPositive(float value, const std::string& errorMsg) {
        if (value <= 0.0f) {
            ConfigurationError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
            ErrorManager::instance().reportError(error);
            return false;
        }
        return true;
    }
    
    bool isNonNegative(float value, const std::string& errorMsg) {
        if (value < 0.0f) {
            ConfigurationError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
            ErrorManager::instance().reportError(error);
            return false;
        }
        return true;
    }
    
    bool isInRange(float value, float min, float max, const std::string& errorMsg) {
        if (value < min || value > max) {
            ConfigurationError error(errorMsg, ErrorContext(__FILE__, __func__, __LINE__));
            ErrorManager::instance().reportError(error);
            return false;
        }
        return true;
    }
}

} // namespace nlm
