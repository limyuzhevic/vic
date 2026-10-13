#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs) 
        : valid(v), errors(errs) {}
    ValidationResult(bool v, const std::vector<std::string>& errs, const std::vector<std::string>& warns)
        : valid(v), errors(errs), warnings(warns) {}
};

// Configuration constraint/validator
using ConfigValidator = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::type_index type;
    bool required;
    std::string default_value;
    std::vector<std::string> allowed_values;
    double min_value;
    double max_value;
    bool case_sensitive;
    
    ConfigValidator validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type(typeid(void)), required(false), default_value(), case_sensitive(true),
                   min_value(std::numeric_limits<double>::lowest()),
                   max_value(std::numeric_limits<double>::max()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          type(getType(v)), required(false), default_value(), case_sensitive(true),
          min_value(std::numeric_limits<double>::lowest()),
          max_value(std::numeric_limits<double>::max()) {}
    
    static std::type_index getType(const ConfigValue& value) {
        return std::visit([](auto&& arg) { return std::type_index(typeid(arg)); }, value);
    }
    
    template<typename T>
    bool isType() const {
        return type == std::type_index(typeid(T));
    }
    
    bool validate() const {
        if (validator) {
            ValidationResult result = validator(key, value);
            return result.valid;
        }
        return true;
    }
};

// Configuration schema for predefined settings
struct ConfigSchema {
    std::string key;
    std::string description;
    ConfigValue default_value;
    ConfigSource default_source;
    std::vector<std::string> dependencies;
    std::string category;
    int priority;
    
    ConfigSchema(const std::string& k, const std::string& desc, const ConfigValue& def,
                 ConfigSource src = ConfigSource::Default, const std::string& cat = "General")
        : key(k), description(desc), default_value(def), default_source(src), 
          dependencies(), category(cat), priority(0) {}
};

// Enhanced validation utilities
namespace ConfigValidation {
    ValidationResult validateInt(const std::string& key, const ConfigValue& value, 
                                double min = std::numeric_limits<double>::lowest(),
                                double max = std::numeric_limits<double>::max());
    
    ValidationResult validateDouble(const std::string& key, const ConfigValue& value,
                                    double min = std::numeric_limits<double>::lowest(),
                                    double max = std::numeric_limits<double>::max());
    
    ValidationResult validateBool(const std::string& key, const ConfigValue& value);
    
    ValidationResult validateString(const std::string& key, const ConfigValue& value,
                                    const std::vector<std::string>& allowed = {});
    
    ValidationResult validateRange(const std::string& key, const ConfigValue& value,
                                   const std::vector<std::string>& categories);
    
    ValidationResult validateRequired(const std::string& key, const ConfigValue& value);
    
    ValidationResult validatePositive(const std::string& key, const ConfigValue& value);
    
    ValidationResult validateNonNegative(const std::string& key, const ConfigValue& value);
    
    ValidationResult validatePercentage(const std::string& key, const ConfigValue& value);
    
    // Composition validators
    ValidationResult validateAnd(const std::vector<ConfigValidator>& validators);
    ValidationResult validateOr(const std::vector<ConfigValidator>& validators);
    
    // Compound validators with different types
    template<typename T>
    ValidationResult validateWith(const std::string& key, const ConfigValue& value,
                                  std::function<ValidationResult(T)> validator) {
        if (value.index() == std::type_index(typeid(T))) {
            return validator(std::get<T>(value));
        }
        return ValidationResult(false, {"Type mismatch: expected " + std::string(typeid(T).name())});
    }
}

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Safe set with type checking
    template<typename T>
    bool set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    ValidationResult validate(const std::string& key) const;
    ValidationResult validateAll() const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void loadDefaults();
    bool hasSchema(const std::string& key) const;
    ConfigSchema getSchema(const std::string& key) const;
    
    // Validation helper methods
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
                          std::function<ValidationResult(T)> validator = nullptr);
    
    // Utility methods
    static bool isValidKey(const std::string& key);
    static std::string normalizeKey(const std::string& key);
    static std::vector<std::string> parseKeyPath(const std::string& keyPath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidType(const ConfigValue& value, const std::type_index& expectedType);
    static void reportError(const std::string& key, const std::string& error);
    
    // JSON/YAML parsing (future)
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
};

// Explicit template instantiations
namespace ConfigTemplates {
    // Validation function specializations
    ValidationResult validateNeurons(int neuronCount);
    ValidationResult validateSynapseProbability(double probability);
    ValidationResult validateTimeStep(double timestep);
    ValidationResult validateLearningRate(double rate);
    ValidationResult validateRange01(double value);
    ValidationResult validatePositiveInt(int value);
    
    // Configuration schema definitions
    const std::vector<ConfigSchema> getDefaultSchemas();
}

} // namespace nlm
