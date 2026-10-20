#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <map>
#include <mutex>

namespace nlm {

// Error categories for NLM
enum class NLMErrorCategory : int {
    Success = 0,
    // Core errors
    ConfigError = 1,
    BrainError = 2,
    MemoryError = 3,
    NeuralError = 4,
    PlasticityError = 5,
    SimulationError = 6,
    // Configuration errors
    ConfigFileNotFound = 100,
    ConfigInvalidFormat = 101,
    ConfigKeyNotFound = 102,
    ConfigInvalidValue = 103,
    ConfigValidationFailed = 104,
    // Brain errors
    BrainInitFailed = 200,
    BrainStepFailed = 201,
    BrainRegionNotFound = 202,
    BrainNeuronNotFound = 203,
    // Memory errors
    MemoryCapacityExceeded = 300,
    MemoryCorruption = 301,
    // Neural errors
    NeuronInvalidState = 400,
    SynapseInvalidState = 401,
    // Plasticity errors
    PlasticityNotEnabled = 500,
    PlasticityInvalidParameters = 501,
    // Simulation errors
    SimulationNotInitialized = 600,
    SimulationStepError = 601,
    SimulationClockError = 602,
    // System errors
    OutOfMemory = 900,
    InvalidParameter = 901,
    NotImplemented = 902,
    Unknown = 999
};

// Custom error category for NLM
class NLMErrorCategoryImpl : public std::error_category {
public:
    const char* name() const noexcept override {
        return "nlm";
    }
    
    std::string message(int ev) const override {
        switch (static_cast<NLMErrorCategory>(ev)) {
            case NLMErrorCategory::Success:
                return "Success";
            case NLMErrorCategory::ConfigError:
                return "Configuration error";
            case NLMErrorCategory::BrainError:
                return "Brain error";
            case NLMErrorCategory::MemoryError:
                return "Memory error";
            case NLMErrorCategory::NeuralError:
                return "Neural error";
            case NLMErrorCategory::PlasticityError:
                return "Plasticity error";
            case NLMErrorCategory::SimulationError:
                return "Simulation error";
            case NLMErrorCategory::ConfigFileNotFound:
                return "Configuration file not found";
            case NLMErrorCategory::ConfigInvalidFormat:
                return "Invalid configuration file format";
            case NLMErrorCategory::ConfigKeyNotFound:
                return "Configuration key not found";
            case NLMErrorCategory::ConfigInvalidValue:
                return "Configuration value is invalid";
            case NLMErrorCategory::ConfigValidationFailed:
                return "Configuration validation failed";
            case NLMErrorCategory::BrainInitFailed:
                return "Brain initialization failed";
            case NLMErrorCategory::BrainStepFailed:
                return "Brain simulation step failed";
            case NLMErrorCategory::BrainRegionNotFound:
                return "Brain region not found";
            case NLMErrorCategory::BrainNeuronNotFound:
                return "Neuron not found";
            case NLMErrorCategory::MemoryCapacityExceeded:
                return "Memory capacity exceeded";
            case NLMErrorCategory::MemoryCorruption:
                return "Memory corruption detected";
            case NLMErrorCategory::NeuronInvalidState:
                return "Neuron in invalid state";
            case NLMErrorCategory::SynapseInvalidState:
                return "Synapse in invalid state";
            case NLMErrorCategory::PlasticityNotEnabled:
                return "Plasticity not enabled";
            case NLMErrorCategory::PlasticityInvalidParameters:
                return "Plasticity invalid parameters";
            case NLMErrorCategory::SimulationNotInitialized:
                return "Simulation not initialized";
            case NLMErrorCategory::SimulationStepError:
                return "Simulation step error";
            case NLMErrorCategory::SimulationClockError:
                return "Simulation clock error";
            case NLMErrorCategory::OutOfMemory:
                return "Out of memory";
            case NLMErrorCategory::InvalidParameter:
                return "Invalid parameter";
            case NLMErrorCategory::NotImplemented:
                return "Not implemented";
            default:
                return "Unknown error";
        }
    }
};

// Global error category instance
inline const std::error_category& nlm_error_category() {
    static NLMErrorCategoryImpl category;
    return category;
}

// NLM exception class
class NLMException : public std::runtime_error {
public:
    NLMException(NLMErrorCategory error, const std::string& message)
        : std::runtime_error(message), errorCode(error), message(message) {}
    
