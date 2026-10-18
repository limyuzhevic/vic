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
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
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
    
    // Validation support
    bool validate(const ConfigValue& min, const ConfigValue& max) const {
        return validateAgainstBounds(value, min, max);
    }
    
    static bool validateAgainstBounds(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max);
};

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    ConfigSource allowedSources;
    std::function<bool(const ConfigValue&)> validator;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    ConfigValue minValue;
    ConfigValue maxValue;
};

// Configuration exceptions
class ConfigException : public std::runtime_error {
public:
    enum class Type { LoadError, ValidationError, ConversionError, KeyError };
    
    ConfigException(const std::string& message, Type type = Type::LoadError)
        : std::runtime_error(message), type_(type) {}
    
    Type getType() const { return type_; }
    
private:
    Type type_;
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
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-specific setters with validation
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration schema management
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    bool validateSchema() const;
    bool validateAllEntries() const;
    std::string getSchemaErrors() const;
    
    // Configuration type conversion
    template<typename T>
    static std::optional<T> convertConfigValue(const ConfigValue& value);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Copy configuration
    Config copy() const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static std::string boolToString(bool value);
};

} // namespace nlm
