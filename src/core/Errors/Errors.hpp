#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>
#include <chrono>
#include "../Logger/Logger.hpp"

namespace nlm {

// ============================================================================
// CUSTOM EXCEPTION HIERARCHY
// ============================================================================

// Base exception class for all NLM errors
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string context_;
    std::string file_;
    int line_;
    std::chrono::system_clock::time_point timestamp_;
    
public:
    explicit NLMException(const std::string& message) : message_(message), line_(0) {
        timestamp_ = std::chrono::system_clock::now();
        logError();
    }
    
    NLMException(const std::string& message, const std::string& context) 
        : message_(message), context_(context), line_(0) {
        timestamp_ = std::chrono::system_clock::now();
        logError();
    }
    
    NLMException(const std::string& message, const std::string& file, int line, const std::string& function) 
        : message_(message), file_(file), line_(line), context_(function) {
        timestamp_ = std::chrono::system_clock::now();
        logError();
    }
    
    virtual ~NLMException() = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getContext() const { return context_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    
    std::string getFullMessage() const {
        std::stringstream ss;
        ss << "[NLM Exception] " << message_;
        if (!context_.empty()) {
            ss << " (Context: " << context_ << ")";
        }
        if (!file_.empty() && line_ > 0) {
            ss << " at " << file_ << ":" << line_;
        }
        return ss.str();
    }
    
protected:
    void logError() const {
        std::stringstream ss;
        ss << "Exception: " << getFullMessage();
        Logger::getGlobal().error(ss.str(), file_, line_, context_);
    }
};

// Configuration-related exceptions
class NLMConfigurationException : public NLMException {
public:
    explicit NLMConfigurationException(const std::string& message) 
        : NLMException("Configuration Error: " + message, "Configuration") {}
    
    NLMConfigurationException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Configuration Error: " + message, file, line, function) {}
};

class NLMValueError : public NLMException {
public:
    explicit NLMValueError(const std::string& message)
        : NLMException("Value Error: " + message, "Validation") {}
    
    NLMValueError(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Value Error: " + message, file, line, function) {}
};

// State-related exceptions
class NLMStateException : public NLMException {
public:
    explicit NLMStateException(const std::string& message)
        : NLMException("State Error: " + message, "State Management") {}
    
    NLMStateException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("State Error: " + message, file, line, function) {}
};

// Memory-related exceptions
class NLMMemoryException : public NLMException {
public:
    explicit NLMMemoryException(const std::string& message)
        : NLMException("Memory Error: " + message, "Memory Management") {}
    
    NLMMemoryException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Memory Error: " + message, file, line, function) {}
};

// Resource-related exceptions
class NLMRessourceException : public NLMException {
public:
    explicit NLMRessourceException(const std::string& message)
        : NLMException("Resource Error: " + message, "Resource Management") {}
    
    NLMRessourceException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Resource Error: " + message, file, line, function) {}
};

// Operation-related exceptions
class NLMOperationException : public NLMException {
public:
    explicit NLMOperationException(const std::string& message)
        : NLMException("Operation Error: " + message, "Operation") {}
    
    NLMOperationException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Operation Error: " + message, file, line, function) {}
};

// ============================================================================
// VALIDATION UTILITIES
// ============================================================================

class ValidationUtils {
public:
    // Parameter validation macros
    #define NLM_VALIDATE_PARAM(condition, message) \
        if (!(condition)) { \
            throw NLMValueError(message, __FILE__, __LINE__, __func__); \
        }
    
    #define NLM_VALIDATE_NOT_NULL(ptr, message) \
        if ((ptr) == nullptr) { \
            throw NLMRessourceException(message, __FILE__, __LINE__, __func__); \
        }
    
    #define NLM_VALIDATE_RANGE(value, min, max, message) \
        if ((value) < (min) || (value) > (max)) { \
            std::stringstream ss; \
            ss << message << ". Got " << (value) << ", expected range [" << (min) << ", " << (max) << "]."; \
            throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__); \
        }
    
    #define NLM_VALIDATE_POSITIVE(value, message) \
        if ((value) <= 0) { \
            std::stringstream ss; \
            ss << message << ". Got " << (value) << ", must be positive"; \
            throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__); \
        }
    
    #define NLM_VALIDATE_FILE_EXISTS(filepath, message) \
        if (!fileExists((filepath))) { \
            throw NLMConfigurationException(message, __FILE__, __LINE__, __func__); \
        }
    
