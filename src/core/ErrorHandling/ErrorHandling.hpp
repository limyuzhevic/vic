// NLM (熙然) - Neural Learning Machine
// Phase 6 Advanced Error Handling System
//
// This module provides comprehensive error handling, logging, and debugging
// capabilities for the NLM brain simulation framework.

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <type_traits>

// Error categories for better error management
enum class ErrorCategory {
    Configuration,      // Configuration and initialization errors
    Simulation,         // Simulation runtime errors
    Memory,             // Memory allocation and management errors
    InputOutput,        // File I/O and data access errors
    Network,            // Network and communication errors
    Resource,           // Resource availability errors
    Validation,         // Data validation errors
    Unknown             // Unknown or unexpected errors
};

// Error severity levels
enum class ErrorSeverity {
    Debug,              // Debug information
    Info,               // General information
    Warning,            // Warning conditions
    Error,              // Error conditions
    Critical,           // Critical errors requiring immediate attention
    Fatal               // Fatal errors causing termination
};

// NLM Error class with detailed information
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             ErrorCategory category = ErrorCategory::Unknown,
             ErrorSeverity severity = ErrorSeverity::Error,
             const std::string& source = "")
        : std::runtime_error(formatMessage(message, category, severity)),
          category_(category),
          severity_(severity),
          source_(source),
          timestamp_(std::chrono::high_resolution_clock::now()),
          stackTrace_(),
          context_(),
          recoveryAction_("none") {
        collectStackTrace();
    }
    
    // Getters
    ErrorCategory getCategory() const noexcept { return category_; }
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    const std::string& getSource() const noexcept { return source_; }
    const std::string& getStackTrace() const noexcept { return stackTrace_; }
    const std::string& getContext() const noexcept { return context_; }
    const std::string& getRecoveryAction() const noexcept { return recoveryAction_; }
    const std::chrono::high_resolution_clock::time_point& getTimestamp() const noexcept { return timestamp_; }
    
    // Setters
    void setContext(const std::string& context) { context_ = context; }
    void setRecoveryAction(const std::string& action) { recoveryAction_ = action; }
    
    // Convert to string with detailed information
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << timestampToString(timestamp_) << "] "
            << "[Category: " << errorCategoryToString(category_) << "] "
            << "[Severity: " << errorSeverityToString(severity_) << "] "
            << "[Source: " << source_ << "]\n"
            << "Message: " << what() << "\n"
            << "Context: " << context_ << "\n"
            << "Recovery Action: " << recoveryAction_ << "\n"
            << "Stack Trace:\n" << stackTrace_;
        return oss.str();
    }
    
    // Check if error is recoverable
    bool isRecoverable() const {
        return severity_ == ErrorSeverity::Warning || 
               severity_ == ErrorSeverity::Error ||
               severity_ == ErrorSeverity::Critical;
    }
    
    // Check if error is fatal
    bool isFatal() const {
        return severity_ == ErrorSeverity::Fatal || severity_ == ErrorSeverity::Critical;
    }
    
    // Static helper methods
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::unordered_map<ErrorCategory, std::string> names = {
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::Simulation, "Simulation"},
            {ErrorCategory::Memory, "Memory"},
            {ErrorCategory::InputOutput, "InputOutput"},
            {ErrorCategory::Network, "Network"},
            {ErrorCategory::Resource, "Resource"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::Unknown, "Unknown"}
        };
        auto it = names.find(category);
        return it != names.end() ? it->second : "Unknown";
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::unordered_map<ErrorSeverity, std::string> names = {
            {ErrorSeverity::Debug, "Debug"},
            {ErrorSeverity::Info, "Info"},
            {ErrorSeverity::Warning, "Warning"},
            {ErrorSeverity::Error, "Error"},
            {ErrorSeverity::Critical, "Critical"},
            {ErrorSeverity::Fatal, "Fatal"}
        };
        auto it = names.find(severity);
        return it != names.end() ? it->second : "Unknown";
    }
    
    static std::string timestampToString(const std::chrono::high_resolution_clock::time_point& tp) {
        auto duration = tp.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        
        std::tm timeInfo;
        std::time_t timeT = static_cast<std::time_t>(seconds);
#ifdef _WIN32
        localtime_s(&timeInfo, &timeT);
#else
        localtime_r(&timeT, &timeInfo);
#endif
        
        std::ostringstream oss;
        oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S") << '.' 
            << std::setfill('0') << std::setw(3) << milliseconds;
        return oss.str();
    }
    
private:
    static std::string formatMessage(const std::string& message, ErrorCategory category, ErrorSeverity severity) {
        std::ostringstream oss;
        oss << "[" << errorCategoryToString(category) << "] "
            << "[" << errorSeverityToString(severity) << "] "
            << message;
        return oss.str();
    }
    
    void collectStackTrace() {
        // Collect stack trace using platform-specific method
        // For now, create a simple placeholder
        stackTrace_ = "Stack trace collection not implemented yet";
    }
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string source_;
    std::chrono::high_resolution_clock::time_point timestamp_;
    std::string stackTrace_;
    std::string context_;
    std::string recoveryAction_;
};

// Exception factory functions
inline NLMError makeError(ErrorCategory category, const std::string& message, 
                         ErrorSeverity severity = ErrorSeverity::Error,
                         const std::string& source = "") {
    return NLMError(message, category, severity, source);
}

