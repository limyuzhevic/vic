#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types - enhanced with proper type safety
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

// Configuration source - simplified
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime
};

// Enhanced configuration entry with better metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req) {}
    
    // Helper to check if this is the best source for a key
    bool isBetterSourceThan(const ConfigEntry& other) const {
        // Runtime overrides everything
        if (source == ConfigSource::Runtime) return true;
        if (other.source == ConfigSource::Runtime) return false;
        
        // Command line overrides file
        if (source == ConfigSource::CommandLine) return true;
        if (other.source == ConfigSource::CommandLine) return false;
        
        // File overrides environment
        if (source == ConfigSource::File) return true;
        if (other.source == ConfigSource::File) return false;
        
        // Environment overrides default
        if (source == ConfigSource::Environment) return true;
        
        return false; // Default is lowest priority
    }
};

// Configuration exceptions
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& message) : ConfigException(message) {}
};

// Type-safe configuration value getters with validation
class ConfigValueHelper {
public:
    // Convert ConfigValue to specific type with validation
    template<typename T>
    static T toType(const ConfigValue& value, const std::string& key = "") {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw ConfigValidationException("Type mismatch for configuration key: " + key);
        }
    }
    
    // Type checking
    template<typename T>
    static bool isType(const ConfigValue& value) {
        return std::holds_alternative<T>(value);
    }
    
    // Get value as string representation
    static std::string toString(const ConfigValue& value);
    
    // Validate value ranges
    static bool validateRange(const ConfigValue& value, double min, double max);
};

// Main configuration class - enhanced
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading methods
    bool load(const std::string& filepath = "", int argc = 0, char** argv = nullptr);
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment();
    
    // Backward compatibility methods
    bool loadFromFile(const std::string& filepath, bool silent = false);
    bool loadFromArgs(int argc, char** argv, bool silent = false);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced value getters with type safety and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Set values with enhanced API
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced setters with validation
    template<typename T>
    void setValidated(const std::string& key, T value, ConfigSource source, 
                     double min = -std::numeric_limits<double>::max(),
                     double max = std::numeric_limits<double>::max(),
                     const std::string& desc = "");
    
    // Check existence and get all keys
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear and reset
    void clear();
    void resetToDefaults();
    
    // Configuration management
    void define(const std::string& key, const ConfigValue& defaultValue, 
                const std::string& description = "", bool required = false);
    bool isDefined(const std::string& key) const;
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigSource source) const;
    
    // Environment variable helpers
    static std::string getEnvVar(const std::string& name, const std::string& defaultValue = "");
    static bool hasEnvVar(const std::string& name);
    
    // Argument parsing helpers
    static std::string parseArgs(int argc, char** argv, const std::string& key);
    static bool hasArg(int argc, char** argv, const std::string& key);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string normalizeKey(const std::string& key);
    
    // Merge configurations from different sources
    void mergeConfigurations(const std::vector<ConfigEntry>& entries);
    
    // Configuration validation
    void validateRequired() const;
};

// Convenience typedefs for common types
using IntConfig = int;
using FloatConfig = double;
using StringConfig = std::string;
using BoolConfig = bool;

} // namespace nlm
