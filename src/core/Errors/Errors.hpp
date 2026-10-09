// Core Exception Hierarchy for NLM
// This file provides comprehensive error handling for the NLM system

#pragma once

#include <string>
#include <stdexcept>
#include <system_error>
#include <map>

namespace nlm {

// Error categories
enum class ErrorCategory {
    Configuration,
    Neural,
    Memory,
    Temporal,
    Physical,
    Biological,
    Simulation,
    FileIO,
    Validation,
    System,
    Development
};

// Custom exception base class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message, ErrorCategory category = ErrorCategory::System)
        : std::runtime_error(message), category_(category) {}
    
    NLMException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::System)
        : std::runtime_error(formatMessage(message, file, line)), category_(category) {}
    
    ErrorCategory getCategory() const { return category_; }
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& file, int line) {
        return "[" + file + ":" + std::to_string(line) + "] " + message;
    }
    
private:
    ErrorCategory category_;
};

// Configuration-specific exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message, ErrorCategory category = ErrorCategory::Configuration)
        : NLMException(message, category) {}
    
    ConfigException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Configuration)
        : NLMException(message, file, line, category) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message, const std::string& file, int line)
        : ConfigException(message, file, line, ErrorCategory::Validation) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& message, const std::string& file, int line)
        : ConfigException(message, file, line, ErrorCategory::FileIO) {}
};

// Neural network exceptions
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message, ErrorCategory category = ErrorCategory::Neural)
        : NLMException(message, category) {}
    
    NeuralException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Neural)
        : NLMException(message, file, line, category) {}
};

class NeuronException : public NeuralException {
public:
    explicit NeuronException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Neural)
        : NeuralException(message, file, line, category) {}
};

class SynapseException : public NeuralException {
public:
    explicit SynapseException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Neural)
        : NeuralException(message, file, line, category) {}
};

// Memory management exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message, ErrorCategory category = ErrorCategory::Memory)
        : NLMException(message, category) {}
    
    MemoryException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Memory)
        : NLMException(message, file, line, category) {}
};

// Simulation exceptions
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message, ErrorCategory category = ErrorCategory::Simulation)
        : NLMException(message, category) {}
    
    SimulationException(const std::string& message, const std::string& file, int line, ErrorCategory category = ErrorCategory::Simulation)
        : NLMException(message, file, line, category) {}
};

class TimeStepException : public SimulationException {
public:
    explicit TimeStepException(const std::string& message, const std::string& file, int line)
        : SimulationException(message, file, line, ErrorCategory::Temporal) {}
};

// Validation exceptions
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message, const std::string& file, int line)
        : NLMException(message, file, line, ErrorCategory::Validation) {}
};

class RangeValidationException : public ValidationException {
public:
    explicit RangeValidationException(const std::string& param, double value, double min, double max, const std::string& file, int line)
        : ValidationException(formatRangeError(param, value, min, max), file, line) {}
    
    static std::string formatRangeError(const std::string& param, double value, double min, double max) {
        return "Parameter '" + param + "': value " + std::to_string(value) + " is outside valid range [" + 
               std::to_string(min) + ", " + std::to_string(max) + "]";
    }
};

class NullPointerException : public ValidationException {
public:
    explicit NullPointerException(const std::string& param, const std::string& file, int line)
        : ValidationException("Parameter '" + param + "' cannot be null", file, line) {}
};

// Development and biological exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message, const std::string& file, int line)
        : NLMException(message, file, line, ErrorCategory::Development) {}
};

class BiologicalException : public NLMException {
public:
    explicit BiologicalException(const std::string& message, const std::string& file, int line)
        : NLMException(message, file, line, ErrorCategory::Biological) {}
};

// Error code enumeration for programmatic error handling
enum class ErrorCode {
    // Configuration errors
    ConfigKeyNotFound = 1000,
    ConfigInvalidType,
    ConfigFileNotFound,
    ConfigParseError,
    
    // Neural errors
    NeuronIdInvalid,
    SynapseIdInvalid,
    NeuralRegionInvalid,
    InvalidMembranePotential,
    InvalidFiringRate,
    RefractoryPeriodInvalid,
    
    // Memory errors
    MemoryAllocationFailed,
    InvalidMemoryReference,
    
    // Time errors
    InvalidTimeStep,
    TimestampOutOfRange,
    
