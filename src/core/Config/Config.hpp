#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <filesystem>

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

// Main configuration class with enhanced functionality
class Config {
public:
    // Configuration validation result
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        
        ValidationResult(bool valid, const std::string& error = "") 
            : isValid(valid), errorMessage(error) {}
    };
    
    // Configuration schema entry for validation
    struct SchemaEntry {
        std::string key;
        std::type_index type;
        ConfigSource allowedSources;
        bool required;
        ConfigValue defaultValue;
        std::string description;
        
        SchemaEntry(const std::string& k, const std::type_index& t, ConfigSource sources = ConfigSource::File,
                   bool req = false, const ConfigValue& defaultVal = {}, const std::string& desc = "")
            : key(k), type(t), allowedSources(sources), required(req), 
              defaultValue(defaultVal), description(desc) {}
    };
    
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file (JSON or YAML)
    bool loadFromFile(const std::string& filepath, const std::string& format);
    
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
    
    // Schema management
    void addSchema(const SchemaEntry& schema);
    ValidationResult validate() const;
    bool hasSchema(const std::string& key) const;
    
    // Schema information
    std::vector<std::string> getSchemaKeys() const;
    std::string getSchemaInfo(const std::string& key) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
};

} // namespace nlm
