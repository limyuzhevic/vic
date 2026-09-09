#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <regex>
#include <sstream>

namespace nlm {

// Custom exceptions for configuration errors
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigLoadError : public std::runtime_error {
public:
    explicit ConfigLoadError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigSaveError : public std::runtime_error {
public:
    explicit ConfigSaveError(const std::string& msg) : std::runtime_error(msg) {}
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
    
    // Load from file (JSON/YAML/toml format)
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
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Helper convenience methods
    void addInt(const std::string& key, int value, const std::string& description = "");
    void addDouble(const std::string& key, double value, const std::string& description = "");
    void addBool(const std::string& key, bool value, const std::string& description = "");
    void addString(const std::string& key, const std::string& value, const std::string& description = "");
    
    int getInt(const std::string& key, int defaultValue = 0, const std::string& description = "") const;
    double getDouble(const std::string& key, double defaultValue = 0.0, const std::string& description = "") const;
    bool getBool(const std::string& key, bool defaultValue = false, const std::string& description = "") const;
    std::string getString(const std::string& key, const std::string& defaultValue = "", const std::string& description = "") const;
    
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
    
    // Validation methods
    static bool isValidKey(const std::string& key);
    static bool isValidPath(const std::string& path);
    static std::string validateAndNormalizeKey(const std::string& key);
    
    // Error handling
    static std::string getLastError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    static std::string lastError;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static bool isAlphaNumericOrUnderscore(char c);
    static bool isDigit(char c);
    static std::string intToString(int value);
    static std::string doubleToString(double value);
    
    // Value validation
    static void validateValue(const ConfigValue& value);
};

} // namespace nlm
