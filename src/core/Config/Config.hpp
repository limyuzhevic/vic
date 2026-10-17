#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <filesystem>

namespace nlm {

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Forward declarations
class Config;
class ConfigSchema;

// Enhanced configuration value types
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
    Runtime,
    Override
};

// Configuration validation rule type
using ValidationRule = std::function<bool(const ConfigValue&)>;

// Enhanced configuration entry with description and validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationRule> validators;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Add validation rule
    ConfigEntry& addValidator(const ValidationRule& validator) {
        validators.push_back(validator);
        return *this;
    }
    
    // Validate the entry
    bool validate() const {
        for (const auto& validator : validators) {
            if (!validator(value)) {
                return false;
            }
        }
        return true;
    }
};

// Configuration schema for type checking and validation
class ConfigSchema {
public:
    struct FieldInfo {
        std::string key;
        ConfigValue defaultValue;
        std::string description;
        bool required;
        std::vector<ValidationRule> validators;
        
        FieldInfo(const std::string& k, const ConfigValue& def, const std::string& desc = "", bool req = false)
            : key(k), defaultValue(def), description(desc), required(req) {}
        
        FieldInfo& addValidator(const ValidationRule& validator) {
            validators.push_back(validator);
            return *this;
        }
        
        bool validate(const ConfigValue& value) const {
            for (const auto& validator : validators) {
                if (!validator(value)) {
                    return false;
                }
            }
            return true;
        }
    };

private:
    std::map<std::string, FieldInfo> fields_;
    std::map<std::string, std::string> fieldDescriptions_;
    std::map<std::string, ConfigValue> defaultValues_;

public:
    // Add a field to the schema
    void addField(const std::string& key, const ConfigValue& defaultValue, 
                  const std::string& description = "", bool required = false) {
        fields_[key] = FieldInfo(key, defaultValue, description, required);
        fieldDescriptions_[key] = description;
        defaultValues_[key] = defaultValue;
    }
    
    // Add a field with validation
    void addField(const std::string& key, const ConfigValue& defaultValue,
                  const std::vector<ValidationRule>& validators,
                  const std::string& description = "", bool required = false) {
        fields_[key] = FieldInfo(key, defaultValue, description, required);
        fields_[key].validators = validators;
        fieldDescriptions_[key] = description;
        defaultValues_[key] = defaultValue;
    }
    
    // Get default value for a field
    ConfigValue getDefault(const std::string& key) const {
        auto it = defaultValues_.find(key);
        if (it != defaultValues_.end()) {
            return it->second;
        }
        throw ConfigValidationError("No default value for field: " + key);
    }
    
    // Validate a value against schema rules
    bool validateField(const std::string& key, const ConfigValue& value) const {
        auto it = fields_.find(key);
        if (it == fields_.end()) {
            return true; // Allow unknown fields (backward compatibility)
        }
        return it->second.validate(value);
    }
    
    // Get description for a field
    std::string getDescription(const std::string& key) const {
        auto it = fieldDescriptions_.find(key);
        if (it != fieldDescriptions_.end()) {
            return it->second;
        }
        return "";
    }
    
    // Get all field keys
    std::vector<std::string> getFieldKeys() const {
        std::vector<std::string> keys;
        keys.reserve(fields_.size());
        for (const auto& pair : fields_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Validate all entries in a config
    static bool validate(const Config& config, const ConfigSchema& schema) {
        bool allValid = true;
        for (const auto& entry : config.getAllEntries()) {
            if (!schema.validateField(entry.key, entry.value)) {
                std::cerr << "Validation error for field '" << entry.key << "': "
                         << "Value " << (entry.value.index() < 5 ? std::get<std::string>(entry.value) : "[complex]")
                         << " does not match schema\n";
                allValid = false;
            }
        }
        return allValid;
    }

private:
    struct ConfigImpl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_", bool prefixRequired = true);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw value
    ConfigValue getRaw(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Apply configuration schema
    bool applySchema(const ConfigSchema& schema);
    
    // Merge with another config (runtime overrides)
    void merge(const Config& other);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSONValue(const std::string& value, ConfigValue& result);
    static std::string toJSON(const ConfigValue& value);
    
    // Error handling
    void validateAndSet(const std::string& key, const ConfigValue& value, ConfigSource source);
};

// Default configuration schema
ConfigSchema getDefaultConfigSchema();

// Type-safe convenience getters
namespace ConfigHelpers {
    // Range validation helpers
    ValidationRule rangeValidator(double min, double max, const std::string& typeName = "double");
    ValidationRule rangeValidator(int64_t min, int64_t max, const std::string& typeName = "integer");
    ValidationRule rangeValidator(float min, float max, const std::string& typeName = "float");
    ValidationRule positiveValidator();
    ValidationRule nonNegativeValidator();
    ValidationRule booleanValidator();
    ValidationRule stringNotEmptyValidator();
    ValidationRule stringLengthValidator(size_t min, size_t max);
    
    // Configuration constant helpers
    ValidationRule getConstantValidator(const std::vector<std::string>& validValues);
    ValidationRule getConstantValidator(const std::vector<int>& validValues);
    
    // File existence validator
    ValidationRule fileExistsValidator(const std::string& pathKey = "path");
}

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm