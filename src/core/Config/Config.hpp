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

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

// Schema entry for configuration validation
struct ConfigSchemaEntry {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> errorFormatter;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    std::string typeName;
    
    ConfigSchemaEntry(const std::string& k, std::function<bool(const ConfigValue&)> val, 
                     const std::string& desc = "", bool req = false, ConfigValue def = ConfigValue())
        : key(k), validator(val), errorFormatter(nullptr), description(desc), required(req), defaultValue(def), typeName("unknown") {}
    
    ConfigSchemaEntry(const std::string& k, std::function<bool(const ConfigValue&)> val,
                     std::function<std::string(const ConfigValue&)> err,
                     const std::string& desc = "", bool req = false, ConfigValue def = ConfigValue())
        : key(k), validator(val), errorFormatter(err), description(desc), required(req), defaultValue(def), typeName("unknown") {}
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
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate() const;
    bool validateEntry(const ConfigEntry& entry) const;
    void setValidator(std::shared_ptr<ConfigValidator> validator);
    
    // Configuration schema
    void addSchemaEntry(const ConfigSchemaEntry& schemaEntry);
    bool validateAgainstSchema() const;
    
    // Configuration presets
    static std::map<std::string, std::vector<ConfigEntry>> getPresets();
    bool applyPreset(const std::string& presetName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getCurrentTimestamp();
};

} // namespace nlm