    NLMException(NLMErrorCategory error, const std::string& message, const std::string& context)
        : std::runtime_error("[" + context + "] " + message), errorCode(error), message(message), context(context) {}
    
    NLMErrorCategory getErrorCode() const { return errorCode; }
    const std::string& getMessage() const { return message; }
    const std::string& getContext() const { return context; }
    
    std::string what() const noexcept override {
        if (context.empty()) {
            return message;
        } else {
            return "[" + context + "] " + message;
        }
    }
    
    // Create a more descriptive error
    static NLMException fromErrorCategory(NLMErrorCategory error, const std::string& context = "") {
        return NLMException(error, nlm_error_category().message(static_cast<int>(error)), context);
    }

private:
    NLMErrorCategory errorCode;
    std::string message;
    std::string context;
};

// NLM error code helper functions
inline std::error_code make_error_code(NLMErrorCategory ec) {
    return std::error_code(static_cast<int>(ec), nlm_error_category());
}

// Exception throwing macros for convenience
#define NLM_THROW_ERROR(error, context) throw nlm::NLMException(nlm::NLMErrorCategory::error, #error, context)
#define NLM_THROW_CONFIG_ERROR(error, context) throw nlm::NLMException(nlm::NLMErrorCategory::error, #error, context)
#define NLM_THROW_RUNTIME_ERROR(msg, context) throw nlm::NLMException(nlm::NLMErrorCategory::Unknown, msg, context)

// Error handling utilities
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Set error callback
    void setErrorCallback(std::function<void(const NLMException&)>) {
        std::lock_guard<std::mutex> lock(mutex);
        errorCallback = std::move(errorCallback);
    }
    
    // Log error
    void logError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (errorCallback) {
            errorCallback(error);
        }
        // Also print to stderr
        std::cerr << "NLM Error: " << error.what() << std::endl;
    }
    
    // Check if error should be thrown
    bool shouldThrow(NLMErrorCategory error) const {
        std::lock_guard<std::mutex> lock(mutex);
        return throwErrors;
    }
    
    // Set throw on error
    void setThrowErrors(bool throwOnError) {
        std::lock_guard<std::mutex> lock(mutex);
        throwErrors = throwOnError;
    }
    
    // Handle error - either log or throw
    void handleError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (throwErrors) {
            throw error;
        } else {
            if (errorCallback) {
                errorCallback(error);
            }
        }
    }
    
    // Get error summary
    std::string getErrorSummary() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::ostringstream oss;
        oss << "NLM Error Handler: " << totalErrors << " errors occurred";
        if (totalErrors > 0) {
            oss << ", last error: " << (lastError ? lastError->what() : "none");
        }
        return oss.str();
    }

private:
    ErrorHandler() : throwErrors(true), totalErrors(0) {}
    
    std::mutex mutex;
    bool throwErrors;
    std::function<void(const NLMException&)> errorCallback;
    size_t totalErrors;
    std::unique_ptr<NLMException> lastError;
    
    // Prevent copying
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
};

// Convenience function to throw if condition fails
inline void nlm_check(bool condition, NLMErrorCategory error, const std::string& message, const std::string& context = "") {
    if (!condition) {
        NLMException exc(error, message, context);
        ErrorHandler::getInstance().handleError(exc);
    }
}

// Convenience function for parameter validation
inline void nlm_validate_param(bool condition, const std::string& paramName, const std::string& expected) {
    if (!condition) {
        throw NLMException(NLMErrorCategory::InvalidParameter,
                         "Invalid parameter " + paramName + ": expected " + expected,
                         "ParameterValidation");
    }
}

// Convenience function for memory validation
inline void nlm_validate_memory(void* ptr, size_t size) {
    if (!ptr && size > 0) {
        throw NLMException(NLMErrorCategory::OutOfMemory,
                         "Failed to allocate memory",
                         "MemoryAllocation");
    }
}

} // namespace nlm