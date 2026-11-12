// Configuration validation and error handling utilities
// Provides comprehensive validation and error checking for the NLM project

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Custom exception types for different error categories
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class InitializationError : public std::runtime_error {
public:
    explicit InitializationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& message) : std::runtime_error(message) {}
};

// Error code enumeration for programmatic error handling
enum class ErrorCode {
    Success = 0,
    InvalidArgument = 1,
    OutOfRange = 2,
    ValidationFailed = 3,
    InitializationFailed = 4,
    ConfigurationError = 5,
    FileError = 6,
    ResourceExhausted = 7,
    InternalError = 8
};

// Error info structure for detailed error reporting
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string context;
    std::map<std::string, std::string> details;
    
    ErrorInfo(ErrorCode c, const std::string& msg, const std::string& ctx = "")
        : code(c), message(msg), context(ctx) {}
    
    void addDetail(const std::string& key, const std::string& value) {
        details[key] = value;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Error " << static_cast<int>(code) << ": " << message;
        if (!context.empty()) {
            ss << " (Context: " << context << ")";
        }
        for (const auto& detail : details) {
            ss << " [" << detail.first << "=" << detail.second << "]";
        }
        return ss.str();
    }
};

// Value validation functions
namespace Validation {
    
    // Validate numeric range
    bool validateRange(double value, double min, double max, const std::string& paramName);
    
    // Validate positive values
    bool validatePositive(int value, const std::string& paramName);
    bool validatePositive(double value, const std::string& paramName);
    
    // Validate string
    bool validateString(const std::string& value, size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                       const std::string& paramName = "");
    
    // Validate integer
    bool validateInteger(int64_t value, int64_t min = std::numeric_limits<int64_t>::min(),
                        int64/atom_t max = std::numeric_limits<int64_t>::max(),
                        const std::string& paramName = "");
    
    // Validate configuration keys
    bool validateConfigKey(const std::string& key);
    
    // Validate neuron count
    bool validateNeuronCount(size_t neuronCount, const std::string& context = "");
    
    // Validate connection probability
    bool validateConnectionProbability(float probability);
    
    // Validate timestep
    bool validateTimestep(float timestep);
    
    // Validate file paths
    bool validateFilePath(const std::string& path, bool mustExist = false);
    
    // Validate directory path
    bool validateDirectoryPath(const std::string& path, bool mustExist = true);
    
    // Validate floating point value
    bool validateFloat(float value, float min = -std::numeric_limits<float>::max(),
                      float max = std::numeric_limits<float>::max(),
                      float tolerance = 0.001f, const std::string& paramName = "");
    
    // Validate boolean
    bool validateBoolean(bool value);
    
    // Validate array bounds
    template<typename T>
    bool validateArrayBounds(const std::vector<T>& array, size_t maxSize, const std::string& paramName = "");
    
    // Validate memory size
    bool validateMemorySize(size_t bytes, size_t maxAllowed = 1ULL << 30);  // Default 1GB
    
    // Validate simulation parameters
    bool validateSimulationParams(size_t maxSteps, float timestep, size_t neuronCount);
}

// Error handler class
class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    // Record an error
    void recordError(ErrorCode code, const std::string& message, const std::string& context = "",
                     const std::map<std::string, std::string>& details = {});
    
    // Get last error
    std::optional<ErrorInfo> getLastError() const;
    
    // Check if errors occurred
    bool hasErrors() const;
    
    // Clear all errors
    void clearErrors();
    
    // Get all errors
    std::vector<ErrorInfo> getAllErrors() const;
    
    // Log errors to logger
    void logErrors(const std::string& source = "");
    
    // Throw first error if any
    void throwFirstError() const;
    
    // Validate and add error if invalid
    template<typename T>
    bool validate(const T& value, ErrorCode errorCode, const std::string& message,
                 const std::string& context = "", const std::string& paramName = "",
                 const std::map<std::string, std::string>& details = {});
    
private:
    std::vector<ErrorInfo> errors;
    std::optional<ErrorInfo> lastError;
};

// Configuration validation wrapper
class ConfigurationValidator {
public:
    ConfigurationValidator(std::shared_ptr<Config> config);
    
