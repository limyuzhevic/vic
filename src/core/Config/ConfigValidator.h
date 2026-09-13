// ConfigValidator.h
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <vector>
#include <functional>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration parameter definition
struct ConfigParameter {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    std::string defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    std::vector<std::string> validValues;  // For enum-like parameters
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    
    // Register a parameter definition
    void registerParameter(const ConfigParameter& param);
    
    // Validate a configuration object
    ValidationResult validate(const std::shared_ptr<Config>& config) const;
    
    // Get parameter definition by key
    std::optional<ConfigParameter> getParameter(const std::string& key) const;
    
    // Get all registered parameter keys
    std::vector<std::string> getParameterKeys() const;
    
    // Validate a specific parameter
    std::vector<std::string> validateParameter(const std::string& key, const ConfigValue& value) const;
    
    // Clear all registered parameters
    void clearParameters();
    
    // Get default configuration as Config object
    std::shared_ptr<Config> getDefaultConfig() const;
    
    // Get documentation string for all parameters
    std::string getDocumentation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global validator instance
extern std::unique_ptr<ConfigValidator> g_configValidator;

// Helper functions
ValidationResult validateConfig(const std::shared_ptr<Config>& config);
bool isValidParameterType(const std::string& key, const ConfigValue& value);
std::string getParameterType(const std::string& key);

} // namespace nlm
