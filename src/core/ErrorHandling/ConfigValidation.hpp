#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace nlm {

class Config;

// Enhanced configuration validation
class ConfigValidator {
public:
    static bool validate(const Config& config, std::vector<std::string>& errors);
    static bool validateValueType(const std::string& key, const ConfigValue& value, 
                                  const std::type_info& expectedType);
    static bool validateRange(const std::string& key, double value, 
                              double min, double max);
    static bool validateConfigSpecific(const std::string& key, const ConfigValue& value);
    static bool validateFilePath(const std::string& path, bool requireWritable = false);
    
    // Configuration-specific validations
    static bool validateNeuronCount(size_t value);
    static bool validateRegionCount(size_t value);
    static bool validateConnectionProbability(float value);
    static bool validatePlasticityParameters(const Config& config);
    static bool validateDevelopmentParameters(const Config& config);
    static bool validateNeuromodulationParameters(const Config& config);
    static bool validateMemoryParameters(const Config& config);
    static bool validateCheckpointParameters(const Config& config);
};

// Exception types for validation
class ConfigValidationError : public ConfigurationError {
public:
    ConfigValidationError(const std::string& message, const std::string& key, 
                          const std::string& expected, const std::string& actual)
        : ConfigurationError(message, key), expected_(expected), actual_(actual) {}
    
    const std::string& getExpected() const { return expected_; }
    const std::string& getActual() const { return actual_; }
    
private:
    std::string expected_;
    std::string actual_;
};

class RequiredValueError : public ConfigValidationError {
public:
    RequiredValueError(const std::string& key)
        : ConfigValidationError("Required configuration value is missing", key, "required", "missing") {}
};

class InvalidValueError : public ConfigValidationError {
public:
    InvalidValueError(const std::string& key, const std::string& expected, const std::string& actual)
        : ConfigValidationError("Invalid configuration value", key, expected, actual) {}
};

class OutOfRangeError : public ConfigValidationError {
public:
    OutOfRangeError(const std::string& key, double actual, double min, double max)
        : ConfigValidationError("Configuration value out of range", key, 
                               "between " + std::to_string(min) + " and " + std::to_string(max),
                               std::to_string(actual)) {}
};

class InvalidTypeError : public ConfigValidationError {
public:
    InvalidTypeError(const std::string& key, const std::type_info& expected, 
                    const std::type_info& actual)
        : ConfigValidationError("Invalid configuration type", key,
                               getTypeName(expected), getTypeName(actual)) {}
    
private:
    static std::string getTypeName(const std::type_info& type) {
        return type.name();
    }
};

} // namespace nlm
