#pragma once

#include "Config.hpp"
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <chrono>

namespace nlm {

// Configuration validation rule
class ValidationRule {
public:
    using ValidationFunc = std::function<bool(const ConfigValue&)>;
    
    ValidationRule() : func(nullptr), errorMessage("Validation failed") {}
    ValidationRule(ValidationFunc f, const std::string& error) 
        : func(f), errorMessage(error) {}
    
    bool validate(const ConfigValue& value) const {
        return !func || func(value);
    }
    
    const std::string& getErrorMessage() const { return errorMessage; }
    
private:
    ValidationFunc func;
    std::string errorMessage;
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigValue defaultValue;
    ConfigSource source;
    std::string description;
    std::vector<ValidationRule> validationRules;
    bool required;
    
    ConfigSchema() : source(ConfigSource::Default), required(false) {}
    ConfigSchema(const std::string& k, const ConfigValue& v, 
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default), 
          description(desc), required(req) {}
    
    ConfigSchema(const std::string& k, const std::string& v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    
    ConfigSchema(const std::string& k, double v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    
    ConfigSchema(const std::string& k, int v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    
    ConfigSchema(const std::string& k, bool v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
};

// Configuration change notification
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    virtual void onConfigChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) = 0;
};

// Advanced configuration manager with validation, schema, and monitoring
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Load and save configuration
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Schema-based initialization
    void applySchema(const std::vector<ConfigSchema>& schema);
    void validateCurrentConfig() const;
    
    // Enhanced get/set with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    bool set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration monitoring
    void addChangeListener(ConfigChangeListener* listener);
    void removeChangeListener(ConfigChangeListener* listener);
    
    // Configuration information
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::string getDescription(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    
    // Configuration management
    void remove(const std::string& key);
    void clear();
    void resetToDefaults();
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::string generateDocumentation() const;
    std::string generateConfigSample() const;
    
    // Configuration validation
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration inheritance
    void inheritFrom(const ConfigManager& other);
    
    // Configuration templates
    void saveTemplate(const std::string& name, const std::vector<std::string>& keys);
    void loadTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration comparison
    bool equals(const ConfigManager& other) const;
    void merge(const ConfigManager& other);
    
    // Configuration access
    const std::shared_ptr<Config>& getConfig() const { return config; }
    std::shared_ptr<Config>& getConfig() { return config; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void notifyListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    bool isValueValid(const std::string& key, const ConfigValue& value) const;
    std::string getValidationErrorForValue(const std::string& key, const ConfigValue& value) const;
    ConfigValue getDefaultValue(const std::string& key) const;
    bool hasDefaultValue(const std::string& key) const;
};

// Helper functions for configuration validation
namespace config {
    // Validation functions
    bool isNonEmptyString(const ConfigValue& value);
    bool isPositiveInt(const ConfigValue& value);
    bool isPositiveDouble(const ConfigValue& value);
    bool isBoolean(const ConfigValue& value);
    bool isWithinRange(const ConfigValue& value, double min, double max);
    bool matchesPattern(const ConfigValue& value, const std::string& pattern);
    
    // Type conversion utilities
    std::string toString(const ConfigValue& value);
    std::string formatValue(const ConfigValue& value);
    
    // Configuration utilities
    std::vector<std::string> getConfigKeysWithPrefix(const Config& config, const std::string& prefix);
    void stripPrefixFromKeys(Config& config, const std::string& prefix);
    
    // Command line parsing
    struct ParsedArgs {
        std::vector<std::string> unknownArgs;
        std::vector<std::pair<std::string, std::string>> knownArgs;
    };
    
    ParsedArgs parseAdvancedArgs(int argc, char** argv, 
                                 const std::vector<std::string>& knownKeys);
}

} // namespace nlm
