#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <utility>

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
    std::vector<std::string>,
    std::vector<bool>
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

// Custom exceptions
class ConfigParseError : public std::runtime_error {
public:
    explicit ConfigParseError(const std::string& message) 
        : std::runtime_error(message) {}
};

class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message) 
        : std::runtime_error(message) {}
};

class ConfigFileError : public std::runtime_error {
public:
    explicit ConfigFileError(const std::string& message) 
        : std::runtime_error(message) {}
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
    
    // Load from file (JSON/YAML/simple format)
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
    
    // Configuration validation
    void validateTypes() const;
    void validateRequiredKeys() const;
    void validateConstraints() const;
    void validateStructure() const;
    
    // Convenience validation method
    void validateAll() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Add validation rules (for testing/setup)
    void addTypeValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    void addRangeValidator(const std::string& key, double min, double max);
    void addArrayBoundsValidator(const std::string& key, size_t minSize, size_t maxSize);
    void addRequiredKey(const std::string& key);
    void addStructureValidator(const std::string& key, const std::string& structureType);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    static bool validateType(const ConfigValue& value, const std::string& expectedType) const;
    static bool validateValueRange(const ConfigValue& value, double min, double max) const;
    static bool validateArrayBounds(const ConfigValue& value, size_t minSize, size_t maxSize) const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing methods
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromSimpleFormat(const std::string& filepath);
    
    void parseJSONValue(const json& j, const std::string& prefix);
    void parseYAMLNode(const yaml& node, const std::string& prefix);
    void parseJSONArray(const json& j, const std::string& key);
    void parseYAMLSequence(const yaml& node, const std::string& key);
    
    ConfigValue convertJSONValue(const json& j);
    ConfigValue convertYAMLValue(const yaml& node);
    
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToSimpleFormat(const std::string& filepath) const;
    
    void ConfigValueToJSON(json& j, const ConfigValue& value);
    
    ConfigValue convertFromJSON(const json& j);
};

} // namespace nlm