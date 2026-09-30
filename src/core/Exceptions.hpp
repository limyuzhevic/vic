#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <typeinfo>

namespace nlm {

// Base exception class with enhanced features
class NLMException : public std::exception {
public:
    enum class Severity {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

protected:
    std::string message_;
    std::string context_;
    Severity severity_;
    int errorCode_;
    std::vector<std::string> stackTrace_;
    mutable std::string formattedMessage_;

public:
    explicit NLMException(const std::string& message, 
                         Severity severity = Severity::ERROR,
                         int errorCode = 0,
                         const std::string& context = "")
        : message_(message), context_(context), 
          severity_(severity), errorCode_(errorCode) {
        captureStackTrace();
    }

    virtual ~NLMException() noexcept = default;

    // Exception interface
    virtual const char* what() const noexcept override {
        if (formattedMessage_.empty()) {
            std::ostringstream oss;
            oss << "NLM Exception: " << message_;
            if (!context_.empty()) oss << " (Context: " << context_ << ")";
            if (errorCode_ != 0) oss << " [Error Code: " << errorCode_ << "]";
            formattedMessage_ = oss.str();
        }
        return formattedMessage_.c_str();
    }

    // Exception severity
    Severity severity() const noexcept { return severity_; }

    // Error code for programmatic handling
    int errorCode() const noexcept { return errorCode_; }

    // Context information
    const std::string& context() const noexcept { return context_; }

    // Stack trace for debugging
    const std::vector<std::string>& stackTrace() const noexcept { return stackTrace_; }

    // Add additional context
    void addContext(const std::string& context) { context_ += " | " + context; }

protected:
    void captureStackTrace() {
        // Simple stack trace capture - can be enhanced with platform-specific calls
        stackTrace_.push_back(message_);
        if (!context_.empty()) {
            stackTrace_.push_back("Context: " + context_);
        }
    }
};

// Configuration errors
class NLMConfigException : public NLMException {
public:
    enum class ConfigError {
        INVALID_FILE_FORMAT,
        FILE_NOT_FOUND,
        INVALID_KEY,
        TYPE_MISMATCH,
        VALIDATION_FAILED,
        MEMORY_ALLOCATION_FAILED
    };

    explicit NLMConfigException(const std::string& message,
                               ConfigError errorType = ConfigError::VALIDATION_FAILED,
                               const std::string& context = "")
        : NLMException(message, Severity::ERROR, 
                      static_cast<int>(errorType), context) {}

    ConfigError errorType() const noexcept {
        return static_cast<ConfigError>(errorCode_);
    }
};

// Brain initialization/simulation errors
class NLMBrainException : public NLMException {
public:
    enum class BrainError {
        INITIALIZATION_FAILED,
        NEURON_ALLOCATION_FAILED,
        SYNAPSE_ALLOCATION_FAILED,
        REGION_ALLOCATION_FAILED,
        STATE_VALIDATION_FAILED,
        MEMORY_CORRUPTION,
        SIMULATION_STEP_FAILED
    };

    explicit NLMBrainException(const std::string& message,
                              BrainError errorType = BrainError::INITIALIZATION_FAILED,
                              const std::string& context = "")
        : NLMException(message, Severity::CRITICAL,
                      static_cast<int>(errorType), context) {}

    BrainError errorType() const noexcept {
        return static_cast<BrainError>(errorCode_);
    }
};

// World simulation errors
class NLMWorldException : public NLMException {
public:
    enum class WorldError {
        CONFIGURATION_FAILED,
        INVALID_POSITION,
        COLLISION_DETECTION_FAILED,
        OBJECT_NOT_FOUND,
        SIMULATION_STEP_FAILED,
        INVALID_TIMESTEP
    };

    explicit NLMWorldException(const std::string& message,
                              WorldError errorType = WorldError::CONFIGURATION_FAILED,
                              const std::string& context = "")
        : NLMException(message, Severity::ERROR,
                      static_cast<int>(errorType), context) {}

    WorldError errorType() const noexcept {
        return static_cast<WorldError>(errorCode_);
    }
};

// Memory management errors
class NLMMemoryException : public NLMException {
public:
    enum class MemoryError {
        ALLOCATION_FAILED,
        DEALLOCATION_FAILED,
        INVALID_POINTER,
        MEMORY_LEAK,
        BUFFER_OVERFLOW,
        OUT_OF_MEMORY
    };

    explicit NLMMemoryException(const std::string& message,
                               MemoryError errorType = MemoryError::ALLOCATION_FAILED,
                               const std::string& context = "")
        : NLMException(message, Severity::CRITICAL,
                      static_cast<int>(errorType), context) {}

    MemoryError errorType() const noexcept {
        return static_cast<MemoryError>(errorCode_);
    }
};

// Plasticity computation errors
class NLMPlasticityException : public NLMException {
public:
    enum class PlasticityError {
        STDP_COMPUTATION_FAILED,
        HEBBIAN_FAILED,
        STRUCTURAL_FAILED,
        WEIGHT_UPDATE_FAILED,
        INVALID_CONNECTIVITY,
        PLASTICITY_CONSTRAINT_VIOLATION
    };

    explicit NLMPlasticityException(const std::string& message,
                                  PlasticityError errorType = PlasticityError::STDP_COMPUTATION_FAILED,
                                  const std::string& context = "")
        : NLMException(message, Severity::ERROR,
                      static_cast<int>(errorType), context) {}

    PlasticityError errorType() const noexcept {
        return static_cast<PlasticityError>(errorCode_);
    }
};

// Neuromodulation errors
class NLMNeuromodulationException : public NLMException {
public:
    enum class NeuromodulationError {
        DOPAMINE_COMPUTATION_FAILED,
        REWARD_SIGNAL_FAILED,
        NOVELTY_COMPUTATION_FAILED,
        PREDICTION_ERROR_COMPUTED,
        CURIOUSY_FAILED,
        INVALID_MODULATION_SIGNAL
    };

    explicit NLMNeuromodulationException(const std::string& message,
                                         NeuromodulationError errorType = NeuromodulationError::PREDICTION_ERROR_COMPUTED,
                                         const std::string& context = "")
        : NLMException(message, Severity::WARNING,
                      static_cast<int>(errorType), context) {}

    NeuromodulationError errorType() const noexcept {
        return static_cast<NeuromodulationError>(errorCode_);
    }
};

// Input validation errors
class NLMValidationException : public NLMException {
public:
    enum class ValidationError {
        INDEX_OUT_OF_BOUNDS,
        NULL_POINTER,
        VALUE_OUT_OF_RANGE,
        SIZE_MISMATCH,
        INVALID_TYPE,
        INVALID_FORMAT,
        DUPLICATE_ENTRY
    };

    explicit NLMValidationException(const std::string& message,
                                   ValidationError errorType = ValidationError::INVALID_TYPE,
                                   const std::string& context = "")
        : NLMException(message, Severity::WARNING,
                      static_cast<int>(errorType), context) {}

    ValidationError errorType() const noexcept {
        return static_cast<ValidationError>(errorCode_);
    }
};

// Exception translation function for logging macros
void logException(const std::exception& e, const std::string& context = "");

// Helper function to convert exception severity to LogLevel
LogLevel exceptionSeverityToLogLevel(NLMException::Severity severity);

} // namespace nlm