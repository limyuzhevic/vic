#pragma once

/**
 * @file EnhancedConfig.hpp
 * @brief Enhanced configuration system with schema validation
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the enhanced configuration system for the NLM
 * computational neuroscience platform, providing robust configuration
 * management with schema validation, type safety, and modern configuration
 * formats (JSON, YAML, TOML).
 * 
 * Key features:
 * - Schema-based configuration validation
 * - Multiple configuration formats (JSON, YAML, TOML, INI)
 * - Type-safe value access with automatic conversion
 * - Configuration inheritance and composition
 * - Runtime configuration updates with validation
 * - Configuration templates and presets
 * - Advanced error handling with context
 * - Configuration history and audit trail
 * - Plugin-based configuration sources
 * 
 * This enhanced system addresses the limitations of the simple
 * key=value format by providing:
 * - Structured configuration with nested objects
 * - Type checking and validation
 * - Rich error messages with context
 * - Better user experience with auto-completion
 * - Integration with the improved error handling system
 */

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <any>
#include <functional>
#include <stdexcept>
#include <format>
#include <filesystem>
#include <chrono>
#include <type_traits>
#include <cassert>
#include "core/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Configuration value types
 */
using ConfigValue = std::variant<
    bool,
    int,
    int64_t,
    double,
    float,
    std::string,
    std::vector<bool>,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>,
    std::unordered_map<std::string, std::any>
>;

/**
 * @brief Configuration source enumeration
 */
enum class ConfigSource {
    Default,           ///< Default configuration
    File,              ///< File-based configuration
    CommandLine,       ///< Command-line configuration
    Runtime,           ///< Runtime configuration
    Environment,       ///< Environment variable configuration
    Programmatic,      ///< Programmatic configuration
    Template           ///< Template configuration
};

/**
 * @brief Configuration entry with metadata
 */
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string validationRule;
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point timestamp;
    std::string filepath;
    size_t lineNumber;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(),
                   category("General"), validationRule(), tags(),
                   timestamp(std::chrono::system_clock::now()),
                   filepath(), lineNumber(0) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "", const std::string& cat = "General")
        : key(k), value(v), source(s), description(desc), category(cat),
          validationRule(), tags(),
          timestamp(std::chrono::system_clock::now()),
          filepath(), lineNumber(0) {}
};

/**
 * @brief Schema validation rule
 */
struct ValidationRule {
    enum class RuleType {
        Range,           ///< Value must be within range
        Enum,            ///< Value must be from enum
        Pattern,         ///< Value must match pattern
        Custom,          ///< Custom validation function
        Required,        ///< Value is required
        Exclusive,       ///< Value cannot be with other values
        Dependent,       ///< Value depends on other values
        Length,          ///< String or collection length
        Type,            ///< Value must be of specific type
        Format          ///< Value must be in specific format
    };
    
    RuleType type;
    std::string name;
    std::any parameter;
    std::string errorMessage;
    std::function<bool(const std::any&)> validator;
    
    ValidationRule(RuleType t, const std::string& n, const std::any& p = std::any(),
                   const std::string& err = "Validation failed")
        : type(t), name(n), parameter(p), errorMessage(err) {
        setupValidator();
    }
    
    bool validate(const std::any& value) const {
        return validator(value);
    }
    
    std::string getErrorMessage(const std::any& value) const {
        return std::format(errorMessage + ": {} = {}", 
                          toString(value), typeToString(type));
    }
    
private:
    void setupValidator();
    std::string typeToString(RuleType type) const;
    std::string toString(const std::any& value) const;
};

/**
 * @brief Configuration schema definition
 */
struct ConfigSchema {
    std::string name;
    std::string description;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, ValidationRule> validationRules;
    std::unordered_map<std::string, std::string> defaultValues;
    bool allowUnknownKeys;
    std::string extendsSchema;  // Reference to parent schema
    
    ConfigSchema() : allowUnknownKeys(false) {}
    
    ConfigSchema(const std::string& n, const std::string& desc)
        : name(n), description(desc), allowUnknownKeys(false) {}
    
    /**
     * @brief Add validation rule for a key
     * 
     * @param key Key to validate
     * @param rule Validation rule
     * @return Reference to this schema
     */
    ConfigSchema& addRule(const std::string& key, const ValidationRule& rule);
    
    /**
     * @brief Add default value for a key
     * 
     * @param key Key to set default
     * @param value Default value
     * @return Reference to this schema
     */
    ConfigSchema& addDefault(const std::string& key, const std::any& value);
    
    /**
     * @brief Mark key as required
     * 
     * @param key Key that must be present
     * @return Reference to this schema
     */
    ConfigSchema& addRequired(const std::string& key);
    
    /**
     * @brief Enable unknown keys
     * 
     * @return Reference to this schema
     */
    ConfigSchema& allowUnknown() {
        allowUnknownKeys = true;
        return *this;
    }
    
    /**
     * @brief Set schema inheritance
     * 
     * @param parentSchema Name of parent schema
     * @return Reference to this schema
     */
    ConfigSchema& inherit(const std::string& parentSchema) {
        extendsSchema = parentSchema;
        return *this;
    }
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param config Configuration to validate
     * @param source Source of configuration
     * @throws ConfigError if validation fails
     */
    void validate(const std::vector<ConfigEntry>& config, ConfigSource source) const;
    
