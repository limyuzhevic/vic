#pragma once

// Enhanced configuration system with JSON/YAML support
// Provides schema validation, type safety, and comprehensive configuration management

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <regex>
#include <filesystem>
#include <sstream>
#include <json/json.h>  // Simple JSON library

namespace nlm {

// ==================== CONFIGURATION VALUE TYPES ====================

// Enhanced configuration value types with validation
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// ==================== CONFIGURATION SOURCE ====================

enum class ConfigSource {
    Default = 0,      // Default value from configuration schema
    File = 1,         // Loaded from configuration file (JSON/YAML)
    CommandLine = 2,  // Command line arguments
    Environment = 3,  // Environment variables
    Runtime = 4,      // Set at runtime programmatically
    Override = 5      // Explicit override (highest priority)
};

// ==================== CONFIGURATION ENTRY WITH METADATA ====================

struct ConfigEntry {
    std::string key;               // Configuration key
    ConfigValue value;              // Configuration value
    ConfigSource source;           // Source of this configuration
    std::string description;       // Human-readable description
    std::string category;          // Configuration category (e.g., "simulation", "plasticity")
    std::string validationPattern; // Regex pattern for string validation
    bool required;                 // Whether this configuration is required
    std::string defaultValue;      // String representation of default
    
    // Validation functions
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> formatter;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), category(), validationPattern(),
          required(false), defaultValue(), validator(nullptr), formatter(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s = ConfigSource::Default,
                const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat), validationPattern(),
          required(false), defaultValue(), validator(nullptr), formatter(nullptr) {}
    
    // Type-specific getters
    template<typename T>
    std::optional<T> getAs() const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Validation methods
    bool validate() const {
        if (validator) return validator(value);
        return true;
    }
    
    std::string format() const {
        if (formatter) return formatter(value);
        return std::visit([](const auto& val) -> std::string {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return val;
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                return val ? "true" : "false";
            } else {
                return std::to_string(val);
            }
        }, value);
    }
};

// ==================== CONFIGURATION SCHEMA ====================

struct ConfigurationSchema {
    std::string filename;                           // Configuration file name
    std::string description;                        // Schema description
    std::unordered_map<std::string, ConfigEntry> entries; // Key -> ConfigEntry mapping
    
    // Schema validation
    bool validate() const {
        for (const auto& pair : entries) {
            const auto& entry = pair.second;
            if (entry.required && !entry.validate()) {
                return false;
            }
        }
        return true;
    }
    
    // Add configuration entry to schema
    void addEntry(const ConfigEntry& entry) {
        entries[entry.key] = entry;
    }
    
    // Get entry by key
    const ConfigEntry* getEntry(const std::string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? &it->second : nullptr;
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
};

// ==================== ENHANCED CONFIGURATION CLASS ====================

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Schema management
    void setSchema(const ConfigurationSchema& schema) {
        currentSchema_ = schema;
    }
    
    const ConfigurationSchema& getSchema() const { return currentSchema_; }
    
    // Load configuration from file (JSON format)
    bool loadFromFile(const std::string& filepath, bool validateSchema = true);
    
    // Load configuration from YAML file
    bool loadFromYAML(const std::string& filepath, bool validateSchema = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, const std::vector<std::string>& allowedKeys = {});
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience methods for common configuration types
    bool getBool(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Type-specific convenience methods
    std::vector<int> getIntVector(const std::string& key) const;
    std::vector<double> getDoubleVector(const std::string& key) const;
    std::vector<std::string> getStringVector(const std::string& key) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration by source
    std::unordered_map<std::string, ConfigEntry> getBySource(ConfigSource source) const;
    
    // Validate configuration against schema
    std::vector<std::string> validateAgainstSchema() const;
    
    // Merge configuration from another config object
    void merge(const Config& other, ConfigSource source = ConfigSource::Override);
    
    // Reset to schema defaults
    void resetToDefaults();
    
    // Get current source of a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key is overridden (from higher priority sources)
    bool isOverridden(const std::string& key) const;
    
    // Get source priority
    static int getSourcePriority(ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ConfigurationSchema currentSchema_;
    
    // Internal helpers
    bool parseJSONFile(const std::string& filepath);
    bool parseYAMLFile(const std::string& filepath);
    std::string normalizeKey(const std::string& key) const;
    bool validateKey(const std::string& key) const;
    ConfigValue jsonToConfigValue(const Json::Value& jsonValue) const;
    Json::Value configValueToJSON(const ConfigValue& value) const;
    std::string configValueToString(const ConfigValue& value) const;
    static bool stringMatchesPattern(const std::string& value, const std::string& pattern);
    static std::string formatConfigValue(const ConfigValue& value);
};

// ==================== CONFIGURATION VALIDATION HELPERS ====================

// Validation functions
namespace Validation {
    bool isPositive(const ConfigValue& value);
    bool isNonNegative(const ConfigValue& value);
    bool isInRange(const ConfigValue& value, double min, double max);
    bool matchesPattern(const ConfigValue& value, const std::string& pattern);
    bool isStringLength(const ConfigValue& value, size_t minLen, size_t maxLen);
    bool isValidNeuronType(const ConfigValue& value);
    bool isValidSynapseType(const ConfigValue& value);
    bool isValidPlasticityRule(const ConfigValue& value);
    bool isValidDevelopmentalStage(const ConfigValue& value);
    bool isValidFiringState(const ConfigValue& value);
    bool isValidLogLevel(const ConfigValue& value);
}

// ==================== CONVENIENCE FUNCTIONS ====================

// Create configuration entry with validation
ConfigEntry createConfigEntry(const std::string& key, const ConfigValue& value,
                             ConfigSource source = ConfigSource::Default,
                             const std::string& description = "",
                             const std::string& category = "",
                             std::function<bool(const ConfigValue&)> validator = nullptr);

// Parse command line arguments with schema
bool parseCommandLineArgs(int argc, char** argv, const ConfigurationSchema& schema,
                         Config& config, bool validateUnknownKeys = false);

// Load environment variables
bool loadEnvironmentVariables(Config& config, const std::vector<std::string>& prefixList = {});

} // namespace nlm

// Include the original configuration header for backward compatibility
#include "core/Config/Config.hpp"