    // Validate entire configuration
    ErrorCode validateConfiguration(bool strict = false) const;
    
    // Validate specific configuration values
    ErrorCode validateConfigValue(const std::string& key, const ConfigValue& value) const;
    
    // Get validation warnings
    std::vector<ErrorInfo> getValidationWarnings() const;
    
    // Get validation errors
    std::vector<ErrorInfo> getValidationErrors() const;
    
    // Add validation rule
    void addValidationRule(const std::string& key,
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage);
    
private:
    std::shared_ptr<Config> config;
    std::vector<ErrorInfo> warnings;
    std::vector<ErrorInfo> errors;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
};

// Initialization validator
class InitializationValidator {
public:
    InitializationValidator(std::shared_ptr<Brain> brain);
    
    // Validate brain initialization
    ErrorCode validateBrainInitialization() const;
    
    // Validate system integration
    ErrorCode validateSystemIntegration() const;
    
    // Validate memory systems
    ErrorCode validateMemorySystems() const;
    
    // Validate neuromodulation systems
    ErrorCode validateNeuromodulationSystems() const;
    
    // Get validation results
    std::map<std::string, ErrorCode> getValidationResults() const;
    
    // Check if validation passed
    bool isValid() const;
    
    // Get validation errors
    std::vector<ErrorInfo> getValidationErrors() const;
    
private:
    std::shared_ptr<Brain> brain;
    std::map<std::string, ErrorCode> validationResults;
};

// Safe configuration getter with validation
template<typename T>
class SafeConfigGetter {
public:
    SafeConfigGetter(std::shared_ptr<Config> config, const std::string& key,
                    const T& defaultValue = T(), bool validate = true);
    
    // Get value with validation
    std::optional<T> get(ErrorCode& errorCode, std::string& errorMessage) const;
    
    // Get value or default (with validation)
    T getOr(ErrorCode& errorCode, std::string& errorMessage) const;
    
    // Check if key exists and value is valid
    bool isValid(ErrorCode& errorCode, std::string& errorMessage) const;
    
    // Get key
    const std::string& getKey() const;
    
private:
    std::shared_ptr<Config> config;
    std::string key;
    T defaultValue;
    bool validateFlag;
};

// Enhanced configuration class with validation
class ValidatedConfig : public Config {
public:
    ValidatedConfig();
    
    // Override set methods with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override;
    
    // Add validation rules
    void addValidationRule(const std::string& key, const std::string& ruleType,
                          const std::string& params, const std::string& errorMessage);
    
    // Validate configuration
    ErrorCode validate(bool strict = false) const;
    
    // Get error handler
    ErrorHandler getErrorHandler() const;
    
    // Get validation results
    std::vector<ErrorInfo> getValidationErrors() const;
    
private:
    ErrorHandler errorHandler;
    ConfigurationValidator validator;
};

// Utility functions for common validation scenarios
namespace ValidationUtils {
    
    // Validate configuration file path
    bool validateConfigFile(const std::string& filepath);
    
    // Validate checkpoint file path
    bool validateCheckpointFile(const std::string& filepath);
    
    // Validate directory creation
    bool ensureDirectoryExists(const std::string& path);
    
    // Validate file write permissions
    bool checkWritePermissions(const std::string& path);
    
    // Validate simulation stability
    bool validateStability(float avgFiringRate, float excitationInhibitionRatio);
    
    // Validate resource usage
    bool validateResourceUsage(size_t memoryMB, float cpuUsage);
    
    // Validate time parameters
    bool validateTimeParams(float timestep, size_t maxSteps);
    
    // Validate neuron parameters
    bool validateNeuronParams(float vThresh, float vRest, float vReset, float tauMem);
    
    // Validate plasticity parameters
    bool validatePlasticityParams(float ltpWeight, float ltdWeight, float stdpTau);
    
    // Validate neuromodulation parameters
    bool validateNeuromodulationParams(float dopamineScale, float curiosityWeight);
    
    // Validate memory parameters
    bool validateMemoryParams(size_t capacity, size_t maxEpisodes);
}