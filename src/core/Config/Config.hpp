#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <any>
#include <functional>
#include <filesystem>
#include <chrono>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Exception for configuration errors
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& msg) : ConfigError(msg) {}
};

// Configuration value types - expanded to support nested structures
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<ConfigValue>,  // Vector of variant values for complex arrays
    std::unordered_map<std::string, ConfigValue>,  // Nested configuration (objects)
    std::vector<std::unordered_map<std::string, ConfigValue>>  // Array of objects
>;

// Configuration source with enhanced tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Schema
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::optional<ConfigValue> defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), 
                   required(false), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "",
                const std::optional<ConfigValue>& def = std::nullopt,
                const std::function<bool(const ConfigValue&)>& val = nullptr,
                bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), 
          defaultValue(def), validator(val), required(req) {}
};

// Parser strategy interface (Strategy pattern)
class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    virtual std::unordered_map<std::string, ConfigValue> parse(const std::string& content) = 0;
    virtual std::string serialize(const std::unordered_map<std::string, ConfigValue>& config) = 0;
    virtual bool canParse(const std::string& filename) const = 0;
};

// JSON parser implementation
class JsonConfigParser : public ConfigParser {
public:
    std::unordered_map<std::string, ConfigValue> parse(const std::string& content) override;
    std::string serialize(const std::unordered_map<std::string, ConfigValue>& config) override;
    bool canParse(const std::string& filename) const override;
};

// YAML parser implementation  
class YamlConfigParser : public ConfigParser {
public:
    std::unordered_map<std::string, ConfigValue> parse(const std::string& content) override;
    std::string serialize(const std::unordered_map<std::string, ConfigValue>& config) override;
    bool canParse(const std::string& filename) const override;
};

// Key-value parser for backward compatibility
class KeyValueConfigParser : public ConfigParser {
public:
    std::unordered_map<std::string, ConfigValue> parse(const std::string& content) override;
    std::string serialize(const std::unordered_map<std::string, ConfigValue>& config) override;
    bool canParse(const std::string& filename) const override;
};

// Configuration schema for validation
struct ConfigSchema {
    std::unordered_map<std::string, ConfigEntry> entries;
    std::string description;
    std::chrono::system_clock::time_point created;
    std::string version;
    
    ConfigSchema() : created(std::chrono::system_clock::now()), version("1.0") {}
    
    void addEntry(const ConfigEntry& entry);
    bool validate(const std::string& key, const ConfigValue& value) const;
    bool validate(const std::unordered_map<std::string, ConfigValue>& config) const;
    std::vector<std::string> getMissingRequired() const;
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectFileFormat(const std::string& filepath);
};

// Helper functions (declarations)
std::string configValueToString(const ConfigValue& value);

} // namespace nlm
