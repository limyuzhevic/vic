#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "Logger.hpp"

namespace nlm {

/**
 * @enum ErrorSeverity
 * @brief Severity levels for errors and warnings
 */
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

/**
 * @enum ErrorCategory
 * @brief Categories for different types of errors
 */
enum class ErrorCategory {
    None = 0,
    Config = 1,
    FileIO = 2,
    Memory = 3,
    NeuralNetwork = 4,
    PythonBindings = 5,
    Plasticity = 6,
    MemoryManagement = 7,
    System = 8
};

/**
 * @class ErrorContext
 * @brief Context information for errors including file, line, and function
 */
class ErrorContext {
public:
    std::string file;
    int line = 0;
    std::string function;
    std::string message;
    std::string additionalInfo;
    
    ErrorContext(const std::string& file, int line, const std::string& function,
                 const std::string& message, const std::string& additionalInfo = "")
        : file(file), line(line), function(function), message(message),
          additionalInfo(additionalInfo) {}
    
    std::string getFullContext() const {
        std::string result = message;
        if (!additionalInfo.empty()) {
            result += " - " + additionalInfo;
        }
        if (line > 0) {
            result += " [" + file + ":" + std::to_string(line) + "]";
        }
        return result;
    }
};

/**
 * @class BaseError
 * @brief Base class for all NLM exceptions
 */
class BaseError : public std::exception {
protected:
    ErrorSeverity severity;
    ErrorCategory category;
    ErrorContext context;
    std::string errorId;
    mutable std::string whatBuffer;
    
public:
    BaseError(ErrorSeverity severity, ErrorCategory category, 
              const ErrorContext& ctx, const std::string& errorId = "")
        : severity(severity), category(category), context(ctx), errorId(errorId) {}
    
    virtual ~BaseError() noexcept = default;
    
    ErrorSeverity getSeverity() const noexcept { return severity; }
    ErrorCategory getCategory() const noexcept { return category; }
    const ErrorContext& getContext() const noexcept { return context; }
    const std::string& getErrorId() const noexcept { return errorId; }
    
    const char* what() const noexcept override {
        whatBuffer = "[" + errorId + "] " + context.getFullContext();
        return whatBuffer.c_str();
    }
    
    virtual void log() const {
        std::string levelStr;
        switch (severity) {
            case ErrorSeverity::Debug: levelStr = "DEBUG"; break;
            case ErrorSeverity::Info: levelStr = "INFO"; break;
            case ErrorSeverity::Warning: levelStr = "WARNING"; break;
            case ErrorSeverity::Error: levelStr = "ERROR"; break;
            case ErrorSeverity::Critical: levelStr = "CRITICAL"; break;
            case ErrorSeverity::Fatal: levelStr = "FATAL"; break;
        }
        
        Logger::getGlobal().log(severity, "[" + levelStr + "] " + std::string(what()),
                               context.file, context.line, context.function);
    }
};

// Specific error types for different categories

/** Config-related errors */
class ConfigError : public BaseError {
public:
    ConfigError(ErrorSeverity severity, const std::string& file, int line,
                const std::string& function, const std::string& message,
                const std::string& key = "", const std::string& additionalInfo = "")
        : BaseError(severity, ErrorCategory::Config,
                    ErrorContext(file, line, function, message, additionalInfo),
                    "CONFIG_ERROR") {}
    
