#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

namespace nlm {

// Error codes for configuration operations
enum class ConfigErrorCode {
    Success = 0,
    FileNotFound = 1,
    PermissionDenied = 2,
    InvalidFormat = 3,
    KeyNotFound = 4,
    InvalidType = 5,
    DuplicateKey = 6,
    OutOfRange = 7,
    ValidationFailed = 8
};

// Result type for configuration operations
class ConfigResult {
public:
    ConfigResult() : code(ConfigErrorCode::Success), message() {}
    ConfigResult(ConfigErrorCode c, const std::string& m) : code(c), message(m) {}
    
    bool success() const { return code == ConfigErrorCode::Success; }
    
    ConfigErrorCode code;
    std::string message;
};

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
    Environment,
    Runtime
};

// Configuration constants
namespace ConfigConstants {
    // Default file paths
    const std::string DEFAULT_CONFIG_FILE = "config.ini";
    const std::string DEFAULT_JSON_CONFIG_FILE = "config.json";
    
    // Environment variable prefix
    const std::string ENV_PREFIX = "APP_";
    
    // Key validation
    const size_t MIN_KEY_LENGTH = 1;
    const size_t MAX_KEY_LENGTH = 100;
    
    // Value limits
    const int MAX_INT_VALUE = INT_MAX;
    const int MIN_INT_VALUE = INT_MIN;
    const double MAX_DOUBLE_VALUE = 1e300;
    const double MIN_DOUBLE_VALUE = -1e300;
    const size_t MAX_STRING_LENGTH = 1000;
}

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
    ConfigResult loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    ConfigResult loadFromJsonFile(const std::string& filepath);
    
    // Load from INI file
    ConfigResult loadFromIniFile(const std::string& filepath);
    
    // Load from environment variables
    ConfigResult loadFromEnv(const std::string& prefix = ConfigConstants::ENV_PREFIX);
    
    // Load from command line arguments
    ConfigResult loadFromArgs(int argc, char** argv);
    
    // Save to file
    ConfigResult saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    ConfigResult saveToJsonFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    ConfigResult get(const std::string& key, T& outValue) const;
    
    // Environment variable support
    template<typename T>
    T getEnv(const std::string& key, const T& defaultValue) const;
    
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
};

} // namespace nlm