    /**
     * @brief Apply schema defaults to configuration
     * 
     * @param config Configuration to update
     * @return Updated configuration
     */
    std::vector<ConfigEntry> applyDefaults(const std::vector<ConfigEntry>& config) const;
    
    /**
     * @brief Get validation errors for configuration
     * 
     * @param config Configuration to validate
     * @param source Source of configuration
     * @return Vector of validation error messages
     */
    std::vector<std::string> getValidationErrors(
        const std::vector<ConfigEntry>& config, ConfigSource source) const;
};

/**
 * @brief Advanced configuration manager with schema support
 */
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig() = default;
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * @param filepath File path (supports JSON, YAML, TOML, INI)
     * @param source Configuration source identifier
     * @return true if loading succeeded
     */
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    /**
     * @brief Load configuration from string
     * 
     * @param content Configuration content
     * @param format Content format
     * @param source Configuration source identifier
     * @return true if loading succeeded
     */
    bool loadFromString(const std::string& content, const std::string& format = "auto",
                       ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Load configuration from JSON
     * 
     * @param jsonString JSON configuration string
     * @param source Configuration source identifier
     * @return true if loading succeeded
     */
    bool loadFromJSON(const std::string& jsonString,
                     ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Load configuration from YAML
     * 
     * @param yamlString YAML configuration string
     * @param source Configuration source identifier
     * @return true if loading succeeded
     */
    bool loadFromYAML(const std::string& yamlString,
                     ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Load configuration from TOML
     * 
     * @param tomlString TOML configuration string
     * @param source Configuration source identifier
     * @return true if loading succeeded
     */
    bool loadFromTOML(const std::string& tomlString,
                     ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Load configuration from environment variables
     * 
     * @param prefix Environment variable prefix
     * @param source Configuration source identifier
     */
    void loadFromEnvironment(const std::string& prefix = "NLM_",
                            ConfigSource source = ConfigSource::Environment);
    
    /**
     * @brief Save configuration to file
     * 
     * @param filepath Output file path
     * @param format Output format
     * @return true if saving succeeded
     */
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    /**
     * @brief Get configuration value
     * 
     * @tparam T Value type
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    /**
     * @brief Get configuration value with type checking
     * 
     * @tparam T Expected value type
     * @param key Configuration key
     * @return Configuration value (nullopt if not found or wrong type)
     */
    template<typename T>
    std::optional<T> getChecked(const std::string& key) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Configuration source
     * @param validate Whether to validate against schema
     */
    void set(const std::string& key, const ConfigValue& value,
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    
    /**
     * @brief Set configuration value with type conversion
     * 
     * @tparam T Value type
     * @param key Configuration key
     * @param value Configuration value
     * @param source Configuration source
     */
    template<typename T>
    void set(const std::string& key, const T& value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Get configuration entries by source
     * 
     * @param source Configuration source
     * @return Vector of configuration entries
     */
    std::vector<ConfigEntry> getEntries(ConfigSource source) const;
    
    /**
     * @brief Get configuration entries by category
     * 
     * @param category Configuration category
     * @return Vector of configuration entries
     */
    std::vector<ConfigEntry> getEntriesByCategory(const std::string& category) const;
    
    /**
     * @brief Clear all configuration
     */
    void clear();
    
    /**
     * @brief Clear configuration from specific source
     * 
     * @param source Configuration source
     */
    void clear(ConfigSource source);
    
    /**
     * @brief Merge configuration from another config
     * 
     * @param other Other configuration to merge
     * @param override Whether to override existing values
     */
    void merge(const EnhancedConfig& other, bool override = true);
    
    /**
     * @brief Set schema for configuration validation
     * 
     * @param schema Configuration schema
     */
    void setSchema(const ConfigSchema& schema);
    
    /**
     * @brief Get current schema
     * 
     * @return Current schema
     */
    const ConfigSchema& getSchema() const;
    
    /**
     * @brief Validate current configuration against schema
     * 
     * @param source Configuration source for validation
     * @return true if validation passed
     */
    bool validate(ConfigSource source = ConfigSource::Runtime) const;
    
    /**
     * @brief Get validation errors
     * 
     * @param source Configuration source
     * @return Vector of validation error messages
     */
    std::vector<std::string> getValidationErrors(ConfigSource source) const;
    
    /**
     * @brief Apply schema defaults
     * 
     * @param source Configuration source
     * @return true if defaults were applied
     */
    bool applyDefaults(ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Get configuration summary
     * 
     * @param source Configuration source
     * @return Configuration summary string
     */
    std::string summary(ConfigSource source = ConfigSource::Runtime) const;
    
    /**
     * @brief Get configuration history
     * 
     * @param source Configuration source
     * @return Vector of configuration history entries
     */
    std::vector<std::pair<std::chrono::system_clock::time_point, ConfigEntry>>
    getHistory(ConfigSource source) const;
    
    /**
     * @brief Get configuration value as string
     * 
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value as string
     */
    std::string getString(const std::string& key,
                         const std::string& defaultValue = "") const;
    
    /**
     * @brief Get configuration value as boolean
     * 
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value as boolean
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief Get configuration value as integer
     * 
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value as integer
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Get configuration value as double
     * 
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value as double
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief Get configuration value as vector
     * 
     * @tparam T Vector element type
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value as vector
     */
    template<typename T>
    std::vector<T> getVector(const std::string& key,
                           const std::vector<T>& defaultValue = {}) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Common configuration schemas
 */
namespace ConfigSchemas {

/**
 * @brief Create neural network configuration schema
 * 
 * @return Neural network configuration schema
 */
ConfigSchema createNeuralNetworkSchema();

/**
 * @brief Create simulation configuration schema
 * 
 * @return Simulation configuration schema
 */
ConfigSchema createSimulationSchema();

/**
 * @brief Create plasticity configuration schema
 * 
 * @return Plasticity configuration schema
 */
ConfigSchema createPlasticitySchema();

/**
 * @brief Create neuromodulation configuration schema
 * 
 * @return Neuromodulation configuration schema
 */
ConfigSchema createNeuromodulationSchema();

/**
 * @brief Create memory configuration schema
 * 
 * @return Memory configuration schema
 */
ConfigSchema createMemorySchema();

/**
 * @brief Create development configuration schema
 * 
 * @return Development configuration schema
 */
ConfigSchema createDevelopmentSchema();

/**
 * @brief Create logging configuration schema
 * 
 * @return Logging configuration schema
 */
ConfigSchema createLoggingSchema();

/**
 * @brief Create visualization configuration schema
 * 
 * @return Visualization configuration schema
 */
ConfigSchema createVisualizationSchema();

/**
 * @brief Create default configuration schema
 * 
 * @return Default configuration schema
 */
ConfigSchema createDefaultSchema();

} // namespace ConfigSchemas

/**
 * @brief Configuration utility functions
 */
namespace ConfigUtils {

/**
 * @brief Parse boolean string value
 * 
 * @param value String value
 * @return Boolean value
 */
bool parseBool(const std::string& value);

/**
 * @brief Parse integer string value
 * 
 * @param value String value
 * @param base Number base
 * @return Integer value
 */
int parseInt(const std::string& value, int base = 10);

/**
 * @brief Parse double string value
 * 
 * @param value String value
 * @return Double value
 */
double parseDouble(const std::string& value);

/**
 * @brief Parse string value
 * 
 * @param value String value
 * @return String value (trimmed)
 */
std::string parseString(const std::string& value);

/**
 * @brief Parse vector string value
 * 
 * @param value String value (comma-separated)
 * @return Vector value
 */
std::vector<std::string> parseVector(const std::string& value);

/**
 * @brief Format configuration value as string
 * 
 * @param value Configuration value
 * @param format Value format
 * @return Formatted string
 */
std::string formatValue(const ConfigValue& value, const std::string& format = "default");

/**
 * @brief Normalize configuration key
 * 
 * @param key Key to normalize
 * @return Normalized key
 */
std::string normalizeKey(const std::string& key);

/**
 * @brief Check if key matches pattern
 * 
 * @param key Key to check
 * @param pattern Pattern to match against
 * @return true if key matches pattern
 */
bool keyMatchesPattern(const std::string& key, const std::string& pattern);

/**
 * @brief Get configuration value type name
 * 
 * @param value Configuration value
 * @return Type name
 */
std::string getValueTypeName(const ConfigValue& value);

/**
 * @brief Convert configuration value between types
 * 
 * @tparam T Target type
 * @param value Source value
 * @return Converted value
 */
template<typename T>
T convertValue(const ConfigValue& value);

} // namespace ConfigUtils

// Explicit instantiations
namespace nlm {
    template int EnhancedConfig::get<int>(const std::string&, const int&) const;
    template double EnhancedConfig::get<double>(const std::string&, const double&) const;
    template std::string EnhancedConfig::get<std::string>(const std::string&, const std::string&) const;
    template bool EnhancedConfig::get<bool>(const std::string&, const bool&) const;
    
    template std::vector<int> EnhancedConfig::getVector<int>(const std::string&, const std::vector<int>&) const;
    template std::vector<double> EnhancedConfig::getVector<double>(const std::vector<double>&) const;
    template std::vector<std::string> EnhancedConfig::getVector<std::string>(const std::string&, const std::vector<std::string>&) const;
    
    template void EnhancedConfig::set<int>(const std::string&, const int&, ConfigSource);
    template void EnhancedConfig::set<double>(const std::string&, const double&, ConfigSource);
    template void EnhancedConfig::set<std::string>(const std::string&, const std::string&, ConfigSource);
    template void EnhancedConfig::set<bool>(const std::string&, const bool&, ConfigSource);
    
    template std::string ConfigUtils::convertValue<std::string>(const ConfigValue&);
    template int ConfigUtils::convertValue<int>(const ConfigValue&);
    template double ConfigUtils::convertValue<double>(const ConfigValue&);
    template bool ConfigUtils::convertValue<bool>(const ConfigValue&);
}