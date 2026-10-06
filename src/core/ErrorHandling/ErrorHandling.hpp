#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <iostream>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

// Exception classes
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(message), severity_(severity) {}

    ErrorSeverity getSeverity() const { return severity_; }

private:
    ErrorSeverity severity_;
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) 
        : NLMException(message, ErrorSeverity::Error) {}
};

class InitializationError : public NLMException {
public:
    InitializationError(const std::string& message)
        : NLMException(message, ErrorSeverity::Error) {}
};

class ResourceError : public NLMException {
public:
    ResourceError(const std::string& message)
        : NLMException(message, ErrorSeverity::Error) {}
};

class InvalidParameterError : public NLMException {
public:
    InvalidParameterError(const std::string& message)
        : NLMException(message, ErrorSeverity::Warning) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message)
        : NLMException(message, ErrorSeverity::Error) {}
};

// Forward declarations
class ErrorHandler;

// Logging functions
void logError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error, const std::string& source = "nlm");
void setErrorHandler(void (*handler)(const std::string& message, ErrorSeverity severity));

// Error handling utilities
class ErrorHandler {
public:
    static std::shared_ptr<ErrorHandler> getInstance() {
        static std::shared_ptr<ErrorHandler> instance(new ErrorHandler());
        return instance;
    }

    void setGlobalErrorHandler(void (*handler)(const std::string& message, ErrorSeverity severity)) {
        globalHandler_ = handler;
    }

    void handleError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                   const std::string& source = "unknown") {
        // Log error
        if (globalHandler_) {
            globalHandler_(formatMessage(message, severity, source), severity);
        } else {
            // Default logging
            logError(formatMessage(message, severity, source), severity, source);
        }

        // For errors, we can throw or handle gracefully
        if (severity == ErrorSeverity::Fatal || severity == ErrorSeverity::Error) {
            // Default behavior: throw exception
            throw NLMException(formatMessage(message, severity, source), severity);
        }
    }

    std::string formatMessage(const std::string& message, ErrorSeverity severity, const std::string& source) {
        std::stringstream ss;
        ss << "[" << source << "] " << severityToString(severity) << ": " << message;
        return ss.str();
    }

    const char* severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    // Safe operations with error checking
    template<typename T, typename Func>
    T safeOperation(const std::string& operationName, Func func, ErrorSeverity severity = ErrorSeverity::Error) {
        try {
            T result = func();
            handleError("Operation successful: " + operationName, ErrorSeverity::Info);
            return result;
        } catch (const NLMException& e) {
            handleError("Operation failed in " + operationName + ": " + e.what(), severity);
            throw;
        } catch (const std::exception& e) {
            handleError("Unexpected error in " + operationName + ": " + std::string(e.what()), severity);
            throw NLMException("Unexpected error in " + operationName + ": " + std::string(e.what()), severity);
        }
    }

    template<typename T>
    T safeAccess(const T& value, const std::string& operationName, ErrorSeverity severity = ErrorSeverity::Warning) {
        if (!value) {
            handleError("Null value accessed in " + operationName, severity);
            return T(); // Return default value
        }
        return value;
    }

private:
    ErrorHandler() = default;
    void (*globalHandler_)(const std::string& message, ErrorSeverity severity) = nullptr;
};

// Macro for error handling
#define NLM_CHECK(condition, message) \
    if (!(condition)) { \
        ErrorHandler::getInstance()->handleError(message, ErrorSeverity::Error, #condition); \
    }

#define NLM_THROW_ERROR(message, severity) \
    throw NLMException(message, severity)

#define NLM_VALIDATE_PARAM(param, condition, message) \
    if (!(condition)) { \
        throw InvalidParameterError(message); \
    }

// Safe resource management template
class SafeResource {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> create(const std::string& name, Args&&... args) {
        try {
            auto resource = std::make_shared<T>(std::forward<Args>(args)...);
            if (!resource) {
                ErrorHandler::getInstance()->handleError("Failed to create resource: " + name, ErrorSeverity::Error);
            }
            return resource;
        } catch (...) {
            ErrorHandler::getInstance()->handleError("Exception creating resource: " + name, ErrorSeverity::Error);
            throw;
        }
    }

    template<typename T>
    static bool isValid(const std::shared_ptr<T>& resource) {
        return resource != nullptr;
    }
};

// Logging function implementations
inline void logError(const std::string& message, ErrorSeverity severity, const std::string& source) {
    std::string severityStr;
    switch (severity) {
        case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
        case ErrorSeverity::Info: severityStr = "INFO"; break;
        case ErrorSeverity::Warning: severityStr = "WARNING"; break;
        case ErrorSeverity::Error: severityStr = "ERROR"; break;
        case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        default: severityStr = "UNKNOWN"; break;
    }
    
    std::cerr << "[" << source << "] " << severityStr << ": " << message << std::endl;
}

inline void setErrorHandler(void (*handler)(const std::string& message, ErrorSeverity severity)) {
    ErrorHandler::getInstance()->setGlobalErrorHandler(handler);
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
