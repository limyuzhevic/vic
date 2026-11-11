#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// Configuration error types
enum class ConfigError {
    Success = 0,
    FileNotFound,
    ParseError,
    InvalidKey,
    InvalidValue,
    InvalidFormat,
    PermissionDenied,
    OutOfMemory,
    ValidationError
};

// Exception for configuration errors
class ConfigException : public std::runtime_error {
public:
    ConfigException(ConfigError error, const std::string& message)
        : std::runtime_error(message), error_(error) {}
    
    ConfigError getError() const { return error_; }
    
private:
    ConfigError error_;
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
    mutable bool valid;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), valid(true) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), valid(true) {}
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
    
    // Load from file (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, 
                      ConfigSource source = ConfigSource::Runtime,
                      std::function<bool(const T&)> validator = nullptr);
    
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
    
    // Print usage/help for configuration
    static void printUsage();
    
    // Validate configuration values
    bool validate() const;
    
    // Get configuration value with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key, const T& defaultValue, 
                                  std::function<bool(const T&)> validator = nullptr) const;
    
    // Error handling utilities
    static std::string errorToString(ConfigError error);
    static ConfigError detectFileFormatError(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);
    static std::string typeToString(const ConfigValue& value);
    
    // File format detection
    enum class FileFormat {
        Unknown,
        Simple,
        JSON,
        YAML
    };
    
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // File format parsers (optional)
#ifdef HAVE_NLOHMANN_JSON
    bool parseJSONFile(const std::string& filepath);
#endif
#ifdef HAVE_YAML_CPP
    bool parseYAMLFile(const std::string& filepath);
#endif
    
    // Simple file parser
    bool parseSimpleFile(const std::string& filepath);
};

} // namespace nlm
