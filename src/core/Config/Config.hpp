#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include <nlohmann/json.hpp>

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

// Configuration error
class ConfigError : public std::runtime_error {
public:
    enum class ErrorCode {
        ParseError,
        ValidationError,
        FileError,
        SchemaError,
        TypeError
    };
    
    ConfigError(ErrorCode code, const std::string& message)
        : std::runtime_error(message), code(code), message(message) {}
    
    ErrorCode getCode() const { return code; }
    const std::string& getMessage() const { return message; }
    
private:
    ErrorCode code;
    std::string message;
};

// Configuration schema
struct ConfigSchema {
    std::string key;
    std::string type;
    bool required;
    std::string description;
    std::map<std::string, std::string> constraints;
    
    ConfigSchema(const std::string& k, const std::string& t, bool req = false, 
                 const std::string& desc = "", const std::map<std::string, std::string>& cons = {})
        : key(k), type(t), required(req), description(desc), constraints(cons) {}
};

// Configuration diff entry
struct ConfigDiffEntry {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string description;
    double diffScore;
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
    
    // Load from file (JSON/YAML format with backward compatibility)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr, bool validate = true);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
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
    
    // Configuration schema validation
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    
    // Configuration type validation
    bool validateType(const std::string& key, const std::string& expectedType) const;
    
    // Configuration parsing error handling
    static ConfigError parseError(ConfigError::ErrorCode code, const std::string& message);
    
    // Experiment configuration support
    bool saveExperimentConfig(const std::string& filepath, const std::string& experimentName) const;
    bool loadExperimentConfig(const std::string& filepath, const std::string& experimentName);
    
    // Configuration diffing for debugging
    std::vector<ConfigDiffEntry> diff(const Config& other) const;
    static std::string diffToString(const std::vector<ConfigDiffEntry>& diff);
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t uniqueSources;
        std::map<ConfigSource, size_t> sourceCounts;
        std::map<std::string, size_t> typeCounts;
        double avgEntriesPerSource;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJSON(const ConfigValue& value);
    static std::string toYAML(const ConfigValue& value);
    static ConfigValue fromJSON(const nlohmann::json& j);
    static ConfigValue fromYAML(const std::string& yamlStr);
    
    // File format detection
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    
    // Error handling
    static bool handleParseError(const std::string& error, ConfigError::ErrorCode code = ConfigError::ErrorCode::ParseError);
};

} // namespace nlm

// Explicit template instantiations namespace nlm {
