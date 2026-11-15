#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations for parameter validation
struct ConfigValidationRules;

// Configuration parameter metadata
struct ConfigParameter {
    std::string key;
    std::string description;
    std::string type;
    ConfigValue defaultValue;
    bool required;
    std::vector<std::pair<ConfigValue, ConfigValue>> validRange;
    std::vector<ConfigValue> validValues;
    
    ConfigParameter(const std::string& k, const std::string& desc, 
                   const std::string& t, const ConfigValue& def,
                   bool req = false)
        : key(k), description(desc), type(t), defaultValue(def), 
          required(req) {}
};

// Global configuration schema
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add a parameter to the schema
    void addParameter(const ConfigParameter& param);
    
    // Get parameter by key
    const ConfigParameter* getParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getAllKeys() const;
    
    // Validate configuration value
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Get validation error message
    std::string getValidationError(const std::string& key, const ConfigValue& value) const;
    
    // Apply preset configurations
    void applyPreset(const std::string& presetName);
    
    // Get available preset names
    std::vector<std::string> getPresetNames() const;
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