    ConfigError& withKey(const std::string& key) {
        // Add key to additional info
        context.additionalInfo = "Key: " + key + ". " + context.additionalInfo;
        return *this;
    }
};

class ConfigFileError : public ConfigError {
public:
    ConfigFileError(const std::string& file, int line, const std::string& function,
                    const std::string& message, const std::string& filepath,
                    const std::string& key = "")
        : ConfigError(ErrorSeverity::Error, file, line, function,
                      "Configuration file error: " + message, key,
                      "File: " + filepath) {}
};

class ConfigParseError : public ConfigError {
public:
    ConfigParseError(const std::string& file, int line, const std::string& function,
                     const std::string& message, const std::string& format,
                     const std::string& key = "")
        : ConfigError(ErrorSeverity::Error, file, line, function,
                      "Configuration parse error: " + message, key,
                      "Format: " + format) {}
};

class ConfigTypeError : public ConfigError {
public:
    ConfigTypeError(const std::string& file, int line, const std::string& function,
                    const std::string& message, const std::string& key,
                    const std::string& expectedType, const std::string& actualType)
        : ConfigError(ErrorSeverity::Error, file, line, function,
                      "Type conversion error: " + message, key,
                      "Expected: " + expectedType + ", Got: " + actualType) {}
};

/** File I/O errors */
class IOError : public BaseError {
public:
    IOError(const std::string& file, int line, const std::string& function,
            const std::string& message, const std::string& filepath,
            const std::string& operation = "")
        : BaseError(ErrorSeverity::Error, ErrorCategory::FileIO,
                    ErrorContext(file, line, function, message),
                    "IO_ERROR") {
        context.additionalInfo = "File: " + filepath + ", Operation: " + operation;
    }
};

class FileNotFoundError : public IOError {
public:
    FileNotFoundError(const std::string& file, int line, const std::string& function,
                      const std::string& filepath, const std::string& operation = "")
        : IOError(file, line, function, "File not found", filepath, operation) {}
};

class PermissionError : public IOError {
public:
    PermissionError(const std::string& file, int line, const std::string& function,
                    const std::string& message, const std::string& filepath,
                    const std::string& operation = "")
        : IOError(file, line, function, "Permission denied: " + message,
                  filepath, operation) {}
};

/** Memory-related errors */
class MemoryError : public BaseError {
public:
    MemoryError(const std::string& file, int line, const std::string& function,
                const std::string& message, size_t size = 0,
                const std::string& context = "")
        : BaseError(ErrorSeverity::Critical, ErrorCategory::Memory,
                    ErrorContext(file, line, function, message),
                    "MEMORY_ERROR") {
        if (size > 0) {
            context += " - Size: " + std::to_string(size) + " bytes";
        }
        context.additionalInfo = context;
    }
};

/** Neural network errors */
class NeuralNetworkError : public BaseError {
public:
    NeuralNetworkError(const std::string& file, int line, const std::string& function,
                       const std::string& message, const std::string& neuronId = "",
                       const std::string& operation = "")
        : BaseError(ErrorSeverity::Error, ErrorCategory::NeuralNetwork,
                    ErrorContext(file, line, function, message),
                    "NN_ERROR") {
        if (!neuronId.empty()) {
            context.additionalInfo = "Neuron: " + neuronId + ", Operation: " + operation;
        }
    }
};

class NetworkInitializationError : public NeuralNetworkError {
public:
    NetworkInitializationError(const std::string& file, int line, const std::string& function,
                               const std::string& message, const std::string& reason = "")
        : NeuralNetworkError(file, line, function,
                             "Network initialization failed: " + message,
                             "", reason) {}
};

/** Plasticity errors */
class PlasticityError : public BaseError {
public:
    PlasticityError(const std::string& file, int line, const std::string& function,
                    const std::string& message, const std::string& rule = "",
                    const std::string& synapseId = "")
        : BaseError(ErrorSeverity::Warning, ErrorCategory::Plasticity,
                    ErrorContext(file, line, function, message),
                    "PLASTICITY_ERROR") {
        std::string info = "Rule: " + rule;
        if (!synapseId.empty()) {
            info += ", Synapse: " + synapseId;
        }
        context.additionalInfo = info;
    }
};

/** Python binding errors */
class PythonBindingError : public BaseError {
public:
    PythonBindingError(const std::string& file, int line, const std::string& function,
                       const std::string& message, const std::string& pyType = "",
                       const std::string& pyValue = "")
        : BaseError(ErrorSeverity::Error, ErrorCategory::PythonBindings,
                    ErrorContext(file, line, function, message),
                    "PYBINDING_ERROR") {
        std::string info = "PyType: " + pyType;
        if (!pyValue.empty()) {
            info += ", PyValue: " + pyValue;
        }
        context.additionalInfo = info;
    }
};

/**
 * @class ErrorManager
 * @brief Manages error reporting and recovery
 */
class ErrorManager {
private:
    static ErrorManager* instance;
    bool enabled = true;
    bool strictMode = false;
    int maxErrorsBeforeExit = 100;
    int errorCount = 0;
    
