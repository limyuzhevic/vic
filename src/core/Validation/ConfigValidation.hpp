// Configuration validation framework for NLM

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation error types
enum class ValidationErrorType {
    InvalidType,      // Wrong type for configuration value
    OutOfRange,       // Value outside valid range
    MissingRequired,  // Required parameter missing
    DuplicateKey,     // Duplicate configuration key
    InvalidFormat     // Value format not recognized
};

// Configuration validation error
struct ValidationError {
    ValidationErrorType type;
    std::string key;
    std::string message;
    std::string suggestion;
    
    ValidationError(ValidationErrorType t, const std::string& k, const std::string& m,
                   const std::string& s = "") : type(t), key(k), message(m), suggestion(s) {}
};

// Configuration parameter type
enum class ConfigParameterType {
    Integer,
    Integer64,
    Double,
    Boolean,
    String,
    StringList,
    DoubleList,
    IntList
};

// Configuration parameter definition with validation rules
struct ConfigParameter {
    std::string key;
    ConfigParameterType type;
    std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    > defaultValue;
    std::string description;
    
    // Validation rules
    std::optional<std::function<bool(const ConfigValue&)>> validator;
    std::optional<std::pair<double, double>> range;  // For numeric types
    std::optional<size_t> minSize;                    // For list types
    std::optional<size_t> maxSize;                    // For list types
    bool required = false;
    
    ConfigParameter(const std::string& k, ConfigParameterType t, 
                   const std::variant<int, int64_t, double, bool, std::string, 
                                      std::vector<int>, std::vector<double>, std::vector<std::string>>& def,
                   const std::string& desc = "")
        : key(k), type(t), defaultValue(def), description(desc) {}
};

// Configuration schema for validation
class ConfigSchema {
public:
    void addParameter(const ConfigParameter& param);
    
    bool isValid(const std::string& key) const;
    bool isValid(const Config& config) const;
    
    std::vector<ValidationError> validate(const Config& config) const;
    std::vector<ValidationError> validate(const std::string& key, 
                                          const ConfigValue& value) const;
    
    std::string getParameterDescription(const std::string& key) const;
    
    std::vector<std::string> getAllKeys() const;
    std::vector<ConfigParameter> getParameters() const;
    
    // Get a parameter by key
    const ConfigParameter* getParameter(const std::string& key) const;
    
    // Predefined standard parameters for common NLM configuration
    static ConfigSchema createStandardSchema();
    
    // Utility functions for validation
    static bool validateType(const ConfigValue& value, ConfigParameterType type);
    static bool validateRange(const ConfigValue& value, const std::pair<double, double>& range);
    static bool validateSize(const ConfigValue& value, size_t minSize, size_t maxSize);
    
    // Error message generators
    static std::string errorMessage(ValidationErrorType type, const std::string& key, 
                                   const std::string& value, const ConfigParameter& param);

private:
    std::vector<ConfigParameter> parameters_;
    std::unordered_map<std::string, size_t> keyIndex_;
};

// Configuration validator with advanced validation capabilities
class ConfigValidator {
public:
    ConfigValidator(const ConfigSchema& schema) : schema_(schema) {}
    
    // Validate a configuration against the schema
    std::vector<ValidationError> validate(const Config& config) const;
    
    // Validate a single key-value pair
    std::vector<ValidationError> validate(const std::string& key, 
                                          const ConfigValue& value) const;
    
    // Get all validation errors
    bool hasErrors() const { return !errors_.empty(); }
    std::vector<ValidationError> getErrors() const { return errors_; }
    
    // Suggest fixes for errors
    std::vector<std::string> getSuggestions() const;
    
    // Reset validator state
    void reset() { errors_.clear(); }

private:
    const ConfigSchema& schema_;
    mutable std::vector<ValidationError> errors_;
    
    void addError(ValidationErrorType type, const std::string& key,
                  const std::string& message, const std::string& suggestion = "") const;
};

// Convenience functions for common validation scenarios
class ConfigValidationHelpers {
public:
    // Validate brain-related parameters
    static bool validateBrainParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate plasticity parameters
    static bool validatePlasticityParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate neuromodulation parameters
    static bool validateNeuromodulationParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate developmental parameters
    static bool validateDevelopmentParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate world/environment parameters
    static bool validateWorldParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate simulation parameters
    static bool validateSimulationParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Validate all parameters
    static bool validateAllParameters(const Config& config, std::vector<ValidationError>& errors);
    
    // Get parameter bounds and descriptions
    static std::string getParameterInfo(const std::string& key);
    static std::unordered_map<std::string, std::pair<double, double>> getParameterRanges();
    static std::unordered_map<std::string, std::string> getParameterDescriptions();
};

// Enhanced configuration class with validation support
class ValidatedConfig {
public:
    ValidatedConfig(const ConfigSchema& schema = ConfigSchema::createStandardSchema());
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load from args with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Set value with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Set value with validation and custom error handling
    bool setWithValidation(const std::string& key, const ConfigValue& value,
                           ConfigSource source = ConfigSource::Runtime,
                           std::vector<ValidationError>* errors = nullptr);
    
    // Get validation errors
    std::vector<ValidationError> getValidationErrors() const { return validator_.getErrors(); }
    
    // Clear all errors
    void clearErrors() { validator_.reset(); }
    
    // Check if configuration is valid
    bool isValid() const { return !validator_.hasErrors(); }
    
    // Validate configuration
    bool validate(std::vector<ValidationError>* errors = nullptr);
    
    // Get validation suggestions
    std::vector<std::string> getValidationSuggestions() const;
    
    // Delegate to underlying Config for normal operations
    bool has(const std::string& key) const { return config_.has(key); }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const { return config_.get<T>(key); }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_.getOr<T>(key, defaultValue);
    }
    
    void remove(const std::string& key) { config_.remove(key); }
    std::vector<std::string> getKeys() const { return config_.getKeys(); }
    void clear() { config_.clear(); }
    std::string summary() const { return config_.summary(); }
    
    // Get the underlying config for direct access
    const Config& getRawConfig() const { return config_; }

private:
    Config config_;
    ConfigSchema schema_;
    ConfigValidator validator_;
    
    // Internal validation helpers
    void applyParameterValidation(const std::string& key, const ConfigValue& value);
    void updateValidationCache();
};

} // namespace nlm