    #define NLM_TRY_CATCH(operation, error_message) \
        try { \
            operation; \
        } catch (const NLMException& e) { \
            throw; \
        } catch (const std::exception& e) { \
            throw NLMOperationException(error_message + ": " + std::string(e.what()), __FILE__, __LINE__, __func__); \
        }
    
    // Helper functions
    static bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    static bool directoryExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IFDIR);
    }
    
    // Type-safe validation functions
    template<typename T>
    static void validateNotNull(const T* ptr, const std::string& message = "Pointer is null") {
        if (ptr == nullptr) {
            throw NLMRessourceException(message, __FILE__, __LINE__, __func__);
        }
    }
    
    template<typename T, typename... Args>
    static void validateLessThan(const T& value, const T& limit, const std::string& message) {
        if (value >= limit) {
            std::stringstream ss;
            ss << message << ". Got " << value << ", must be < " << limit;
            throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__);
        }
    }
    
    template<typename T, typename... Args>
    static void validateGreaterThan(const T& value, const T& limit, const std::string& message) {
        if (value <= limit) {
            std::stringstream ss;
            ss << message << ". Got " << value << ", must be > " << limit;
            throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__);
        }
    }
    
    template<typename T, typename... Args>
    static void validateInRange(const T& value, const T& min, const T& max, const std::string& message) {
        if (value < min || value > max) {
            std::stringstream ss;
            ss << message << ". Got " << value << ", expected range [" << min << ", " << max << "]";
            throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__);
        }
    }
    
    // Configuration validation
    static void validateConfig(const std::string& key, const std::string& value);
    
    // State validation
    static bool isValidState(const std::string& component);
    
    // Memory validation
    static bool isMemoryAvailable(size_t bytesNeeded);
    
    // Error recovery utilities
    static void safeResourceCleanup(std::function<void()> cleanupFunc);
    static bool attemptRecovery(std::function<bool()> operation, int maxRetries = 3);
};

// ============================================================================
// ERROR REPORTING UTILITIES
// ============================================================================

class ErrorReportingUtils {
public:
    struct ErrorReport {
        std::string errorType;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::string stackTrace;
        std::map<std::string, std::string> context;
    };
    
