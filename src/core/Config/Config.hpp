#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
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

// Configuration error types
enum class ConfigErrorType {
    None,
    FileNotFound,
    ParseError,
    InvalidFormat,
    InvalidValue,
    DuplicateKey,
    OutOfRange,
    MissingRequiredKey
};

// Configuration exception class
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message, ConfigErrorType type = ConfigErrorType::None)
        : std::runtime_error(message), errorType(type), message(message) {}
    
    ConfigErrorType getErrorType() const { return errorType; }
    const std::string& getMessage() const { return message; }
    
private:
    ConfigErrorType errorType;
    std::string message;
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
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Error handling
    bool hasError() const { return !lastErrorMessage.empty(); }
    std::string getLastError() const { return lastErrorMessage; }
    void clearError() { lastErrorMessage.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string lastErrorMessage;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper functions for file format detection
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool containsJSONBraces(const std::string& content);
    static bool containsYAMLDash(const std::string& content);
    
    // JSON/YAML parsing
    bool loadFromJSON(const std::string& content, ConfigSource source);
    bool loadFromYAML(const std::string& content, ConfigSource source);
    bool loadFromSimpleFormat(const std::string& content, ConfigSource source);
    
    // Error reporting
    void setError(const std::string& message, ConfigErrorType type = ConfigErrorType::None);
};

} // namespace nlm
