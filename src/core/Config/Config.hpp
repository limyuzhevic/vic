#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// JSON support
namespace json = nlohmann;

// YAML support
namespace yaml = YAML;

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

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Exception types
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException(message) {}
};

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

// Nested config path
using ConfigPath = std::vector<std::string>;

// Validator function type
using ValidatorFunc = std::function<bool(const ConfigValue&)>;

// Schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type;
    ConfigValue defaultValue;
    std::string description;
    ValidatorFunc validator;
    bool required;
    std::string units;
    std::string range;
    
    ConfigSchemaEntry() : key(), type(), defaultValue(), description(), validator(nullptr), required(false) {}
    ConfigSchemaEntry(const std::string& k, const std::string& t, const ConfigValue& def, const std::string& desc = "",
                     const ValidatorFunc& val = nullptr, bool req = false, const std::string& unitsStr = "", 
                     const std::string& rangeStr = "")
        : key(k), type(t), defaultValue(def), description(desc), validator(val), required(req), units(unitsStr), range(rangeStr) {}
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
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> get(const ConfigPath& path) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const ConfigPath& path, const T& defaultValue) const;
    
    // Get string representation of nested value (dot notation)
    template<typename T>
    std::optional<T> get(const std::string& dotNotationPath) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Nested set operations
    void set(const ConfigPath& path, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    bool has(const ConfigPath& path) const;
    bool has(const std::string& dotNotationPath) const;
    
    // Remove key
    void remove(const std::string& key);
    void remove(const ConfigPath& path);
    void remove(const std::string& dotNotationPath);
    
    // Get all keys (flat)
    std::vector<std::string> getKeys() const;
    
    // Get all nested keys
    std::vector<std::string> getAllKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate() const;
    void addValidator(const std::string& key, const ValidatorFunc& validator);
    
    // Schema support
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    bool hasSchema() const;
    bool validateSchema() const;
    
    // Default value system
    void setDefault(const std::string& key, const ConfigValue& value);
    void setDefault(const ConfigPath& path, const ConfigValue& value);
    void setDefault(const std::string& dotNotationPath, const ConfigValue& value);
    
    // Config merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigPath parsePath(const std::string& dotNotationPath);
    static std::string pathToString(const ConfigPath& path);
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
