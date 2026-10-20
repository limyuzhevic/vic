#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration error codes
enum class ConfigErrorCode {
    None,
    FileNotFound,
    FileInvalid,
    ParseError,
    KeyNotFound,
    DuplicateKey,
    TypeMismatch,
    InvalidValue,
    ValidationError,
    OutOfRange,
    ThreadSafetyError,
    MemoryError,
    UnknownError
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message, ConfigErrorCode code = ConfigErrorCode::UnknownError)
        : std::runtime_error(message), errorCode(code) {}
    
    ConfigErrorCode getErrorCode() const { return errorCode; }
    
private:
    ConfigErrorCode errorCode;
};

// Configuration result type
class ConfigResult {
public:
    ConfigResult() : success(true), errorCode(ConfigErrorCode::None), message() {}
    ConfigResult(ConfigErrorCode code, const std::string& msg)
        : success(false), errorCode(code), message(msg) {}
    
    bool isSuccess() const { return success; }
    bool isFailure() const { return !success; }
    ConfigErrorCode getErrorCode() const { return errorCode; }
    const std::string& getMessage() const { return message; }
    
private:
    bool success;
    ConfigErrorCode errorCode;
    std::string message;
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string
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
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s)
        : key(k), value(v), source(s), description(), timestamp(std::chrono::system_clock::now()) {}
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
    ConfigResult loadFromFile(const std::string& filepath);
    
    // Load from command line arguments (enhanced)
    ConfigResult loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    ConfigResult loadFromJson(const std::string& jsonContent);
    
    // Load from YAML string
    ConfigResult loadFromYaml(const std::string& yamlContent);
    
    // Save to file (JSON format)
    ConfigResult saveToFile(const std::string& filepath) const;
    
    // Get values with error handling
    template<typename T>
    ConfigResult get(const std::string& key, T& outValue) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    ConfigResult set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    ConfigResult set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    ConfigResult set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    ConfigResult set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    ConfigResult set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Enhanced CLI interface (to be implemented in new class)
    static ConfigResult processCLI(int argc, char** argv, Config& config);
    
    // Configuration validation
    ConfigResult validate() const;
    
    // Error handling helpers
    static std::string errorCodeToString(ConfigErrorCode code);
    
    // Legacy API for backward compatibility
    bool legacyLoadFromFile(const std::string& filepath);
    bool legacyLoadFromArgs(int argc, char** argv);
    bool legacySaveToFile(const std::string& filepath) const;
    
    template<typename T>
    std::optional<T> legacyGet(const std::string& key) const;
    
    template<typename T>
    T legacyGetOr(const std::string& key, const T& defaultValue) const;
    
    void legacySet(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void legacySet(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void legacySet(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void legacySet(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void legacySet(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    bool legacyHas(const std::string& key) const;
    void legacyRemove(const std::string& key);
    std::vector<std::string> legacyGetKeys() const;
    void legacyClear();
    std::string legacySummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex configMutex;  // Thread safety
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Type conversion helpers
    static bool configValueToBool(const ConfigValue& val);
    static int configValueToInt(const ConfigValue& val);
    static int64_t configValueToInt64(const ConfigValue& val);
    static double configValueToDouble(const ConfigValue& val);
    static std::string configValueToString(const ConfigValue& val);
    static bool configValueToBool(const ConfigValue& val, bool& outValue);
    static bool configValueToInt(const ConfigValue& val, int& outValue);
    static bool configValueToInt64(const ConfigValue& val, int64_t& outValue);
    static bool configValueToDouble(const ConfigValue& val, double& outValue);
    static bool configValueToString(const ConfigValue& val, std::string& outValue);
    
    // JSON/YAML conversion helpers
    static ConfigValue jsonValueToConfigValue(const nlohmann::json& jsonVal);
    static nlohmann::json configValueToJsonValue(const ConfigValue& configVal);
    
    // CLI parsing helpers
    static bool stringToBool(const std::string& str);
    static bool stringToInt(const std::string& str, ConfigValue& outValue);
    static bool stringToDouble(const std::string& str, ConfigValue& outValue);
    static ConfigResult parseCLIValue(const std::string& arg, ConfigValue& outValue);
    
    // Validation helpers
    static ConfigResult validateConfigEntry(const ConfigEntry& entry);
    static ConfigResult validateValueType(const ConfigValue& value, const std::string& expectedType);
    static ConfigResult validateValueRange(const ConfigEntry& entry);
    static ConfigResult validateConfigInput(const std::string& key, const std::string& value);
};

} // namespace nlm