#pragma once

// Configuration Validation System for NLM
// Provides comprehensive validation for NLM configuration parameters
// Supports range checking, dependency validation, and type validation

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <sstream>

namespace nlm {
namespace config {

// Configuration validation exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigRangeError : public ConfigValidationError {
public:
    ConfigRangeError(const std::string& key, double value, double min, double max)
        : ConfigValidationError("Configuration key \"" + key + "\" has invalid value " + 
                               std::to_string(value) + ". Must be in range [" + 
                               std::to_string(min) + ", " + std::to_string(max) + "]") {}
};

class ConfigDependencyError : public ConfigValidationError {
public:
    ConfigDependencyError(const std::string& key, const std::string& dependency)
        : ConfigValidationError("Configuration key \"" + key + "\" requires dependency \"" + dependency + "\"") {}
};

class ConfigTypeError : public ConfigValidationError {
public:
    ConfigTypeError(const std::string& key, const std::string& expectedType)
        : ConfigValidationError("Configuration key \"" + key + "\" has invalid type. Expected " + expectedType) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "float", "bool", "string"
    double minValue;
    double maxValue;
    bool required;
    std::string defaultValue;
    std::vector<std::string> dependencies; // Keys that must be set for this to be valid
    std::string description;
};

// Global configuration schema
class ConfigValidator {
private:
    std::vector<ConfigSchema> schema;
    std::map<std::string, bool> validationEnabled;
    
public:
    ConfigValidator();
    
    // Enable or disable validation for a specific key
    void setValidationEnabled(const std::string& key, bool enabled);
    
    // Validate a configuration
    void validate(const Config& config);
    
    // Validate a configuration with additional checks
    void validate(const Config& config, bool enableSpecialValidation);
    
    // Get validation errors for a configuration (doesn't throw)
    std::vector<std::string> getValidationErrors(const Config& config);
    
    // Check if configuration is valid (doesn't throw)
    bool isValid(const Config& config);
    
    // Get schema information
    std::vector<ConfigSchema> getSchema() const;
    
    // Get validation status for a key
    bool isValidationEnabled(const std::string& key) const;
    
    // Get schema entry for a key
    ConfigSchema getSchemaEntry(const std::string& key) const;
    
private:
    void initializeSchema();
    void validateKey(const Config& config, const ConfigSchema& entry, std::vector<std::string>& errors);
    void validateDependencies(const Config& config, const ConfigSchema& entry, std::vector<std::string>& errors);
    void validateSpecialCombinations(const Config& config, std::vector<std::string>& errors);
    std::string getConfigValue(const Config& config, const std::string& key);
};

// Global validator instance
extern ConfigValidator globalValidator;

// Convenience functions
void validateConfig(const Config& config);
void validateConfig(const Config& config, bool enableSpecialValidation);
bool isValidConfig(const Config& config);
std::vector<std::string> getConfigValidationErrors(const Config& config);

// Configuration migration system
class ConfigMigrator {
private:
    std::map<std::string, std::string> migrations;
    
public:
    ConfigMigrator();
    
    void migrate(Config& config);
    
private:
    void initializeMigrations();
};

// Global migrator instance
extern ConfigMigrator globalMigrator;

void migrateConfig(Config& config);

// Configuration backup and restore
void backupConfig(const Config& config, const std::string& filepath);
Config restoreConfig(const std::string& filepath);

// Configuration diff utility
std::string diffConfig(const Config& config1, const Config& config2);

} // namespace config
} // namespace nlm
