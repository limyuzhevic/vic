#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
#include <map>
#include <functional>
#include <regex>

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

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    ConfigSource source;
    
    ValidationError(const std::string& k, const std::string& msg, ConfigSource s)
        : key(k), message(msg), source(s) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string description;
    bool required;
    std::string defaultValue;
    
    ConfigSchemaEntry(const std::string& k, const std::function<bool(const ConfigValue&)>& val,
                     const std::string& desc, bool req = false, const std::string& def = "")
        : key(k), validator(val), description(desc), required(req), defaultValue(def) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
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
    
    // Validation methods
    bool validate() const;
    std::vector<ValidationError> getValidationErrors() const;
    
    // Schema methods
    void setSchema(const std::vector<ConfigSchemaEntry>& schema);
    bool hasSchema() const;
    bool validateAgainstSchema() const;
    
    // Configuration inheritance
    void setParent(const std::shared_ptr<Config>& parent);
    void clearParent();
    
    // Configuration override
    bool hasOverride(const std::string& key) const;
    ConfigSource getOverrideSource(const std::string& key) const;
    
    // Configuration reset
    void resetToDefaults();
    
    // Configuration info
    size_t getEntryCount() const;
    bool isEmpty() const;
    
    // File operations
    static bool fileExists(const std::string& filepath);
    static std::string getConfigDirectory();
    static std::string getUserConfigPath(const std::string& filename);
    
    // Statistics
    size_t getStatsEntryCount() const;
    std::string getStatsSummary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& value);
};

} // namespace nlm
