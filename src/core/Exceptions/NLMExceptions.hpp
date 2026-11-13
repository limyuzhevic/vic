// NLM Exception Hierarchy
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, const std::string& details = "")
        : std::runtime_error(formatMessage(message, details)),
          message_(message), details_(details) {
        Logger::getGlobal().error("NLM Exception: " + what(), "", 0, "");
    }
    
    virtual ~NLMException() = default;
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getDetails() const { return details_; }
    
    virtual std::string getErrorCode() const { return "NLM_0"; }
    
protected:
    std::string formatMessage(const std::string& message, const std::string& details) {
        std::ostringstream oss;
        oss << message;
        if (!details.empty()) {
            oss << " (Details: " << details << ")";
        }
        return oss.str();
    }
    
private:
    std::string message_;
    std::string details_;
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_CONFIG_"; }
};

class ConfigLoadException : public ConfigException {
public:
    ConfigLoadException(const std::string& message, const std::string& details = "")
        : ConfigException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_CONFIG_LOAD_"; }
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message, const std::string& details = "")
        : ConfigException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_CONFIG_VALIDATION_"; }
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& message, const std::string& details = "")
        : ConfigException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_CONFIG_PARSE_"; }
};

// Brain/Network-related exceptions
class BrainException : public NLMException {
public:
    BrainException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_BRAIN_"; }
};

class BrainInitializationException : public BrainException {
public:
    BrainInitializationException(const std::string& message, const std::string& details = "")
        : BrainException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_BRAIN_INIT_"; }
};

class BrainStepException : public BrainException {
public:
    BrainStepException(const std::string& message, const std::string& details = "")
        : BrainException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_BRAIN_STEP_"; }
};

// Memory-related exceptions
class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_MEMORY_"; }
};

class MemoryAllocationException : public MemoryException {
public:
    MemoryAllocationException(const std::string& message, const std::string& details = "")
        : MemoryException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_MEMORY_ALLOC_"; }
};

class MemoryCorruptionException : public MemoryException {
public:
    MemoryCorruptionException(const std::string& message, const std::string& details = "")
        : MemoryException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_MEMORY_CORRUPT_"; }
};

// Plasticity-related exceptions
class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_PLASTICITY_"; }
};

// Neuromodulation-related exceptions
class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_NEUROMODULATION_"; }
};

// Development-related exceptions
class DevelopmentException : public NLMException {
public:
    DevelopmentException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_DEVELOPMENT_"; }
};

// Prediction-related exceptions
class PredictionException : public NLMException {
public:
    PredictionException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_PREDICTION_"; }
};

// Cognitive-related exceptions
class CognitiveException : public NLMException {
public:
    CognitiveException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_COGNITIVE_"; }
};

// Sensor-related exceptions
class SensorException : public NLMException {
public:
    SensorException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_SENSOR_"; }
};

// Action-related exceptions
class ActionException : public NLMException {
public:
    ActionException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_ACTION_"; }
};

// Environment-related exceptions
class EnvironmentException : public NLMException {
public:
    EnvironmentException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_ENVIRONMENT_"; }
};

// Version and compatibility exceptions
class VersionException : public NLMException {
public:
    VersionException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_VERSION_"; }
};

class CompatibilityException : public NLMException {
public:
    CompatibilityException(const std::string& message, const std::string& details = "")
        : NLMException(message, details) {}
    
    virtual std::string getErrorCode() const override { return "NLM_COMPATIBILITY_"; }
};

// Utility function to convert exceptions to string for logging
inline std::string formatException(const std::exception& e) {
    std::ostringstream oss;
    oss << "Exception: " << e.what();
    if (const NLMException* nlmEx = dynamic_cast<const NLMException*>(&e)) {
        oss << " [Code: " << nlmEx->getErrorCode() << "]";
    }
    return oss.str();
}

// Helper macro for throwing NLM exceptions with location info
#define NLM_THROW_EXCEPTION(ExceptionType, message, details) \
    do { \
        std::string fullDetails = details; \
        if (!fullDetails.empty()) { \
            fullDetails = " at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ", " + fullDetails; \
        } else { \
            fullDetails = " at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ", " + details; \
        } \
        throw ExceptionType(message, fullDetails); \
    } while(0)

// Convenience macros for common NLM exceptions
#define NLM_THROW_CONFIG(msg, details) NLM_THROW_EXCEPTION(nlm::ConfigException, msg, details)
#define NLM_THROW_BRAIN(msg, details) NLM_THROW_EXCEPTION(nlm::BrainException, msg, details)
#define NLM_THROW_MEMORY(msg, details) NLM_THROW_EXCEPTION(nlm::MemoryException, msg, details)

} // namespace nlm
