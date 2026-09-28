#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>

// Try to include nlohmann/json, but handle gracefully if not available
try {
    #include "nlohmann/json.hpp"
} catch (...) {
    // If nlohmann/json is not available, define a minimal stub
    namespace nlohmann {
        class json {};
        class json_exception : public std::exception {
        public:
            const char* what() const noexcept override { return "nlohmann/json not available"; }
        };
        class json_overloader {};
        class parse_error : public json_exception {};
    }
}

namespace nlm {

// Forward declarations
class Config;
class JSONConfig;

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
    Runtime,
    JSON
};

// Configuration format detection
enum class ConfigFormat {
    KeyValue,
    JSON,
    YAML,
    Unknown
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeHint;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), typeHint(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& type = "",
                bool req = false)
        : key(k), value(v), source(s), description(desc), typeHint(type), required(req) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::unordered_map<std::string, std::string> types;
    std::unordered_map<std::string, bool> required;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> defaultValues;
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    enum class ErrorCode {
        FileNotFound,
        ParseError,
        TypeError,
        ValidationError,
        SchemaError,
        ConversionError
    };
    
    ConfigException(ErrorCode code, const std::string& message)
        : std::runtime_error(message), errorCode(code) {}
    
    ErrorCode getErrorCode() const { return errorCode; }
    
private:
    ErrorCode errorCode;
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm