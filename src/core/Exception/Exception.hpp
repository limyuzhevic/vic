#pragma once

#include <string>
#include <stdexcept>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        // Configuration errors
        CONFIG_FILE_NOT_FOUND = 1,
        CONFIG_INVALID_FORMAT = 2,
        CONFIG_KEY_NOT_FOUND = 3,
        CONFIG_INVALID_VALUE = 4,
        CONFIG_PARSER_ERROR = 5,
        
        // Neural computation errors
        NEURON_INVALID_STATE = 10,
        NEURON_THRESHOLD_EXCEEDED = 11,
        NEURON_REFractory_VIOLATION = 12,
        SYNAPSE_INVALID_WEIGHT = 13,
        DYNAMICS_COMPUTATION_ERROR = 14,
        
        // Plasticity errors
        PLASTICITY_RULE_INVALID = 20,
        PLASTICITY_STDP_TIMING = 21,
        PLASTICITY_HOMEOSTATIC_FAILURE = 22,
        
        // Neuromodulation errors
        NEUROMODULATOR_INVALID_LEVEL = 30,
        NEUROMODULATOR_DYNAMICS_ERROR = 31,
        
        // Memory errors
        MEMORY_ALLOCATION_FAILED = 40,
        MEMORY_CORRUPTION = 41,
        MEMORY_FULL = 42,
        
        // I/O errors
        FILE_READ_ERROR = 50,
        FILE_WRITE_ERROR = 51,
        FILE_NOT_FOUND = 52,
        PERMISSION_DENIED = 53,
        
        // Network/runtime errors
        THREAD_LOCK_FAILED = 60,
        THREAD_JOIN_FAILED = 61,
        OUT_OF_MEMORY = 62,
        TIMEOUT = 63,
        
        // Python binding errors
        PYTHON_BINDING_ERROR = 70,
        PYTHON_TYPE_MISMATCH = 71,
        PYTHON_METHOD_NOT_FOUND = 72,
        
        // General errors
        UNKNOWN_ERROR = 99,
        NOT_IMPLEMENTED = 100
    };
    
protected:
    std::string component_;
    ErrorCode code_;
    std::chrono::system_clock::time_point timestamp_;
    std::unordered_map<std::string, std::string> context_;
    
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR,
                 const std::string& component = "")
        : std::runtime_error(formatMessage(message, code)),
          component_(component),
          code_(code),
          timestamp_(std::chrono::system_clock::now()) {}
    
    NLMException(const std::string& message, const std::string& component)
        : NLMException(message, ErrorCode::UNKNOWN_ERROR, component) {}
    
    virtual ~NLMException() = default;
    
    const std::string& getComponent() const { return component_; }
    ErrorCode getCode() const { return code_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    const std::string& getContext(const std::string& key) const {
        static const std::string empty = "";
        auto it = context_.find(key);
        return (it != context_.end()) ? it->second : empty;
    }
    
    void setContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getAllContext() const {
        return context_;
    }
    
    virtual std::string getErrorTypeName() const {
        return "NLMException";
    }
    
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "[" << getErrorTypeName() << "] " << what()
            << " (Code: " << static_cast<int>(code_) << ")"
            << " Component: " << component_;
        return oss.str();
    }
    
protected:
    static std::string formatMessage(const std::string& message, ErrorCode code) {
        std::ostringstream oss;
        oss << message << " (Error Code: " << static_cast<int>(code) << ")";
        return oss.str();
    }
};

// Specific exception classes for different error categories
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, ErrorCode code = ErrorCode::CONFIG_INVALID_FORMAT,
                   const std::string& component = "Config")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "ConfigException";
    }
};

class NeuronException : public NLMException {
public:
    NeuronException(const std::string& message, ErrorCode code = ErrorCode::NEURON_INVALID_STATE,
                   const std::string& component = "Neuron")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "NeuronException";
    }
};

class SynapseException : public NLMException {
public:
    SynapseException(const std::string& message, ErrorCode code = ErrorCode::SYNAPSE_INVALID_WEIGHT,
                    const std::string& component = "Synapse")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "SynapseException";
    }
};

class NeuralDynamicsException : public NLMException {
public:
    NeuralDynamicsException(const std::string& message,
                           ErrorCode code = ErrorCode::DYNAMICS_COMPUTATION_ERROR,
                           const std::string& component = "NeuralDynamics")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "NeuralDynamicsException";
    }
};

