#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <system_error>

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

// Error types for configuration operations
enum class ConfigErrorType {
    None,
    FileNotFound,
    PermissionDenied,
    JsonParseError,
    FileWriteError,
    InvalidKey,
    TypeMismatch,
    OutOfRange,
    ValidationError,
    InvalidInput,
    MemoryError
};

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message, ConfigErrorType type = ConfigErrorType::None)
        : std::runtime_error(message), errorType_(type) {}
    
    ConfigErrorType getType() const noexcept { return errorType_; }
    
private:
    ConfigErrorType errorType_;
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
    
    // Error handling methods
    std::string getLastError() const noexcept;
    void clearLastError();
    void enableErrorReporting(bool enable);
    
    // Validation methods
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    bool validateAgainstSchema() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string lastError_;
    bool errorReportingEnabled_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    void validateInput(const std::string& key, const ConfigValue& value) const;
    void logOperation(const std::string& operation, const std::string& key, 
                      bool success, const std::string& details = "");
};

} // namespace nlm
