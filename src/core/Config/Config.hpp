#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>
#include <any>
#include <functional>
#include <stdexcept>

namespace nlm {

// Configuration value types
struct ConfigInt {
    int value;
    std::string description;
    ConfigInt(int v, const std::string& desc = "") : value(v), description(desc) {}
};

struct ConfigDouble {
    double value;
    std::string description;
    ConfigDouble(double v, const std::string& desc = "") : value(v), description(desc) {}
};

struct ConfigString {
    std::string value;
    std::string description;
    ConfigString(const std::string& v, const std::string& desc = "") : value(v), description(desc) {}
};

struct ConfigBool {
    bool value;
    std::string description;
    ConfigBool(bool v, const std::string& desc = "") : value(v), description(desc) {}
};

struct ConfigVectorInt {
    std::vector<int> value;
    std::string description;
    ConfigVectorInt(const std::vector<int>& v, const std::string& desc = "") : value(v), description(desc) {}
};

struct ConfigVectorDouble {
    std::vector<double> value;
    std::string description;
    ConfigVectorDouble(const std::vector<double>& v, const std::string& desc = "") : value(v), description(desc) {}
};

// Type-safe configuration value union
using ConfigValue = std::variant<
    ConfigInt,
    ConfigDouble,
    ConfigString,
    ConfigBool,
    ConfigVectorInt,
    ConfigVectorDouble
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation
class ConfigValidator {
public:
    template<typename T>
    static bool validate(const ConfigValue& value) {
        return std::holds_alternative<T>(value);
    }
    
    template<typename T>
    static T* getValue(ConfigValue& value) {
        if (std::holds_alternative<T>(value)) {
            return &std::get<T>(value);
        }
        return nullptr;
    }
    
    template<typename T>
    static const T* getValue(const ConfigValue& value) {
        if (std::holds_alternative<T>(value)) {
            return &std::get<T>(value);
        }
        return nullptr;
    }
};

// Configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", std::function<bool(const ConfigValue&)> val = nullptr)
        : key(k), value(v), source(s), description(desc), validator(val) {}
    
    bool isValid() const {
        return validator ? validator(value) : true;
    }
};

// Type-safe configuration accessor
class ConfigAccessor {
public:
    // Template getter with type checking
    template<typename T>
    static T get(const std::unordered_map<std::string, ConfigEntry>& config, const std::string& key, 
                 const T& defaultValue = T()) {
        auto it = config.find(key);
        if (it != config.end()) {
            const ConfigEntry& entry = it->second;
            if (entry.isValid() && std::holds_alternative<T>(entry.value)) {
                return std::get<T>(entry.value);
            }
        }
        return defaultValue;
    }
    
    template<typename T>
    static std::optional<T> getOptional(const std::unordered_map<std::string, ConfigEntry>& config, 
                                       const std::string& key) {
        auto it = config.find(key);
        if (it != config.end()) {
            const ConfigEntry& entry = it->second;
            if (entry.isValid() && std::holds_alternative<T>(entry.value)) {
                return std::get<T>(entry.value);
            }
        }
        return std::nullopt;
    }
};

// Main configuration class with improved design
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set default values with type safety
    void setDefault(const std::string& key, int value, const std::string& description = "");
    void setDefault(const std::string& key, double value, const std::string& description = "");
    void setDefault(const std::string& key, const std::string& value, const std::string& description = "");
    void setDefault(const std::string& key, bool value, const std::string& description = "");
    void setDefault(const std::string& key, const std::vector<int>& value, const std::string& description = "");
    void setDefault(const std::string& key, const std::vector<double>& value, const std::string& description = "");
    
    // Override with validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, 
                     std::function<bool(const ConfigValue&)> validator, 
                     ConfigSource source = ConfigSource::Runtime);
    
    // Set values
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    template<typename T>
    std::optional<T> getOptional(const std::string& key) const;
    
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
    
    // Get all entries (for inspection)
    const std::unordered_map<std::string, ConfigEntry>& getAllEntries() const;
    
    // Validate all entries
    std::vector<std::string> validateAll() const;
    
    // Configuration value getters with type safety
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::vector<int>> getVectorInt(const std::string& key) const;
    std::optional<std::vector<double>> getVectorDouble(const std::string& key) const;
    
    // Configuration value setters with type safety
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setVectorInt(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void setVectorDouble(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe getters with defaults
    int getOrInt(const std::string& key, int defaultValue) const;
    double getOrDouble(const std::string& key, double defaultValue) const;
    std::string getOrString(const std::string& key, const std::string& defaultValue) const;
    bool getOrBool(const std::string& key, bool defaultValue) const;
    std::vector<int> getOrVectorInt(const std::string& key, const std::vector<int>& defaultValue) const;
    std::vector<double> getOrVectorDouble(const std::string& key, const std::vector<double>& defaultValue) const;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Advanced configuration utilities
    std::string dumpJSON() const;
    bool validateAll() const;
    static Config loadFromDefaults(const std::string& filepath = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