    // Physical errors
    InvalidPhysicalParameters,
    
    // Validation errors
    ParameterOutOfRange,
    NullPointerDetected,
    InvalidInputData,
    
    // System errors
    SystemInitializationFailed,
    ResourceUnavailable,
    
    // Simulation errors
    SimulationNotRunning,
    InvalidSimulationState,
    
    // Generic errors
    UnknownError = 9999
};

// Error reporting utilities
class ErrorReport {
public:
    static void reportError(const std::string& message, ErrorCode code, ErrorCategory category,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    
    static void logError(const std::string& message, ErrorCode code, ErrorCategory category,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    static std::string getErrorCodeDescription(ErrorCode code);
    static std::string getErrorCategoryDescription(ErrorCategory category);
    
    // Custom exception factory methods
    template<typename T, typename... Args>
    static T createException(Args&&... args) {
        return T(std::forward<Args>(args)...);
    }
    
private:
    static std::map<ErrorCode, std::string> errorCodeDescriptions_;
    static std::map<ErrorCategory, std::string> errorCategoryDescriptions_;
};

// Validation helper functions
class Validator {
public:
    // Parameter validation
    static void validateNotNull(const void* ptr, const std::string& paramName, const std::string& file = "", int line = 0);
    
    static void validateRange(double value, double min, double max, const std::string& paramName, const std::string& file = "", int line = 0);
    
    static void validateRange(int64_t value, int64_t min, int64_t max, const std::string& paramName, const std::string& file = "", int line = 0);
    
    static void validateRange(size_t value, size_t min, size_t max, const std::string& paramName, const std::string& file = "", int line = 0);
    
    static void validatePositive(const std::string& paramName, double value, const std::string& file = "", int line = 0);
    
    static void validateNonNegative(const std::string& paramName, int64_t value, const std::string& file = "", int line = 0);
    
    // Neural parameter validation
    static void validateMembranePotential(const std::string& paramName, double potential, const std::string& file = "", int line = 0);
    static void validateFiringRate(const std::string& paramName, double rate, const std::string& file = "", int line = 0);
    static void validateTimeStep(const std::string& paramName, double dt, const std::string& file = "", int line = 0);
    
    // Configuration validation
    static void validateConfigKey(const std::string& key, const std::string& file = "", int line = 0);
    static void validateConfigValue(const std::string& key, const std::string& value, const std::string& file = "", int line = 0);
    
    // Array/bounds validation
    static void validateArrayBounds(const void* ptr, size_t size, size_t maxSize, const std::string& paramName, const std::string& file = "", int line = 0);
    
    // Generic validation
    static void validateState(bool condition, const std::string& errorMessage, const std::string& file = "", int line = 0);
    
    static void validatePrecondition(bool condition, const std::string& errorMessage, const std::string& file = "", int line = 0);
    
    static void validatePostcondition(bool condition, const std::string& errorMessage, const std::string& file = "", int line = 0);
};

} // namespace nlm

// Error macros for common validation patterns
#define NLM_VALIDATE_NOT_NULL(ptr, name) \
    nlm::Validator::validateNotNull(ptr, name, __FILE__, __LINE__)

#define NLM_VALIDATE_RANGE(value, min, max, name) \
    nlm::Validator::validateRange(value, min, max, name, __FILE__, __LINE__)

#define NLM_VALIDATE_PRECONDITION(condition, message) \
    nlm::Validator::validatePrecondition(condition, message, __FILE__, __LINE__)

#define NLM_VALIDATE_STATE(condition, message) \
    nlm::Validator::validateState(condition, message, __FILE__, __LINE__)

#define NLM_THROW(exception_class, message) \
    throw nlm::exception_class(message, __FILE__, __LINE__)

#define NLM_THROW_RANGE_ERROR(param, value, min, max) \
    throw nlm::RangeValidationException(param, value, min, max, __FILE__, __LINE__)

#define NLM_THROW_NULL_ERROR(param) \
    throw nlm::NullPointerException(param, __FILE__, __LINE__)

#define NLM_CHECK_ERROR(condition, message, error_code) \
    do { \
        if (!(condition)) { \
            nlm::ErrorReport::reportError(message, error_code, nlm::ErrorCategory::Validation, __FILE__, __LINE__, __func__); \
            throw nlm::ValidationException(message, __FILE__, __LINE__); \
        } \
    } while(0)
