#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

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

// Configuration validation rules
struct ConfigValidationRules {
    bool required = false;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigValidationRules(bool req = false, const std::string& desc = "", 
                         const std::function<bool(const ConfigValue&)>& val = nullptr,
                         const std::string& err = "Invalid value")
        : required(req), description(desc), validator(val), errorMessage(err) {}
    
    bool validate(const ConfigValue& value) const {
        if (validator) {
            return validator(value);
        }
        return true; // If no validator, assume valid
    }
};

// Configuration metadata
struct ConfigMetadata {
    std::string category; // e.g., "core", "plasticity", "development", "memory", "neuromodulation"
    std::string group;    // e.g., "general", "advanced", "debug"
    bool editable = true;
    std::string exampleValue;
    
    ConfigMetadata(const std::string& cat = "general", const std::string& grp = "core", 
                   bool editable = true, const std::string& example = "")
        : category(cat), group(grp), editable(editable), exampleValue(example) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    ConfigValidationRules validation;
    ConfigMetadata metadata;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), validation(), metadata(), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const ConfigValidationRules& val = ConfigValidationRules(),
                const ConfigMetadata& meta = ConfigMetadata(),
                const std::string& desc = "")
        : key(k), value(v), source(s), validation(val), metadata(meta), description(desc) {}
    
    bool isValid() const {
        return validation.validate(value);
    }
    
    std::string getValidationError() const {
        return validation.errorMessage;
    }
};

// Configuration schema manager
class ConfigSchema {
public:
    ConfigSchema() {}
    
    // Register a configuration parameter
    void registerParam(const std::string& key, const ConfigValue& defaultValue,
                      const ConfigValidationRules& validation = ConfigValidationRules(),
                      const ConfigMetadata& metadata = ConfigMetadata(),
                      const std::string& description = "") {
        validation_[
            key] = std::make_pair(defaultValue, std::make_pair(validation, description));
    }
    
    // Get parameter metadata
    bool hasParam(const std::string& key) const {
        return validation_.find(key) != validation_.end();
    }
    
    // Get default value
    ConfigValue getDefault(const std::string& key) const {
        if (hasParam(key)) {
            return validation_.at(key).first;
        }
        throw std::runtime_error("Unknown configuration parameter: " + key);
    }
    
    // Get validation rules
    ConfigValidationRules getValidation(const std::string& key) const {
        if (hasParam(key)) {
            return validation_.at(key).second.first;
        }
        return ConfigValidationRules();
    }
    
    // Get description
    std::string getDescription(const std::string& key) const {
        if (hasParam(key)) {
            return validation_.at(key).second.second;
        }
        return "";
    }
    
    // Get all parameter keys
    std::vector<std::string> getAllKeys() const {
        std::vector<std::string> keys;
        keys.reserve(validation_.size());
        for (const auto& pair : validation_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Get parameters by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const {
        std::vector<std::string> keys;
        for (const auto& pair : validation_) {
            // Extract category from key if not explicitly stored
            if (pair.first.find(category) != std::string::npos || 
                pair.first.find("." + category) != std::string::npos) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }
    
    // Generate help text for command line
    std::string generateHelpText() const {
        std::ostringstream oss;
        oss << "Configuration Parameters:" << std::endl;
        
        // Group by category
        std::map<std::string, std::vector<std::string>> categoryGroups;
        for (const auto& pair : validation_) {
            std::string category = "general";
            // Simple category detection based on key prefix
            if (pair.first.find("plasticity_") != std::string::npos) {
                category = "plasticity";
            } else if (pair.first.find("development_") != std::string::npos) {
                category = "development";
            } else if (pair.first.find("memory_") != std::string::npos) {
                category = "memory";
            } else if (pair.first.find("neuromodulation_") != std::string::npos) {
                category = "neuromodulation";
            } else if (pair.first.find("debug_") != std::string::npos) {
                category = "debug";
            }
            
            categoryGroups[category].push_back(pair.first);
        }
        
        for (const auto& categoryGroup : categoryGroups) {
            oss << categoryGroup.first << ":" << std::endl;
            for (const auto& key : categoryGroup.second) {
                auto val = validation_.at(key);
                oss << "  --" << key << "=VALUE    " << getDescription(key) << std::endl;
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
private:
    std::map<std::string, std::pair<ConfigValue, std::pair<ConfigValidationRules, std::string>>> validation_;
};

// Enhanced configuration manager
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
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with validation
    template<typename T>
    bool set(const std::string& key, T value, ConfigSource source = ConfigSource::Runtime) {
        return set(key, ConfigValue(value), source);
    }
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    std::string getValidationError(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all valid keys
    std::vector<std::string> getValidKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration schema
    ConfigSchema& getSchema() { return schema_; }
    const ConfigSchema& getSchema() const { return schema_; }
    
    // Load default schema
    void loadDefaultSchema();
    
    // Validate entire configuration
    bool validateAll() const;
    
    // Get configuration errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get value type name for debugging
    static std::string getValueTypeName(const ConfigValue& value);
    
    // Parse value from string
    static std::optional<ConfigValue> parseValueFromString(const std::string& key, const std::string& str);
    
    // Get example value for key
    static std::string getExampleValue(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration schema
    ConfigSchema schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isInteger(const std::string& str, int64_t& result);
    static bool isDouble(const std::string& str, double& result);
    static bool isBoolean(const std::string& str, bool& result);
    
    // Get entry reference
    ConfigEntry& getEntry(const std::string& key);
    const ConfigEntry& getEntry(const std::string& key) const;
    
    // Check if key exists in schema
    bool isKeyInSchema(const std::string& key) const;
    
    // Get default value for key
    ConfigValue getDefaultValue(const std::string& key) const;
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm