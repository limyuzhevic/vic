#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

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

// Configuration entry with section support
struct ConfigEntry {
    std::string section;
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : section(), key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& s, const std::string& k, const ConfigValue& v, ConfigSource src, const std::string& desc = "")
        : section(s), key(k), value(v), source(src), description(desc) {}
    
    // For backward compatibility
    std::string getFullKey() const {
        if (section.empty()) return key;
        return section + "." + key;
    }
};

// Configuration parameter validation
struct ConfigParameter {
    std::string section;
    std::string key;
    std::string description;
    std::string defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    
    ConfigParameter(const std::string& s, const std::string& k, const std::string& desc,
                   const std::string& def = "", bool req = false)
        : section(s), key(k), description(desc), defaultValue(def), required(req), validator(nullptr) {}
};

// Main configuration class
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
    
    // Get values with section support
    template<typename T>
    std::optional<T> get(const std::string& section, const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& section, const std::string& key, const T& defaultValue) const;
    
    // Backward compatibility
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return getOr("", key, defaultValue);
    }
    
    // Set values
    void set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        set("", key, value, source);
    }
    
    void set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set("", key, value, source);
    }
    
    void set(const std::string& section, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set("", key, value, source);
    }
    
    void set(const std::string& section, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set("", key, value, source);
    }
    
    void set(const std::string& section, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set("", key, value, source);
    }
    
    // Check existence
    bool has(const std::string& section, const std::string& key) const;
    bool has(const std::string& key) const {
        return has("", key);
    }
    
    // Remove key
    void remove(const std::string& section, const std::string& key);
    void remove(const std::string& key) {
        remove("", key);
    }
    
    // Get all keys, optionally filtered by section
    std::vector<std::string> getKeys(const std::string& section = "") const;
    
    // Get all sections
    std::vector<std::string> getSections() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Parameter validation
    bool validateParameter(const ConfigParameter& param) const;
    bool validateAllParameters(const std::vector<ConfigParameter>& parameters) const;
    
    // Configuration file path management
    void setDefaultConfigFile(const std::string& filepath);
    std::string getDefaultConfigFile() const;
    
    // Type conversion utilities
    template<typename T>
    static std::string valueToString(const ConfigValue& value);
    
    template<typename T>
    static std::optional<T> stringToValue(const std::string& str);
    
    // Error reporting
    std::vector<std::string> getErrors() const;
    void clearErrors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValuePair(const std::string& line, std::string& section, std::string& key, std::string& value);
    static ConfigValue parseValue(const std::string& valueStr);
    
    // Error handling
    void addError(const std::string& error);
};

} // namespace nlm
