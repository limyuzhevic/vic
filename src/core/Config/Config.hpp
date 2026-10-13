#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description()
        , validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc, std::function<bool(const ConfigValue&)> val)
        : key(k), value(v), source(s), description(desc), validator(val) {}
    
    bool isValid() const {
        return !validator || validator(value);
    }
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&)>;

// Enhanced configuration manager with validation, callbacks, and schema support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/TOML support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON/YAML/TOML parsing)
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format support
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation and callbacks
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Validation and schema
    bool validate() const;
    void addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    void addSchema(const std::string& key, const std::string& type, const std::string& description);
    
    // Configuration change management
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
    // Check existence and metadata
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries for debugging
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // File format support
    enum class FileFormat {
        Auto,
        JSON,
        YAML,
        TOML,
        INI
    };
    
    // Validation error information
    struct ValidationError {
        std::string key;
        std::string message;
        std::string type;
        
        ValidationError(const std::string& k, const std::string& msg, const std::string& t = "validation")
            : key(k), message(msg), type(t) {}
    };
    
    // Get validation errors
    std::vector<ValidationError> getValidationErrors() const;
    
    // Configuration presets
    static Config createDefault();
    static Config createMinimal();
    static Config createResearch();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string serializeValue(const ConfigValue& value);
    static ConfigValue parseValue(const std::string& str, const std::string& key);
    
    // Change notification
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Schema validation
    bool validateAgainstSchema() const;
};