class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& message,
                       ErrorCode code = ErrorCode::PLASTICITY_RULE_INVALID,
                       const std::string& component = "Plasticity")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "PlasticityException";
    }
};

class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message,
                             ErrorCode code = ErrorCode::NEUROMODULATOR_INVALID_LEVEL,
                             const std::string& component = "Neuromodulation")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "NeuromodulationException";
    }
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message,
                   ErrorCode code = ErrorCode::MEMORY_ALLOCATION_FAILED,
                   const std::string& component = "Memory")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "MemoryException";
    }
};

class IOError : public NLMException {
public:
    IOError(const std::string& message,
           ErrorCode code = ErrorCode::FILE_READ_ERROR,
           const std::string& component = "IO")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "IOError";
    }
};

class PythonBindingException : public NLMException {
public:
    PythonBindingException(const std::string& message,
                          ErrorCode code = ErrorCode::PYTHON_BINDING_ERROR,
                          const std::string& component = "PythonBindings")
        : NLMException(message, code, component) {}
    
    std::string getErrorTypeName() const override {
        return "PythonBindingException";
    }
};

// Exception handling utilities
class ExceptionHandler {
public:
    enum class Action {
        THROW,      // Re-throw the exception
        LOG_AND_THROW, // Log and re-throw
        LOG_AND_CONTINUE, // Log but continue (if possible)
        IGNORE,     // Ignore the exception
        RETRY,      // Retry the operation
        FALLBACK    // Use fallback value/behavior
    };
    
    struct HandlerInfo {
        std::vector<std::string> components;
        std::vector<ErrorCode> errorCodes;
        Action action;
        std::string fallbackMessage;
        int maxRetries;
        double retryDelay;
        bool enableLogging;
    };
    
private:
    static std::vector<HandlerInfo> handlers_;
    static std::mutex mutex_;
    
public:
    static void initialize() {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.clear();
        
        // Default handlers
        registerHandler({
            components: {"Config", "NeuralDynamics", "Plasticity", "Neuromodulation"},
            errorCodes: {ErrorCode::CONFIG_INVALID_FORMAT, ErrorCode::CONFIG_KEY_NOT_FOUND,
                        ErrorCode::NEURON_INVALID_STATE, ErrorCode::SYNAPSE_INVALID_WEIGHT,
                        ErrorCode::PLASTICITY_RULE_INVALID, ErrorCode::NEUROMODULATOR_INVALID_LEVEL},
            action: Action::LOG_AND_THROW,
            fallbackMessage: "Operation failed but error was handled",
            maxRetries: 3,
            retryDelay: 1.0,
            enableLogging: true
        });
        
        registerHandler({
            components: {"Memory"},
            errorCodes: {ErrorCode::MEMORY_ALLOCATION_FAILED, ErrorCode::MEMORY_CORRUPTION,
                        ErrorCode::OUT_OF_MEMORY},
            action: Action::LOG_AND_THROW,
            fallbackMessage: "Using reduced memory allocation",
            maxRetries: 5,
            retryDelay: 0.1,
            enableLogging: true
        });
        
        registerHandler({
            components: {"IO"},
            errorCodes: {ErrorCode::FILE_READ_ERROR, ErrorCode::FILE_WRITE_ERROR,
                        ErrorCode::FILE_NOT_FOUND, ErrorCode::PERMISSION_DENIED},
            action: Action::LOG_AND_THROW,
            fallbackMessage: "Using default configuration",
            maxRetries: 2,
            retryDelay: 2.0,
            enableLogging: true
        });
        
        registerHandler({
            components: {"PythonBindings"},
            errorCodes: {ErrorCode::PYTHON_BINDING_ERROR, ErrorCode::PYTHON_TYPE_MISMATCH},
            action: Action::LOG_AND_THROW,
            fallbackMessage: "Python binding operation failed",
            maxRetries: 1,
            retryDelay: 0.5,
            enableLogging: true
        });
    }
    
    static void registerHandler(const HandlerInfo& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(handler);
    }
    
