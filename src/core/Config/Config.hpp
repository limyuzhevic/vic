#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <sstream>
#include <iomanip>

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

// Enhanced Config class with nested support
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
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonContent);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with nested key support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with nested key support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence with nested key support
    bool has(const std::string& key) const;
    
    // Remove key with nested key support
    void remove(const std::string& key);
    
    // Get all keys (flattened)
    std::vector<std::string> getKeys() const;
    
    // Get keys for specific prefix
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Nested key utilities
    static std::string getParentKey(const std::string& key);
    static std::string getChildKey(const std::string& parentKey, const std::string& childKey);
    static bool isNestedKey(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string jsonValueToString(const ConfigValue& value);
    static ConfigValue parseJSONValue(const std::string& valueStr);
    
    // Nested structure helpers
    void ensureNestedStructure(const std::string& key);
    ConfigValue& getNestedValue(const std::string& key, bool createIfMissing = false);
    std::optional<ConfigValue> getNestedValue(const std::string& key) const;
    
    // Type conversion
    template<typename T>
    static std::string valueToString(const T& value);
    template<typename T>
    static std::string valueToString(const std::vector<T>& value);
    
    // Serialization
    static std::string serializeToJSON(const Config& config);
    static std::string serializeToINI(const Config& config);
    
    // Validation helpers
    static bool isValidKey(const std::string& key);
    static std::string normalizeKey(const std::string& key);
};

} // namespace nlm
