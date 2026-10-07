#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <any>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Configuration exceptions
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath) 
        : ConfigException("Configuration file not found: " + filepath) {}
};

class ConfigInvalidFormatException : public ConfigException {
public:
    explicit ConfigInvalidFormatException(const std::string& message) 
        : ConfigException("Invalid configuration format: " + message) {}
};

class ConfigSchemaValidationException : public ConfigException {
public:
    explicit ConfigSchemaValidationException(const std::string& message) 
        : ConfigException("Configuration schema validation failed: " + message) {}
};

class ConfigValueErrorException : public ConfigException {
public:
    explicit ConfigValueErrorException(const std::string& message) 
        : ConfigException("Configuration value error: " + message) {}
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

using ConfigSchemaValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, ConfigValue>,
    std::map<std::string, std::any>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Inherited
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

// Schema validation types
struct ConfigSchemaEntry {
    std::string key;
    ConfigSchemaValue type;
    bool required;
    std::string description;
    std::any defaultValue;
    std::optional<std::pair<double, double>> range;
};

struct ConfigSchema {
    std::vector<ConfigSchemaEntry> entries;
    std::string name;
    std::string version;
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
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
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
    
    // Schema validation
    bool validateSchema(const ConfigSchema& schema) const;
    
    // Schema building helpers
    static ConfigSchema buildSchema(const std::vector<ConfigSchemaEntry>& entries);
    
    // Nested configuration support
    std::shared_ptr<Config> getNested(const std::string& key) const;
    void setNested(const std::string& key, const std::shared_ptr<Config>& nestedConfig);
    
    // Environment-specific configuration
    void loadEnvironmentConfig(const std::string& environment);
    
    // Configuration inheritance
    void addParentConfig(const std::shared_ptr<Config>& parentConfig);
    
    // Hot-reload support
    bool reloadFromFile(const std::string& filepath);
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Inherited);
    
    // Configuration sources management
    void setEnvironmentVariable(const std::string& key, const std::string& value);
    void loadFromEnvironment();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing helpers
    static ConfigValue parseJsonValue(const std::string& valueStr);
    static std::string serializeToJson(const ConfigValue& value) const;
    static ConfigValue parseYamlValue(const std::string& valueStr);
    static std::string serializeToYaml(const ConfigValue& value) const;
    static std::map<std::string, std::string> parseYamlObject(const std::string& yamlStr);
};

// Configuration manager class
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept;
    
    // Single instance access
    static ConfigurationManager& getInstance();
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool reloadFromFile(const std::string& filepath);
    
    // Environment management
    void setEnvironment(const std::string& environment);
    std::string getEnvironment() const;
    
    // Configuration access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value);
    
    // Configuration validation
    bool validateSchema(const ConfigSchema& schema) const;
    
    // Configuration summary
    std::string summary() const;
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    
    // Configuration management
    void clear();
    std::vector<std::string> getKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