    static bool handleException(const NLMException& ex, const std::string& operation = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find matching handler
        for (const auto& handler : handlers_) {
            bool componentMatch = std::find(handler.components.begin(), handler.components.end(), ex.getComponent()) != handler.components.end();
            bool codeMatch = std::find(handler.errorCodes.begin(), handler.errorCodes.end(), ex.getCode()) != handler.errorCodes.end();
            
            if (componentMatch && codeMatch) {
                return executeHandler(ex, handler, operation);
            }
        }
        
        // No matching handler found
        return false;
    }
    
private:
    static bool executeHandler(const NLMException& ex, const HandlerInfo& handler, const std::string& operation) {
        switch (handler.action) {
            case Action::THROW:
                return false; // Let the exception propagate
            
            case Action::LOG_AND_THROW:
                if (handler.enableLogging) {
                    logException(ex, operation);
                }
                return false;
            
            case Action::LOG_AND_CONTINUE:
                if (handler.enableLogging) {
                    logException(ex, operation);
                }
                return true;
            
            case Action::IGNORE:
                return true;
            
            case Action::RETRY:
                return executeWithRetry(ex, handler, operation);
            
            case Action::FALLBACK:
                if (handler.enableLogging) {
                    logException(ex, operation);
                }
                return true;
            
            default:
                return false;
        }
    }
    
    static bool executeWithRetry(const NLMException& ex, const HandlerInfo& handler, const std::string& operation) {
        for (int attempt = 0; attempt < handler.maxRetries; ++attempt) {
            try {
                // In a real implementation, we would retry the operation here
                // For now, we'll just log the retry attempt
                if (handler.enableLogging) {
                    std::cout << "Retrying operation after failure (attempt " 
                              << (attempt + 1) << "/" << handler.maxRetries << "): "
                              << ex.what() << std::endl;
                }
                
                if (attempt < handler.maxRetries - 1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(
                        static_cast<int>(handler.retryDelay * 1000)));
                }
                
                return false; // Continue waiting for retry
            } catch (const std::exception& e) {
                if (attempt == handler.maxRetries - 1) {
                    if (handler.enableLogging) {
                        std::cout << "Retry failed after " << handler.maxRetries 
                                  << " attempts: " << e.what() << std::endl;
                    }
                    return false; // Let the exception propagate
                }
            }
        }
        
        return false;
    }
    
    static void logException(const NLMException& ex, const std::string& operation) {
        std::ostringstream oss;
        oss << "Exception in " << ex.getComponent() << "::" << operation << ": "
            << ex.what()
            << " (Code: " << static_cast<int>(ex.getCode()) << ")";
        
        if (!ex.getContext("file").empty()) {
            oss << " at " << ex.getContext("file") << ":" << ex.getContext("line");
        }
        
        std::cout << oss.str() << std::endl;
        
        // Log context if available
        const auto& context = ex.getAllContext();
        if (!context.empty()) {
            oss.str("");
            oss << "  Context: ";
            bool first = true;
            for (const auto& pair : context) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            std::cout << oss.str() << std::endl;
        }
    }
};

// Convenience exception throwing functions
#define THROW_NLM_EX(component, error_code, message) throw nlm::component##Exception(message, nlm::component##Exception::ErrorCode::error_code, #component)

#define THROW_CONFIG_EX(error_code, message) THROW_NLM_EX(Config, error_code, message)
#define THROW_NEURON_EX(error_code, message) THROW_NLM_EX(Neuron, error_code, message)
#define THROW_SYNAPSE_EX(error_code, message) THROW_NLM_EX(Synapse, error_code, message)
#define THROW_DYNAMICS_EX(error_code, message) THROW_NLM_EX(NeuralDynamics, error_code, message)
#define THROW_PLASTICITY_EX(error_code, message) THROW_NLM_EX(Plasticity, error_code, message)
#define THROW_NEUMODULATION_EX(error_code, message) THROW_NLM_EX(Neuromodulation, error_code, message)
#define THROW_MEMORY_EX(error_code, message) THROW_NLM_EX(Memory, error_code, message)
#define THROW_IO_EX(error_code, message) THROW_NLM_EX(IO, error_code, message)
#define THROW_PYTHON_EX(error_code, message) THROW_NLM_EX(PythonBindings, error_code, message)