    static void reportError(const std::string& type, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    
    static void reportWarning(const std::string& type, const std::string& message,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
    
    static std::vector<ErrorReport> getRecentErrors(size_t limit = 100);
    static void clearErrorHistory();
    
    static std::string generateErrorSummary();
    
    static void setErrorHandler(std::function<void(const ErrorReport&)> handler);
};

// ============================================================================
// ERROR RECOVERY MECHANISMS
// ============================================================================

class ErrorRecoveryUtils {
public:
    enum class RecoveryStrategy {
        NoRecovery,
        Retry,
        Fallback,
        GracefulDegradation,
        CleanupAndRetry
    };
    
    struct RecoveryContext {
        std::string operation;
        RecoveryStrategy strategy;
        int maxRetries;
        std::chrono::milliseconds retryDelay;
        std::function<bool()> validationCheck;
        std::function<void()> cleanupAction;
        std::function<void*> fallbackProvider;
    };
    
    static bool executeWithRecovery(
        std::function<bool()> operation,
        const RecoveryContext& context,
        std::function<void(const std::string&)> errorCallback = nullptr
    );
    
    static void registerErrorHandler(const std::string& component,
                                     std::function<void(const NLMException&)> handler);
    
    static void unregisterErrorHandler(const std::string& component);
    
    static void attemptGracefulShutdown(const std::string& reason);
};

// ============================================================================
// SAFE GUARDS
// ============================================================================

class SafeGuardUtils {
public:
    class Guard {
    public:
        virtual ~Guard() = default;
        virtual bool check() const = 0;
        virtual std::string getDescription() const = 0;
    };
    
    class MemoryGuard : public Guard {
        size_t requiredMemory_;
    public:
        MemoryGuard(size_t requiredMemory) : requiredMemory_(requiredMemory) {}
        bool check() const override;
        std::string getDescription() const override {
            return "Memory availability check (requires " + std::to_string(requiredMemory_) + " bytes)";
        }
    };
    
    class StateGuard : public Guard {
        std::string component_;
        std::function<bool()> validationFunc_;
    public:
        StateGuard(const std::string& component, std::function<bool()> validationFunc)
            : component_(component), validationFunc_(validationFunc) {}
        bool check() const override;
        std::string getDescription() const override {
            return "State validation for " + component_;
        }
    };
    
    class ResourceGuard : public Guard {
        std::string resourceName_;
        bool (*checkFunc_)(const std::string&);
    public:
        ResourceGuard(const std::string& resourceName, bool (*checkFunc)(const std::string&))
            : resourceName_(resourceName), checkFunc_(checkFunc) {}
        bool check() const override;
        std::string getDescription() const override {
            return "Resource availability check for " + resourceName_;
        }
    };
    
    static bool runAllGuards();
    static void addGuard(std::unique_ptr<Guard> guard);
    static void removeGuard(const std::string& description);
    static void clearAllGuards();
};

// ============================================================================
// ERROR LOGGING ENHANCEMENTS
// ============================================================================

class StructuredErrorLogger {
public:
    struct StructuredError {
        std::string severity;
        std::string category;
        std::string message;
        std::string component;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
    };
    
    static void logStructuredError(const StructuredError& error);
    static void logStructuredWarning(const StructuredError& warning);
    static std::vector<StructuredError> getRecentStructuredErrors(size_t limit = 100);
    static void clearStructuredErrorHistory();
    
    static void setStructuredErrorCallback(std::function<void(const StructuredError&)> callback);
    
    // Convenience functions for common error types
    static void logMemoryError(const std::string& message, const std::string& component = "");
    static void logValidationError(const std::string& message, const std::string& component = "");
    static void logStateError(const std::string& message, const std::string& component = "");
    static void logResourceError(const std::string& message, const std::string& component = "");
    static void logConfigurationError(const std::string& message, const std::string& component = "");
};

// ============================================================================
// EXCEPTION TRANSLATION FOR BINDINGS
// ============================================================================

// Exception translation utilities for converting C++ exceptions to Python exceptions
// (primarily for the Python bindings)

namespace pybind11 {
    template<typename T>
    void register_exception_translator();
}

namespace bindings {
    // Register Python exceptions
    void registerPythonExceptions(py::module& m);
    
    // Translate C++ NLM exceptions to Python exceptions
    py::object translateNLMException(const NLMException& e);
    
    // Global exception translator
    void setupExceptionTranslation();
    
    // Custom exception translator for specific error types
    template<typename T>
    void registerExceptionTranslator();
    
    // Helper to catch and translate exceptions
    template<typename Func, typename... Args>
    auto safeExecute(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...));
    
    // Exception translator for pybind11
    void setupPybindExceptionTranslation(py::module& m);
    
    // Exception handling decorator
    template<typename Func>
    auto exceptionSafe(Func&& func);
}

// ============================================================================
// GLOBAL ERROR HANDLING SETUP
// ============================================================================

class GlobalErrorHandler {
public:
    static void initialize();
    static void shutdown();
    
    static void enableStructuredLogging(bool enable = true);
    static void enableErrorRecovery(bool enable = true);
    static void enableGuardChecking(bool enable = true);
    
    static bool isStructuredLoggingEnabled();
    static bool isErrorRecoveryEnabled();
    static bool isGuardCheckingEnabled();
    
    static std::string getErrorSummary();
    static void logSystemStatus();
};

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

#define NLM_THROW(exc_type, message) \
    throw exc_type(message, __FILE__, __LINE__, __func__)

#define NLM_THROW_CONFIG(message) \
    NLM_THROW(NLMConfigurationException, message)

#define NLM_THROW_VALUE(message) \
    NLM_THROW(NLMValueError, message)

#define NLM_THROW_STATE(message) \
    NLM_THROW(NLMStateException, message)

#define NLM_THROW_MEMORY(message) \
    NLM_THROW(NLMMemoryException, message)

#define NLM_THROW_RESOURCE(message) \
    NLM_THROW(NLMRessourceException, message)

#define NLM_THROW_OPERATION(message) \
    NLM_THROW(NLMOperationException, message)

#define NLM_VALIDATE_AND_THROW(condition, message, exc_type) \
    if (!(condition)) { \
        throw exc_type(message, __FILE__, __LINE__, __func__); \
    }

#define NLM_VALIDATE_PARAM_NUL(ptr, message) \
    if ((ptr) == nullptr) { \
        throw NLMRessourceException(message, __FILE__, __LINE__, __func__); \
    }

#define NLM_VALIDATE_RANGE(value, min, max, message) \
    if ((value) < (min) || (value) > (max)) { \
        std::stringstream ss; \
        ss << message << ". Got " << (value) << ", expected range [" << (min) << ", " << (max) << "]."; \
        throw NLMValueError(ss.str(), __FILE__, __LINE__, __func__); \
    }

// ============================================================================
// INITIALIZATION/TERMINATION
// ============================================================================

void initializeErrorHandlingSystem();
void shutdownErrorHandlingSystem();

} // namespace nlm
