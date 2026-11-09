#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {

// JSON/YAML value structure for configuration parsing
struct JsonValue {
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };
    
    JsonValue() : type(Type::Null) {}
    explicit JsonValue(bool b) : type(Type::Bool), boolValue(b) {}
    explicit JsonValue(double n) : type(Type::Number), numberValue(n) {}
    explicit JsonValue(const std::string& s) : type(Type::String), stringValue(s) {}
    explicit JsonValue(const std::vector<JsonValue>& arr) : type(Type::Array), arrayValue(arr) {}
    explicit JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : type(Type::Object), objectValue(obj) {}
    
    Type type;
    union {
        bool boolValue;
        double numberValue;
        std::string stringValue;
        std::vector<JsonValue> arrayValue;
        std::unordered_map<std::string, JsonValue> objectValue;
    };
    
    ~JsonValue() {
        switch (type) {
            case Type::String:
                stringValue.~basic_string();
                break;
            case Type::Array:
                arrayValue.~vector();
                break;
            case Type::Object:
                objectValue.~unordered_map();
                break;
            default:
                break;
        }
    }
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
    
    // Load from file (JSON/YAML format)
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
    
    // Validation methods
    bool validateRequiredKeys() const;
    static bool validateTypes(const std::unordered_map<std::string, JsonValue>& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::optional<ConfigValue> jsonToConfigValue(const JsonValue& jsonValue);
    static JsonValue configValueToJson(const ConfigValue& configValue);
};

} // namespace nlm