// Exception checking macros
#define CATCH_NLM_EX(component, error_code, var_name) \
    catch (const nlm::component##Exception& var_name) { \
        if (nlm::ExceptionHandler::handleException(var_name, #component)) { \
            continue; // Retry if handler returned true \
        } else { \
            throw; // Re-throw if no handler found \
        } \
    }

#define TRY_NLM_EX(component, error_code) \
    try { \

// Safe execution wrapper
template<typename F>
auto safeExecute(F&& func, const std::string& operation = "") -> decltype(func()) {
    try {
        return func();
    } catch (const NLMException& ex) {
        if (!ExceptionHandler::handleException(ex, operation)) {
            throw; // Re-throw if no handler found
        }
        // If handler returned true, return default value
        return decltype(func())(); // This will compile but might not be ideal
    } catch (const std::exception& ex) {
        // Convert standard exceptions to NLM exceptions
        THROW_NLM_EX(Generic, UNKNOWN_ERROR, std::string("Standard exception: ") + ex.what());
    }
}

// Error context management
class ErrorContext {
public:
    ErrorContext(const std::string& component, const std::string& operation = "")
        : component_(component), operation_(operation) {
        pushContext();
    }
    
    ~ErrorContext() {
        popContext();
    }
    
    void setValue(const std::string& key, const std::string& value) {
        currentContext_[key] = value;
    }
    
    const std::string& getValue(const std::string& key) const {
        static const std::string empty = "";
        auto it = currentContext_.find(key);
        return (it != currentContext_.end()) ? it->second : empty;
    }
    
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        currentContext_[key] = std::to_string(value);
    }
    
    void addStackTrace(const std::string& trace) {
        stackTrace_ += "\n  " + trace;
    }
    
    std::string getStackTrace() const {
        return stackTrace_;
    }
    
    std::string getFullContext() const {
        std::ostringstream oss;
        oss << "Component: " << component_ << "\n"
            << "Operation: " << operation_ << "\n"
            << "Values: ";
        bool first = true;
        for (const auto& pair : currentContext_) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        if (!stackTrace_.empty()) {
            oss << "\nStack Trace:" << stackTrace_;
        }
        return oss.str();
    }
    
private:
    void pushContext() {
        contextStack_.push_back(&currentContext_);
    }
    
    void popContext() {
        if (!contextStack_.empty()) {
            contextStack_.pop_back();
        }
    }
    
    std::unordered_map<std::string, std::string>& getCurrentContext() {
        return contextStack_.empty() ? currentContext_ : *contextStack_.back();
    }
    
    std::vector<std::unordered_map<std::string, std::string>*> contextStack_;
    std::unordered_map<std::string, std::string> currentContext_;
    std::string component_;
    std::string operation_;
    std::string stackTrace_;
};

// Macro for context-aware exception throwing
#define CONTEXT_THROW(component, error_code, message) \
    do { \
        nlm::ErrorContext ctx(#component, #error_code); \
        ctx.setValue("operation", __func__); \
        ctx.setValue("file", __FILE__); \
        ctx.setValue("line", std::to_string(__LINE__)); \
        THROW_NLM_EX(component, error_code, message " [" + ctx.getFullContext() + "]") \
    } while (0)

// Exception validation and recovery
class ExceptionValidator {
public:
    enum class ValidationResult {
        VALID, INVALID, PARTIAL, UNKNOWN
    };
    
    struct ValidationInfo {
        ValidationResult result;
        std::string message;
        std::vector<std::string> recommendations;
    };
    
    static ValidationInfo validateException(const NLMException& ex) {
        ValidationInfo info;
        info.result = ValidationResult::UNKNOWN;
        info.message = "Validation completed";
        
        // Check for recoverable errors
        switch (ex.getCode()) {
            case NLMException::ErrorCode::CONFIG_INVALID_FORMAT:
                info.result = ValidationResult::INVALID;
                info.recommendations.push_back("Check configuration file syntax");
                info.recommendations.push_back("Use JSON or YAML format if specified");
                break;
            
            case NLMException::ErrorCode::FILE_NOT_FOUND:
                info.result = ValidationResult::PARTIAL;
                info.recommendations.push_back("Use default configuration");
                info.recommendations.push_back("Create missing file or specify alternate path");
                break;
            
            case NLMException::ErrorCode::NEURON_INVALID_STATE:
                info.result = ValidationResult::INVALID;
                info.recommendations.push_back("Check neuron initialization parameters");
                info.recommendations.push_back("Reset neuron to valid state");
                break;
            
            case NLMException::ErrorCode::MEMORY_ALLOCATION_FAILED:
                info.result = ValidationResult::INVALID;
                info.recommendations.push_back("Reduce memory allocation");
                info.recommendations.push_back("Use memory pools or arenas");
                break;
            
            default:
                info.result = ValidationResult::UNKNOWN;
                break;
        }
        
        return info;
    }
};

// Global exception handling initialization
namespace {
    class ExceptionHandlerInitializer {
    public:
        ExceptionHandlerInitializer() {
            ExceptionHandler::initialize();
        }
    };
    
    ExceptionHandlerInitializer exceptionHandlerInitializer_;
}

} // namespace nlm