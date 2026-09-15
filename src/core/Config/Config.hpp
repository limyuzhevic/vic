#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>
#include <filesystem>
#include <stdexcept>

// JSON and YAML library includes - conditionally compiled
#ifdef HAS_NLOHMANN_JSON
#include "nlohmann/json.hpp"
#endif

#ifdef HAS_YAML_CPP
#include "yaml-cpp/yaml.h"
#endif

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

// Advanced configuration value types for nested structures
struct ConfigObject {
    std::map<std::string, ConfigValue> fields;
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Merged
};

// Configuration validation error
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message) : std::runtime_error(message) {}
    ConfigError(const std::string& message, const std::string& key, const std::string& value)
        : std::runtime_error(message + " (key: " + key + ", value: " + value + ")") {}
};

// Configuration constraint for validation
struct ConfigConstraint {
    std::string type;  // "range", "enum", "regex", "length"
    std::any min;
    std::any max;
    std::vector<std::any> allowedValues;
    std::string pattern;
    int minLength;
    int maxLength;
};

// Schema for configuration structure validation
struct ConfigSchema {
    std::map<std::string, ConfigConstraint> constraints;
    bool requireAll = false;
    bool allowExtra = true;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Type-safe configuration access template helpers
namespace config_detail {
    template<typename T>
    struct TypeTag {};
    
    template<typename T>
    T getValueFromVariant(const ConfigValue& value);
    
    template<typename T>
    bool setValueInVariant(ConfigValue& value, const T& val);
    
    // Type detection for variant
    template<typename T>
    struct TypeInfo {
        static bool isType(const ConfigValue& v) {
            return std::holds_alternative<T>(v);
        }
    };
    
    template<typename T>
    struct TypeInfo<std::vector<T>> {
        static bool isType(const ConfigValue& v) {
            if (!std::holds_alternative<std::vector<T>>(v)) return false;
            return true;
        }
    };
}

// Advanced Configuration class with full features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/simple format)
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML format)
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Type-safe getters
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe setters
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation helpers
    bool validate(const ConfigSchema& schema) const;
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
    // Configuration management
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Merged);
    
    // Default value management
    template<typename T>
    void setDefault(const std::string& key, const T& value);
    
    // Validation constraint management
    template<typename T>
    void addConstraint(const std::string& key, const ConfigConstraint& constraint);
    
    // Schema validation
    bool validateSchema(const ConfigSchema& schema) const;
    
    // Advanced get methods for nested objects
    ConfigObject getObject(const std::string& key) const;
    ConfigValue getValue(const std::string& key) const;
    
    // File format detection and parsing
    static std::string detectFormat(const std::string& filename);
    static std::string detectFormatFromContent(const std::string& content);
    
    // JSON/YAML parsing (implementations)
#ifdef HAS_NLOHMANN_JSON
    static nlohmann::json parseJSON(const std::string& content);
#endif
#ifdef HAS_YAML_CPP
    static YAML::Node parseYAML(const std::string& content);
#endif
    
    // Conversion utilities
    static ConfigValue toConfigValue(const std::any& value);
    static std::any fromConfigValue(const ConfigValue& value);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBoolean(const std::string& str);
    
    // Exception handling
    void setError(const std::string& error) { lastError = error; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // For now, disable validation features until Config.cpp implementation is updated
    mutable std::string lastError;
    std::map<std::string, ConfigConstraint> constraints;
    std::map<std::string, ConfigValue> defaults;
    
    // Internal parsing methods
    bool parseSimpleFormat(const std::string& content);
    bool parseJSONFormat(const std::string& content);
    bool parseYAMLFormat(const std::string& content);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) const;
};

// Simple config function for backward compatibility
Config& getDefaultConfig();

} // namespace nlm

// Explicit template instantiations for common types
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    
    template void Config::set<int>(const std::string&, const int&, ConfigSource);
    template void Config::set<double>(const std::string&, const double&, ConfigSource);
    template void Config::set<bool>(const std::string&, const bool&, ConfigSource);
    template void Config::set<std::string>(const std::string&, const std::string&, ConfigSource);
    
    template void Config::setDefault<int>(const std::string&, const int&);
    template void Config::setDefault<double>(const std::string&, const double&);
    template void Config::setDefault<bool>(const std::string&, const bool&);
    template void Config::setDefault<std::string>(const std::string&, const std::string&);
    
    template void Config::addConstraint<int>(const std::string&, const ConfigConstraint&);
    template void Config::addConstraint<double>(const std::string&, const ConfigConstraint&);
    template void Config::addConstraint<bool>(const std::string&, const ConfigConstraint&);
    template void Config::addConstraint<std::string>(const std::string&, const ConfigConstraint&);
}