    ErrorManager() = default;
    ~ErrorManager() = default;
    
public:
    static ErrorManager& getInstance() {
        if (!instance) {
            instance = new ErrorManager();
        }
        return *instance;
    }
    
    void enable(bool enable) { this->enabled = enable; }
    bool isEnabled() const { return enabled; }
    
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
    void setMaxErrorsBeforeExit(int max) { maxErrorsBeforeExit = max; }
    int getMaxErrorsBeforeExit() const { return maxErrorsBeforeExit; }
    
    void incrementErrorCount() { errorCount++; }
    int getErrorCount() const { return errorCount; }
    
    bool shouldExit() const { return errorCount >= maxErrorsBeforeExit; }
    
    template<typename T>
    T handleError(const T& error) {
        if (!enabled) return error;
        
        error.log();
        incrementErrorCount();
        
        if (strictMode) {
            throw error;
        }
        
        return error;
    }
    
    // Helper methods for common error handling patterns
    template<typename T>
    void checkAndThrow(const T& condition, const T& error) {
        if (!condition) {
            handleError(error);
        }
    }
    
    // Create common error types with context
    ConfigFileError configFileError(const std::string& file, int line,
                                    const std::string& function,
                                    const std::string& message,
                                    const std::string& filepath,
                                    const std::string& key = "") {
        return ConfigFileError(file, line, function, message, filepath, key);
    }
    
    IOError ioError(const std::string& file, int line,
                    const std::string& function,
                    const std::string& message,
                    const std::string& filepath,
                    const std::string& operation = "") {
        return IOError(file, line, function, message, filepath, operation);
    }
    
    MemoryError memoryError(const std::string& file, int line,
                            const std::string& function,
                            const std::string& message,
                            size_t size = 0,
                            const std::string& context = "") {
        return MemoryError(file, line, function, message, size, context);
    }
    
    NeuralNetworkError nnError(const std::string& file, int line,
                               const std::string& function,
                               const std::string& message,
                               const std::string& neuronId = "",
                               const std::string& operation = "") {
        return NeuralNetworkError(file, line, function, message, neuronId, operation);
    }
    
    PlasticityError plasticityError(const std::string& file, int line,
                                    const std::string& function,
                                    const std::string& message,
                                    const std::string& rule = "",
                                    const std::string& synapseId = "") {
        return PlasticityError(file, line, function, message, rule, synapseId);
    }
    
    PythonBindingError pyError(const std::string& file, int line,
                              const std::string& function,
                              const std::string& message,
                              const std::string& pyType = "",
                              const std::string& pyValue = "") {
        return PythonBindingError(file, line, function, message, pyType, pyValue);
    }
};

// Global error manager instance
inline ErrorManager& getErrorManager() {
    return ErrorManager::getInstance();
}

// Convenience macros for error handling
#define TRY_PERFORM(expr, errorHandler) try { expr; } catch (const std::exception& e) { errorHandler(e); }

#define CHECK_CONDITION(cond, error) if (!(cond)) { getErrorManager().handleError(error); }

#define CHECK_NULL(ptr, error) if ((ptr) == nullptr) { getErrorManager().handleError(error); }

#define LOG_AND_THROW(error) do { auto& em = getErrorManager(); em.handleError(error); throw error; } while(0)

} // namespace nlm
