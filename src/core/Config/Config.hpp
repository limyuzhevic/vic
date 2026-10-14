#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

// Forward declaration for nlohmann::json
namespace nlohmann {
    template<typename> class basic_json;
    using json = basic_json<std::string>;
}

namespace nlm {

// Configuration parsing error
class ConfigParseError : public std::runtime_error {
public:
    explicit ConfigParseError(const std::string& msg) : std::runtime_error(msg) {}
};

// Configuration file error
class ConfigFileError : public std::runtime_error {
public:
    explicit ConfigFileError(const std::string& msg) : std::runtime_error(msg) {}
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& msg) : std::runtime_error(msg) {}
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
    
    // Load from file (JSON or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (format determined by extension)
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
    
    // JSON conversion helpers
    ConfigValue jsonToConfigValue(const nlohmann::json& j);
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    
    // Validation
    void validateKey(const std::string& key);
    void validateValue(const ConfigValue& value);
    
    // Format-specific save/load
    void saveToJSON(const std::string& filepath) const;
    void saveToSimpleFormat(const std::string& filepath) const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