inline NLMError makeConfigError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
    return makeError(ErrorCategory::Configuration, message, severity, "Config");
}

inline NLMError makeSimulationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
    return makeError(ErrorCategory::Simulation, message, severity, "Simulation");
}

inline NLMError makeMemoryError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
    return makeError(ErrorCategory::Memory, message, severity, "Memory");
}

inline NLMError makeIOError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
    return makeError(ErrorCategory::InputOutput, message, severity, "IO");
}

// Custom exception types for different error scenarios
class ConfigValidationError : public NLMError {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : NLMError("Configuration validation failed: " + message, 
                  ErrorCategory::Validation, ErrorSeverity::Error, "Config") {
        if (!key.empty()) {
            setContext("Key: " + key);
        }
        setRecoveryAction("Check configuration file format and key names");
    }
};

class SimulationRuntimeError : public NLMError {
public:
    SimulationRuntimeError(const std::string& message, uint64_t step = 0)
        : NLMError("Simulation runtime error: " + message, 
                  ErrorCategory::Simulation, ErrorSeverity::Critical, "Simulation") {
        if (step > 0) {
            setContext("Step: " + std::to_string(step));
        }
        setRecoveryAction("Check simulation parameters and reset brain state");
    }
};

class MemoryAllocationError : public NLMError {
public:
    MemoryAllocationError(const std::string& message, size_t size = 0)
        : NLMError("Memory allocation failed: " + message, 
                  ErrorCategory::Memory, ErrorSeverity::Critical, "Memory") {
        if (size > 0) {
            setContext("Requested size: " + std::to_string(size) + " bytes");
        }
        setRecoveryAction("Increase system resources or reduce simulation scale");
    }
};

class FileIOError : public NLMError {
public:
    FileIOError(const std::string& message, const std::string& filepath = "")
        : NLMError("File I/O error: " + message, 
                  ErrorCategory::InputOutput, ErrorSeverity::Error, "IO") {
        if (!filepath.empty()) {
            setContext("File: " + filepath);
        }
        setRecoveryAction("Check file permissions and disk space");
    }
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const NLMError& error) = 0;
    
    virtual ErrorSeverity getMinimumSeverity() const { return ErrorSeverity::Error; }
    
    virtual bool shouldTerminate(const NLMError& error) {
        return error.isFatal();
    }
};

// Default error handler - logs to console
class ConsoleErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMError& error) override {
        std::string message = error.toString();
        
        switch (error.getSeverity()) {
            case ErrorSeverity::Debug:
                std::cout << "[DEBUG] " << message << std::endl;
                break;
            case ErrorSeverity::Info:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case ErrorSeverity::Warning:
                std::cerr << "[WARNING] " << message << std::endl;
                break;
            case ErrorSeverity::Error:
                std::cerr << "[ERROR] " << message << std::endl;
                break;
            case ErrorSeverity::Critical:
                std::cerr << "[CRITICAL] " << message << std::endl;
                break;
            case ErrorSeverity::Fatal:
                std::cerr << "[FATAL] " << message << std::endl;
                break;
        }
    }
    
    ErrorSeverity getMinimumSeverity() const override {
        return ErrorSeverity::Warning;
    }
};

// Error manager - handles error reporting and recovery
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void registerHandler(IErrorHandler* handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (std::find(handlers_.begin(), handlers_.end(), handler) == handlers_.end()) {
            handlers_.push_back(handler);
        }
    }
    
    void unregisterHandler(IErrorHandler* handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find(handlers_.begin(), handlers_.end(), handler);
        if (it != handlers_.end()) {
            handlers_.erase(it);
        }
    }
    
    void handleError(const NLMError& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto* handler : handlers_) {
            if (error.getSeverity() >= handler->getMinimumSeverity()) {
                handler->handleError(error);
                
                if (handler->shouldTerminate(error)) {
                    std::cerr << "Error handler requested termination." << std::endl;
                    std::exit(1);
                }
            }
        }
    }
    
    void report(const NLMError& error) {
        handleError(error);
    }
    
    // Convenience methods for different error types
    void reportConfigError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        report(makeConfigError(message, severity));
    }
    
    void reportSimulationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        report(makeSimulationError(message, severity));
    }
    
    void reportMemoryError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        report(makeMemoryError(message, severity));
    }
    
    void reportIOError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        report(makeIOError(message, severity));
    }
    
private:
    ErrorManager() {}
    ~ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
private:
    std::vector<IErrorHandler*> handlers_;
    std::mutex mutex_;
};

// Error handling macros for convenience
#define TRY_CATCH_ERROR(category, operation) \
    try { \
        operation; \
    } catch (const NLMError& e) { \
        ErrorManager::getInstance().report(e); \
        return false; \
    } catch (const std::exception& e) { \
        ErrorManager::getInstance().report(NLMError( \
            std::string("Unexpected error in " #operation ": ") + e.what(), \
            category, ErrorSeverity::Critical, #operation)); \
        return false; \
    }

#define CHECK_ERROR(condition, error) \
    if (!(condition)) { \
        ErrorManager::getInstance().report(error); \
        return false; \
    }

// Initialize default error handlers
inline void initializeErrorHandling() {
    static ConsoleErrorHandler consoleHandler;
    ErrorManager::getInstance().registerHandler(&consoleHandler);
}

#endif // NLM_ERROR_HANDLING_HPP
