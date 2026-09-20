#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <deque>
#include <any>
#include <stdexcept>
#include <nlohmann/json.hpp>

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
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration validation error
struct ConfigValidationError {
    std::string path;
    std::string message;
    std::string expected;
    std::string actual;
};

// Configuration validation function type
using ConfigValidator = std::function<bool(const std::string&, const ConfigValue&, std::vector<ConfigValidationError>&)>;

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    std::string description;
    std::vector<ConfigValue> allowedValues;
    ConfigValidator validator;
    bool required;
    ConfigValue defaultValue;
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string reason;
};

// Configuration change callback type
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)>;

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
    
    // Configuration validation
    bool validate(const std::string& key, const ConfigValue& value) const;
    bool validateAll() const;
    
    // Schema support
    void addSchemaEntry(const std::string& key, const std::type_info& type, 
                       const std::string& description, bool required = false,
                       const ConfigValue& defaultValue = {});
    bool validateSchema() const;
    
    // Copy-on-write support
    void enableCopyOnWrite(bool enable);
    bool isCopyOnWriteEnabled() const;
    
    // Configuration flags
    void setFlag(const std::string& flag, bool value);
    bool getFlag(const std::string& flag) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
