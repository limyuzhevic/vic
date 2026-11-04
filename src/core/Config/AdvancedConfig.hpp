// Advanced Configuration System
// Enhanced configuration with validation, serialization, and advanced features
// Supports JSON, YAML, TOML, and custom formats
// Provides validation, defaults, and user-friendly API

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>

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

// Configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Imported
};

// Configuration validation error
struct ConfigError : public std::runtime_error {
    ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   required(false), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::function<bool(const ConfigValue&)>& val,
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req), validator(val) {}
    
    // Validation
    bool isValid() const {
        if (validator && !validator(value)) {
            return false;
        }
        return true;
    }
    
    // Type checking helpers
    template<typename T>
    bool isType() const {
        return std::holds_alternative<T>(value);
    }
    
    // Value conversion helpers
    template<typename T>
    T as() const {
        if (!isType<T>()) {
            throw ConfigError("Type mismatch for key: " + key);
        }
        return std::get<T>(value);
    }
    
    // Safe conversion with default
    template<typename T>
    T asOr(const T& defaultValue) const {
        if (isType<T>()) {
            return std::get<T>(value);
        }
        return defaultValue;
    }
};

// Advanced configuration class with validation and serialization
class AdvancedConfig {
public:
    AdvancedConfig() {
        setupDefaultSchema();
    }
    
    ~AdvancedConfig() {}
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept = default;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept = default;
    
    // Load from file (JSON, YAML, TOML support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Get value with type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set value with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Add schema validation rule
    void addValidationRule(const std::string& key, 
                          const std::function<bool(const ConfigValue&)>& validator,
                          const std::string& errorMessage = "");
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration entry
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Clear all configuration
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate entire configuration
    std::vector<std::string> validate() const;
    
    // Get configuration statistics
    std::string getStats() const;
    
    // Import configuration from another config
    bool importFrom(const AdvancedConfig& other, ConfigSource source = ConfigSource::Imported);
    
    // Merge configuration (other overrides this)
    void mergeFrom(const AdvancedConfig& other);
    
    // Get configuration by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Reset to defaults
    void resetToDefaults();
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Get configuration value type
    std::string getValueType(const std::string& key) const;
    
    // Check if key is required
    bool isRequired(const std::string& key) const;
    
    // Set key as required
    void setRequired(const std::string& key, bool required);
    
    // Get all keys with their descriptions
    std::map<std::string, std::string> getSchema() const;
    
    // Set description for a key
    void setDescription(const std::string& key, const std::string& description);
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
private:
    // Internal storage
    std::map<std::string, ConfigEntry> configMap;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, bool> requiredFlags;
    
    // Default schema setup
    void setupDefaultSchema();
    
    // Configuration parsing helpers
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    bool parseCommandLineArg(const std::string& arg, std::string& key, ConfigValue& value);
    
    // Configuration serialization helpers
    std::string toJson(const ConfigEntry& entry) const;
    bool parseJsonValue(const std::string& jsonStr, ConfigValue& value);
    
    // Configuration validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalKeys;
        size_t validKeys;
        size_t invalidKeys;
        std::map<ConfigSource, size_t> keysBySource;
        std::map<std::string, size_t> typesByCount;
        
        ConfigStats() : totalKeys(0), validKeys(0), invalidKeys(0) {}
    };
    
    ConfigStats calculateStats() const;
}; 

// Convenience type alias for backward compatibility
using Config = AdvancedConfig;

// Helper functions for common configuration tasks
namespace ConfigHelpers {
    // Get common NLM configuration values
    double getSimulationTimestep(const AdvancedConfig& config);
    size_t getNeuronCount(const AdvancedConfig& config);
    size_t getRegionCount(const AdvancedConfig& config);
    float getConnectionProbability(const AdvancedConfig& config);
    int64_t getRandomSeed(const AdvancedConfig& config);
    
    // Set common NLM configuration values
    void setSimulationTimestep(AdvancedConfig& config, double timestep);
    void setNeuronCount(AdvancedConfig& config, size_t count);
    void setRegionCount(AdvancedConfig& config, size_t count);
    void setConnectionProbability(AdvancedConfig& config, float probability);
    void setRandomSeed(AdvancedConfig& config, int64_t seed);
    
    // Validate NLM configuration
    std::vector<std::string> validateNLMConfig(const AdvancedConfig& config);
    
    // Create default NLM configuration
    AdvancedConfig createDefaultNLMConfig();
}

// Configuration template implementation
namespace detail {
    template<typename T>
    std::optional<T> AdvancedConfig::get(const std::string& key) const {
        auto it = configMap.find(key);
        if (it == configMap.end()) {
            return std::nullopt;
        }
        
        const ConfigEntry& entry = it->second;
        if (entry.isType<T>()) {
            return std::get<T>(entry.value);
        }
        return std::nullopt;
    }
    
    template<typename T>
    T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        if (value.has_value()) {
            return *value;
        }
        return defaultValue;
    }
}

} // namespace nlm
